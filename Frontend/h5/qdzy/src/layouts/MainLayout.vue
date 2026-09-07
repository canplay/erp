<template>
  <q-layout view="hHh Lpr lFf" class="my-font bg-light-blue-2">
    <div v-if="countdown > 0">
      <q-page-sticky position="top-right" :offset="[18, 18]">
        <q-btn
          outline
          dense
          class="fixed-top-right text-black"
          style="width: 64px"
          :label="countdown"
          disable
        />
      </q-page-sticky>
      <div class="background-opening"></div>
    </div>
    <div v-else>
      <q-page-container>
        <q-scroll-area
          :style="
            route.path != '/view'
              ? 'width: 100vw; height: calc(100vh - 56.01px)'
              : 'width: 100vw; height: calc(100vh)'
          "
        >
          <router-view />
        </q-scroll-area>
      </q-page-container>

      <q-footer elevated class="bg-grey-2 text-dark">
        <q-toolbar>
          <q-btn
            flat
            stack
            class="fit"
            label="首页"
            icon="img:imgs/index.svg"
            :color="store.page.path === '/' ? 'primary' : 'black'"
            to="/"
          />
          <q-btn
            flat
            stack
            class="fit"
            label="我的"
            icon="img:imgs/order.svg"
            :color="store.page.path === '/my' ? 'primary' : 'black'"
            to="/my"
          />
        </q-toolbar>
      </q-footer>
    </div>
  </q-layout>
</template>

<style lang="scss" scoped>
.background-opening {
  position: fixed;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  width: 100vw;
  height: 100vh;
  background-image: url('/imgs/loading.jpg');
  background-size: cover;
  z-index: -1;
}

.background-image {
  position: fixed;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  width: 100vw;
  height: 100vh;
  background-image: url('/imgs/bg.jpg');
  background-size: cover;
  z-index: -1;
}
</style>

<script setup lang="ts">
import { useRoute } from 'vue-router'
import { onMounted, ref } from 'vue'
import { useStore } from 'src/stores/store'
import { useLogto } from '@logto/vue'
import { useQuasar } from 'quasar'

const quasar = useQuasar()
const route = useRoute()
const store = useStore()
const { isAuthenticated } = useLogto()
const { getIdTokenClaims } = useLogto()

const countdown = ref(0)
// const onSubmit = () => {
//   alova
//     .Put<Response<string>>(
//       `${store.server.erp}/post?service=qdzy&auth=log&path=/api/order/create`,
//       {
//         zone: store.order.zone.model,
//         type: store.order.type.model,
//         name: store.order.name,
//         phone: store.order.phone,
//         address: store.order.address,
//         src: store.order.src,
//         remark: store.order.remark,
//         create_user: uid(),
//         update_user: uid(),
//       },
//     )
//     .then((resp) => {
//       if (resp.status) {
//         quasar.notify('预约成功')
//       } else {
//         quasar.notify('预约失败')
//       }
//     })
//     .catch((e) => {
//       console.log(e)
//     })
// }

onMounted(() => {
  // const { registerInterval } = useInterval()
  // registerInterval(() => {
  //   countdown.value -= 1
  // }, 1000)

  if (isAuthenticated.value) {
    ;(async () => {
      const token = quasar.localStorage.getItem(`logto:${store.logto.clientId}:idToken`)
      quasar.cookies.set('qdsh_session', JSON.stringify(token).replace(/"/g, ''))

      const claims = await getIdTokenClaims()
      store.user = {
        id: claims!.sub,
        nickname: claims?.name,
        username: claims?.username,
        email: claims?.email,
        avatar: claims?.picture,
        qq: '',
        wechat: '',
        phone: claims?.phone_number,
        orgs: claims?.organizations,
        roles: claims?.roles,
        orgRoles: claims?.organization_roles,
        permission: [],
      }
    })().catch((e) => {
      console.log(e)
    })
  }
})
</script>
