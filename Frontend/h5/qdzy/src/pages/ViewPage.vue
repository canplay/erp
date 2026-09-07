<template>
  <q-page>
    <q-scroll-area style="width: 100vw; height: calc(100vh - 56.01px)">
      <div class="row">
        <q-btn class="col-1" dense flat icon="keyboard_arrow_left" @click="router.back" />

        <div class="col-auto" style="width: 8px" />

        <q-item class="col" style="padding: 0">
          <q-item-section>
            <q-item-label lines="2">{{ info.title }}</q-item-label>
            <q-item-label caption lines="2">{{ info.update_date }}</q-item-label>
          </q-item-section>
        </q-item>
      </div>

      <div style="height: 8px" />

      <q-carousel
        v-if="list.slide.list.length > 0"
        v-model="list.slide.model"
        animated
        infinite
        autoplay
        arrows
        swipeable
        transition-prev="slide-right"
        transition-next="slide-left"
        style="height: 250px"
      >
        <q-carousel-slide
          :name="index"
          class="flex items-center"
          v-for="(item, index) in list.slide.list"
          :key="index"
          style="padding: 0"
        >
          <q-img class="fit" fit="cover" :src="item" />
        </q-carousel-slide>
      </q-carousel>

      <div v-if="list.slide.list.length > 0" style="height: 8px" />

      <div class="content" v-html="info.content" />
    </q-scroll-area>
  </q-page>
</template>

<style lang="scss" scoped>
:deep(.content) {
  font-family: '阿里妈妈方圆体 VF Thin' !important;
  max-width: 100%;
  overflow: hidden;
  word-wrap: break-word;
  white-space: normal;

  img,
  table,
  iframe {
    max-width: 100% !important;
    height: auto !important;
  }

  * {
    font-family: '阿里妈妈方圆体 VF Thin' !important;
  }
}
</style>

<script setup lang="ts">
import { date } from 'quasar'
import { useStore } from 'src/stores/store'
import { onMounted, ref } from 'vue'
import DOMPurify from 'dompurify'
import { useRoute, useRouter } from 'vue-router'
import { alova } from 'src/boot/axios'
import type { Response, News } from 'src/components/model'

const router = useRouter()
const route = useRoute()
const store = useStore()

const info = ref({
  title: '',
  preview: '',
  content: '',
  top: '',
  type: 0,
  create_date: '',
  update_date: '',
  slide: '',
})

const list = ref({
  slide: {
    model: 0,
    list: <string[]>[],
  },
})

onMounted(() => {
  store.page = {
    path: '/',
    title: '详情',
  }

  alova
    .Post<Response<News>>(`${store.server.erp}/post?service=qdzy&auth=log&path=/api/news/info`, {
      id: route.params.id,
    })
    .then((resp) => {
      info.value.title = resp.data.title
      info.value.content = DOMPurify.sanitize(decodeURIComponent(resp.data.content))
      info.value.update_date = date.formatDate(resp.data.update_date, 'YYYY-MM-DD')

      const slide = resp.data.slide?.slide
      if (slide === undefined) return
      for (let i = 0; i < slide.length; i++) {
        list.value.slide.list.push(slide[i]!.img)
      }
    })
    .catch((e) => {
      console.log(e)
    })
})
</script>
