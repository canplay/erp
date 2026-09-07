<template>
  <q-layout view="hHh LpR fFf" class="my-font">
    <q-header
      elevated
      class="desktop-only q-electron-drag"
      style="user-select: none"
    >
      <q-toolbar class="row">
        <q-btn
          class="col-auto"
          flat
          dense
          padding="xs"
          no-caps
          @click="store.leftmenu = !store.leftmenu"
        >
          <q-avatar>
            <q-img src="/icons/favicon-96x96.png" />
          </q-avatar>

          <q-tooltip v-if="store.signed">菜单</q-tooltip>
        </q-btn>

        <q-toolbar-title class="col" data-tauri-drag-region>
          业务中心
        </q-toolbar-title>

        <q-btn-dropdown flat v-if="store.signed" label="APP">
          <q-list class="my-font">
            <q-item
              clickable
              v-ripple
              @click="
                goto('https://cbis.cm.example.com/downloads/app/client.7z')
              "
            >
              <q-item-section>
                <q-item-label>下载 Windows App</q-item-label>
              </q-item-section>
            </q-item>

            <q-item
              clickable
              v-ripple
              @click="
                goto('https://cbis.cm.example.com/downloads/app/client.mac')
              "
            >
              <q-item-section>
                <q-item-label>下载 Mac App</q-item-label>
              </q-item-section>
            </q-item>

            <q-item
              clickable
              v-ripple
              @click="
                goto('https://cbis.cm.example.com/downloads/app/client.apk')
              "
            >
              <q-item-section>
                <q-item-label>下载 Android App</q-item-label>
              </q-item-section>
            </q-item>

            <q-item
              clickable
              v-ripple
              @click="
                goto('https://cbis.cm.example.com/downloads/app/client.ipa')
              "
            >
              <q-item-section>
                <q-item-label>下载 iOS App</q-item-label>
              </q-item-section>
            </q-item>

            <q-item
              clickable
              v-ripple
              @click="goto('https://qdsh.cm.example.com/')"
            >
              <q-item-section>
                <q-item-label>打开七都商合</q-item-label>
              </q-item-section>
            </q-item>
          </q-list>
        </q-btn-dropdown>

        <q-btn-dropdown flat v-if="store.signed" :label="store.user.name">
          <q-list class="my-font">
            <q-item clickable v-ripple @click="onSignout">
              <q-item-section>
                <q-item-label>注销</q-item-label>
              </q-item-section>
            </q-item>
          </q-list>
        </q-btn-dropdown>

        <q-btn
          class="electron-only"
          dense
          flat
          icon="minimize"
          @click="setMinimize"
        >
          <q-tooltip content-style="font-size: 12px">最小化</q-tooltip>
        </q-btn>

        <q-btn
          class="electron-only"
          dense
          flat
          icon="crop_square"
          @click="setMaximize"
        >
          <q-tooltip content-style="font-size: 12px">最大化</q-tooltip>
        </q-btn>

        <q-btn class="electron-only" dense flat icon="close" @click="setClose">
          <q-tooltip content-style="font-size: 12px">关闭</q-tooltip>
        </q-btn>
      </q-toolbar>
    </q-header>

    <!-- 左边部分 -->
    <q-drawer
      v-if="store.signed"
      v-model="store.leftmenu"
      show-if-above
      :width="200"
      :breakpoint="700"
      elevated
      class="bg-primary text-white"
    >
      <menuLeft />
    </q-drawer>
    <!-- 中间部分 -->
    <q-page-container class="desktop-only" style="padding-top: 50px">
      <q-scroll-area :style="store.height">
        <router-view />
      </q-scroll-area>
    </q-page-container>

    <q-page-container class="mobile-only" style="padding-top: 50px">
      <router-view />
    </q-page-container>
  </q-layout>

  <!-- <q-inner-loading :showing="loading">
    <q-spinner size="50px" color="primary" />
  </q-inner-loading>-->
</template>

