declare namespace NodeJS {
  interface ProcessEnv {
    NODE_ENV: string;
    VUE_ROUTER_MODE: 'hash' | 'history' | 'abstract' | undefined;
    VUE_ROUTER_BASE: string | undefined;
  }
}

declare module 'vue-baidu-map-3x' {
  import type { Plugin } from 'vue';
  const BaiduMap: Plugin;
  export default BaiduMap;
}
