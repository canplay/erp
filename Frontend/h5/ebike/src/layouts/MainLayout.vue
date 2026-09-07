<template>
  <q-layout view="lHh Lpr lFf">
    <q-header elevated style="background-color: rgba(0, 0, 0, 0.5); backdrop-filter: blur(2px)">
      <q-toolbar class="text-info">
        <q-btn-group v-if="store.user.level != 0" spread flat class="desktop-only text-no-wrap">
          <q-btn label="实时预览" to="index" />
          <q-btn v-if="store.user.level === 1" label="车辆列表" to="car" />
          <q-btn v-if="store.user.level === 1" label="存放点列表" to="storage" />
          <q-btn v-if="store.user.level === 1" label="订单列表" to="order" />
          <q-btn v-if="store.user.level === 1" label="统计报表" to="reports" />
          <q-btn v-if="store.user.level === 1" label="系统设置" to="options" />
        </q-btn-group>

        <q-space class="desktop-only" />

        <q-toolbar-title v-if="title" class="row fixed-center">
          <q-btn class="col-auto" flat icon="img:icons/favicon-128x128.png" size="20px" />

          <div class="col text-bold text-center">
            共享电踏车监管平台
            <div class="text-caption">示例城市A市城乡资源开发管理有限公司</div>
          </div>
        </q-toolbar-title>

        <q-space v-if="store.user.level === 1" class="desktop-only" />

        <div class="row" v-if="store.user.level != 0">
          <q-checkbox
            v-if="store.user.level === 1"
            class="col-auto"
            v-model="store.showDrawPanel"
            keep-color
            color="primary"
            label="电子围栏控制"
            @update:model-value="onDrawPanel"
          />

          <div class="col-auto" style="width: 10px" />

          <div class="col-auto flex items-center">
            {{ store.schedule.now.time }}
          </div>

          <div class="col-auto" style="width: 10px" />

          <q-btn-dropdown class="col" flat dense icon="person" :label="store.user.name">
            <q-list>
              <q-item clickable v-close-popup @click="onLoginout">
                <q-item-section>
                  <q-item-label>退出登录</q-item-label>
                </q-item-section>
              </q-item>
            </q-list>
          </q-btn-dropdown>
        </div>
      </q-toolbar>
    </q-header>

    <q-page-container>
      <router-view />
    </q-page-container>
  </q-layout>
</template>

<script setup lang="ts">
import { onBeforeUnmount, onMounted, ref } from 'vue';
import { useStore } from 'src/stores/store';
import { api } from 'src/boot/axios';
import { date } from 'quasar';

const store = useStore();

const title = ref(true);

const onLoginout = () => {
  api
    .get(store.backend.private + '/loginout')
    .then((resp) => {
      if (resp.data === 'ok') {
        store.quasar.cookies.remove('ebike');
        store.login({
          username: '',
          password: '',
          level: 0,
          name: '',
        });
        if (store.route.path != '/') {
          void store.router.push('/');
        }
      } else {
        store.quasar.notify('网络错误，请稍后重试');
      }
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
    });
};

const onDrawPanel = (value: boolean) => {
  store.showDrawPanel = value;
  const iframe = document.querySelector('#q-app > div > div > main > iframe');
  if (iframe instanceof HTMLIFrameElement && iframe.contentWindow) {
    const win = iframe.contentWindow as Window & { drawPanel?: (value: boolean) => void };
    win.drawPanel?.(!value);
  }
};

onMounted(() => {
  // TEST
  if (store.test) {
    store.login({
      username: 'public',
      password: '',
      level: 0,
      name: '',
    });
    store.quasar.cookies.set('canplay', '{"username":"\\"public\\"","cookie":"\\"test\\""}', {
      expires: 1,
    });
  }

  store.setNow(
    setInterval(() => {
      store.getNow(date.formatDate(Date.now(), 'dddd YYYY-MM-DD H:mm:ss'));
    }, 1000) as unknown as number
  );
});

onBeforeUnmount(() => {
  store.clearSchedule();
});
</script>
