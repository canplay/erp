import { defineStore, acceptHMRUpdate } from 'pinia';
import { useQuasar } from 'quasar';
import { useRoute } from 'vue-router';
import { ref } from 'vue';

export interface LoginParams {
  username: string;
  password: string;
  level: number;
  name: string;
}

export interface ScheduleParams {
  system: number;
  alert: number;
}

export interface ScheduleItem {
  key: string;
  value: unknown;
  level: 'system' | 'alert';
}

export interface ScheduleLevel {
  run: boolean;
  list: Map<string, unknown> | null;
  interval: number;
}

export interface ScheduleState {
  now: {
    time: string;
    interval: number;
  };
  system: ScheduleLevel;
  alert: ScheduleLevel;
}

export interface OptionsState {
  system: number;
  alert: number;
  list: Array<Record<string, unknown>>;
}

export const useStore = defineStore('global', {
  state: () => ({
    quasar: useQuasar(),
    route: useRoute(),
    backend: {
      public: 'https://ebikepubsvc.cm.example.com',
      private: 'https://ebikeprisvc.cm.example.com',
    },
    test: false,
    showDrawPanel: false,
    options: ref<OptionsState>({
      system: 5,
      alert: 5,
      list: [],
    }),
    schedule: ref<ScheduleState>({
      now: {
        time: '0000-00-00 00:00:00',
        interval: -1,
      },
      system: {
        run: true,
        list: null,
        interval: -1,
      },
      alert: {
        run: true,
        list: null,
        interval: -1,
      },
    }),
    user: {
      username: '',
      password: '',
      level: 0,
      name: '',
    },
  }),
  getters: {
    getScheduleList: (state) => (level: 'system' | 'alert') => {
      return state.schedule[level]?.list ?? null;
    },
    isScheduleRunning: (state) => (level: 'system' | 'alert') => {
      return state.schedule[level]?.run ?? false;
    },
  },
  actions: {
    login(params: LoginParams) {
      this.user.username = params.username;
      this.user.password = params.password;
      this.user.level = params.level;
      this.user.name = params.name;
    },
    options_system(params: ScheduleParams) {
      this.options.system = params.system;
      this.options.alert = params.alert;
    },
    options_list(params: Array<Record<string, unknown>>) {
      this.options.list = params;
    },
    setNow(params: number) {
      this.schedule.now.interval = params;
    },
    getNow(params: string) {
      this.schedule.now.time = params;
    },
    initSchedule(params: ScheduleParams) {
      if (this.schedule.system.list === null) {
        this.schedule.system.list = new Map<string, unknown>();
        this.schedule.system.interval = params.system;
      }

      if (this.schedule.alert.list === null) {
        this.schedule.alert.list = new Map<string, unknown>();
        this.schedule.alert.interval = params.alert;
      }
    },
    clearSchedule() {
      clearInterval(this.schedule.now.interval);
      clearInterval(this.schedule.system.interval);
      clearInterval(this.schedule.alert.interval);
      this.schedule.system.list = null;
      this.schedule.alert.list = null;
    },
    addSchedule(params: ScheduleItem) {
      const list = this.getScheduleList(params.level);
      if (list != null) {
        list.set(params.key, params.value);
      }
    },
    delSchedule(params: { key: string; level: 'system' | 'alert' }) {
      const list = this.getScheduleList(params.level);
      if (list != null) {
        list.delete(params.key);
      }
    },
    runPauseSchedule(params: { level: 'system' | 'alert'; value: boolean }) {
      this.schedule[params.level].run = params.value;
    },
  },
  });

if (import.meta.hot) {
  import.meta.hot.accept(acceptHMRUpdate(useStore, import.meta.hot));
}
