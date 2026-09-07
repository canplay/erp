import { defineBoot } from '#q-app/wrappers';
import type { LogtoConfig } from '@logto/vue';
import { createLogto } from '@logto/vue';
import { useStore } from 'src/stores/store';

export default defineBoot(({ app }) => {
  const store = useStore();
  const config: LogtoConfig = {
    endpoint: store.logto,
    appId: 'snc88g2jc3bqpwirnubei',
  };
  app.use(createLogto, config);
});
