#!/bin/bash

# ===== delete k8s =====
kubectl drain --ignore-daemonsets --delete-emptydir-data --force $(kubectl get nodes -o name) 2>/dev/null || true
kubectl delete node --all --force --grace-period=0 2>/dev/null
kubeadm reset -f
systemctl list-units --type=service | grep -E 'kube|docker|containerd|etcd' | awk '{print $1}' | xargs -r -I{} sh -c 'systemctl stop {}; systemctl disable {}'yum remove -y kube*
yum remove -y kube* docker* containerd cri-tools etcd kubernetes-cni
yum autoremove -y
declare -a cleanup_dirs=(
    "$HOME/.kube" "/etc/kubernetes" "/var/lib/kube*" "/var/lib/etcd"
    "/var/lib/docker" "/var/lib/containerd" "/var/lib/cni"
    "/etc/cni" "/opt/cni" "/run/containerd" "/run/flannel"
    "/usr/libexec/kubernetes" "/usr/local/bin/kube*"
    "/var/log/containers" "/var/log/pods" "/var/run/calico"
)
find "${cleanup_dirs[@]}" -maxdepth 0 -exec rm -rf {} + 2>/dev/null
iptables -F && iptables -t nat -F && iptables -t mangle -F
ipvsadm --clear
modprobe -r ipip || true
sysctl -w net.bridge.bridge-nf-call-iptables=0 2>/dev/null
sysctl -w net.bridge.bridge-nf-call-ip6tables=0 2>/dev/null
journalctl --vacuum-size=10M --vacuum-time=1d
find /var/log -name 'kube*' -delete 2>/dev/null
du -h --max-depth=1 /var/lib/ /etc/ 2>/dev/null | grep -E 'kube|docker|containerd' || true
reboot
# ===== delete k8s =====

hostnamectl set-hostname master1 && reboot
hostnamectl set-hostname master2 && reboot
hostnamectl set-hostname master3 && reboot
hostnamectl set-hostname worker1 && reboot
hostnamectl set-hostname worker2 && reboot
hostnamectl set-hostname worker3 && reboot

