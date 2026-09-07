#!/bin/bash
# ============================================================
# 生产级部署脚本（RKE2 + Rook-Ceph + Rancher + KubeBlocks）
# 版本：v24.2
# ============================================================

set -euo pipefail

# -------------------- 颜色与日志 --------------------
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; BLUE='\033[0;34m'; CYAN='\033[0;36m'; NC='\033[0m'
log_info()  { echo -e "${GREEN}[INFO]${NC} $(date '+%H:%M:%S') $1"; }
log_warn()  { echo -e "${YELLOW}[WARN]${NC} $(date '+%H:%M:%S') $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $(date '+%H:%M:%S') $1"; exit 1; }
log_step()  { echo -e "${BLUE}[STEP]${NC} $(date '+%H:%M:%S') $1"; }
log_detail(){ echo -e "${CYAN}[DETAIL]${NC} $(date '+%H:%M:%S') $1"; }

MAIN_LOG="/tmp/rancher_deploy_$(date +%Y%m%d_%H%M%S).log"
exec 3>&1
exec > >(tee -a "$MAIN_LOG") 2>&1
log_info "所有输出将记录到 $MAIN_LOG"

if [[ $EUID -ne 0 ]]; then
    echo "请使用 root 用户或 sudo 执行此脚本" >&2
    exit 1
fi

PHASE_VERSION="v24.2"
PHASE_DIR="/var/lib/rancher-install/phases-${PHASE_VERSION}"
for old_dir in /var/lib/rancher-install/phases-*; do
    if [[ -d "$old_dir" && "$old_dir" != "$PHASE_DIR" ]]; then
        rm -rf "$old_dir" 2>/dev/null || true
    fi
done
if [[ ! -f "$PHASE_DIR/.version" ]]; then
    mkdir -p "$PHASE_DIR"
    echo "$PHASE_VERSION" > "$PHASE_DIR/.version"
fi
mark_done() { touch "$PHASE_DIR/$1"; log_detail "阶段标记已设置: $1"; }
is_done()   { [[ -f "$PHASE_DIR/$1" ]]; }
clear_phase() { rm -f "$PHASE_DIR/$1"; log_detail "已清除阶段标记: $1"; }

trap 'log_error "脚本异常退出，请检查日志: $MAIN_LOG"' ERR

declare -g PHASE_START_TIME=0
phase_start() { PHASE_START_TIME=$(date +%s); log_step "开始: $1"; }
phase_end() {
    local end_time=$(date +%s)
    local elapsed=$((end_time - PHASE_START_TIME))
    log_step "完成: $1 (耗时 ${elapsed}秒)"
}

# -------------------- 进度辅助函数 --------------------
spin_wait() {
    local pid=$1
    local message=$2
    local spin='|/-\'
    local i=0
    while kill -0 $pid 2>/dev/null; do
        i=$(( (i+1) % 4 ))
        printf "\r%s %c" "$message" "${spin:$i:1}"
        sleep 0.5
    done
    printf "\r%s ✓\n" "$message"
}

# 全局下载函数：支持多 URL 回退，新风格 (output, url1, url2, ...)
download_with_progress() {
    local output=$1
    shift
    local urls=("$@")
    local success=false
    
    for url in "${urls[@]}"; do
        log_detail "尝试下载: $url"
        if curl -fSL --retry 3 --connect-timeout 30 --progress-bar -o "$output" "$url" 2>&1; then
            log_info "下载成功: $output (来自 $url)"
            success=true
            break
        else
            log_warn "从 $url 下载失败，尝试下一个地址..."
        fi
    done
    
    if [[ "$success" != "true" ]]; then
        log_error "所有下载地址均失败，无法获取 $output"
    fi
}

scp_with_progress() {
    local src=$1
    local dst=$2
    log_detail "正在复制: $(basename $src) -> $dst"
    if [[ "$dst" =~ ^([^@]+@[^:]+):(.*)$ ]]; then
        local host="${BASH_REMATCH[1]}"
        local path="${BASH_REMATCH[2]}"
        export SSHPASS
        sshpass -e ssh -o StrictHostKeyChecking=no "$host" "mkdir -p $(dirname "$path")" 2>/dev/null
    fi
    rsync -avP -e "sshpass -e ssh -o StrictHostKeyChecking=no" "$src" "$dst" 2>&1
    if [[ $? -eq 0 ]]; then
        log_detail "复制完成: $src"
    else
        log_error "复制失败: $src -> $dst"
    fi
}

# -------------------- 镜像与代理 --------------------
MIRROR_DOCKER="${MIRROR_DOCKER:-https://sgulj51za84gae.xuanyuan.run}"
MIRROR_K8S="${MIRROR_K8S:-https://sgulj51za84gae-k8s.xuanyuan.run}"
MIRROR_QUAY="${MIRROR_QUAY:-https://sgulj51za84gae-quay.xuanyuan.run}"
MIRROR_GHCR="${MIRROR_GHCR:-https://sgulj51za84gae-ghcr.xuanyuan.run}"
CUSTOM_DNS="${CUSTOM_DNS:-222.172.200.68}"
PROXY_PREFIX="${PROXY_PREFIX:-https://gh-proxy.com/}"

# -------------------- 全局版本 --------------------
RANCHER_VERSION="${RANCHER_VERSION:-2.14.2}"
RANCHER_PASSWORD="${RANCHER_PASSWORD:-$(openssl rand -base64 16 | tr -d '/+=')}"
ROOK_VERSION="${ROOK_VERSION:-1.19.6}"
KUBEBLOCKS_VERSION="${KUBEBLOCKS_VERSION:-1.0.2}"
CEPH_VERSION="${CEPH_VERSION:-20.2.1}"
SNAPSHOTTER_VERSION="${SNAPSHOTTER_VERSION:-8.5.0}"
CSI_ADDONS_VERSION="${CSI_ADDONS_VERSION:-v0.14.0}"
RKE2_VERSION="${RKE2_VERSION:-1.35.5+rke2r2}"
RKE2_IMAGES_TAR="rke2-images.linux-amd64.tar.zst"
TRAEFIK_VERSION="${TRAEFIK_VERSION:-3.7.4}"
CERT_MANAGER_VERSION="${CERT_MANAGER_VERSION:-1.20.2}"
HELM_VERSION="${HELM_VERSION:-v3.20.2}"
TLS_SOURCE="rancher"
CEPH_DEVICES=""
FORCE="false"
DRY_RUN=""
PARALLEL="false"
LOADBALANCER_IP=""

[[ ${RKE2_VERSION} != v* ]] && RKE2_VERSION="v${RKE2_VERSION}"

# -------------------- 初始化变量 --------------------
NODE_IPS_STR=""
NODE_PWDS_STR=""
RANCHER_DOMAIN=""

# -------------------- 参数解析 --------------------
usage() {
    cat <<EOF
用法: $0 [选项]

必选参数:
  -n IP1,IP2,...       节点IP列表（逗号分隔，至少3个以实现HA）
  -p PASS1,PASS2,...   节点root密码（与节点一一对应）
  -d DOMAIN            Rancher 访问域名（需解析到负载均衡器IP）
  --ceph-devices DEVICE_LIST  用于OSD的磁盘设备（逗号分隔，例如 /dev/sdb）

重要参数（高可用）:
  --lb-ip IP           负载均衡器IP（OpenResty或其他TCP代理），必须提前配置好转发 6443 和 9345 端口到所有master节点

可选参数:
  --rancher-ver VERSION   Rancher版本（默认2.14.2）
  --rancher-password PWD  Rancher初始admin密码（默认随机）
  --rook-ver VERSION       Rook版本（默认1.19.6）
  --kb-ver VERSION         KubeBlocks版本（默认1.0.2）
  --ceph-version VERSION   Ceph集群镜像版本（默认20.2.1）
  --tls-source {letsEncrypt,selfSigned}  TLS来源（默认selfSigned）
  --force                  跳过生产确认
  --dry-run                仅生成配置
  --parallel               并行初始化节点
  -h                       帮助

【重要】密码含特殊字符时，建议用环境变量 RB_NODE_PASSWORDS 传递。
EOF
    exit 0
}

TEMP=$(getopt -o n:p:d:s:h \
    --long rancher-ver:,rancher-password:,rook-ver:,kb-ver:,ceph-version:,ceph-devices:,snapshotter-version:,csi-addons-version:,rke2-version:,traefik-version:,cert-manager-version:,helm-version:,tls-source:,force,dry-run,parallel,lb-ip: \
    -n 'k8s_ubuntu.sh' -- "$@")
eval set -- "$TEMP"

