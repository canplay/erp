import { defineStore, acceptHMRUpdate } from 'pinia';

export const useStore = defineStore('global', {
  state: () => ({
    page: {
      current: '',
    },
    server: '',
  }),
});

if (import.meta.hot) {
  import.meta.hot.accept(acceptHMRUpdate(useStore, import.meta.hot));
}