rm -rf /etc/yum.repos.d/*.*
curl -o /etc/yum.repos.d/CentOS-Base.repo https://mirrors.aliyun.com/repo/Centos-vault-8.5.2111.repo
sed -i "s|mirrors.cloud.aliyuncs.com|mirrors.aliyun.com|g" /etc/yum.repos.d/CentOS-*
sed -i "s|releasever|releasever-stream|g" /etc/yum.repos.d/CentOS-*
rpm --import https://www.elrepo.org/RPM-GPG-KEY-elrepo.org
yum install -y https://www.elrepo.org/elrepo-release-8.el8.elrepo.noarch.rpm epel-release yum-utils
yum --enablerepo=elrepo-kernel install -y kernel-lt
grub2-set-default 0
yum-config-manager --enable epel
yum clean all && yum update -y && yum makecache

swapoff -a
sed -ri '/\bswap\b/s/^/#/' /etc/fstab

setenforce 0
sed -i "s|SELINUX=enforcing|SELINUX=permissive|g" /etc/selinux/config

/sbin/grubby --update-kernel=ALL --args='cgroup_enable=memory cgroup.memory=nokmem swapaccount=1'
grub2-set-default 0
echo "*                soft    nofile         1048576" >> /etc/security/limits.conf
echo "*                hard    nofile         1048576" >> /etc/security/limits.conf
echo "*                soft    memlock        unlimited" >> /etc/security/limits.conf
echo "*                hard    memlock        unlimited" >> /etc/security/limits.conf
echo "DefaultLimitNOFILE=1048576" >> /etc/systemd/system.conf

modprobe overlay && modprobe br_netfilter
cat > /etc/modules-load.d/k8s.conf <<EOF
overlay
br_netfilter
ip_vs
ip_vs_rr
ip_vs_wrr
ip_vs_sh
nf_conntrack
nf_nat
nf_nat_redirect
xt_REDIRECT
xt_owner
iptable_nat
iptable_mangle
iptable_filter
EOF
sysctl --system

cat > /etc/sysctl.d/99-k8s.conf <<EOF
net.ipv4.ip_forward = 1
net.bridge.bridge-nf-call-iptables = 1
net.bridge.bridge-nf-call-ip6tables = 1
fs.may_detach_mounts = 1
vm.swappiness = 0
vm.overcommit_memory = 1
vm.panic_on_oom = 0
fs.inotify.max_user_instances = 524288
fs.file-max = 2097152
net.core.somaxconn = 32768
net.ipv4.tcp_keepalive_time = 600
EOF
sysctl -p

modprobe rbd
cat > /etc/rc.sysinit << EOF
#!/bin/bash
for file in /etc/sysconfig/modules/*.modules
do
  [ -x \$file ] && \$file
done
EOF
cat > /etc/sysconfig/modules/rbd.modules << EOF
modprobe rbd
EOF
chmod 755 /etc/sysconfig/modules/rbd.modules
lsmod | grep rbd

systemctl start firewalld
firewall-cmd --permanent --add-port={6443,2379-2380,10250,10257,10259}/tcp
firewall-cmd --permanent --add-port={8472,30000-32767}/udp
firewall-cmd --reload

yum install gcc-toolset-11
source /opt/rh/gcc-toolset-11/enable

yum install -y make zlib-devel bzip2-devel openssl-devel ncurses-devel sqlite-devel readline-devel tk-devel libffi-devel unzip zip tar
# yum install -y gcc --allowerasing
curl -O https://www.python.org/ftp/python/3.12.4/Python-3.12.4.tgz
tar -xf Python-3.12.4.tgz
cd Python-3.12.4
./configure --enable-optimizations
make -j 8
make altinstall
python3.12 -m pip install --user ansible
cd ..
rm -rf Python-3.12.4 Python-3.12.4.tgz

yum install -y chrony wget lsof openssl zlib patch iptables iscsi-initiator-utils
yum install -y socat conntrack ipvsadm lvm2 xfsprogs nfs-utils net-tools fail2ban iptables-services ebtables ipset

systemctl enable chronyd
systemctl start chronyd
timedatectl set-ntp true
timedatectl set-timezone Asia/Shanghai
systemctl enable fail2ban
systemctl start fail2ban

yum install -y https://download-ib01.fedoraproject.org/pub/epel/8/Everything/x86_64/Packages/g/gperftools-libs-2.7-9.el8.x86_64.rpm
yum install -y https://download-ib01.fedoraproject.org/pub/epel/8/Everything/x86_64/Packages/l/liboath-2.6.2-3.el8.x86_64.rpm
yum install -y https://download-ib01.fedoraproject.org/pub/epel/8/Everything/x86_64/Packages/l/libunwind-1.3.1-3.el8.x86_64.rpm

cat >> /etc/ssh/sshd_config <<EOF
AllowTcpForwarding yes
EOF
ssh-keygen
ssh-copy-id root@192.168.21.102 -f && ssh-copy-id root@192.168.21.103 -f && ssh-copy-id root@192.168.21.104 -f && ssh-copy-id root@192.168.21.105 -f \
  && ssh-copy-id root@192.168.21.106 -f && ssh-copy-id root@192.168.21.107 -f
systemctl restart sshd

sed -i 's|#DNS=|DNS=222.172.200.68|' /etc/systemd/resolved.conf
service systemd-resolved restart
systemctl enable systemd-resolved

modprobe ip_tables
echo 'ip_tables' >> /etc/modules
systemctl enable iptables
systemctl start iptables
iptables -A INPUT -p tcp -j ACCEPT
iptables -A INPUT -p udp -j ACCEPT
iptables -F && service iptables save

reboot

# ===== 删除分区 =====
sgdisk --zap-all /dev/vdb
partprobe /dev/vdb
reboot
# ===== 删除分区 =====
# ===== 创建分区 =====
fdisk -l
parted /dev/vdb
mklabel gpt
mkpart primary 1 100%
align-check optimal 1
q
# ===== 创建分区 =====
# ===== 挂载磁盘 =====
mkfs -t xfs /dev/vdb1
mkdir /data
mount /dev/vdb1 /data
df -h
echo `blkid /dev/vdb1 | awk '{print $2}' | sed 's/\"//g'` /data xfs defaults 0 0 >> /etc/fstab
# ===== 挂载磁盘 =====

# ===== 1panel =====
curl -sSL https://resource.fit2cloud.com/1panel/package/quick_start.sh -o quick_start.sh && sh quick_start.sh
# ===== 1panel =====

# ===== helm =====
curl https://www.gh-proxy.com/https://raw.githubusercontent.com/helm/helm/main/scripts/get-helm-3 | bash
helm repo remove stable
helm repo add stable https://kubernetes.oss-cn-hangzhou.aliyuncs.com/charts
helm repo add azure http://mirror.azure.cn/kubernetes/charts
helm repo add bitnami https://charts.bitnami.com/bitnami
helm repo add jetstack https://charts.jetstack.io
helm repo update
rm -rf helm-v3.15.3-linux-amd64.tar.gz linux-amd64
# ===== helm =====

# ===== containerd =====
vi /etc/containerd/config.toml
    [plugins."io.containerd.grpc.v1.cri".registry]
      config_path = "/etc/containerd/certs.d"
      [plugins."io.containerd.grpc.v1.cri".registry.auths]
      [plugins."io.containerd.grpc.v1.cri".registry.configs]
      [plugins."io.containerd.grpc.v1.cri".registry.headers]
      [plugins."io.containerd.grpc.v1.cri".registry.mirrors]
scp /etc/containerd/config.toml root@192.168.21.103:/etc/containerd/config.toml && \
  scp /etc/containerd/config.toml root@192.168.21.104:/etc/containerd/config.toml && \
  scp /etc/containerd/config.toml root@192.168.21.105:/etc/containerd/config.toml && \
  scp /etc/containerd/config.toml root@192.168.21.106:/etc/containerd/config.toml && \
  scp /etc/containerd/config.toml root@192.168.21.107:/etc/containerd/config.toml
systemctl restart containerd.service

mkdir -p /etc/containerd/certs.d
mkdir -p /etc/containerd/certs.d/docker.io
tee /etc/containerd/certs.d/docker.io/hosts.toml << 'EOF'
server = "https://registry-1.docker.io"
[host."https://sgulj51za84gae.xuanyuan.run"]
  capabilities = ["pull", "resolve"]
[host."https://registry-1.docker.io"]
  capabilities = ["pull", "resolve"]
EOF
mkdir -p /etc/containerd/certs.d/registry.k8s.io
tee /etc/containerd/certs.d/registry.k8s.io/hosts.toml << 'EOF'
server = "https://registry.k8s.io"
[host."https://sgulj51za84gae-k8s.xuanyuan.run"]
  capabilities = ["pull", "resolve"]
[host."https://registry.k8s.io"]
  capabilities = ["pull", "resolve"]
EOF
mkdir -p /etc/containerd/certs.d/gcr.io
tee /etc/containerd/certs.d/gcr.io/hosts.toml << 'EOF'
server = "https://gcr.io"
[host."https://sgulj51za84gae-gcr.xuanyuan.run"]
  capabilities = ["pull", "resolve"]
[host."https://gcr.io"]
  capabilities = ["pull", "resolve"]
EOF
mkdir -p /etc/containerd/certs.d/ghcr.io
tee /etc/containerd/certs.d/ghcr.io/hosts.toml << 'EOF'
server = "https://ghcr.io"
[host."https://sgulj51za84gae-ghcr.xuanyuan.run"]
  capabilities = ["pull", "resolve"]
[host."https://ghcr.io"]
  capabilities = ["pull", "resolve"]
EOF
mkdir -p /etc/containerd/certs.d/k8s.gcr.io
tee /etc/containerd/certs.d/k8s.gcr.io/hosts.toml << 'EOF'
server = "https://k8s.gcr.io"
[host."https://sgulj51za84gae-k8s.xuanyuan.run"]
  capabilities = ["pull", "resolve"]
[host."https://k8s.gcr.io"]
  capabilities = ["pull", "resolve"]
EOF
mkdir -p /etc/containerd/certs.d/quay.io
tee /etc/containerd/certs.d/quay.io/hosts.toml << 'EOF'
server = "https://quay.io"
[host."https://sgulj51za84gae-quay.xuanyuan.run"]
  capabilities = ["pull", "resolve"]
[host."https://quay.io"]
  capabilities = ["pull", "resolve"]
EOF
mkdir -p /etc/containerd/certs.d/harbor.cm.example.com
tee /etc/containerd/certs.d/harbor.cm.example.com/hosts.toml << 'EOF'
server = "https://harbor.cm.example.com"
[host."https://harbor.cm.example.com"]
  capabilities = ["pull", "resolve", "push"]
EOF
scp -r /etc/containerd/certs.d root@192.168.21.103:/etc/containerd/certs.d && \
  scp -r /etc/containerd/certs.d root@192.168.21.104:/etc/containerd/certs.d && \
  scp -r /etc/containerd/certs.d root@192.168.21.105:/etc/containerd/certs.d && \
  scp -r /etc/containerd/certs.d root@192.168.21.106:/etc/containerd/certs.d && \
  scp -r /etc/containerd/certs.d root@192.168.21.107:/etc/containerd/certs.d
# ===== containerd =====

# ===== kubekey =====
export KKZONE=cn && curl -sfL https://get-kk.kubesphere.io | sh -
chmod +x kk
scp ./kk root@192.168.21.103:~/kk && scp ./kk root@192.168.21.104:~/kk && scp ./kk root@192.168.21.105:~/kk && scp ./kk root@192.168.21.106:~/kk \
  && scp ./kk root@192.168.21.107:~/kk
vi k8s_kubekey_config.yaml
scp ./k8s_kubekey_config.yaml root@192.168.21.103:~/k8s_kubekey_config.yaml && \
  scp ./k8s_kubekey_config.yaml root@192.168.21.104:~/k8s_kubekey_config.yaml && \
  scp ./k8s_kubekey_config.yaml root@192.168.21.105:~/k8s_kubekey_config.yaml && \
  scp ./k8s_kubekey_config.yaml root@192.168.21.106:~/k8s_kubekey_config.yaml && \
  scp ./k8s_kubekey_config.yaml root@192.168.21.107:~/k8s_kubekey_config.yaml
export KKZONE=cn && ./kk create cluster -f k8s_kubekey_config.yaml
# admin/P@88w0rd
helm install cert-manager jetstack/cert-manager -n cert-manager --create-namespace --set prometheus.enabled=false
helm upgrade --install -n kubesphere-system --create-namespace ks-core https://charts.kubesphere.io/main/ks-core-1.1.3.tgz --debug --wait \
  --set global.imageRegistry=swr.cn-southwest-2.myhuaweicloud.com/ks --set extension.imageRegistry=swr.cn-southwest-2.myhuaweicloud.com/ks --set ingress.enabled=true \
  --set portal.hostname=ks.example.com --set ingress.tls.source=letsEncrypt --set letsEncrypt.email=canplay@vip.qq.com --set ingress.ingressClassName=nginx
# ===== kubekey =====

# ===== delete kubekey =====
helm -n kubesphere-system uninstall ks-core
./kk delete cluster -f k8s_kubekey_config.yaml
export KKZONE=cn && ./kk add nodes -f k8s_kubekey_config.yaml
export KKZONE=cn && ./kk delete node worker2 -f k8s_kubekey_config.yaml
# ===== delete kubekey =====

# ===== rook-ceph =====
wget https://www.gh-proxy.com/https://github.com/kubernetes-csi/external-snapshotter/archive/refs/tags/v8.2.0.tar.gz
tar -xf v8.2.0.tar.gz
cd external-snapshotter-8.2.0
kubectl kustomize client/config/crd | kubectl create -f -
sed -i "s|registry.k8s.io/sig-storage|sgulj51za84gae-k8s.xuanyuan.run/sig-storage|g" \
  deploy/kubernetes/snapshot-controller/setup-snapshot-controller.yaml
kubectl -n kube-system kustomize deploy/kubernetes/snapshot-controller | kubectl -n kube-system create -f -
sed -i "s|registry.k8s.io/sig-storage|sgulj51za84gae-k8s.xuanyuan.run/sig-storage|g" \
  deploy/kubernetes/csi-snapshotter/setup-csi-snapshotter.yaml
kubectl -n kube-system create kustomize deploy/kubernetes/csi-snapshotter | kubectl -n kube-system create -f -

kubectl label nodes {master1,master2,master3,worker1,worker2,worker3} ceph-osd=enabled
kubectl label nodes {master1,master2,master3} ceph-mon=enabled
kubectl label nodes {master1,master2,master3} ceph-mgr=enabled
wget https://www.gh-proxy.com/https://github.com/rook/rook/archive/refs/tags/v1.16.4.tar.gz
tar -xf v1.16.4.tar.gz
cd rook-1.16.4/deploy/examples/
sed -i "s|quay.io/cephcsi|sgulj51za84gae-quay.xuanyuan.run/cephcsi|g" operator.yaml
sed -i "s|k8s.gcr.io/sig-storage|sgulj51za84gae-k8s.xuanyuan.run/sig-storage|g" operator.yaml
sed -i "s|registry.k8s.io/sig-storage|sgulj51za84gae-k8s.xuanyuan.run/sig-storage|g" operator.yaml

sed -i "s|ROOK_CURRENT_NAMESPACE_ONLY false|ROOK_CURRENT_NAMESPACE_ONLY true|g" operator.yaml
sed -i "s|ROOK_ENABLE_DISCOVERY_DAEMON: "false"|ROOK_ENABLE_DISCOVERY_DAEMON: "true"|g" operator.yaml
sed -i "s|#ROOK_CSI_CEPH_IMAGE: quay.io/cephcsi|sgulj51za84gae-quay.xuanyuan.run/cephcsi|g" operator.yaml
sed -i "s|#ROOK_CSI_REGISTRAR_IMAGE: k8s.gcr.io/sig-storage|sgulj51za84gae-k8s.xuanyuan.run/sig-storage|g" operator.yaml
sed -i "s|#ROOK_CSI_RESIZER_IMAGE: registry.k8s.io/sig-storage|sgulj51za84gae-k8s.xuanyuan.run/sig-storage|g" operator.yaml
sed -i "s|#ROOK_CSI_PROVISIONER_IMAGE: registry.k8s.io/sig-storage|sgulj51za84gae-k8s.xuanyuan.run/sig-storage|g" operator.yaml
sed -i "s|#ROOK_CSI_SNAPSHOTTER_IMAGE: registry.k8s.io/sig-storage|sgulj51za84gae-k8s.xuanyuan.run/sig-storage|g" operator.yaml
sed -i "s|#ROOK_CSI_ATTACHER_IMAGE: registry.k8s.io/sig-storage|sgulj51za84gae-k8s.xuanyuan.run/sig-storage|g" operator.yaml
vi cluster.yaml
dashboard:
  ssl: false
kubectl apply -f crds.yaml -f common.yaml -f operator.yaml
kubectl apply -f cluster.yaml
kubectl apply -f pool.yaml -f csi/rbd/storageclass.yaml -f csi/rbd/snapshotclass.yaml
kubectl patch storageclass rook-ceph-block -p '{"metadata": {"annotations":{"storageclass.kubernetes.io/is-default-class":"true"}}}'
kubectl apply -f toolbox.yaml -n rook-ceph
vi dashboard-external-https.yaml
  ports:
    - name: dashboard
      port: 8443
      protocol: TCP
      targetPort: 7000
kubectl apply -f dashboard-external-https.yaml -n rook-ceph
kubectl -n rook-ceph get secret rook-ceph-dashboard-password -o jsonpath="{['data']['password']}"|base64 --decode && echo
vi object-store.yaml
apiVersion: ceph.rook.io/v1
kind: CephObjectStore
metadata:
  name: store
  namespace: rook-ceph
spec:
  gateway:
    port: 8080
    instances: 3
    placement:
      podAntiAffinity:
        requiredDuringSchedulingIgnoredDuringExecution:
          - labelSelector:
              matchExpressions:
                - key: app
                  operator: In
                  values: ["rook-ceph-rgw"]
            topologyKey: "kubernetes.io/hostname"
  metadataPool:
    replicated:
      size: 3
      requireSafeReplicaSize: true
    parameters:
      compression_mode: none
  dataPool:
    erasureCoded:
      dataChunks: 4
      codingChunks: 2
    parameters:
      compression_mode: aggressive
kubectl apply -f object-store.yaml
rm -rf rook-1.16.4 v1.16.4.tar.gz
# ===== rook-ceph =====

# ===== delete rook-ceph =====
kubectl patch cephcluster rook-ceph --type merge -p '{"spec":{"cleanupPolicy":{"confirmation":"yes-really-destroy-data"}}}' -n rook-ceph
for CRD in $(kubectl get crd -n rook-ceph | awk '/ceph.rook.io/ {print $1}'); do
    kubectl get -n rook-ceph "$CRD" -o name | \
    xargs -I {} kubectl patch -n rook-ceph {} --type merge -p '{"metadata":{"finalizers": []}}'
done
kubectl -n rook-ceph patch configmap rook-ceph-mon-endpoints --type merge -p '{"metadata":{"finalizers": []}}'
kubectl -n rook-ceph patch secrets rook-ceph-mon --type merge -p '{"metadata":{"finalizers": []}}'
kubectl -n rook-ceph delete cephcluster rook-ceph
cd rook-1.16.4/deploy/examples/
kubectl delete -f dashboard-external-https.yaml
kubectl delete -f toolbox.yaml
kubectl delete -f pool.yaml -f csi/rbd/storageclass.yaml -f csi/rbd/snapshotclass.yaml
kubectl delete -f cluster.yaml
kubectl delete -f crds.yaml -f common.yaml -f operator.yaml
rm -rf /var/lib/rook/*
# ===== delete rook-ceph =====

# ===== kubeblocks =====
curl -fsSL https://kubeblocks.io/installer/install_cli.sh | bash
kbcli kubeblocks install
kbcli cluster create mysql --mode raftGroup mysql -n default
kbcli cluster configure mysql -n default --set=max_connections=2000
kbcli cluster create postgresql --mode replication postgresql -n default
kbcli cluster configure postgresql -n default --set=max_connections=2000
kubectl edit cm postgresql-postgresql-pgbouncer-configuration -n default
    max_client_conn = 2000
kbcli cluster create redis --mode replication redis -n default
kbcli cluster create kafka --mode='combined' --replicas=3 --cpu=4 --memory=4 --monitor-enable=true kafka -n default
# ===== kubeblocks =====

# ===== delete kubeblocks =====
kbcli cluster delete mysql
kbcli cluster delete postgresql
kbcli cluster delete redis
kbcli kubeblocks uninstall
# ===== delete kubeblocks =====

# ===== openelb =====
vi /etc/kubernetes/manifests/kube-apiserver.yaml
# 找到 --service-cluster-ip-range 这一行，在这一行的下一行增加 如下内容:
    - --service-node-port-range=1-65535
systemctl daemon-reload && systemctl restart kubelet
wget https://www.gh-proxy.com/https://raw.githubusercontent.com/openelb/openelb/release-0.6/deploy/openelb.yaml
kubectl apply -f openelb.yaml
kubectl edit ds -n openelb-system openelb-speaker
    - --enable-layer2=true
kubectl edit configmap kube-proxy -n kube-system
  ipvs:
    strictARP: true
kubectl rollout restart daemonset kube-proxy -n kube-system
vi k8s_eip_openelb_layer2.yaml
apiVersion: network.kubesphere.io/v1alpha2
kind: Eip
metadata:
  name: eip-openelb-layer2
  annotations:
    eip.openelb.kubesphere.io/is-default-eip: "true"
spec:
  address: 192.168.1.10-192.168.1.200
  interface: ens5
  protocol: layer2
kubectl apply -f k8s_eip_openelb_layer2.yaml
kubectl annotate node master1 layer2.openelb.kubesphere.io/v1alpha1=192.168.21.102
kubectl annotate node master2 layer2.openelb.kubesphere.io/v1alpha1=192.168.21.103
kubectl annotate node master3 layer2.openelb.kubesphere.io/v1alpha1=192.168.21.104
kubectl annotate node worker1 layer2.openelb.kubesphere.io/v1alpha1=192.168.21.105
kubectl annotate node worker2 layer2.openelb.kubesphere.io/v1alpha1=192.168.21.106
kubectl annotate node worker3 layer2.openelb.kubesphere.io/v1alpha1=192.168.21.107
#将下面的键值对填写到注解列表中
lb.kubesphere.io/v1alpha1: openelb
protocol.openelb.kubesphere.io/v1alpha1: layer2
eip.openelb.kubesphere.io/v1alpha2: eip-openelb-layer2
# ===== openelb =====

# ===== mysql =====
helm repo add bitnami https://charts.bitnami.com/bitnami
vi helm_mysql.yaml
helm install mysql bitnami/mysql -f helm_mysql.yaml -n default
# ===== mysql =====

# ===== redis =====
vi helm_redis.yaml
helm install redis bitnami/redis -f helm_redis.yaml -n default
# ===== redis =====

# ===== redis-cluster =====
vi helm_redis_cluster.yaml
helm install redis bitnami/redis-cluster -f helm_redis_cluster.yaml -n default
# ===== redis-cluster =====

# ===== postgresql =====
vi helm_postgresql.yaml
helm install postgresql bitnami/postgresql -f helm_postgresql.yaml -n default
# ===== postgresql =====

# ===== postgresql-ha =====
vi helm_postgresql_ha.yaml
helm install postgresql bitnami/postgresql-ha -f helm_postgresql_ha.yaml -n default
# ===== postgresql-ha =====

# ===== kafka =====
vi helm_kafka.yaml
helm install kafka bitnami/kafka -f helm_kafka.yaml -n default
# ===== kafka =====

# ===== nats =====
vi helm_nats.yaml
helm repo add nats https://www.gh-proxy.com/https://github.com/nats-io/k8s/blob/gh-pages/helm/charts/
helm install nats nats/nats -f helm_nats.yaml -n default
# ===== nats =====