<template>
  <q-page>
    <q-carousel
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
        :name="item.id"
        class="flex items-center"
        v-for="item in list.slide.list"
        :key="item.id"
        style="padding: 0"
      >
        <q-img
          class="fit"
          fit="cover"
          :src="item.preview"
          @click="
            router.push({
              name: 'view',
              params: { type: 1, id: item.id },
            })
          "
        />
        <div
          class="absolute-bottom"
          style="
            text-align: center;
            padding: 12px;
            color: white;
            background-color: rgba(0, 0, 0, 0.6);
          "
        >
          <div style="font-size: 18px">{{ item.title }}</div>
        </div>
      </q-carousel-slide>
    </q-carousel>

    <div style="height: 8px" />

    <q-card style="margin: 0 8px 0 8px">
      <q-stepper v-model="step.model" ref="stepper" color="primary" animated>
        <q-step :name="1" title="选择房源" icon="settings" :done="step.model > 1">
          <q-select
            v-model="step.zone.model"
            :options="step.zone.options"
            option-label="label"
            option-value="value"
            label="楼盘"
          />

          <div style="height: 8px" />

          <q-select
            v-model="step.building.model"
            :options="step.building.options"
            option-label="label"
            option-value="value"
            label="楼栋/单元"
          />

          <div style="height: 8px" />

          <q-select
            v-model="step.house.model"
            :options="step.house.options"
            option-label="label"
            option-value="value"
            label="楼层/房号"
          />

          <div style="height: 8px" />

          <q-input label="朝向" outlined readonly v-model="step.orient" />

          <div style="height: 8px" />

          <q-input label="面积" outlined readonly v-model="step.area" />

          <div style="height: 8px" />

          <q-card>
            <q-card-section style="color: rgba(0, 0, 0, 0.6)">户型</q-card-section>

            <q-separator />

            <q-card-section>
              <q-img :src="step.type" />
            </q-card-section>
          </q-card>

          <div style="height: 8px" />

          <q-input label="月租金" outlined readonly v-model="step.money" />
        </q-step>
        <q-step :name="2" title="联系信息" icon="assignment" :done="step.model > 2">
          <q-input label="联系人" outlined v-model="step.contact" />

          <div style="height: 8px" />

          <q-input label="联系电话" outlined v-model="step.phone" />
        </q-step>
        <q-step :name="3" title="确认信息" icon="add_comment" :done="step.model > 3">
          <q-input label="楼盘" outlined readonly v-model="step.zone.model.label" />

          <div style="height: 8px" />

          <q-input label="楼栋/单元" outlined readonly v-model="step.building.model.label" />

          <div style="height: 8px" />

          <q-input label="楼层/房号" outlined readonly v-model="step.house.model.label" />

          <div style="height: 8px" />

          <q-input label="朝向" outlined readonly v-model="step.orient" />

          <div style="height: 8px" />

          <q-input label="面积" outlined readonly v-model="step.area" />

          <div style="height: 8px" />

          <q-input label="月租金" outlined readonly v-model="step.money" />

          <div style="height: 8px" />

          <q-input label="联系人" outlined readonly v-model="step.contact" />

          <div style="height: 8px" />

          <q-input label="联系电话" outlined readonly v-model="step.phone" />
        </q-step>

        <template v-slot:navigation>
          <q-stepper-navigation class="row">
            <q-btn
              v-if="step.model > 1"
              color="negative"
              @click="stepper.previous()"
              label="返回"
              class="col"
            />

            <div class="col-auto" style="width: 8px" />

            <q-btn
              class="col"
              @click="step.model === 3 ? onSubmit() : stepper.next()"
              color="primary"
              :label="step.model === 3 ? '提交' : '下一步'"
            />
          </q-stepper-navigation>
        </template>
      </q-stepper>
    </q-card>

    <div style="height: 8px" />

    <q-card style="margin: 0 8px 0 8px">
      <q-card-section class="text-center text-h4 text-bold">新闻公告</q-card-section>

      <q-separator />

      <q-card-section>
        <q-scroll-area style="height: 300px">
          <q-list>
            <q-item
              clickable
              v-ripple
              dense
              v-for="item in list.news.list"
              :key="item.id"
              @click="
                router.push({
                  name: 'view',
                  params: { type: 1, id: item.id },
                })
              "
            >
              <q-item-section thumbnail>
                <img :src="item.preview" />
              </q-item-section>

              <q-item-section>
                <q-item-label>{{ item.title }}</q-item-label>
              </q-item-section>

              <q-item-section side>
                <q-item-label caption>{{ item.update_date }}</q-item-label>
              </q-item-section>
            </q-item>
          </q-list>
        </q-scroll-area>
      </q-card-section>
    </q-card>

    <div style="height: 8px" />
  </q-page>
</template>

<script setup lang="ts">
import { onMounted, ref } from 'vue'
import { useRouter } from 'vue-router'
import { useStore } from 'src/stores/store'
import { alova } from 'src/boot/axios'
import type { Response, News } from 'src/components/model'
import DOMPurify from 'dompurify'
import { date, QStepper, useQuasar } from 'quasar'
import { useLogto } from '@logto/vue'

