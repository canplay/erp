import { defineBoot } from '#q-app/wrappers';
import BaiduMap from 'vue-baidu-map-3x';

export default defineBoot(({ app }) => {
  app.use(BaiduMap, {
    ak: '3Nmg9MkgsR6sAMkHI2t1RH2FtW5nWZAk',
    v: '3.0',
    type: 'WebGL',
  });
});
