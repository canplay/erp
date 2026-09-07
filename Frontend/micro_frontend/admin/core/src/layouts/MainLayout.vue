<template>
  <q-layout view="hHh Lpr fFf">
    <div v-if="isAuthenticated">
      <q-header elevated>
        <q-toolbar>
          <q-btn outline dense @click="leftMenu = !leftMenu">
            <q-avatar>
              <q-icon :name="leftMenu ? 'menu_open' : 'menu'" />
            </q-avatar>
          </q-btn>

          <q-toolbar-title>微前端主应用</q-toolbar-title>

          <q-btn round dense outline>
            <q-avatar>
              <q-icon name="person" />
            </q-avatar>

            <q-menu>
              <q-list>
                <q-item clickable v-close-popup @click="dialog.my.show = !dialog.my.show">
                  <q-item-section>个人信息</q-item-section>
                </q-item>
                <q-item clickable v-close-popup>
                  <q-item-section>系统设置</q-item-section>
                </q-item>
                <q-separator />
                <q-item clickable v-close-popup @click="onClickSignOut">
                  <q-item-section>注销</q-item-section>
                </q-item>
              </q-list>
            </q-menu>
          </q-btn>
        </q-toolbar>
      </q-header>

      <q-footer elevated>
        <q-toolbar>
          <q-toolbar-title class="text-center">Powered by CaNplay</q-toolbar-title>
        </q-toolbar>
      </q-footer>
    </div>

    <div v-else class="fullscreen" style="background-color: cyan">
      <div class="fixed-center">
        <div class="flex justify-center">
          <q-img src="icons/favicon-128x128.png" width="20vw" />
        </div>

        <div style="height: 8px" />

        <div class="flex justify-center">
          <div class="text-h4 text-bold">示例城市A市七都科技产业发展有限责任公司</div>
        </div>

        <div style="height: 128px" />

        <div class="flex justify-center">
          <q-btn color="primary" label="立即登录" size="xl" @click="onClickSignIn" />
        </div>
      </div>
    </div>

    <q-drawer
      v-model="leftMenu"
      show-if-above
      :width="200"
      :breakpoint="500"
      bordered
      :class="$q.dark.isActive ? 'bg-grey-9' : 'bg-grey-3'"
      @update:model-value="onLeftMenuVisible"
    >
      <q-scroll-area class="fit">
        <q-list>
          <template v-for="(menuItem, index) in menuList" :key="index">
            <q-expansion-item
              v-if="menuItem.expand.length != 0"
              :active="menuItem.url === activeMenu"
              :icon="menuItem.icon"
              :label="menuItem.label"
            >
              <template v-for="(subItem, subIndex) in menuItem.expand" :key="subIndex">
                <q-item
                  clickable
                  :active="subItem.url === activeMenu"
                  v-ripple
                  @click="activeMenu = subItem.url"
                >
                  <q-item-section avatar>
                    <q-icon :name="subItem.icon" />
                  </q-item-section>
                  <q-item-section>
                    {{ subItem.label }}
                  </q-item-section>
                </q-item>
              </template>
            </q-expansion-item>
            <q-item
              v-else
              clickable
              :active="menuItem.url === activeMenu"
              v-ripple
              @click="activeMenu = menuItem.url"
            >
              <q-item-section avatar>
                <q-icon :name="menuItem.icon" />
              </q-item-section>
              <q-item-section>
                {{ menuItem.label }}
              </q-item-section>
            </q-item>
            <q-separator :key="'sep' + index" v-if="menuItem.separator" />
          </template>
        </q-list>
      </q-scroll-area>
    </q-drawer>

    <q-page-container>
      <router-view v-if="activeMenu === '/'" />

      <q-page v-else-if="activeMenu === '/qxzs/index'">
        <qxzsIndex :componentSize="componentSize" />
      </q-page>

      <q-page v-else-if="activeMenu === '/qxzs/projects'">
        <qxzsProject :componentSize="componentSize" />
      </q-page>

      <q-page v-else-if="activeMenu === '/qxzs/assets'">
        <qxzsAssets :componentSize="componentSize" />
      </q-page>

      <q-page v-else-if="activeMenu === '/qxzs/business'">
        <qxzsBusiness :componentSize="componentSize" />
      </q-page>

      <q-page v-else-if="activeMenu === '/qxzs/customer'">
        <qxzsCustomer :store="store" />
      </q-page>

      <q-page v-else-if="activeMenu === '/subapp_b'">
        <remoteComponentB :componentSize="componentSize" />
      </q-page>
    </q-page-container>
  </q-layout>

  <q-dialog v-model="dialog.my.show" persistent>
    <q-card style="width: 90vw">
      <q-card-section>
        <div class="text-h6 text-center">个人信息</div>
      </q-card-section>

      <q-card-section>
        <div class="row">
          <div class="col-auto">用户名：</div>
          <div class="col-auto">{{ store.user?.username }}</div>
        </div>

        <div style="height: 8px" />

        <div class="row">
          <div class="col-auto">邮箱：</div>
          <div class="col-auto">{{ store.user?.email }}</div>
        </div>

        <div style="height: 8px" />

        <div class="row">
          <div class="col-auto">姓名：</div>
          <div class="col-auto">{{ store.user?.name }}</div>
        </div>

        <div style="height: 8px" />

        <div class="row">
          <div class="col-auto">手机号：</div>
          <div class="col-auto">{{ store.user?.phone_number }}</div>
        </div>

        <div style="height: 8px" />

        <div class="row">
          <div class="col-auto">头像：</div>
          <div class="col-auto">{{ store.user?.picture }}</div>
        </div>
      </q-card-section>

      <q-card-actions align="right">
        <q-btn flat label="取消" v-close-popup />
      </q-card-actions>
    </q-card>
  </q-dialog>