const quasar = useQuasar()
const router = useRouter()
const store = useStore()
const { isAuthenticated } = useLogto()

const stepper = ref(<QStepper>{})
const step = ref({
  model: 1,
  zone: {
    model: { label: '理想之城', value: 1 },
    options: [{ label: '理想之城', value: 1 }],
  },
  building: {
    model: { label: '1栋', value: 1 },
    options: [{ label: '1栋', value: 1 }],
  },
  house: {
    model: { label: '101', value: 1 },
    options: [{ label: '101', value: 1 }],
  },
  orient: '',
  area: '',
  type: '',
  money: '',
  contact: '',
  phone: '',
})

const list = ref({
  slide: {
    model: '1',
    list: <News[]>[],
  },
  news: {
    list: <News[]>[],
  },
  zone: {
    ownList: <News[]>[],
    oldList: <News[]>[],
    newList: <News[]>[],
    rentingList: <News[]>[],
  },
})

const onSubmit = () => {
  console.log(1)
  if (!isAuthenticated) {
    quasar
      .dialog({
        title: '提示',
        message: '您还未登录，请先注册/登录',
        ok: {
          label: '立即注册/登录',
        },
      })
      .onOk(() => {
        router.push('my').catch((e) => {
          console.log(e)
        })
      })
  }
}

onMounted(() => {
  store.page = {
    path: '/',
    title: '首页',
  }

  alova
    .Post<Response<News[]>>(
      `${store.server.erp}/post?service=qdzy&auth=log&path=/api/news/latest`,
      { top: 1, type: 1 },
    )
    .then((resp) => {
      if (resp.data === null) return

      list.value.slide.model = resp.data[0]!.id

      resp.data.forEach((element) => {
        list.value.slide.list.push({
          id: element.id,
          title: element.title,
          preview: element.preview,
          top: element.top,
          content: DOMPurify.sanitize(decodeURIComponent(element.content)),
          update_date: element.update_date,
        })
      })
    })
    .catch((e) => {
      console.log(e)
    })

  alova
    .Post<Response<News[]>>(
      `${store.server.erp}/post?service=qdzy&auth=log&path=/api/news/latest`,
      { type: 1 },
    )
    .then((resp) => {
      if (resp.data === null) return

      resp.data.forEach((element) => {
        list.value.news.list.push({
          id: element.id,
          title: element.title,
          preview: element.preview,
          top: element.top,
          content: DOMPurify.sanitize(decodeURIComponent(element.content)),
          update_date: date.formatDate(element.update_date, 'YYYY-MM-DD'),
        })
      })
    })
    .catch((e) => {
      console.log(e)
    })

  alova
    .Post<Response<News[]>>(
      `${store.server.erp}/post?service=qdzy&auth=log&path=/api/news/latest`,
      { type: 2 },
    )
    .then((resp) => {
      if (resp.data === null) return

      resp.data.forEach((element) => {
        list.value.zone.ownList.push({
          id: element.id,
          title: element.title,
          preview: element.preview,
          top: element.top,
          content: DOMPurify.sanitize(decodeURIComponent(element.content)),
          update_date: date.formatDate(element.update_date, 'YYYY-MM-DD'),
        })
      })
    })
    .catch((e) => {
      console.log(e)
    })

  alova
    .Post<Response<News[]>>(
      `${store.server.erp}/post?service=qdzy&auth=log&path=/api/news/latest`,
      { type: 3 },
    )
    .then((resp) => {
      if (resp.data === null) return

      resp.data.forEach((element) => {
        list.value.zone.newList.push({
          id: element.id,
          title: element.title,
          preview: element.preview,
          top: element.top,
          content: DOMPurify.sanitize(decodeURIComponent(element.content)),
          update_date: date.formatDate(element.update_date, 'YYYY-MM-DD'),
        })
      })
    })
    .catch((e) => {
      console.log(e)
    })

  alova
    .Post<Response<News[]>>(
      `${store.server.erp}/post?service=qdzy&auth=log&path=/api/news/latest`,
      { type: 4 },
    )
    .then((resp) => {
      if (resp.data === null) return

      resp.data.forEach((element) => {
        list.value.zone.oldList.push({
          id: element.id,
          title: element.title,
          preview: element.preview,
          top: element.top,
          content: DOMPurify.sanitize(decodeURIComponent(element.content)),
          update_date: date.formatDate(element.update_date, 'YYYY-MM-DD'),
        })
      })
    })
    .catch((e) => {
      console.log(e)
    })

  alova
    .Post<Response<News[]>>(
      `${store.server.erp}/post?service=qdzy&auth=log&path=/api/news/latest`,
      { type: 5 },
    )
    .then((resp) => {
      if (resp.data === null) return

      resp.data.forEach((element) => {
        list.value.zone.rentingList.push({
          id: element.id,
          title: element.title,
          preview: element.preview,
          top: element.top,
          content: DOMPurify.sanitize(decodeURIComponent(element.content)),
          update_date: date.formatDate(element.update_date, 'YYYY-MM-DD'),
        })
      })
    })
    .catch((e) => {
      console.log(e)
    })
})
</script>
