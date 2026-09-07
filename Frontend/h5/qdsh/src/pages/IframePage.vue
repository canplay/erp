<template>
  <q-page>
    <IframeComp
      :url="route.query.target"
      style="width: 100vw; height: calc(100vh - 50px - 62.88px)"
    />
  </q-page>
</template>

<script setup lang="ts">
import { useStore } from 'src/stores/store'
import { onMounted } from 'vue'
import IframeComp from 'src/components/IframeComp.vue'
import { useRoute } from 'vue-router'

const route = useRoute()
const store = useStore()

onMounted(() => {
  store.page = {
    title: route.query.title!.toString(),
    current: route.query.src!.toString(),
  }
})
</script>
