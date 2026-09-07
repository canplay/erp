import { defineStore } from 'pinia';
import { useQuasar } from 'quasar';
import { useRoute, useRouter } from 'vue-router';

// 插件菜单类型定义
interface PluginMenu {
  index?: boolean;
  meeting?: boolean;
  shop?: boolean;
  shop_sadmin?: boolean;
  shop_padmin?: boolean;
  shop_admin?: boolean;
  pay?: boolean;
  pay_manager?: boolean;
  pay_merchant?: boolean;
  property?: boolean;
  viusal?: boolean;
  git?: boolean;
  hub?: boolean;
  k8s?: boolean;
  focalboard?: boolean;
  api?: boolean;
  sentry?: boolean;
  nats?: boolean;
  tools?: boolean;
  parking?: boolean;
  lamp?: boolean;
  assets?: boolean;
  order?: boolean;
  staff?: boolean;
  car?: boolean;
  estate?: boolean;
  feedback?: boolean;
  workorder?: boolean;
  receipt?: boolean;
  old_system?: boolean;
  browser?: boolean;
  zone?: boolean;
  plant?: boolean;
  patrol?: boolean;
  news?: boolean;
  publicInfo?: boolean;
  notify?: boolean;
  detail?: boolean;
  info?: boolean;
  user?: boolean;
  permission?: boolean;
  option?: boolean;
  seatsOrder?: boolean;
  healthex?: boolean;
  groupbuy?: boolean;
  sellhouse?: boolean;
  supplier?: boolean;
}

// 系统配置选项类型
interface SystemOptions {
  [key: string]: string | number | boolean | object | Array<unknown> | null;
}

// 告警配置选项类型
interface AlertOptions {
  [key: string]: string | number | boolean | object | Array<unknown> | null;
}

// 工单车辆类型
export interface TowCarOptionItem {
  id?: string;
  type?: string;
  ccid?: string;
  cost_day?: number;
  gratis_day?: number;
  cpt?: string;
  remark?: string;
  [key: string]: unknown;
}

interface TowCarOptions {
  class: TowCarOptionItem[];
  type: TowCarOptionItem[];
  color: string[];
}

// 工单原因类型
interface TowCausesOptions {
  type: string[];
  list: string[];
}

// 应用配置选项类型
interface AppOptions {
  system: SystemOptions | null;
  alert: AlertOptions | null;
  list: Array<unknown>;
  tow: {
    car: TowCarOptions;
    causes: TowCausesOptions;
  };
}

// 用户信息类型
interface UserInfo {
  id: string;
  name: string;
  level: number;
  username: string;
  password: string;
}

// 权限菜单标志类型（已合并到 AccessConfig）

// 菜单配置类型 - 所有菜单项的联合类型
type MenuItemType = PluginMenu | PublicAssetMenu | boolean;

// 公共资产菜单类型
interface PublicAssetMenu {
  index?: boolean;
  parking?: boolean;
  lamp?: boolean;
  assets?: boolean;
  order?: boolean;
  staff?: boolean;
  car?: boolean;
  estate?: boolean;
  feedback?: boolean;
  workorder?: boolean;
  receipt?: boolean;
  old_system?: boolean;
  browser?: boolean;
  zone?: boolean;
  plant?: boolean;
  patrol?: boolean;
  news?: boolean;
  publicInfo?: boolean;
  notify?: boolean;
  detail?: boolean;
  info?: boolean;
  user?: boolean;
  permission?: boolean;
  option?: boolean;
  seatsOrder?: boolean;
  healthex?: boolean;
  groupbuy?: boolean;
  sellhouse?: boolean;
  supplier?: boolean;
  report?: boolean;
  storage?: boolean;
  [key: string]: boolean | string | number | null | undefined;
}

// eslint-disable-next-line @typescript-eslint/no-unused-vars
interface _MenuConfig {
  plugin?: PluginMenu;
  public_assets?: PublicAssetMenu;
  [key: string]: MenuItemType;
}

