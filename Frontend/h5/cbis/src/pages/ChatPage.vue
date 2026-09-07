<template>
  <q-page>
    <iframe
      title="iframe"
      name="iframe"
      src="https://chat.cm.example.com"
      frameborder="0"
      style="width: 100%; height: calc(100vh - 58px)"
      ref="iframeRef"
    />
  </q-page>
</template>

<script setup lang="ts">
import { onMounted, onBeforeUnmount } from 'vue';
import { useStore } from 'stores/store';
import { ref } from 'vue';

const store = useStore();

const iframeRef = ref<HTMLIFrameElement | null>(null);

const handleMessage = (ev: MessageEvent) => {
  console.log(ev.data.channel, ev.data.msg);
};

onMounted(() => {
  window.addEventListener('message', handleMessage);

   iframeRef.value!.onload = () => {
     iframeRef.value!.contentWindow!.postMessage(
      {
        channel: 'chatLogin',
        msg: {
          username: String(store.user.username) + '@vip.qq.com',
          password: store.user.password,
        },
      },
      'https://chat.cm.example.com'
    );
    // window.electron.setLogin('https://chat.example.com', store.user.username, store.user.password);
  };
});

onBeforeUnmount(() => {
  window.removeEventListener('message', handleMessage);
});
</script>
