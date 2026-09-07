import type { UserInfoResponse } from '@logto/vue';
import { defineStore, acceptHMRUpdate } from 'pinia';
import { useQuasar } from 'quasar';
import { alova } from 'src/boot/axios';

export const useStore = defineStore('core', {
  state: () => ({
    logto: 'https://logto.cm.example.com/',
    callback:
      process.env.NODE_ENV === 'development'
        ? 'http://localhost:9000/callback'
        : 'https://cbis-n.cm.example.com',
    version: {
      current: '0.0.1',
      latest: '',
      status: 'updated',
      lastChecked: '',
      showUpdateDialog: false,
      autoCheck: true,
      notifyOnUpdate: true,
      checkInterval: null as number | null,
    },
    user: null as UserInfoResponse | null,
  }),
  actions: {
    checkVersion() {
      // 获取服务器上的版本信息
      const fetchLatest = async () => {
        try {
          this.version.status = 'checking';
          // 添加时间戳防止缓存
          const response = await alova.Get<{ version: string }>(`/version.json?t=${Date.now()}`);

          this.version.latest = response.version;
          this.version.lastChecked = new Date().toLocaleString();

          return response.version;
        } catch (error: unknown) {
          console.error('版本检查失败:', error);
          const $q = useQuasar();
          $q.notify({
            type: 'negative',
            message: '版本检查失败: ' + (error as Error).message,
            position: 'top',
          });
          this.version.status = 'updated';
          return null;
        }
      };

      // 检查版本更新
      const check = async () => {
        const latest = await fetchLatest();

        if (!latest) return;

        if (latest !== this.version.current) {
          this.version.status = 'outdated';
          if (this.version.notifyOnUpdate) {
            this.version.showUpdateDialog = true;
          }
        } else {
          this.version.status = 'updated';
          const $q = useQuasar();
          $q.notify({
            type: 'positive',
            message: '已是最新版本',
            position: 'top',
            timeout: 1000,
          });
        }
      };

      // 强制更新页面
      const forceUpdate = () => {
        // 使用URL参数法强制刷新，绕过浏览器缓存
        const url = new URL(window.location.href);
        url.searchParams.set('t', Date.now().toString());

        // 保存用户偏好设置到localStorage
        localStorage.setItem('appVersion', this.version.latest);
        localStorage.setItem('autoCheck', this.version.autoCheck.toString());
        localStorage.setItem('notifyOnUpdate', this.version.notifyOnUpdate.toString());

        window.location.href = url.toString();
      };

      // 稍后提醒
      const postponeUpdate = () => {
        this.version.showUpdateDialog = false;
        const $q = useQuasar();
        $q.notify({
          message: '已推迟更新，下次检查时提醒',
          position: 'top',
          timeout: 1000,
        });
      };

      // 清除定时器
      const clearCheckInterval = () => {
        if (this.version.checkInterval) {
          clearInterval(this.version.checkInterval);
          this.version.checkInterval = null;
        }
      };

      // 初始化
      const init = async () => {
        // 从localStorage加载设置
        const savedVersion = localStorage.getItem('appVersion');
        const savedAutoCheck = localStorage.getItem('autoCheck');
        const savedNotify = localStorage.getItem('notifyOnUpdate');

        if (savedVersion) {
          this.version.current = savedVersion;
        }

        if (savedAutoCheck !== null) {
          this.version.autoCheck = savedAutoCheck === 'true';
        }

        if (savedNotify !== null) {
          this.version.notifyOnUpdate = savedNotify === 'true';
        }

        // 设置定时检查
        if (this.version.autoCheck) {
          // 先清除已存在的定时器
          clearCheckInterval();
          this.version.checkInterval = window.setInterval(
            () => {
              check().catch((error) => {
                console.error('定时检查版本失败:', error);
              });
            },
            5 * 60 * 1000,
          ); // 5分钟检查一次

          // 初始检查
          await check();
        }
      };

      return { forceUpdate, postponeUpdate, init };
    },
  },
});

if (import.meta.hot) {
  import.meta.hot.accept(acceptHMRUpdate(useStore, import.meta.hot));
}