<style lang="sass">
.customTab
  .q-tab__label
    font-size: 24px
</style>

<script setup lang="ts">
import { onMounted, ref } from 'vue';
import { openURL } from 'quasar';
import { useStore } from 'stores/store';
import menuLeft from 'src/components/MenuLeft.vue';
import * as jose from 'jose'; //JOSE ：Javascript对象签名和加密
// import customProtocolCheck from 'custom-protocol-check';
import { api } from 'src/boot/axios';

const store = useStore();

const loading = ref(false);

const setMinimize = () => {
  window.electron.setMinimize();
};

const setMaximize = () => {
  window.electron.setMaximize();
};

const setClose = () => {
  window.electron.setClose();
};

const goto = (name: string) => {
  if (!store.quasar.platform.is.electron) {
    openURL(name);
  } else {
    window.electron.openURL(name);
  }
};

onMounted(() => {
  if (store.quasar.platform.is.electron) {
    window.electron.onUpdate((ev: unknown, args: { app: { windows: { desc: string; url: string } } }) => {
      store.quasar
        .dialog({
          title: '发现新版本',
          message: args.app.windows.desc,
          ok: '立即下载更新',
          persistent: true,
        })
        .onOk(() => {
          window.electron.openURL(args.app.windows.url);
        });
    });
  }
});

const onSignout = () => {
  const decoded = jose.decodeJwt(store.quasar.cookies.get('qdsh'));
  void api.get(
    store.server +
      '/api/route/get?auth=jwt&service=cbis&path=/api/user/signout/' +
      String(decoded.id),
    {
      headers: {
        Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
      },
    }
  );

  store.quasar.cookies.remove('qdsh');

  store.signed = false;

  store.user = { id: '', name: '', level: 0, username: '', password: '' };

  store.permissions = {
    id: '',
    name: '',
    access: {},
  };

  store.options = {
    system: null,
    alert: null,
    list: [],
    tow: {
      car: {
        class: [],
        type: [],
        color: [],
      },
      causes: {
        type: [],
        list: [],
      },
    },
  };

  store.schedule = {
    update: {
      remote: null,
      interval: null,
    },
    now: {
      time: '0000-00-00 00:00:00',
      interval: null,
    },
    system: {
      run: true,
      list: null,
      interval: null,
    },
    alert: {
      run: true,
      list: null,
      interval: null,
    },
  };
  void store.router.replace('/');
};

if (
  store.quasar.cookies.has('qdsh') &&
  store.quasar.cookies.get('qdsh') !== ''
) {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  const decodedInfo = jose.decodeJwt(store.quasar.cookies.get('qdsh'));
  api
    .get(
      store.server +
        '/api/route/get?auth=jwt&service=cbis&path=/api/user/info/' +
        String(decodedInfo.id),
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      }
    )
    .then((resp) => {
      if (resp.data.status === 0) {
        store.quasar.notify(resp.data.message);
        loading.value = false;
        clearTimeout(time);
        return;
      }

      // customProtocolCheck(
      //   'CBISLauncher://login',
      //   () => {
      //     store.quasar.notify('请点击右上角 APP 下载并安装 Windows App');
      //   },
      //   () => {
      //     console.log('successfully');
      //   }, 3000
      // );

      store.user = resp.data.data.user;
      store.permissions = resp.data.data.permissions;
      store.options = {
        system: resp.data.data.options.system,
        alert: resp.data.data.options.alert,
        list: [],
        tow: {
          car: {
            class: [],
            type: [],
            color: [],
          },
          causes: {
            type: [],
            list: [],
          },
        },
      };

      store.signed = true;

      if (
        store.router.currentRoute.redirectedFrom !== undefined &&
        store.router.currentRoute.redirectedFrom?.path !== ''
      ) {
        void store.router.replace(store.router.currentRoute.redirectedFrom.path);
      } else {
        void store.router.replace('index');
      }

      loading.value = false;
      clearTimeout(time);
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      loading.value = false;
      clearTimeout(time);
    });
}
</script>