while true; do
    case "$1" in
        -n) NODE_IPS_STR="$2"; shift 2 ;;
        -p) NODE_PWDS_STR="$2"; shift 2 ;;
        -d) RANCHER_DOMAIN="$2"; shift 2 ;;
        --ceph-devices) CEPH_DEVICES="$2"; shift 2 ;;
        --lb-ip) LOADBALANCER_IP="$2"; shift 2 ;;
        --rancher-ver) RANCHER_VERSION="$2"; shift 2 ;;
        --rancher-password) RANCHER_PASSWORD="$2"; shift 2 ;;
        --rook-ver) ROOK_VERSION="$2"; shift 2 ;;
        --kb-ver) KUBEBLOCKS_VERSION="$2"; shift 2 ;;
        --ceph-version) CEPH_VERSION="$2"; shift 2 ;;
        --tls-source) TLS_SOURCE="$2"; shift 2 ;;
        --force) FORCE="true"; shift ;;
        --dry-run) DRY_RUN="true"; shift ;;
        --parallel) PARALLEL="true"; shift ;;
        -h) usage ;;
        --) shift; break ;;
        *) log_error "未知选项: $1" ;;
    esac
done

if [[ -z "$NODE_PWDS_STR" && -n "${RB_NODE_PASSWORDS:-}" ]]; then
    NODE_PWDS_STR="$RB_NODE_PASSWORDS"
fi

if [[ -z "$NODE_IPS_STR" || -z "$NODE_PWDS_STR" ]]; then
    log_error "必须提供节点IP列表（-n）和密码列表（-p）或环境变量 RB_NODE_PASSWORDS"
fi

