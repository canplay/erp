<template>
  <q-page class="row items-center justify-evenly text-h1 text-bold">
    <p v-if="isLoading">重定向...</p>
  </q-page>
</template>

<script setup lang="ts">
import { useHandleSignInCallback, useLogto } from '@logto/vue'
import { useStore } from 'src/stores/store'
import { useRouter } from 'vue-router'

const router = useRouter()
const store = useStore()
const logto = useLogto()

const { isLoading } = useHandleSignInCallback(() => {
  void (async () => {
    if (logto.isAuthenticated.value) {
      const userInfo = await logto.fetchUserInfo()
      console.log('userInfo:', userInfo)

      store.user = {
        signed: true,
        id: userInfo!.sub,
        username: userInfo!.username || '',
        password: '',
        realName: userInfo!.name || '',
        nickName: '',
        avatar: userInfo!.picture || '',
        phone: '',
        source: '',
        sex: '',
        province: '',
        city: '',
        country: '',
        wechat: '',
        alipay: '',
        weibo: '',
        email: userInfo!.email || '',
        tiktok: '',
        idCard: '',
        permissions: {
          name: '游客',
          access: {
            all: false,
          },
        },
        options: {},
        additive: {
          hik_id: 0,
          parking: {
            current: [],
            arrears: {
              num: 0,
              money: 0,
            },
          },
        },
        address: '',
        backImage: '',
        cityDesc: '',
        clientId: '',
        county: '',
        countyDesc: '',
        frontImage: '',
        mobile: '',
        modifyId: '',
        pkId: '',
        remark: '',
        status: '',
        thirds: '',
        vip: '',
      }
    } else {
      store.user = {
        signed: false,
        id: '',
        source: '',
        username: '',
        password: '',
        realName: '',
        nickName: '游客',
        sex: '',
        province: '',
        city: '',
        country: '',
        avatar: 'imgs/noimg.svg',
        phone: '',
        wechat: '',
        alipay: '',
        weibo: '',
        email: '',
        tiktok: '',
        idCard: '',
        permissions: {
          name: '普通用户',
          access: {
            all: true,
          },
        },
        options: {},
        additive: {
          hik_id: 0,
          parking: {
            current: [],
            arrears: {
              num: 0,
              money: 0,
            },
          },
        },
        address: '',
        backImage: '',
        cityDesc: '',
        clientId: '',
        county: '',
        countyDesc: '',
        frontImage: '',
        mobile: '',
        modifyId: '',
        pkId: '',
        remark: '',
        status: '',
        thirds: '',
        vip: '',
      }
    }

    router.push('/').catch(() => {
      window.location.reload()
    })
  })()
})
</script>