// 权限类型 - 合并所有菜单属性为一个扁平类型
interface AllMenuFlags {
  index?: boolean;
  meeting?: boolean;
  shop?: boolean;
  shop_sadmin?: boolean;
  shop_padmin?: boolean;
  shop_admin?: boolean;
  pay?: boolean;
  pay_manager?: boolean;
  pay_merchant?: boolean;
  property?: boolean;
  viusal?: boolean;
  git?: boolean;
  hub?: boolean;
  k8s?: boolean;
  focalboard?: boolean;
  api?: boolean;
  sentry?: boolean;
  nats?: boolean;
  tools?: boolean;
  parking?: boolean;
  lamp?: boolean;
  assets?: boolean;
  order?: boolean;
  staff?: boolean;
  car?: boolean;
  estate?: boolean;
  feedback?: boolean;
  workorder?: boolean;
  receipt?: boolean;
  old_system?: boolean;
  browser?: boolean;
  zone?: boolean;
  plant?: boolean;
  patrol?: boolean;
  news?: boolean;
  publicInfo?: boolean;
  notify?: boolean;
  detail?: boolean;
  info?: boolean;
  user?: boolean;
  permission?: boolean;
  option?: boolean;
  seatsOrder?: boolean;
  healthex?: boolean;
  groupbuy?: boolean;
  sellhouse?: boolean;
  supplier?: boolean;
  report?: boolean;
  storage?: boolean;
}

// 菜单嵌套配置类型 - 支持嵌套对象和布尔值
interface NestedMenuConfig {
  plugin?: PluginMenu;
  public_assets?: PublicAssetMenu;
  property?: AllMenuFlags;
  livestock?: AllMenuFlags;
  clean?: AllMenuFlags;
  distribution?: AllMenuFlags;
  afforest?: AllMenuFlags;
  tow?: AllMenuFlags;
  funeral?: AllMenuFlags;
  repair?: AllMenuFlags;
  ticket?: AllMenuFlags;
  project?: AllMenuFlags;
  supply?: AllMenuFlags;
  customer?: AllMenuFlags;
  app?: AllMenuFlags;
  pay?: AllMenuFlags;
  personal?: AllMenuFlags;
  system?: AllMenuFlags;
  [key: string]: AllMenuFlags | PluginMenu | PublicAssetMenu | boolean | string | number | null | undefined;
}

// 权限配置类型
interface AccessConfig {
  all?: boolean;
  menu?: NestedMenuConfig;
  public_assets?: PublicAssetMenu;
  plugin?: PluginMenu;
  [key: string]: boolean | string | number | null | undefined | AllMenuFlags | PublicAssetMenu | PluginMenu | NestedMenuConfig;
}

export const useStore = defineStore('global', {
  state: () => {
    const permissions: {
      id: string;
      name: string;
      access: AccessConfig;
    } = {
      id: '',
      name: '',
      access: {},
    };

    return {
      quasar: useQuasar(),
      route: useRoute(),
      router: useRouter(),
      server: 'https://erp.cm.example.com',
      is_app: false,
      signed: false,
      app: {
        url: '',
        params: {} as Record<string, string | number | boolean>,
      },
      leftmenu: true,
      user: {} as UserInfo,
      permissions,
      options: {} as AppOptions,
      schedule: {
        update: {
          remote: null as ReturnType<typeof setInterval> | null,
          interval: null as ReturnType<typeof setInterval> | null,
        },
        now: {
          time: '0000-00-00 00:00:00',
          interval: null as ReturnType<typeof setInterval> | null,
        },
        system: {
          run: true,
          list: null as Array<unknown> | null,
          interval: null as ReturnType<typeof setInterval> | null,
        },
        alert: {
          run: true,
          list: null as Array<unknown> | null,
          interval: null as ReturnType<typeof setInterval> | null,
        },
      },
    };
  },
  getters: {
    height(state) {
      if (state.is_app) {
        return 'height: 100vh;';
      }
      return 'height: calc(100vh - 50px);';
    },
    width(state) {
      if (state.leftmenu) {
        return 'width: calc(100vw - 200px);';
      }
      return 'width: 100vw;';
    },
  },
});