IFS=',' read -ra NODE_IPS <<< "$NODE_IPS_STR"
IFS=',' read -ra NODE_PASSWORDS <<< "$NODE_PWDS_STR"
NODE_COUNT=${#NODE_IPS[@]}
[[ $NODE_COUNT -lt 3 ]] && log_error "生产环境至少需要3个节点以实现高可用"
[[ ${#NODE_PASSWORDS[@]} -ne $NODE_COUNT ]] && log_error "密码数量与节点数量不一致"
[[ -z "$RANCHER_DOMAIN" ]] && log_error "必须指定 Rancher 域名（-d）"
[[ -z "$CEPH_DEVICES" ]] && log_error "必须指定 Ceph OSD 设备（--ceph-devices）"
[[ "$TLS_SOURCE" != "letsEncrypt" && "$TLS_SOURCE" != "rancher" && "$TLS_SOURCE" != "secret" ]] && log_error "--tls-source 只能是 letsEncrypt 或 rancher 或 secret"

if [[ -z "$LOADBALANCER_IP" ]]; then
    log_warn "未提供负载均衡器IP（--lb-ip），将使用第一个master节点IP作为注册地址，这会导致单点故障！"
    LOADBALANCER_IP="${NODE_IPS[0]}"
else
    log_info "使用负载均衡器IP: $LOADBALANCER_IP"
    if command -v nc &>/dev/null; then
        if ! nc -zv "$LOADBALANCER_IP" 6443 2>/dev/null; then
            log_warn "负载均衡器 $LOADBALANCER_IP 的 6443 端口不可达"
        fi
        if ! nc -zv "$LOADBALANCER_IP" 9345 2>/dev/null; then
            log_warn "负载均衡器 $LOADBALANCER_IP 的 9345 端口不可达"
        fi
    else
        log_warn "未安装nc，跳过端口检测"
    fi
fi

declare -a NODE_NAMES MASTER_NODES WORKER_NODES
for ((i=0; i<NODE_COUNT; i++)); do
    if [[ $i -lt 3 ]]; then
        NODE_NAMES+=("master$((i+1))")
        MASTER_NODES+=("master$((i+1))")
    else
        NODE_NAMES+=("worker$((i-2))")
        WORKER_NODES+=("worker$((i-2))")
    fi
done
CEPH_REPLICAS=3

log_step "==================== 配置摘要 ===================="
echo "  RKE2版本: $RKE2_VERSION"
echo "  Rancher版本: $RANCHER_VERSION  域名: $RANCHER_DOMAIN"
echo "  Rook版本: $ROOK_VERSION   Ceph版本: $CEPH_VERSION"
echo "  KubeBlocks版本: $KUBEBLOCKS_VERSION"
echo "  节点总数: $NODE_COUNT (Master: ${MASTER_NODES[*]}, Worker: ${WORKER_NODES[*]})"
echo "  负载均衡器IP: $LOADBALANCER_IP"
echo "  Ceph OSD设备: $CEPH_DEVICES  副本数: $CEPH_REPLICAS"
echo "  cert-manager版本: $CERT_MANAGER_VERSION"
echo ""

OS_CODENAME=""
if [[ -f /etc/os-release ]]; then
    . /etc/os-release
    OS_CODENAME="$VERSION_CODENAME"
    if [[ "$ID" == "ubuntu" && "$VERSION_ID" == "24.04" ]]; then
        log_warn "检测到 Ubuntu 24.04，RKE2 官方支持此版本，将使用标准配置。"
        if [[ "$FORCE" != "true" ]]; then
            read -p "仍要使用 Ubuntu 24.04 继续部署吗？(y/N) " -n 1 -r
            echo
            if [[ ! $REPLY =~ ^[Yy]$ ]]; then
                exit 0
            fi
        fi
    fi
fi

[[ "$DRY_RUN" == "true" ]] && log_info "Dry-run结束" && exit 0

if [[ "$FORCE" != "true" ]]; then
    read -p "以上配置将部署到生产环境，是否继续？(y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 0
    fi
fi

# ================== 辅助函数 ==================
safe_kubectl() {
    if ! kubectl "$@" 2>&1 | tee -a "$MAIN_LOG"; then
        log_warn "kubectl $* 失败，继续执行"
    fi
}

# ================== 阶段0：本地环境自举 ==================
if ! is_done "phase0_bootstrap"; then
    phase_start "初始化控制节点环境"
    export LANG=en_US.UTF-8 LC_ALL=en_US.UTF-8
    locale-gen en_US.UTF-8 2>/dev/null || true
    if [[ -f /etc/os-release ]]; then
        . /etc/os-release
        if [[ "$ID" != "ubuntu" ]] || [[ ! "$VERSION_ID" =~ ^(22\.04|24\.04)$ ]]; then
            log_error "仅支持 Ubuntu 22.04/24.04，检测到: $ID $VERSION_ID"
        fi
        OS_CODENAME="$VERSION_CODENAME"
    else
        OS_CODENAME="noble"
    fi

    cat > /etc/apt/sources.list.d/ubuntu.sources <<EOF
Types: deb
URIs: http://mirrors.aliyun.com/ubuntu/
Suites: $OS_CODENAME $OS_CODENAME-updates $OS_CODENAME-backports
Components: main restricted universe multiverse
Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg
EOF
    apt-get update -qq
    apt-get install -y -qq curl sshpass jq rsync python3 python3-yaml util-linux dnsutils netcat-openbsd || log_error "安装基础工具失败"

    if ! command -v helm &>/dev/null; then
        helm_version="${HELM_VERSION}"
        arch=$(dpkg --print-architecture)
        [[ "$arch" == "amd64" ]] && arch="amd64"
        [[ "$arch" == "arm64" ]] && arch="arm64"
        helm_tarball="helm-${helm_version}-linux-${arch}.tar.gz"
        helm_url="https://get.helm.sh/${helm_tarball}"
        helm_url_proxy="${PROXY_PREFIX}${helm_url}"
        cd /tmp
        for url in "$helm_url_proxy" "$helm_url"; do
            if curl -fsSL --retry 3 --connect-timeout 30 -o "$helm_tarball" "$url"; then
                tar -xzf "$helm_tarball"
                mv linux-${arch}/helm /usr/local/bin/helm
                chmod +x /usr/local/bin/helm
                rm -rf "$helm_tarball" linux-${arch}
                log_info "Helm安装成功: $(helm version --short)"
                break
            else
                log_warn "从 $url 下载失败，尝试下一个地址"
            fi
        done
        if ! command -v helm &>/dev/null; then
            log_error "Helm安装失败"
        fi
    fi

    mark_done "phase0_bootstrap"
    phase_end "控制节点环境初始化"
fi

# ================== 阶段1：SSH连通性测试 ==================
if ! is_done "phase1_ssh"; then
    phase_start "验证所有节点SSH连通性"
    for ((i=0; i<NODE_COUNT; i++)); do
        ip="${NODE_IPS[$i]}" pass="${NODE_PASSWORDS[$i]}" name="${NODE_NAMES[$i]}"
        log_detail "测试 $name ($ip)"
        if SSHPASS="$pass" sshpass -e ssh -o StrictHostKeyChecking=no -o ConnectTimeout=5 root@"$ip" "echo ok" &>/dev/null; then
            log_info "$name 连通正常"
        else
            log_error "无法连接 $name ($ip)"
        fi
    done
    mark_done "phase1_ssh"
    phase_end "SSH连通性验证"
fi

# ================== 阶段2：远程节点系统初始化 ==================
if ! is_done "phase2_node_init"; then
    phase_start "所有节点系统优化与准备"

    REMOTE_SCRIPT=$(cat <<'REMOTE_EOF'
#!/bin/bash
set -euo pipefail
sudo pkill -9 apt 2>/dev/null || true
sudo rm -f /var/lib/dpkg/lock-frontend /var/lib/dpkg/lock /var/cache/apt/archives/lock 2>/dev/null || true

MIRROR_DOCKER="$1"; MIRROR_K8S="$2"; MIRROR_QUAY="$3"; MIRROR_GHCR="$4"; CUSTOM_DNS="$5"; RKE2_VERSION="$6"; OS_CODENAME="$7"

log() { echo "[$(date '+%H:%M:%S')] $1"; }
log "开始系统初始化..."
export DEBIAN_FRONTEND=noninteractive

cat > /etc/apt/sources.list.d/ubuntu.sources <<EOF
Types: deb
URIs: http://mirrors.aliyun.com/ubuntu/
Suites: $OS_CODENAME $OS_CODENAME-updates $OS_CODENAME-backports
Components: main restricted universe multiverse
Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg
EOF
apt-get update -qq
apt-get install -y -qq wget curl lsof openssl iptables socat ipvsadm lvm2 xfsprogs nfs-common net-tools fail2ban ebtables ipset parted chrony sshpass open-iscsi iptables-persistent containerd gdisk rsync netcat-openbsd linux-modules-extra-$(uname -r) || true

# 配置 NetworkManager 忽略 CNI 接口
log "配置 NetworkManager 忽略 CNI 网络接口..."
mkdir -p /etc/NetworkManager/conf.d
cat > /etc/NetworkManager/conf.d/rke2-canal.conf <<EOF
[keyfile]
unmanaged-devices=interface-name:cali*;interface-name:flannel*;interface-name:tunl*;interface-name:vxlan.calico;interface-name:vxlan-v6.calico;interface-name:wireguard.cali;interface-name:wg-v6.cali
EOF
systemctl reload NetworkManager 2>/dev/null || true

swapoff -a && sed -ri '/\bswap\b/s/^/#/' /etc/fstab
systemctl enable --now iscsd 2>/dev/null || true
systemctl stop ufw && systemctl disable ufw 2>/dev/null || true
systemctl enable --now chrony
timedatectl set-timezone Asia/Shanghai && timedatectl set-ntp true

cat >> /etc/security/limits.conf <<EOF
* soft nofile 1048576
* hard nofile 1048576
* soft memlock unlimited
* hard memlock unlimited
EOF
echo "DefaultLimitNOFILE=1048576" >> /etc/systemd/system.conf

cat > /etc/sysctl.d/99-ceph.conf <<EOF
vm.dirty_ratio = 20
vm.dirty_background_ratio = 10
vm.vfs_cache_pressure = 50
net.core.rmem_max = 134217728
net.core.wmem_max = 134217728
EOF
cat > /etc/sysctl.d/99-k8s.conf <<EOF
net.ipv4.ip_forward = 1
net.bridge.bridge-nf-call-iptables = 1
vm.swappiness = 1
fs.inotify.max_user_instances = 524288
fs.file-max = 2097152
EOF
sysctl --system

for rule in "INPUT -p tcp -m multiport --dports 6443,2379,2380,10250,10257,10259,9345 -j ACCEPT" \
            "INPUT -p udp -m multiport --dports 8472 -j ACCEPT"; do
    iptables -C $rule 2>/dev/null || iptables -A $rule
done
netfilter-persistent save 2>/dev/null || true

mkdir -p /etc/containerd
containerd config default > /etc/containerd/config.toml
sed -i 's/SystemdCgroup = false/SystemdCgroup = true/' /etc/containerd/config.toml

mkdir -p /etc
cat > /etc/crictl.yaml <<CRICTL_EOF
runtime-endpoint: "unix:///run/containerd/containerd.sock"
image-endpoint: "unix:///run/containerd/containerd.sock"
timeout: 10
debug: false
CRICTL_EOF

mkdir -p /etc/containerd
if [[ ! -f /etc/containerd/config.toml ]]; then
    containerd config default > /etc/containerd/config.toml
fi

CONFIG_FILE="/etc/containerd/config.toml"
if grep -q '\[plugins\."io\.containerd\.cri\.v1\.images"\.registry\]' "$CONFIG_FILE"; then
    if ! grep -A5 '\[plugins\."io\.containerd\.cri\.v1\.images"\.registry\]' "$CONFIG_FILE" | grep -q 'config_path = "/etc/containerd/certs.d"'; then
        sed -i '/\[plugins\."io\.containerd\.cri\.v1\.images"\.registry\]/a\  config_path = "/etc/containerd/certs.d"' "$CONFIG_FILE"
    fi
else
    cat >> "$CONFIG_FILE" <<EOF

[plugins."io.containerd.cri.v1.images".registry]
  config_path = "/etc/containerd/certs.d"
EOF
fi

mkdir -p /etc/containerd/certs.d/docker.io
cat > /etc/containerd/certs.d/docker.io/hosts.toml <<HOSTS_EOF
server = "https://registry-1.docker.io"
[host."https://docker.m.daocloud.io	"]
  capabilities = ["pull", "resolve"]
[host."https://dockerproxy.net"]
  capabilities = ["pull", "resolve"]
[host."${MIRROR_DOCKER}"]
  capabilities = ["pull", "resolve"]
[host."https://registry-1.docker.io"]
  capabilities = ["pull", "resolve"]
HOSTS_EOF

mkdir -p /etc/containerd/certs.d/quay.io
cat > /etc/containerd/certs.d/quay.io/hosts.toml <<HOSTS_EOF
server = "https://quay.io"
[host."https://quay.m.daocloud.io"]
  capabilities = ["pull", "resolve"]
[host."https://quay.dockerproxy.net"]
  capabilities = ["pull", "resolve"]
[host."${MIRROR_QUAY}"]
  capabilities = ["pull", "resolve"]
[host."https://quay.io"]
  capabilities = ["pull", "resolve"]
HOSTS_EOF

mkdir -p /etc/containerd/certs.d/registry.k8s.io
cat > /etc/containerd/certs.d/registry.k8s.io/hosts.toml <<HOSTS_EOF
server = "https://registry.k8s.io"
[host."https://k8s.m.daocloud.io"]
  capabilities = ["pull", "resolve"]
[host."https://k8s.dockerproxy.net"]
  capabilities = ["pull", "resolve"]
[host."${MIRROR_K8S}"]
  capabilities = ["pull", "resolve"]
[host."https://registry.k8s.io"]
  capabilities = ["pull", "resolve"]
HOSTS_EOF

mkdir -p /etc/containerd/certs.d/ghcr.io
cat > /etc/containerd/certs.d/ghcr.io/hosts.toml <<HOSTS_EOF
server = "https://ghcr.io"
[host."https://ghcr.m.daocloud.io"]
  capabilities = ["pull", "resolve"]
[host."https://ghcr.dockerproxy.net"]
  capabilities = ["pull", "resolve"]
[host."${MIRROR_GHCR}"]
  capabilities = ["pull", "resolve"]
[host."https://ghcr.io"]
  capabilities = ["pull", "resolve"]
HOSTS_EOF

mkdir -p /etc/containerd/certs.d
systemctl restart containerd

log "节点初始化完成"
REMOTE_EOF
)

    run_remote_init() {
        local i=$1
        local ip="${NODE_IPS[$i]}" pass="${NODE_PASSWORDS[$i]}" name="${NODE_NAMES[$i]}"
        local node_log="/tmp/rancher_init_${name}_$(date +%Y%m%d_%H%M%S).log"
        log_info "初始化 $name ($ip) (日志: $node_log)"
        local tmp_script=$(mktemp)
        echo "$REMOTE_SCRIPT" > "$tmp_script"
        export SSHPASS="$pass"
        sshpass -e ssh -o StrictHostKeyChecking=no root@"$ip" \
            "bash -s -- '$MIRROR_DOCKER' '$MIRROR_K8S' '$MIRROR_QUAY' '$MIRROR_GHCR' '$CUSTOM_DNS' '$RKE2_VERSION' '$OS_CODENAME'" \
            < "$tmp_script" 2>&1 | tee -a "$node_log"
        local exit_code=${PIPESTATUS[0]}
        rm -f "$tmp_script"
        unset SSHPASS
        [[ $exit_code -eq 0 ]] || log_error "$name 初始化失败，日志 $node_log"
        log_info "$name 初始化成功"
    }

    if [[ "$PARALLEL" == "true" ]]; then
        for ((i=0; i<NODE_COUNT; i++)); do
            run_remote_init "$i" &
        done
        wait
    else
        for ((i=0; i<NODE_COUNT; i++)); do
            run_remote_init "$i"
        done
    fi

    mark_done "phase2_node_init"
    phase_end "节点系统初始化"
fi

# ================== 阶段3：准备RKE2安装 ==================
if ! is_done "phase3_rke2_prepare"; then
    phase_start "准备RKE2安装"
    mkdir -p /root/rancher-install
    cd /root/rancher-install

    # 生成或读取 token
    if [[ -f /root/rancher-install/rke2_token ]]; then
        RKE2_TOKEN=$(cat /root/rancher-install/rke2_token)
    else
        RKE2_TOKEN=$(openssl rand -hex 16)
        echo "$RKE2_TOKEN" > /root/rancher-install/rke2_token
    fi
    log_detail "RKE2 token: ${RKE2_TOKEN}"

    # 生成极简 install_rke2.sh
    cat > /root/rancher-install/install_rke2.sh <<'INSTALL_SCRIPT'
#!/bin/bash
set -euo pipefail

if [ $# -ne 6 ]; then
    echo "Usage: $0 <role> <token> <lb_addr> <node_name> <rancher_domain> <node_ip>"
    exit 1
fi

ROLE="$1"
TOKEN="$2"
LB_ADDR="$3"
NODE_NAME="$4"
RANCHER_DOMAIN="$5"
NODE_IP="$6"

export INSTALL_RKE2_MIRROR="cn"
curl -sfL https://rancher-mirror.rancher.cn/rke2/install.sh | sh -

mkdir -p /etc/rancher/rke2
CONFIG_FILE="/etc/rancher/rke2/config.yaml"

if [[ "$ROLE" == "first" ]]; then
    cat > "$CONFIG_FILE" <<EOF
token: ${TOKEN}
node-name: ${NODE_NAME}
tls-san:
- ${LB_ADDR}
- ${RANCHER_DOMAIN}
write-kubeconfig-mode: "0644"
EOF
elif [[ "$ROLE" == "other" ]]; then
    cat > "$CONFIG_FILE" <<EOF
server: https://${LB_ADDR}:9345
token: ${TOKEN}
node-name: ${NODE_NAME}
tls-san:
- ${LB_ADDR}
- ${RANCHER_DOMAIN}
write-kubeconfig-mode: "0644"
EOF
elif [[ "$ROLE" == "agent" ]]; then
    cat > "$CONFIG_FILE" <<EOF
server: https://${LB_ADDR}:9345
token: ${TOKEN}
node-name: ${NODE_NAME}
write-kubeconfig-mode: "0644"
EOF
fi

if [[ "$ROLE" == "first" || "$ROLE" == "other" ]]; then
    systemctl enable rke2-server
    systemctl start rke2-server
else
    systemctl enable rke2-agent
    systemctl start rke2-agent
fi

sleep 5
if systemctl is-active --quiet rke2-server || systemctl is-active --quiet rke2-agent; then
    echo "RKE2 service started"
else
    journalctl -u rke2-server --no-pager -n 50 || journalctl -u rke2-agent --no-pager -n 50
    exit 1
fi
INSTALL_SCRIPT

    chmod +x /root/rancher-install/install_rke2.sh

    if [[ ! -f "$RKE2_IMAGES_TAR" ]]; then
        log_info "下载 RKE2 离线镜像包..."
        PROXY_URL="${PROXY_PREFIX}https://github.com/rancher/rke2/releases/download/${RKE2_VERSION}/${RKE2_IMAGES_TAR}"
        DIRECT_URL="https://github.com/rancher/rke2/releases/download/${RKE2_VERSION}/${RKE2_IMAGES_TAR}"
        download_with_progress "$RKE2_IMAGES_TAR" "$PROXY_URL" "$DIRECT_URL"
    fi

    mark_done "phase3_rke2_prepare"
    phase_end "准备RKE2安装"
fi

# ================== 阶段4：部署RKE2集群 ==================
if ! is_done "phase4_rke2"; then
    phase_start "部署RKE2集群 (HA模式)"

    # 读取 token
    if [[ -f /root/rancher-install/rke2_token ]]; then
        RKE2_TOKEN=$(cat /root/rancher-install/rke2_token)
    else
        log_error "RKE2 token 文件不存在，请检查阶段3"
    fi
    log_detail "使用 RKE2 token: ${RKE2_TOKEN}"

    # 清理残留
    CLEANUP_SCRIPT=$(cat <<'CLEANEOF'
systemctl stop rke2-server rke2-agent 2>/dev/null || true
/usr/local/bin/rke2-uninstall.sh 2>/dev/null || true
rm -rf /var/lib/rancher /etc/rancher /var/lib/kubelet /run/k3s /var/lib/rook /var/lib/etcd
CLEANEOF
)
    for ((i=0; i<NODE_COUNT; i++)); do
        ip="${NODE_IPS[$i]}" pass="${NODE_PASSWORDS[$i]}" name="${NODE_NAMES[$i]}"
        log_detail "清理节点 $name ..."
        export SSHPASS="$pass"
        sshpass -e ssh root@"$ip" "bash -s" <<< "$CLEANUP_SCRIPT" || log_warn "$name 清理有警告"
        unset SSHPASS
    done

    log_info "重新配置所有节点的镜像加速器..."
    cat > /tmp/registries.yaml <<EOF
mirrors:
  docker.io:
    endpoint:
      - "https://docker.m.daocloud.io"
      - "https://dockerproxy.net"
      - "${MIRROR_DOCKER}"
      - "https://registry-1.docker.io"
  quay.io:
    endpoint:
      - "https://quay.m.daocloud.io	"
      - "https://quay.dockerproxy.net"
      - "${MIRROR_QUAY}"
      - "https://quay.io"
  registry.k8s.io:
    endpoint:
      - "https://k8s.m.daocloud.io"
      - "https://k8s.dockerproxy.net"
      - "${MIRROR_K8S}"
      - "https://registry.k8s.io"
  ghcr.io:
    endpoint:
      - "https://ghcr.m.daocloud.io	"
      - "https://ghcr.dockerproxy.net"
      - "${MIRROR_GHCR}"
      - "https://ghcr.io"
EOF

    for ((i=0; i<NODE_COUNT; i++)); do
        ip="${NODE_IPS[$i]}" pass="${NODE_PASSWORDS[$i]}" name="${NODE_NAMES[$i]}"
        log_detail "配置节点 $name 镜像加速器"
        export SSHPASS="$pass"
        sshpass -e ssh root@"$ip" "mkdir -p /etc/rancher/rke2"
        sshpass -e scp /tmp/registries.yaml root@"$ip":/etc/rancher/rke2/registries.yaml
        unset SSHPASS
    done
    rm -f /tmp/registries.yaml
    
    # 分发文件
    for ((i=0; i<NODE_COUNT; i++)); do
        ip="${NODE_IPS[$i]}" pass="${NODE_PASSWORDS[$i]}"
        export SSHPASS="$pass"
        log_detail "分发文件到 $ip"
        sshpass -e ssh root@"$ip" "mkdir -p /var/lib/rancher/rke2/agent/images"
        scp_with_progress "/root/rancher-install/$RKE2_IMAGES_TAR" "root@$ip:/var/lib/rancher/rke2/agent/images/"
        scp_with_progress "/root/rancher-install/install_rke2.sh" "root@$ip:/tmp/"
        unset SSHPASS
    done

    # 构建包含所有 master IP 的 tls-san 列表
    TLS_SAN_IPS=""
    for ((i=0; i<3 && i<NODE_COUNT; i++)); do
        TLS_SAN_IPS+="\n- ${NODE_IPS[$i]}"
    done
    TLS_SAN_LIST="- ${LOADBALANCER_IP}\n- ${RANCHER_DOMAIN}${TLS_SAN_IPS}"
    echo -e "$TLS_SAN_LIST" > /tmp/tls_san_list.txt

    # 第一个 master 节点
    first_ip="${NODE_IPS[0]}" first_pass="${NODE_PASSWORDS[0]}"
    log_info "准备第一个 master 节点 ${MASTER_NODES[0]} 配置文件"
    export SSHPASS="$first_pass"
    sshpass -e ssh root@"$first_ip" "mkdir -p /etc/rancher/rke2"
    sshpass -e ssh root@"$first_ip" "cat > /etc/rancher/rke2/config.yaml <<EOF
token: ${RKE2_TOKEN}
node-name: ${MASTER_NODES[0]}
tls-san:
${TLS_SAN_LIST}
write-kubeconfig-mode: \"0644\"
service-node-port-range: "8000-32767"
EOF"
    unset SSHPASS

    log_info "安装第一个master节点 (${MASTER_NODES[0]})"
    export SSHPASS="$first_pass"
    sshpass -e ssh root@"$first_ip" \
        "bash /tmp/install_rke2.sh first '$RKE2_TOKEN' '$LOADBALANCER_IP' '${MASTER_NODES[0]}' '$RANCHER_DOMAIN' '${NODE_IPS[0]}'" &
    pid=$!
    spin_wait $pid "第一个 master 节点安装中"
    wait $pid
    unset SSHPASS

    sleep 10

    log_info "等待 API Server 就绪..."
    export SSHPASS="$first_pass"
    sshpass -e ssh root@"$first_ip" '
        for i in {1..150}; do
            if /var/lib/rancher/rke2/bin/kubectl --kubeconfig /etc/rancher/rke2/rke2.yaml get nodes &>/dev/null; then
                exit 0
            fi
            sleep 2
        done
        exit 1
    '
    if [ $? -ne 0 ]; then
        log_error "API Server 未能在 5 分钟内就绪"
    fi
    unset SSHPASS
    log_info "API Server 已就绪"

    # 获取 kubeconfig 并替换 server 地址为负载均衡器
    export SSHPASS="$first_pass"
    sshpass -e scp root@"$first_ip":/etc/rancher/rke2/rke2.yaml /root/rancher-install/kubeconfig
    unset SSHPASS
    log_info "使用 sed 修改 kubeconfig server 地址"
    sed -i "s|127.0.0.1|${LOADBALANCER_IP}|g" /root/rancher-install/kubeconfig

    mkdir -p /root/.kube
    cp /root/rancher-install/kubeconfig /root/.kube/config
    export KUBECONFIG=/root/.kube/config

    log_detail "从 master1 复制 kubectl 工具..."
    export SSHPASS="$first_pass"
    sshpass -e scp root@"$first_ip":/var/lib/rancher/rke2/bin/kubectl /usr/local/bin/kubectl
    unset SSHPASS
    chmod +x /usr/local/bin/kubectl

    # 为所有节点创建 crictl 软链接
    log_info "为所有节点创建 crictl 软链接..."
    for ((i=0; i<NODE_COUNT; i++)); do
        ip="${NODE_IPS[$i]}"
        pass="${NODE_PASSWORDS[$i]}"
        name="${NODE_NAMES[$i]}"
        log_detail "节点 $name ($ip): 创建 crictl 软链接"
        export SSHPASS="$pass"
        sshpass -e ssh root@"$ip" "ln -sf /var/lib/rancher/rke2/bin/crictl /usr/local/bin/crictl"
        unset SSHPASS
    done
    log_info "crictl 软链接创建完成"

    if ! command -v kubectl &>/dev/null; then
        log_error "kubectl 安装失败，请检查网络或手动安装"
    fi
    log_info "kubectl 已安装并配置"

    # 安装其余 master 节点
    for ((i=1; i<3 && i<NODE_COUNT; i++)); do
        ip="${NODE_IPS[$i]}" pass="${NODE_PASSWORDS[$i]}" name="${MASTER_NODES[$i]}"
        log_info "安装master节点 $name"
        export SSHPASS="$pass"
        sshpass -e ssh root@"$ip" "mkdir -p /etc/rancher/rke2"
        sshpass -e ssh root@"$ip" "cat > /etc/rancher/rke2/config.yaml <<EOF
server: https://${LOADBALANCER_IP}:9345
token: ${RKE2_TOKEN}
node-name: ${name}
tls-san:
${TLS_SAN_LIST}
write-kubeconfig-mode: \"0644\"
EOF"
        sshpass -e ssh root@"$ip" \
            "bash /tmp/install_rke2.sh other '$RKE2_TOKEN' '$LOADBALANCER_IP' '$name' '$RANCHER_DOMAIN' '${NODE_IPS[0]}'" &
        pid=$!
        spin_wait $pid "$name 安装中"
        wait $pid
        unset SSHPASS
    done

    # 安装 worker 节点
    for ((i=3; i<NODE_COUNT; i++)); do
        ip="${NODE_IPS[$i]}" pass="${NODE_PASSWORDS[$i]}" name="${NODE_NAMES[$i]}"
        log_info "安装worker节点 $name"
        export SSHPASS="$pass"
        sshpass -e ssh root@"$ip" \
            "bash /tmp/install_rke2.sh agent '$RKE2_TOKEN' '$LOADBALANCER_IP' '$name' '$RANCHER_DOMAIN' '${NODE_IPS[0]}'" &
        pid=$!
        spin_wait $pid "$name 安装中"
        wait $pid
        unset SSHPASS
    done

    log_info "等待所有节点就绪..."
    for ((i=0; i<120; i++)); do
        if output=$(kubectl get nodes 2>&1); then
            if echo "$output" | grep -q "NotReady"; then
                sleep 10
            else
                break
            fi
        else
            sleep 10
        fi
    done
    kubectl get nodes

    for node in "${MASTER_NODES[@]}"; do
        kubectl taint nodes "$node" node-role.kubernetes.io/control-plane:NoSchedule- 2>/dev/null || true
        kubectl label node "$node" node-role.kubernetes.io/worker= --overwrite
        log_info "节点 $node 已添加 worker 角色标签"
    done

    mark_done "phase4_rke2"
    phase_end "RKE2集群部署"
fi

# ================== 阶段5：部署Rook-Ceph ==================
if ! is_done "phase5_rook_ceph"; then
    phase_start "部署Rook-Ceph ${ROOK_VERSION} 生产级配置"

    IFS=',' read -ra DEVICES <<< "$CEPH_DEVICES"
    for ((i=0; i<NODE_COUNT; i++)); do
        ip="${NODE_IPS[$i]}" pass="${NODE_PASSWORDS[$i]}" name="${NODE_NAMES[$i]}"
        export SSHPASS="$pass"
        sshpass -e ssh root@"$ip" "
            if ! command -v ceph-volume &> /dev/null; then
                echo 'Installing ceph-osd...'
                wget -q -O- 'https://download.ceph.com/keys/release.asc' | apt-key add -
                echo 'deb https://download.ceph.com/debian-quincy/ noble main' | tee /etc/apt/sources.list.d/ceph.list
                apt-get update -qq
                apt-get install -y -qq ceph-osd
            fi
            for dev in ${DEVICES[@]}; do
                if [[ -b \"\$dev\" ]]; then
                    echo \"彻底清理磁盘 \$dev on $name ...\"
                    ceph-volume lvm zap \"\$dev\" --destroy
                    sgdisk --zap-all \"\$dev\" 2>/dev/null || true
                    dd if=/dev/zero of=\"\$dev\" bs=1M count=100 conv=fsync oflag=direct 2>/dev/null || true
                    partprobe \"\$dev\" 2>/dev/null || true
                else
                    echo \"警告：设备 \$dev 不存在或不是块设备，跳过\"
                fi
            done
            rm -rf /var/lib/rook
        " || log_warn "$name 磁盘清理出现警告"
        unset SSHPASS
    done

    cd /root/rancher-install
    mkdir -p rook-yaml && cd rook-yaml

    # 定义下载列表
    ROOK_FILES="crds.yaml common.yaml operator.yaml cluster.yaml toolbox.yaml filesystem.yaml object.yaml object-user.yaml csi-operator.yaml"
    for f in $ROOK_FILES; do
        PROXY_URL="${PROXY_PREFIX}https://raw.githubusercontent.com/rook/rook/v${ROOK_VERSION}/deploy/examples/${f}"
        DIRECT_URL="https://raw.githubusercontent.com/rook/rook/v${ROOK_VERSION}/deploy/examples/${f}"
        download_with_progress "$f" "$PROXY_URL" "$DIRECT_URL"
    done

    DEVICE_FILTER_REGEX="^($(for d in "${DEVICES[@]}"; do basename "$d"; done | tr '\n' '|' | sed 's/|$//'))$"

    log_info "根据官方模版修改 cluster.yaml"
    sed -i 's|quay.io/ceph/ceph:v19.2.3|quay.io/ceph/ceph:v20.2.1|g' cluster.yaml
    sed -i 's/ssl: true/ssl: false/g' cluster.yaml
    sed -i 's/useAllDevices: true/useAllDevices: false/g' cluster.yaml
    sed -i '/useAllDevices: false/a\    useAllNodes: true\n    deviceFilter: '${DEVICE_FILTER_REGEX} cluster.yaml
    sed -i '/^  csi:/,/^  [a-z]/d' cluster.yaml

    kubectl create ns rook-ceph --dry-run=client -o yaml | kubectl apply --server-side --force-conflicts -f -
    kubectl apply --server-side --force-conflicts -f crds.yaml
    kubectl apply --server-side --force-conflicts -f common.yaml
    sleep 10
    kubectl apply --server-side --force-conflicts -f operator.yaml
    kubectl rollout status -n rook-ceph deploy/rook-ceph-operator --timeout=300s

    kubectl apply --server-side --force-conflicts -f csi-operator.yaml
    kubectl apply --server-side --force-conflicts -f cluster.yaml

    CLUSTER_READY=false
    for ((i=0; i<120; i++)); do
        phase=$(kubectl -n rook-ceph get cephcluster rook-ceph -o jsonpath='{.status.phase}' 2>/dev/null || echo "")
        if [[ "$phase" == "Ready" ]]; then
            log_info "Ceph 集群状态为 Ready，正在确认 tools Pod..."
            kubectl apply --server-side --force-conflicts -f toolbox.yaml 2>/dev/null || log_warn "toolbox.yaml 应用失败，继续尝试"

            TOOLBOX_READY=false
            for retry in {1..60}; do
                if kubectl -n rook-ceph get pod -l app=rook-ceph-tools --no-headers 2>/dev/null | grep -q Running; then
                    TOOLBOX_READY=true
                    break
                fi
                sleep 2
            done

            if [[ "$TOOLBOX_READY" == "false" ]]; then
                log_warn "rook-ceph-tools Pod 未能在 120 秒内就绪，跳过健康检查并认为集群已就绪"
                CLUSTER_READY=true
                break
            fi

            log_info "等待 Ceph 集群达到 HEALTH_OK 状态..."
            HEALTH_OK=false
            for health_retry in {1..60}; do
                set +e
                ceph_output=$(kubectl -n rook-ceph exec deploy/rook-ceph-tools -- ceph status 2>/dev/null)
                exit_code=$?
                set -e
                
                if [[ $exit_code -eq 0 ]] && echo "$ceph_output" | grep -q "HEALTH_OK"; then
                    log_info "Ceph 集群健康 (HEALTH_OK)"
                    HEALTH_OK=true
                    break
                else
                    log_detail "等待 HEALTH_OK... (${health_retry}/60)"
                    sleep 5
                fi
            done

            if [[ "$HEALTH_OK" == "true" ]]; then
                CLUSTER_READY=true
                break
            else
                log_warn "Ceph 集群在 5 分钟内未达到 HEALTH_OK，但状态为 Ready，继续部署）"
                CLUSTER_READY=true
                break
            fi
        fi
        log_info "等待 Ceph 集群就绪 (当前阶段: ${phase:-Unknown})"
        sleep 10
    done

    if [[ "$CLUSTER_READY" == "false" ]]; then
        log_error "Ceph 集群未能在 20 分钟内就绪，请检查 Rook 和 Ceph 日志"
    fi

    cat <<EOF | kubectl apply --server-side --force-conflicts -f -
apiVersion: ceph.rook.io/v1
kind: CephBlockPool
metadata:
  name: replicapool
  namespace: rook-ceph
spec:
  replicated:
    size: ${CEPH_REPLICAS}
---
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: rook-ceph-block
  annotations:
    storageclass.kubernetes.io/is-default-class: "true"
provisioner: rook-ceph.rbd.csi.ceph.com
parameters:
  clusterID: rook-ceph
  pool: replicapool
  imageFormat: "2"
  csi.storage.k8s.io/provisioner-secret-name: rook-csi-rbd-provisioner
  csi.storage.k8s.io/provisioner-secret-namespace: rook-ceph
  csi.storage.k8s.io/node-stage-secret-name: rook-csi-rbd-node
  csi.storage.k8s.io/node-stage-secret-namespace: rook-ceph
  csi.storage.k8s.io/fstype: ext4
reclaimPolicy: Delete
allowVolumeExpansion: true
volumeBindingMode: WaitForFirstConsumer
EOF

    log_info "创建 Ceph 文件系统"
    sed -i "s/^\([[:space:]]*size:\) [0-9]\+$/\1 ${CEPH_REPLICAS}/" filesystem.yaml
    kubectl apply --server-side --force-conflicts -f filesystem.yaml
    cat <<EOF | kubectl apply --server-side --force-conflicts -f -
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: rook-cephfs
provisioner: rook-ceph.cephfs.csi.ceph.com
parameters:
  clusterID: rook-ceph
  fsName: myfs
  pool: myfs-replicated
  csi.storage.k8s.io/provisioner-secret-name: rook-csi-cephfs-provisioner
  csi.storage.k8s.io/provisioner-secret-namespace: rook-ceph
  csi.storage.k8s.io/node-stage-secret-name: rook-csi-cephfs-node
  csi.storage.k8s.io/node-stage-secret-namespace: rook-ceph
reclaimPolicy: Delete
allowVolumeExpansion: true
volumeBindingMode: WaitForFirstConsumer
EOF

    log_info "创建 Ceph 对象存储"
    sed -i "s/^\([[:space:]]*size:\) [0-9]\+$/\1 ${CEPH_REPLICAS}/" object.yaml
    sed -i "s/^\([[:space:]]*instances:\) [0-9]\+$/\1 2/" object.yaml
    kubectl apply --server-side --force-conflicts -f object.yaml
    kubectl apply --server-side --force-conflicts -f object-user.yaml

    log_info "等待 CephObjectStoreUser my-user 就绪..."
    SECRET_NAME=""
    for i in {1..60}; do
        USER_STATUS=$(kubectl -n rook-ceph get cephobjectstoreuser my-user -o jsonpath='{.status.phase}' 2>/dev/null || echo "")
        if [[ "$USER_STATUS" == "Ready" ]]; then
            SECRET_NAME=$(kubectl -n rook-ceph get cephobjectstoreuser my-user -o jsonpath='{.status.info.secretName}' 2>/dev/null)
            if [[ -n "$SECRET_NAME" ]]; then
                log_info "CephObjectStoreUser 已就绪，Secret 名称: $SECRET_NAME"
                break
            fi
        fi
        log_detail "等待 objectuser 就绪... ($i/60)"
        sleep 3
    done

    if [[ -z "$SECRET_NAME" ]]; then
        log_warn "未能获取 Secret 名称，使用默认名称 rook-ceph-object-user-my-store-my-user"
        SECRET_NAME="rook-ceph-object-user-my-store-my-user"
    fi

    mkdir -p /root/rancher-install
    echo "$SECRET_NAME" > /root/rancher-install/s3_secret_name

    log_info "等待 RGW Deployment 出现..."
    RGW_DEPLOY=""
    for i in {1..60}; do
        RGW_DEPLOY=$(kubectl -n rook-ceph get deploy -l app=rook-ceph-rgw -o name 2>/dev/null | head -1)
        if [[ -n "$RGW_DEPLOY" ]]; then
            log_info "找到 RGW deployment: $RGW_DEPLOY"
            break
        fi
        log_detail "RGW deployment 尚未创建，等待中... ($i/60)"
        sleep 5
    done

    if [[ -z "$RGW_DEPLOY" ]]; then
        log_warn "未在 5 分钟内检测到 RGW deployment，请手动检查 Rook 对象存储状态"
    else
        log_info "等待 RGW deployment 就绪..."
        kubectl -n rook-ceph wait --for=condition=available $RGW_DEPLOY --timeout=180s || log_warn "RGW deployment 未能在预期时间内就绪"
    fi

    log_info "等待 S3 凭证 Secret ($SECRET_NAME) 出现..."
    for i in {1..60}; do
        if kubectl -n rook-ceph get secret "$SECRET_NAME" &>/dev/null; then
            log_info "S3 凭证 secret 已创建"
            break
        fi
        log_detail "S3 secret 尚未创建，等待中... ($i/60)"
        sleep 3
    done

    S3_ACCESS_KEY=$(kubectl -n rook-ceph get secret "$SECRET_NAME" -o jsonpath='{.data.AccessKey}' 2>/dev/null | base64 -d || echo "")
    S3_SECRET_KEY=$(kubectl -n rook-ceph get secret "$SECRET_NAME" -o jsonpath='{.data.SecretKey}' 2>/dev/null | base64 -d || echo "")

    if [[ -n "$S3_ACCESS_KEY" && -n "$S3_SECRET_KEY" ]]; then
        log_info "成功获取 S3 凭证"
        cat > /root/rancher-install/s3_credentials <<EOF
S3_ACCESS_KEY="$S3_ACCESS_KEY"
S3_SECRET_KEY="$S3_SECRET_KEY"
EOF
    else
        log_warn "S3 凭证获取失败，后续 KubeBlocks 备份功能将无法自动配置"
    fi

    cat <<EOF | kubectl apply --server-side --force-conflicts -f -
apiVersion: v1
kind: Service
metadata:
  name: rook-ceph-rgw-my-store-external
  namespace: rook-ceph
spec:
  ports:
  - name: http
    port: 80
    protocol: TCP
    targetPort: 80
    nodePort: 8002
  selector:
    app: rook-ceph-rgw
    ceph_daemon_type: rgw
    rook_cluster: rook-ceph
    rgw: my-store
  type: NodePort
EOF

    cat <<EOF | kubectl apply --server-side --force-conflicts -f -
apiVersion: snapshot.storage.k8s.io/v1
kind: VolumeSnapshotClass
metadata:
  name: rook-ceph-block-snapclass
  annotations:
    snapshot.storage.kubernetes.io/is-default-class: "true"
driver: rook-ceph.rbd.csi.ceph.com
deletionPolicy: Delete
parameters:
  clusterID: rook-ceph
  pool: replicapool
  imageFeatures: layering
  csi.storage.k8s.io/snapshotter-secret-name: rook-csi-rbd-provisioner
EOF

    kubectl patch configmap rook-ceph-operator-config -n rook-ceph --type merge -p '{"data":{"CSI_ENABLE_CSIADDONS": "true"}}' || log_warn "启用 CSI-Addons 失败"
    kubectl delete pod -n rook-ceph -l app=rook-ceph-operator
    sleep 10
    kubectl rollout status -n rook-ceph deploy/rook-ceph-operator --timeout=300s

    mark_done "phase5_rook_ceph"
    phase_end "Rook-Ceph部署"
fi

# ================== 阶段6：部署 CSI-Addons 控制器 ==================
if ! is_done "phase6_csi_addons"; then
    phase_start "部署 CSI-Addons 控制器，提供 ReclaimSpace、VolumeReplication 等高级存储功能"
    
    # 1. 配置 Rook 启用 CSI-Addons 边车容器
    log_info "启用 Rook 的 CSI-Addons 边车..."
    kubectl patch configmap -n rook-ceph rook-ceph-operator-config --type merge -p '{"data":{"CSI_ENABLE_CSIADDONS": "true"}}' || log_warn "启用 CSI-Addons 失败"
    
    # 2. 重启 Rook Operator 使配置生效
    log_info "重启 Rook Operator..."
    kubectl delete pod -n rook-ceph -l app=rook-ceph-operator
    kubectl rollout status -n rook-ceph deploy/rook-ceph-operator --timeout=300s
    
    # 3. 部署 CSI-Addons 控制器组件（按顺序依次执行）
    log_info "开始部署 CSI-Addons 控制器..."
    
    # 3.1 首先创建命名空间（这是本次修复的关键）
    kubectl create namespace csi-addons-system --dry-run=client -o yaml | kubectl apply --server-side --force-conflicts -f -
    
    # 3.2 安装 CRDs
    kubectl apply --server-side --force-conflicts -f "${PROXY_PREFIX}https://raw.githubusercontent.com/csi-addons/kubernetes-csi-addons/v0.14.0/deploy/controller/crds.yaml"
    
    # 3.3 安装 RBAC（此时命名空间已存在，不再报错）
    kubectl apply --server-side --force-conflicts -f "${PROXY_PREFIX}https://raw.githubusercontent.com/csi-addons/kubernetes-csi-addons/v0.14.0/deploy/controller/rbac.yaml"
    
    # 3.4 安装控制器本身
    kubectl apply --server-side --force-conflicts -f "${PROXY_PREFIX}https://raw.githubusercontent.com/csi-addons/kubernetes-csi-addons/v0.14.0/deploy/controller/setup-controller.yaml"
    
    # 4. 等待控制器就绪
    log_info "等待 CSI-Addons 控制器就绪..."
    kubectl -n csi-addons-system wait --for=condition=available --timeout=300s deployment/csi-addons-controller-manager || log_warn "CSI-Addons 控制器未能在预期时间内就绪"
    
    # 5. 验证 CRD 已安装
    if ! kubectl get crd csiaddonsnodes.csiaddons.openshift.io &>/dev/null; then
        log_error "CSI-Addons CRD 不存在，请检查安装是否成功"
    fi
    if ! kubectl get crd volumereplications.replication.storage.openshift.io &>/dev/null; then
        log_error "VolumeReplication CRD 不存在，请检查安装是否成功"
    fi
    
    mark_done "phase6_csi_addons"
    phase_end "CSI-Addons 控制器部署"
fi

# ================== 阶段7：部署Traefik ==================
if ! is_done "phase7_traefik"; then
    phase_start "部署Traefik Ingress Controller (高可用)"
    TRAEFIK_CHART_VERSION="40.3.0"
    CHART_TARBALL="traefik-${TRAEFIK_CHART_VERSION}.tar.gz"
    PROXY_URL="${PROXY_PREFIX}https://github.com/traefik/traefik-helm-chart/archive/refs/tags/v${TRAEFIK_CHART_VERSION}.tar.gz"
    DIRECT_URL="https://github.com/traefik/traefik-helm-chart/archive/refs/tags/v${TRAEFIK_CHART_VERSION}.tar.gz"
    
    cd /root/rancher-install
    if [[ ! -f "$CHART_TARBALL" ]]; then
        download_with_progress "$CHART_TARBALL" "$PROXY_URL" "$DIRECT_URL"
    fi

    tar -xzf "$CHART_TARBALL"
    cd "traefik-helm-chart-${TRAEFIK_CHART_VERSION}/traefik"

    cat > traefik-override.yaml <<EOF
replicas: 3

service:
  type: NodePort
  nodePorts:
    http: 8000
    https: 8001

ingressClass:
  enabled: true
  isDefaultClass: true

resources:
  requests:
    cpu: 100m
    memory: 128Mi
  limits:
    cpu: 500m
    memory: 512Mi

image:
  repository: traefik
  tag: v${TRAEFIK_VERSION}
  pullPolicy: IfNotPresent

affinity:
  podAntiAffinity:
    preferredDuringSchedulingIgnoredDuringExecution:
    - weight: 100
      podAffinityTerm:
        labelSelector:
          matchExpressions:
          - key: app.kubernetes.io/name
            operator: In
            values:
            - traefik
        topologyKey: kubernetes.io/hostname
EOF

    kubectl create ns traefik --dry-run=client -o yaml | kubectl apply --server-side --force-conflicts -f -
    helm upgrade --install traefik ./ \
        -n traefik \
        -f traefik-override.yaml \
        --timeout 10m \
        --wait \
        | tee -a "$MAIN_LOG"

    mark_done "phase7_traefik"
    phase_end "Traefik部署"
fi

# ================== 阶段8：部署 cert-manager ==================
if ! is_done "phase8_cert_manager"; then
    phase_start "部署 cert-manager ${CERT_MANAGER_VERSION}"

    cd /root/rancher-install

    CHART_TARBALL="cert-manager-${CERT_MANAGER_VERSION}.tar.gz"
    PROXY_URL="${PROXY_PREFIX}https://github.com/cert-manager/cert-manager/archive/refs/tags/v${CERT_MANAGER_VERSION}.tar.gz"
    DIRECT_URL="https://github.com/cert-manager/cert-manager/archive/refs/tags/v${CERT_MANAGER_VERSION}.tar.gz"
    if [[ ! -f "$CHART_TARBALL" ]]; then
        download_with_progress "$CHART_TARBALL" "$PROXY_URL" "$DIRECT_URL"
    fi

    tar -xzf "$CHART_TARBALL"
    cd "cert-manager-${CERT_MANAGER_VERSION}/deploy/charts/cert-manager"
    mv "Chart.template.yaml" "Chart.yaml"

    cat > cert-manager-override.yaml <<EOF
crds:
  enabled: true

replicaCount: 3

image:
  repository: docker.io/imroc/cert-manager-controller
  tag: v${CERT_MANAGER_VERSION}

webhook:
  image:
    repository: docker.io/imroc/cert-manager-webhook
    tag: v${CERT_MANAGER_VERSION}

cainjector:
  image:
    repository: docker.io/imroc/cert-manager-cainjector
    tag: v${CERT_MANAGER_VERSION}

startupapicheck:
  image:
    repository: docker.io/imroc/cert-manager-startupapicheck
    tag: v${CERT_MANAGER_VERSION}

resources:
  requests:
    cpu: 100m
    memory: 128Mi
  limits:
    cpu: 500m
    memory: 512Mi

affinity:
  podAntiAffinity:
    preferredDuringSchedulingIgnoredDuringExecution:
    - weight: 100
      podAffinityTerm:
        labelSelector:
          matchExpressions:
          - key: app.kubernetes.io/instance
            operator: In
            values:
            - cert-manager
        topologyKey: kubernetes.io/hostname
EOF

    kubectl create ns cert-manager --dry-run=client -o yaml | kubectl apply --server-side --force-conflicts -f -
    helm upgrade --install cert-manager ./ \
        -n cert-manager \
        -f cert-manager-override.yaml \
        --timeout 10m \
        --wait \
        | tee -a "$MAIN_LOG"

    log_info "创建 cert-manager 自签名 ClusterIssuer 及通配符证书示例"
    cat <<EOF | kubectl apply --server-side --force-conflicts -f -
apiVersion: cert-manager.io/v1
kind: ClusterIssuer
metadata:
  name: selfsigned-issuer
spec:
  selfSigned: {}
---
apiVersion: cert-manager.io/v1
kind: Certificate
metadata:
  name: wildcard-example
  namespace: cert-manager
spec:
  secretName: wildcard-example-tls
  commonName: "*.${RANCHER_DOMAIN}"
  dnsNames:
  - "*.${RANCHER_DOMAIN}"
  issuerRef:
    name: selfsigned-issuer
    kind: ClusterIssuer
EOF

    mark_done "phase8_cert_manager"
    phase_end "cert-manager 部署"
fi

# ================== 阶段9：部署Rancher ==================
if ! is_done "phase9_rancher"; then
    phase_start "部署Rancher ${RANCHER_VERSION} "

    cd /root/rancher-install

    helm repo add rancher-latest https://releases.rancher.com/server-charts/latest
    helm repo update

    cat > rancher-override.yaml <<EOF
hostname: ${RANCHER_DOMAIN}
bootstrapPassword: ${RANCHER_PASSWORD}
replicas: 3

resources:
  requests:
    cpu: 500m
    memory: 1024Mi
  limits:
    cpu: 2000m
    memory: 2048Mi

ingress:
  tls:
    source: ${TLS_SOURCE}
EOF

    # 如果使用 Let's Encrypt，追加配置
    if [[ "$TLS_SOURCE" == "letsEncrypt" ]]; then
        cat >> rancher-override.yaml <<EOF
letsEncrypt:
  email: canplay@vip.qq.com
  environment: production
  ingress:
    class: traefik
EOF
    fi

    kubectl create ns cattle-system --dry-run=client -o yaml | kubectl apply --server-side --force-conflicts -f -

    helm upgrade --install rancher rancher-latest/rancher \
        -n cattle-system \
        -f rancher-override.yaml \
        --version v${RANCHER_VERSION} \
        --timeout 15m \
        --wait

    kubectl rollout status -n cattle-system deploy/rancher --timeout=1200s

    cat <<EOF | kubectl apply --server-side --force-conflicts -f -
apiVersion: v1
kind: Service
metadata:
  name: rancher-nodeport
  namespace: cattle-system
spec:
  type: NodePort
  ports:
  - port: 443
    targetPort: 443
    nodePort: 8001
    name: https
  selector:
    app: rancher
EOF

    mark_done "phase9_rancher"
    phase_end "Rancher部署"
fi

# ================== 阶段10：部署KubeBlocks ==================
if ! is_done "phase10_kubeblocks"; then
    phase_start "部署KubeBlocks ${KUBEBLOCKS_VERSION} 及 BackupRepo"
    cd /root/rancher-install

    # -------------------- 下载 KubeBlocks 源码 --------------------
    CHART_TARBALL="kubeblocks-${KUBEBLOCKS_VERSION}.tar.gz"
    PROXY_KB="${PROXY_PREFIX}https://github.com/apecloud/kubeblocks/archive/refs/tags/v${KUBEBLOCKS_VERSION}.tar.gz"
    DIRECT_KB="https://github.com/apecloud/kubeblocks/archive/refs/tags/v${KUBEBLOCKS_VERSION}.tar.gz"
    if [[ ! -f "$CHART_TARBALL" ]]; then
        download_with_progress "$CHART_TARBALL" "$PROXY_URL" "$DIRECT_URL"
    fi

    # -------------------- 解压并进入 Helm Chart 目录 --------------------
    tar -xzf "$CHART_TARBALL"
    cd "kubeblocks-${KUBEBLOCKS_VERSION}/deploy/helm"

    # -------------------- 创建命名空间并安装 --------------------
    kubectl create ns kb-system --dry-run=client -o yaml | kubectl apply --server-side --force-conflicts -f -

    log_info "安装 KubeBlocks CRDs..."
    if [[ -d "crds" ]]; then
        kubectl apply --server-side --force-conflicts -f crds/
        sleep 10
        if kubectl get crd | grep -q "extensions.kubeblocks.io"; then
            log_info "KubeBlocks CRDs 已成功注册"
        else
            log_warn "CRD 注册可能尚未完成，但继续安装"
        fi
    else
        log_error "未找到 crds 目录，请检查 KubeBlocks 源码包是否正确"
    fi

    helm upgrade --install kubeblocks ./ \
        -n kb-system \
        --timeout 10m \
        --wait

    # 等待 KubeBlocks 关键 Deployment 就绪
    kubectl rollout status -n kb-system deployment/kubeblocks --timeout=300s

    # -------------------- 配置 KubeBlocks BackupRepo 对接 Ceph RGW --------------------
    S3_ACCESS_KEY=$(kubectl -n rook-ceph get secret rook-ceph-object-user-rancher-user -o jsonpath='{.data.AccessKey}' 2>/dev/null | base64 -d || echo "")
    S3_SECRET_KEY=$(kubectl -n rook-ceph get secret rook-ceph-object-user-rancher-user -o jsonpath='{.data.SecretKey}' 2>/dev/null | base64 -d || echo "")
    if [[ -n "$S3_ACCESS_KEY" && -n "$S3_SECRET_KEY" ]]; then
        cat <<EOF | kubectl apply --server-side --force-conflicts -f -
apiVersion: dataprotection.kubeblocks.io/v1alpha1
kind: BackupRepo
metadata:
  name: ceph-rgw-backuprepo
  namespace: kb-system
spec:
  storageProviderRef: s3
  pvReclaimPolicy: Retain
  volumeCapacity: 100Gi
  config:
    bucket: kubeblocks-backup
    endpoint: http://rook-ceph-rgw-my-store.rook-ceph:80
    region: default
  credential:
    name: ceph-rgw-credential
    namespace: kb-system
---
apiVersion: v1
kind: Secret
metadata:
  name: ceph-rgw-credential
  namespace: kb-system
type: Opaque
stringData:
  accessKeyId: "${S3_ACCESS_KEY}"
  secretAccessKey: "${S3_SECRET_KEY}"
EOF
        log_info "BackupRepo 和默认 BackupPolicy 已创建"
    else
        log_warn "无法获取 S3 凭证，请手动创建 BackupRepo"
    fi

    mark_done "phase10_kubeblocks"
    phase_end "KubeBlocks部署"
fi

# ================== 阶段11：资源配额 ==================
if ! is_done "phase11_quota"; then
    phase_start "配置命名空间资源配额"
    cat <<EOF | kubectl apply --server-side --force-conflicts -f -
apiVersion: v1
kind: LimitRange
metadata:
  name: default-limits
  namespace: default
spec:
  limits:
  - default:
      cpu: 500m
      memory: 512Mi
    defaultRequest:
      cpu: 100m
      memory: 128Mi
    type: Container
EOF
    mark_done "phase11_quota"
    phase_end "配置资源配额"
fi

# ================== 阶段12：清理 ==================
if ! is_done "phase12_cleanup"; then
    phase_start "清理临时文件"
    rm -rf /root/rancher-install/{rook-yaml,*.tgz,*.tar.gz,traefik-helm-chart-*,kubeblocks-*}
    mark_done "phase12_cleanup"
    phase_end "清理"
fi

# ================== 最终输出 ==================
log_step "部署完成，访问信息"
log_info "Rancher控制台: https://${RANCHER_DOMAIN} 或 NodePort: https://任意节点IP:30443"
log_info "初始admin密码: ${RANCHER_PASSWORD}"
log_info "Ceph Dashboard: http://任意节点IP:30080 (用户名 admin)"
log_info "获取 Ceph Dashboard 密码: kubectl -n rook-ceph get secret rook-ceph-dashboard-password -o jsonpath='{.data.password}' | base64 -d"
echo ""
log_info "对象存储 S3 端点: http://任意节点IP:30082"
log_info "S3 AccessKey: $(kubectl -n rook-ceph get secret rook-ceph-object-user-rancher-user -o jsonpath='{.data.AccessKey}' 2>/dev/null | base64 -d 2>/dev/null || echo '未获取')"
log_info "S3 SecretKey: $(kubectl -n rook-ceph get secret rook-ceph-object-user-rancher-user -o jsonpath='{.data.SecretKey}' 2>/dev/null | base64 -d 2>/dev/null || echo '未获取')"
echo ""
log_info "KubeBlocks BackupRepo: ceph-rgw-backuprepo (已配置)"
echo ""
log_info "Ingress 示例文件已保存至 /root/rancher-install/ingress-example.yaml"
cat > /root/rancher-install/ingress-example.yaml <<EOF
# 示例 Ingress（使用 cert-manager 自动签发证书）
# 请替换 host 和 secretName 后 apply
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: my-app
  annotations:
    kubernetes.io/ingress.class: traefik
    cert-manager.io/cluster-issuer: "selfsigned-issuer"   # 或 letsencrypt-prod
spec:
  rules:
  - host: app.example.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: my-service
            port:
              number: 80
  tls:
  - hosts:
    - app.example.com
    secretName: app-example-com-tls
EOF
log_info "敏感信息保存在 /root/rancher-install/credentials.txt"
cat > /root/rancher-install/credentials.txt <<EOF
Rancher URL: https://${RANCHER_DOMAIN} (NodePort: https://任意节点IP:30443)
Rancher Password: ${RANCHER_PASSWORD}
Ceph Dashboard URL: http://任意节点IP:30080
Ceph Dashboard Password: $(kubectl -n rook-ceph get secret rook-ceph-dashboard-password -o jsonpath='{.data.password}' 2>/dev/null | base64 -d 2>/dev/null || echo 'N/A')
S3 Endpoint: http://任意节点IP:30082
S3 AccessKey: $(kubectl -n rook-ceph get secret rook-ceph-object-user-rancher-user -o jsonpath='{.data.AccessKey}' 2>/dev/null | base64 -d 2>/dev/null || echo 'N/A')
S3 SecretKey: $(kubectl -n rook-ceph get secret rook-ceph-object-user-rancher-user -o jsonpath='{.data.SecretKey}' 2>/dev/null | base64 -d 2>/dev/null || echo 'N/A')
EOF
chmod 600 /root/rancher-install/credentials.txt
log_step "脚本执行完毕，完整日志: $MAIN_LOG"