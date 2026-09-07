<template>
  <q-banner
    v-if="!store.signin || tip"
    dense
    inline-actions
    class="bg-negative text-white fixed-bottom"
  >
    立即登录使用更多服务
    <template v-slot:action>
      <q-btn flat color="white" label="取消" @click="tip = !tip" />
      <q-btn flat color="white" label="登录" @click="onSingin" />
    </template>
  </q-banner>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import { useStore } from 'src/stores/store'

const store = useStore()

const tip = ref(true)

const onSingin = async () => {
  await store.signin('mp-wechat')
  // window.uniWeb.navigateBack();
  // window.uniWeb.postMessage({
  //   data: {
  //     action: 'location',
  //   },
  // });
}
</script>
