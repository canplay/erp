<template>
  <!-- <q-page padding class="desktop-only electron-hide flex items-center justify-center row"
    style="background-image: url(imgs/bg.jpg); background-position: right bottom, left top; background-repeat: no-repeat, no-repeat; background-size: 100% 100%;">
    <q-card class="col">
      <q-card-section>
        <q-img src="imgs/download_app_windows.png" />
        <div style="height: 8px;" />
        <q-btn flat class="bg-white fit text-primary" size="24px" label="下载 Windows App" />
      </q-card-section>
    </q-card>

    <div class="col-auto" style="width: 8px;" />

    <q-card class="col">
      <q-card-section>
        <q-img src="imgs/download_app_android.png" />
        <div style="height: 8px;" />
        <q-btn flat class="bg-white fit text-primary" size="24px" label="下载 Android App" />
      </q-card-section>
    </q-card>

    <div class="col-auto" style="width: 8px;" />

    <q-card class="col">
      <q-card-section>
        <q-img src="imgs/download_app_ios.png" />
        <div style="height: 8px;" />
        <q-btn flat class="bg-white fit text-primary" size="24px" label="下载 iOS App" />
      </q-card-section>
    </q-card>

    <div class="col-auto" style="width: 8px;" />

    <q-card class="col">
      <q-card-section>
        <q-img src="imgs/download_app_h5.png" />
        <div style="height: 8px;" />
        <q-btn flat class="bg-white fit text-primary" size="24px" label="打开 H5 页面" />
      </q-card-section>
    </q-card>
  </q-page> -->

  <q-page
    class="flex items-center justify-center"
    style="
      background-image: url(imgs/bg.jpg);
      background-position:
        right bottom,
        left top;
      background-repeat: no-repeat, no-repeat;
      background-size: 100% 100%;
    "
  >
    <q-card>
      <q-card-section style="width: 60vw; width: 40vh">
        <q-tabs
          v-model="tab"
          class="bg-white"
          active-color="primary"
          indicator-color="primary"
          align="justify"
          narrow-indicator
        >
          <!-- <q-tab name="phone" label="手机号" /> -->
          <q-tab name="account" label="账号" />
        </q-tabs>

        <q-separator />

        <q-tab-panels v-model="tab" animated>
          <!-- <q-tab-panel name="phone">
            <q-input
              v-model="phone"
              label="手机号"
              lazy-rules
              :rules="[(val) => (val && val.length > 0) || '手机号不能为空']"
            >
              <template v-slot:append>
                <q-btn
                  :disable="verifyBtn.disable"
                  color="primary"
                  :label="verifyBtn.label"
                  @click="onSendVerify"
                />
              </template>
            </q-input>

            <q-input
              v-model="verify"
              label="验证码"
              lazy-rules
              :rules="[(val) => (val && val.length > 0) || '验证码不能为空']"
            />
          </q-tab-panel> -->

          <q-tab-panel name="account">
            <q-input
              v-model="username"
              label="账号"
              lazy-rules
              :rules="[(val) => (val && val.length > 0) || '账号不能为空']"
            />

            <q-input
              v-model="password"
              :type="isPwd ? 'password' : 'text'"
              label="密码"
              lazy-rules
              :rules="[(val) => (val && val.length > 0) || '密码不能为空']"
            >
              <template v-slot:append>
                <q-icon
                  :name="isPwd ? 'visibility_off' : 'visibility'"
                  class="cursor-pointer"
                  @click="isPwd = !isPwd"
                />
              </template>
            </q-input>
          </q-tab-panel>
        </q-tab-panels>

        <q-checkbox v-model="options.remember" label="记住密码" />

        <q-checkbox v-model="options.auto" label="自动登录" />
      </q-card-section>

      <q-card-actions class="row">
        <!-- <q-btn class="col" label="注册" color="negative" @click="onSignup" />
        <div class="col-auto" style="width: 8px" /> -->
        <q-btn class="col" label="登录" color="primary" @click="onSignin" />
      </q-card-actions>
    </q-card>
  </q-page>

  <q-inner-loading :showing="loading">
    <q-spinner size="50px" color="primary" />
  </q-inner-loading>
</template>

<style lang="sass">
.q-field--standout.q-field--dark .q-field__control
  background: rgba(255, 255, 255, 0.2)
</style>

