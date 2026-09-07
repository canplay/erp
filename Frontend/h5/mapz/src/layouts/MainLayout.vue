<template>
  <q-layout
    view="lHh Lpr lFf"
    style="
      background-image: url('imgs/bg.png');
      background-size: cover;
      background-repeat: no-repeat;
      background-position: center;
    "
  >
    <q-page-container>
      <router-view />
    </q-page-container>

    <q-footer elevated class="bg-grey-2 text-black">
      <q-toolbar>
        <q-btn flat stack class="fit" label="立即预约" icon="img:imgs/phone.png" @click="onCall" />

        <q-btn
          flat
          stack
          class="fit"
          label="我的预约"
          icon="img:imgs/todo.png"
          @click="dialog = !dialog"
        />
      </q-toolbar>
    </q-footer>

    <DialogTodo v-model:show="dialog" />
  </q-layout>
</template>

<script setup lang="ts">
import DialogTodo from 'src/components/DialogTodo.vue';
import { ref } from 'vue';

const dialog = ref(false);

const onCall = () => {
  const isWechat = /MicroMessenger/i.test(navigator.userAgent);
  const dialUrl = isWechat ? `tel:+8613312763472#mp.weixin.qq.com` : `tel:+8613800000000`;

  const link = document.createElement('a');
  link.href = dialUrl;
  document.body.appendChild(link);
  link.click();
  document.body.removeChild(link);
};
</script>
