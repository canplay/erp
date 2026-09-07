<template>
  <q-page padding>
    <q-scroll-area style="height: calc(100vh - 16px - 62.88px - 40px - 8px)">
      <div class="row">
        <q-btn class="col-1" dense flat icon="keyboard_arrow_left" @click="router.back" />

        <div class="col-auto" style="width: 8px" />

        <q-item class="col" style="padding: 0">
          <q-item-section avatar>
            <q-avatar>
              <q-img src="imgs/noimg.svg" />
            </q-avatar>
          </q-item-section>

          <q-item-section>
            <q-item-label lines="2">{{ info.title }}</q-item-label>
            <q-item-label caption lines="2">{{ info.date }}</q-item-label>
          </q-item-section>
        </q-item>
      </div>

      <div style="height: 8px" />

      <div class="text-h5">{{ info.title }}</div>

      <div style="height: 8px" />

      <div v-html="info.content" />

      <div style="height: 8px" />

      <q-tabs
        v-model="tabs"
        dense
        active-color="primary"
        indicator-color="primary"
        align="justify"
        narrow-indicator
      >
        <q-tab name="transpond" label="转发" />
        <q-tab name="comment" label="评论" />
      </q-tabs>

      <q-separator />

      <q-tab-panels class="transparent" v-model="tabs" animated>
        <q-tab-panel name="transpond"> </q-tab-panel>

        <q-tab-panel name="comment"> </q-tab-panel>
      </q-tab-panels>
    </q-scroll-area>

    <div class="row" style="padding-top: 8px">
      <q-input class="col" dense outlined v-model="comment" placeholder="文明发言，理性讨论" />
      <div class="col-auto" style="width: 8px" />
      <q-btn class="col-2" color="primary" dense label="发布" />
    </div>
  </q-page>
</template>

<script setup lang="ts">
import { date, useQuasar } from 'quasar';
import { useStore } from 'src/stores/store';
import { onMounted, ref } from 'vue';
import DOMPurify from 'dompurify';
import { alova } from 'src/boot/axios';
import { useRoute, useRouter } from 'vue-router';
import type { CommonNews, CommonResponse } from 'src/components/models';

const quasar = useQuasar();
const route = useRoute();
const router = useRouter();
const store = useStore();

const info = ref({
  title: '',
  date: '',
  author: '',
  content: '',
});
const tabs = ref('comment');
const comment = ref('');

onMounted(() => {
  const time = setTimeout(() => {
    quasar.loading.hide();
  }, 30000);

  quasar.loading.show({
    message: '正在处理，请稍后...',
  });

  alova
    .Post<CommonResponse<CommonNews>>(`${store.server}/api/News/info`, {
      id: parseInt(route.query.id!.toString()),
    })
    .then((resp) => {
      quasar.loading.hide();
      clearTimeout(time);

      info.value.title = resp.data!.title;
      info.value.date = date.formatDate(resp.data!.date, 'YYYY-MM-DD HH:mm:ss');
      info.value.author = resp.data!.author;
      info.value.content = DOMPurify.sanitize(decodeURIComponent(resp.data!.content));
    })
    .catch(() => {
      quasar.loading.hide();
      clearTimeout(time);
    });
});
</script>
