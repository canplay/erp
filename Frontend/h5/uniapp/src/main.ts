import { createSSRApp } from 'vue';
import App from './App.vue';
import { createPinia } from 'pinia';
import piniaPluginPersistedstate from 'pinia-plugin-persistedstate';
import share from './utils/share';

export function createApp() {
  const app = createSSRApp(App);

  const pinia = createPinia();
  pinia.use(piniaPluginPersistedstate);
  app.use(pinia);

  app.mixin(share);

  return {
    app,
    pinia,
  };
}