</template>

<script setup lang="ts">
import { useStore } from 'src/stores/store';
import { ref, defineAsyncComponent, onMounted } from 'vue';
import { useLogto } from '@logto/vue';

const store = useStore();

const leftMenu = ref(false);

const menuList = [
  {
    icon: 'home',
    label: '概览',
    separator: true,
    url: '/',
    expand: [],
  },
  {
    icon: 'apps',
    label: '产业招商',
    separator: false,
    url: '/qxzs',
    expand: [
      {
        icon: 'folder',
        label: '概览',
        separator: false,
        url: '/qxzs/index',
        expand: [],
      },
      {
        icon: 'folder',
        label: '项目管理',
        separator: false,
        url: '/qxzs/projects',
        expand: [],
      },
      {
        icon: 'folder',
        label: '资产管理',
        separator: false,
        url: '/qxzs/assets',
        expand: [],
      },
      {
        icon: 'folder',
        label: '业务管理',
        separator: false,
        url: '/qxzs/business',
        expand: [],
      },
      {
        icon: 'folder',
        label: '客户管理',
        separator: false,
        url: '/qxzs/customer',
        expand: [],
      },
    ],
  },
  {
    icon: 'apps',
    label: '子应用B',
    separator: true,
    url: '/subapp_b',
    expand: [],
  },
  {
    icon: 'person',
    label: '个人信息',
    separator: false,
    url: '/profile',
    expand: [],
  },
  {
    icon: 'settings',
    label: '系统设置',
    separator: false,
    url: '/settings',
    expand: [],
  },
];

const activeMenu = ref('/');

const componentSize = ref({
  width: 0,
  height: 0,
});

const qxzsIndex = defineAsyncComponent(() => import('qxzs/index'));
const qxzsProject = defineAsyncComponent(() => import('qxzs/project'));
const qxzsAssets = defineAsyncComponent(() => import('qxzs/assets'));
const qxzsBusiness = defineAsyncComponent(() => import('qxzs/business'));
const qxzsCustomer = defineAsyncComponent(() => import('qxzs/customer'));
const remoteComponentB = defineAsyncComponent(() => import('subapp_b/IndexPage'));

const dialog = ref({
  my: {
    show: false,
  },
});

// 计算可用空间
const calculateComponentSize = () => {
  // 估算header和footer的高度，你可能需要根据实际情况调整这些值
  const headerHeight = 50; // q-header 的高度
  const footerHeight = 50; // q-footer 的高度
  const totalReservedHeight = headerHeight + footerHeight;

  // 计算可用高度和宽度
  componentSize.value.height = window.innerHeight - totalReservedHeight;
  componentSize.value.width = window.innerWidth;
};

const onLeftMenuVisible = () => {
  // 左侧菜单隐藏时重新计算组件大小
  calculateComponentSize();
};

const { signIn, signOut, isAuthenticated, fetchUserInfo } = useLogto();

const onClickSignIn = () => signIn(store.callback);
const onClickSignOut = () => signOut(store.callback);

onMounted(async () => {
  // 初始计算
  calculateComponentSize();

  if (isAuthenticated.value) {
    await (async () => {
      const userInfo = await fetchUserInfo();
      store.user = userInfo!;
    })();
    leftMenu.value = true;
  } else {
    leftMenu.value = false;
  }
});
</script>
