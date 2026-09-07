<template>
  <q-page class="flex flex-center">
    <q-card style="min-width: 250px">
      <q-card-section>
        <q-form @submit="onSubmit" @reset="onReset" class="q-gutter-md">
          <q-input
            v-model="username"
            label="用户名"
            lazy-rules
            :rules="[(val) => (val && val.length > 0) || '此为必填项']"
          />

          <q-input
            v-model="password"
            type="password"
            label="密码"
            lazy-rules
            :rules="[(val) => (val && val.length > 0) || '此为必填项']"
          />

          <div class="row">
            <q-btn class="col" label="重置" type="reset" color="primary" flat />
            <div class="col-auto" style="width: 5px" />
            <q-btn class="col" label="登录" type="submit" color="primary" />
          </div>
        </q-form>
      </q-card-section>
    </q-card>
  </q-page>
</template>

<script setup lang="ts">
import { onMounted, ref } from 'vue';
import { useStore } from 'src/stores/store';
import { api } from 'src/boot/axios';

const store = useStore();

const username = ref('');
const password = ref('');

const onReset = () => {
  username.value = '';
  password.value = '';
};

const onSubmit = () => {
  store.quasar.cookies.remove('ebike');

  api
    .post(store.backend.private + '/login', {
      method: 'login',
      username: username.value,
      password: password.value,
    })
    .then((resp) => {
      if (resp.data.msg === 'username or password error') {
        store.quasar.notify('用户名或密码错误');
        return;
      }

      onReLogin(resp.data);
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
    });
};

interface UserInfo {
  msg: {
    id: string;
    username: string;
    password: string;
    level: number;
    name: string;
  };
}

const onReLogin = (user?: UserInfo) => {
  // TEST
  if (!store.test) {
    if (user) {
      login(user);
    } else if (
      store.quasar.cookies.has('ebike') &&
      store.quasar.cookies.get('ebike') != 'undefined'
    ) {
      api
        .get(
          store.backend.private + '/info/' + store.quasar.cookies.get('ebike')
        )
        .then((resp) => {
          login(resp.data);
        })
        .catch(() => {});
    } else {
      return;
    }
  } else {
    void store.router.push('/index');
  }
};

const login = (user: UserInfo): void => {
  const time = setTimeout(() => {
    store.quasar.loading.hide();
    clearTimeout(time);
  }, 120000);

  store.quasar.loading.show();

  if (!store.test) {
    api
      .post(store.backend.private + '/options', {
        method: 'query',
      })
      .then((resp) => {
        for (let index = 0; index < resp.data.length; index++) {
          const element = resp.data[index];

          if (element.level === 0) {
            store.options_system(element.options);
          } else {
            store.options_list(element);
          }
        }

        store.initSchedule({
          system: setInterval(() => {
            if (store.schedule.system.run && store.schedule.system.list != null) {
              store.schedule.system.list.forEach((element: unknown) => {
                if (element && typeof element === 'function') element();
              });
            }
          }, store.options.system * 1000) as unknown as number,
          alert: setInterval(() => {
            if (store.schedule.alert.run && store.schedule.alert.list != null) {
              store.schedule.alert.list.forEach((element: unknown) => {
                if (element && typeof element === 'function') element();
              });
            }
          }, store.options.alert * 1000) as unknown as number,
        });

        store.quasar.cookies.set('ebike', user.msg.id);

        store.login({
          username: user.msg.username,
          password: user.msg.password,
          level: user.msg.level,
          name: user.msg.name,
        });

        store.quasar.loading.hide();
        clearTimeout(time);
        void store.router.push('/index');
      })
      .catch((e) => {
        console.log(e);

        store.quasar.loading.hide();
        clearTimeout(time);
        store.quasar.notify('网络错误，请稍后重试');
      });
  } else {
    store.quasar.loading.hide();
    clearTimeout(time);
    void store.router.push('/index');
  }
};

onMounted(() => {
  onReLogin();
});
</script>
