#!/bin/bash
# ============================================================
# 彻底清理 RKE2 + Rook-Ceph + Kubernetes + containerd 环境
# 版本：v2.1
# ============================================================

set -eo pipefail

# -------------------- 颜色与日志 --------------------
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; BLUE='\033[0;34m'; CYAN='\033[0;36m'; NC='\033[0m'
log_info()  { echo -e "${GREEN}[INFO]${NC} $(date '+%H:%M:%S') $1"; }
log_warn()  { echo -e "${YELLOW}[WARN]${NC} $(date '+%H:%M:%S') $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $(date '+%H:%M:%S') $1"; exit 1; }
log_step()  { echo -e "${BLUE}[STEP]${NC} $(date '+%H:%M:%S') $1"; }
log_detail(){ echo -e "${CYAN}[DETAIL]${NC} $(date '+%H:%M:%S') $1"; }

# -------------------- 参数解析 --------------------
usage() {
    cat <<EOF
用法: $0 [选项]

可选参数:
  -n IP1,IP2,...        节点IP列表（逗号分隔），不指定则清理本机
  -p PASS1,PASS2,...    节点root密码（逗号分隔，与IP一一对应）【不推荐，建议用SSH密钥】
  --ceph-devices DEVICE_LIST  用于 OSD 的磁盘设备（逗号分隔，例如 /dev/vdb,/dev/sdc）
                       如果不指定，则不会清理磁盘数据
  --force               跳过所有确认提示
  -h                    显示此帮助信息

【重要】推荐使用SSH密钥认证，无需密码。
若必须使用密码，请设置环境变量 RB_NODE_PASSWORDS='pwd1,pwd2,...' 并运行（无需 -p）。
密码中如含特殊字符，请用单引号包围。

警告：此脚本会永久删除 Kubernetes/Rook 集群及所有数据！
EOF
    exit 0
}

CLEAN_DEVICES=""
FORCE=false
NODE_IPS_STR=""
NODE_PWDS_STR=""
CLEAN_LOCAL=true

TEMP=$(getopt -o n:p:h --long ceph-devices:,force -n 'cleanup.sh' -- "$@")
eval set -- "$TEMP"
while true; do
    case "$1" in
        -n) NODE_IPS_STR="$2"; CLEAN_LOCAL=false; shift 2 ;;
        -p) NODE_PWDS_STR="$2"; shift 2 ;;
        --ceph-devices) CLEAN_DEVICES="$2"; shift 2 ;;
        --force) FORCE=true; shift ;;
        -h) usage ;;
        --) shift; break ;;
        *) log_error "未知选项: $1" ;;
    esac
done

if [[ -z "$NODE_PWDS_STR" && -n "$RB_NODE_PASSWORDS" ]]; then
    NODE_PWDS_STR="$RB_NODE_PASSWORDS"
fi