<script setup lang="ts">
import { ref } from 'vue';
// import { date } from 'quasar';
import { useStore } from 'stores/store';
// import customProtocolCheck from 'custom-protocol-check';
import * as jose from 'jose';
import { api } from 'src/boot/axios';

const store = useStore();

const loading = ref(false);
const tab = ref('account');

const username = ref('');
const password = ref('');
const phone = ref('');
const verify = ref('');
// const verifyBtn = ref({
//   label: '发送',
//   disable: false,
// });
const isPwd = ref(true);

const options = ref({
  remember: false,
  auto: false,
});

const onSignin = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 180000);

  loading.value = true;

  switch (tab.value) {
    case 'account':
      api
        .post(store.server + '/api/route/post?auth=public&service=cbis&path=/api/user/signin', {
          username: username.value,
          password: password.value,
          remember: options.value.remember,
          type: 'account',
        })
        .then((resp) => {
          if (resp.data.status === 0) {
            loading.value = false;
            clearTimeout(time);
            store.quasar.notify(resp.data.message);
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

          store.quasar.cookies.set('qdsh', resp.data.data.token);

          store.user = resp.data.data;

          void api
            .get(
              store.server +
                '/api/route/get?auth=jwt&service=cbis&path=/api/user/info/' +
                String(jose.decodeJwt(store.quasar.cookies.get('qdsh')).id),
              {
                headers: {
                  Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
                },
              },
            )
            .then((resp) => {
              loading.value = false;
              clearTimeout(time);

              if (resp.data.status === 0) {
                store.quasar.notify(resp.data.message);
                return;
              }

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
              void store.router.replace('index');
            });
        })
        .catch(() => {
          loading.value = false;
          clearTimeout(time);
          store.quasar.notify('网络错误，请稍后重试');
        });
      break;
    case 'phone':
      api
        .post(store.server + '/api/route/post?auth=jwt&service=cbis&path=/api/user/signin', {
          phone: phone.value,
          verify: verify.value,
          type: 'phone',
        })
        .then((resp) => {
          if (resp.data.status === 0) {
            loading.value = false;
            clearTimeout(time);
            store.quasar.notify(resp.data.message);
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

          store.quasar.cookies.set('qdsh', resp.data.data.token);

          store.user = resp.data.data;

          void api
            .get(
              store.server +
                '/api/route/get?auth=jwt&service=cbis&path=/api/user/info/' +
                String(jose.decodeJwt(store.quasar.cookies.get('qdsh')).id),
              {
                headers: {
                  Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
                },
              },
            )
            .then((resp) => {
              loading.value = false;
              clearTimeout(time);

              if (resp.data.status === 0) {
                store.quasar.notify(resp.data.message);
                return;
              }

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
              void store.router.replace('index');
            });
        })
        .catch(() => {
          loading.value = false;
          clearTimeout(time);
          store.quasar.notify('网络错误，请稍后重试');
        });
      break;
  }
};

// const onSendVerify = () => {
//   if (phone.value === '') {
//     store.quasar.notify('请输入手机号');
//     return;
//   }

//   const time = setTimeout(() => {
//     store.quasar.loading.hide();
//   }, 30000);

//   store.quasar.loading.show({
//     message: '正在处理，请稍后...',
//   });

//   api
//     .post(store.server + '/api/route/post?service=notify&auth=public&path=/api/sms/verify', {
//       type: 'cm',
//       to: phone.value,
//       user: 0,
//     })
//     .then((resp) => {
//       store.quasar.loading.hide();
//       clearTimeout(time);

//       if (resp.data.status === 0) {
//         store.quasar.notify(resp.data.message);
//         return;
//       }

//       verifyBtn.value.disable = true;

//       const targetTime = date.addToDate(Date.now(), { seconds: 30 });
//       const timer = setInterval(() => {
//         const nowTime = Date.now();
//         const times = date.getDateDiff(targetTime, nowTime, 'seconds');
//         verifyBtn.value.label = '发送(' + times + ')';

//         if (times <= 0) {
//           clearInterval(timer);
//           verifyBtn.value.disable = false;
//           verifyBtn.value.label = '发送';
//         }
//       }, 1000);

//       store.quasar.notify('验证码发送成功, 请注意查收');
//     })
//     .catch(() => {
//       store.quasar.loading.hide();
//       clearTimeout(time);
//       store.quasar.notify('验证码发送失败, 请稍后重试');
//     });
// };
</script>
