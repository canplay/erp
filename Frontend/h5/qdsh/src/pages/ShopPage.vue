<template>
  <q-page>
    <IframeComp :url="url" style="width: 100vw; height: calc(100vh - 62.88px)" />
  </q-page>
</template>

<script setup lang="ts">
import { useStore } from 'src/stores/store'
import { onMounted, ref } from 'vue'
import IframeComp from 'src/components/IframeComp.vue'
import { useQuasar } from 'quasar'
import { useRoute } from 'vue-router'
import type { CommonSession } from 'src/components/models/common'

const quasar = useQuasar()
const route = useRoute()
const store = useStore()

const url = ref('https://shop-h5.ct.example.com')

onMounted(() => {
  store.page = {
    title: '商城',
    current: '/shop',
  }

  if (store.user.signed) {
    const token: CommonSession = quasar.cookies.get('qdsh_session')
    url.value += `/#/pages/tabBar/home?type=login&token=${token?.access_token}&phone=${store.user.phone}`
  } else {
    url.value = 'https://shop-h5.ct.example.com/#/pages/tabBar/home?type=loginout'
  }

  if (route.query.method === 'orderList') {
    url.value = 'https://shop-h5.ct.example.com/#/pages/order/myOrder?status=0'
  }
})
</script>