declare -a NODE_IPS NODE_PASSWORDS
if [[ "$CLEAN_LOCAL" == false ]]; then
    IFS=',' read -ra NODE_IPS <<< "$NODE_IPS_STR"
    if [[ -n "$NODE_PWDS_STR" ]]; then
        IFS=',' read -ra NODE_PASSWORDS <<< "$NODE_PWDS_STR"
        if [[ ${#NODE_IPS[@]} -ne ${#NODE_PASSWORDS[@]} ]]; then
            log_error "IP 数量与密码数量不匹配"
        fi
        USE_SSHPASS=true
    else
        USE_SSHPASS=false
        log_warn "未提供密码，将尝试使用SSH密钥认证（需预先配置）"
    fi
    log_step "将清理以下节点: ${NODE_IPS[*]}"
else
    log_step "将清理本机"
fi

if [[ "$FORCE" != true ]]; then
    echo -e "${RED}警告：此操作将永久删除整个 Kubernetes/RKE2/Rook-Ceph 集群及其所有数据！${NC}"
    echo -e "${YELLOW}建议: 在继续之前，请确保已备份所有重要数据（etcd、PV、应用配置等）。${NC}"
    read -p "请确认是否继续？(输入 yes 继续): " confirm
    if [[ "$confirm" != "yes" ]]; then
        log_info "已取消清理操作"
        exit 0
    fi
fi

if [[ "$CLEAN_LOCAL" == false && "$USE_SSHPASS" == true && ! -x "$(command -v sshpass)" ]]; then
    log_error "使用密码认证需要安装 sshpass，请先安装（apt install sshpass）或改用SSH密钥"
fi

# -------------------- 清理函数（将在每个节点上执行） --------------------
CLEANUP_SCRIPT=$(cat <<'EOF'
#!/bin/bash
set +e  # 关闭错误退出，确保所有清理步骤都能执行
CEPH_DEVICES_LIST="$1"
log() { echo "[$(date '+%H:%M:%S')] $1"; }

# 增强型安全删除：强制卸载所有子挂载点，然后使用普通 rm -rf（兼容性最好）
safe_rm() {
    local path="$1"
    if [[ ! -e "$path" ]]; then
        return 0
    fi
    log "正在清理: $path"
    # 找出并强制卸载该路径下的所有挂载点（包括子挂载）
    while mount | grep -E "on ${path} " | awk '{print $3}' | read -r mnt; do
        log "卸载挂载点: $mnt"
        umount -l "$mnt" 2>/dev/null || true
    done
    # 再次尝试卸载可能残留的子目录（如 /var/lib/kubelet/pods/*/volumes）
    if [[ -d "$path" ]]; then
        find "$path" -type d -name "mount" -o -name "volumes" 2>/dev/null | while read -r subdir; do
            umount -l "$subdir" 2>/dev/null || true
        done
    fi
    # 直接删除（兼容所有 rm 版本，不依赖 --one-file-system）
    rm -rf "$path" 2>/dev/null || {
        log "警告: 无法删除 $path，尝试使用 find 逐层删除..."
        find "$path" -type f -exec rm -f {} \; 2>/dev/null
        find "$path" -type d -delete 2>/dev/null
        rm -rf "$path" 2>/dev/null || true
    }
    log "完成清理: $path"
}

# 1. 停止所有 RKE2 容器和进程 (killall 脚本优先)
log "停止 RKE2 服务及容器..."
if [[ -f /usr/local/bin/rke2-killall.sh ]]; then
    /usr/local/bin/rke2-killall.sh 2>/dev/null || true
elif [[ -f /usr/bin/rke2-killall.sh ]]; then
    /usr/bin/rke2-killall.sh 2>/dev/null || true
elif [[ -f /opt/rke2/bin/rke2-killall.sh ]]; then
    /opt/rke2/bin/rke2-killall.sh 2>/dev/null || true
else
    systemctl stop rke2-server rke2-agent 2>/dev/null || true
fi

# 等待进程完全退出
sleep 3

# 2. 运行官方卸载脚本
UNINSTALL_SCRIPT=""
for path in /usr/local/bin/rke2-uninstall.sh /usr/bin/rke2-uninstall.sh /opt/rke2/bin/rke2-uninstall.sh; do
    if [[ -f "$path" ]]; then
        UNINSTALL_SCRIPT="$path"
        break
    fi
done
if [[ -n "$UNINSTALL_SCRIPT" ]]; then
    log "运行 RKE2 官方卸载脚本: $UNINSTALL_SCRIPT"
    $UNINSTALL_SCRIPT 2>/dev/null || true
else
    log "未找到 RKE2 卸载脚本，将手动清理"
fi

# 3. 停止 containerd 并卸载
log "停止 containerd..."
systemctl stop containerd 2>/dev/null || true
systemctl disable containerd 2>/dev/null || true
killall containerd 2>/dev/null || true

# 4. 删除主要数据目录（顺序重要）
log "删除主要数据目录..."
safe_rm /var/lib/rancher
safe_rm /etc/rancher
safe_rm /var/lib/kubelet
safe_rm /run/k3s
safe_rm /var/lib/etcd
safe_rm /var/lib/rook
safe_rm /var/lib/containerd
safe_rm /etc/containerd

# 5. 删除 CNI 相关目录
log "删除 CNI 目录..."
safe_rm /opt/cni
safe_rm /var/lib/cni
safe_rm /var/lib/calico
safe_rm /var/lib/weave
safe_rm /etc/cni

# 6. 清理 iptables 和 ipvs
log "清理 iptables/ipvs 规则..."
iptables -P INPUT ACCEPT 2>/dev/null || true
iptables -P FORWARD ACCEPT 2>/dev/null || true
iptables -P OUTPUT ACCEPT 2>/dev/null || true
for table in filter nat mangle raw; do
    iptables -t $table -F 2>/dev/null || true
    iptables -t $table -X 2>/dev/null || true
done
ip6tables -t nat -F 2>/dev/null || true
ip6tables -F 2>/dev/null || true
ip6tables -X 2>/dev/null || true
ipvsadm -C 2>/dev/null || true
if command -v ipset &>/dev/null; then
    ipset destroy 2>/dev/null || true
fi

# 7. 删除虚拟网络接口
log "删除虚拟网络接口..."
for iface in flannel.1 cni0 flannel-v6.1 vxlan.calico weave kube-bridge docker0; do
    ip link delete $iface 2>/dev/null || true
done

# 8. 清理 Ceph OSD 磁盘（如指定）
if [[ -n "$CEPH_DEVICES_LIST" ]]; then
    IFS=',' read -ra DEVICES <<< "$CEPH_DEVICES_LIST"
    for dev in "${DEVICES[@]}"; do
        if [[ -b "$dev" ]]; then
            log "彻底清理磁盘设备: $dev"

            # 尝试安装 ceph-osd 以获取 ceph-volume 工具
            if ! command -v ceph-volume &>/dev/null; then
                log "ceph-volume 未找到，尝试安装 ceph-osd ..."
                export DEBIAN_FRONTEND=noninteractive
                # 添加 Ceph 官方仓库（使用 Quincy 版本，适配 Ubuntu 22.04/24.04）
                wget -q -O- 'https://download.ceph.com/keys/release.asc' | apt-key add - 2>/dev/null || true
                echo "deb https://download.ceph.com/debian-quincy/ $(lsb_release -sc) main" > /etc/apt/sources.list.d/ceph.list
                apt-get update -qq 2>/dev/null || true
                apt-get install -y -qq ceph-osd 2>/dev/null || log_warn "ceph-osd 安装失败，将使用手动清理方式"
            fi

            # 方法一：使用 ceph-volume（最彻底）
            if command -v ceph-volume &>/dev/null; then
                log "使用 ceph-volume lvm zap --destroy 清理 $dev"
                ceph-volume lvm zap "$dev" --destroy 2>/dev/null || {
                    log_warn "ceph-volume 清理失败，尝试手动清理"
                    # 手动清理回退
                    dmsetup remove_all 2>/dev/null || true
                    pv_info=$(pvs --noheadings -o pv_name,vg_name 2>/dev/null | grep "$dev" || true)
                    if [[ -n "$pv_info" ]]; then
                        echo "$pv_info" | awk '{print $2}' | sort -u | while read vg_name; do
                            vgchange -an "$vg_name" 2>/dev/null || true
                            lvremove -f "$vg_name" 2>/dev/null || true
                            vgremove -f "$vg_name" 2>/dev/null || true
                        done
                        pvremove -f "$dev" 2>/dev/null || true
                    fi
                    sgdisk --zap-all "$dev" 2>/dev/null || true
                    wipefs -a -f "$dev" 2>/dev/null || true
                    dd if=/dev/zero of="$dev" bs=1M count=200 conv=fsync oflag=direct 2>/dev/null || true
                }
            else
                # 方法二：手动清理（无 ceph-volume 时的回退）
                log "使用手动 LVM 清理方式清理 $dev"
                dmsetup remove_all 2>/dev/null || true
                pv_info=$(pvs --noheadings -o pv_name,vg_name 2>/dev/null | grep "$dev" || true)
                if [[ -n "$pv_info" ]]; then
                    echo "$pv_info" | awk '{print $2}' | sort -u | while read vg_name; do
                        vgchange -an "$vg_name" 2>/dev/null || true
                        lvremove -f "$vg_name" 2>/dev/null || true
                        vgremove -f "$vg_name" 2>/dev/null || true
                    done
                    pvremove -f "$dev" 2>/dev/null || true
                fi
                sgdisk --zap-all "$dev" 2>/dev/null || true
                wipefs -a -f "$dev" 2>/dev/null || true
                dd if=/dev/zero of="$dev" bs=1M count=200 conv=fsync oflag=direct 2>/dev/null || true
            fi

            partprobe "$dev" 2>/dev/null || true
            log "设备 $dev 清理完成"
        else
            log "警告：设备 $dev 不存在或不是块设备，跳过"
        fi
    done
fi

# 9. 删除 kubectl/helm/crictl 等工具（可选）
log "删除 kubectl/helm/crictl 二进制..."
rm -f /usr/local/bin/kubectl /usr/local/bin/helm /usr/local/bin/crictl /usr/local/bin/ctr

# 10. 删除 kubeconfig 文件
safe_rm /root/.kube
safe_rm /etc/kubernetes

# 11. 最后再次尝试卸载所有遗留挂载点（防止未清理干净）
log "最终清理残留挂载点..."
mount | grep -E '/var/lib/kubelet|/var/lib/rancher|/run/k3s' | awk '{print $3}' | xargs -r umount -l 2>/dev/null || true

log "节点清理完成！"
EOF
)

# -------------------- 执行清理（本机或远程） --------------------
run_cleanup_on_node() {
    local ip="$1"
    local pass="$2"
    log_info "开始清理节点 $ip ..."
    if [[ "$USE_SSHPASS" == true ]]; then
        export SSHPASS="$pass"
        local ssh_cmd="sshpass -e ssh -o StrictHostKeyChecking=no -o ConnectTimeout=10 -o PasswordAuthentication=yes"
    else
        local ssh_cmd="ssh -o StrictHostKeyChecking=no -o ConnectTimeout=10 -o PasswordAuthentication=no"
    fi
    # 将清理脚本通过 stdin 传递，并捕获退出码（使用 set +e 避免 ssh 命令自身失败导致脚本退出）
    if ! $ssh_cmd root@"$ip" bash -s -- "$CLEAN_DEVICES" <<< "$CLEANUP_SCRIPT"; then
        log_error "节点 $ip 清理失败"
    fi
    if [[ "$USE_SSHPASS" == true ]]; then
        unset SSHPASS
    fi
    log_info "节点 $ip 清理完成"
}

# 单机模式
if [[ "$CLEAN_LOCAL" == true ]]; then
    log_info "开始清理本机..."
    bash -c "$CLEANUP_SCRIPT" -- "$CLEAN_DEVICES"
    log_info "本机清理完成"
else
    for ((i=0; i<${#NODE_IPS[@]}; i++)); do
        if [[ "$USE_SSHPASS" == true ]]; then
            run_cleanup_on_node "${NODE_IPS[$i]}" "${NODE_PASSWORDS[$i]}"
        else
            run_cleanup_on_node "${NODE_IPS[$i]}" ""
        fi
    done
fi

log_step "所有节点清理完毕。"
log_warn "为确保所有资源完全释放，建议重启所有节点。"
log_info "如有 NetworkManager 干扰问题，重启后网络接口将恢复正常。"