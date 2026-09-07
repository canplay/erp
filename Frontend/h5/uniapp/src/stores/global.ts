import { defineStore } from 'pinia';
import { ref } from 'vue';

export const useStore = defineStore(
  'global',
  () => {
    const session = ref({} as any);
    return { session };
  },
  {
    persist: {
      storage: {
        setItem(key: any, value: any) {
          uni.setStorageSync(key, value);
        },
        getItem(key: any) {
          return uni.getStorageSync(key);
        },
      },
    },
  }
);
