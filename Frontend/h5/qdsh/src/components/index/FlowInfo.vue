<template>
  <q-infinite-scroll class="row q-col-gutter-sm" @load="onLoad">
    <div class="col-6" v-for="item in list" :key="item.id">
      <q-card
        @click="
          router.push({
            path: '/view',
            query: { id: item.id },
          })
        "
        style="max-width: 250px"
      >
        <q-item>
          <q-item-section avatar>
            <q-avatar>
              <q-img src="/imgs/noimg.svg" />
            </q-avatar>
          </q-item-section>

          <q-item-section>
            <q-item-label lines="1">
              {{ item.title }}
            </q-item-label>
            <q-item-label caption lines="1">
              {{ item.author }}
            </q-item-label>
          </q-item-section>
        </q-item>

        <q-img fit="cover" :src="item.preview" style="height: 189px" />
      </q-card>
    </div>

    <template v-slot:loading>
      <div class="fixed-center">
        <q-spinner-facebook color="primary" size="40px" />
      </div>
    </template>
  </q-infinite-scroll>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import { useStore } from 'src/stores/store'
import { alova } from 'src/boot/axios'
import { useQuasar } from 'quasar'
import { useRouter } from 'vue-router'
import type { CommonNews, CommonResponse } from '../models/common'

const quasar = useQuasar()
const router = useRouter()
const store = useStore()

const list = ref<CommonNews[]>([])

const onLoad = (index: number, done: (stop?: boolean) => void) => {
  alova
    .Post<CommonResponse<number | string>>(
      `${store.server.erp}/post?service=cbis&auth=jwt&path=/api/News/count`,
      {
        class: 1,
      },
    )
    .then((resp) => {
      if (index - 1 >= (resp.data as number)) {
        done(true)
        return
      }

      if (!resp.status) {
        done(true)
        quasar.notify({
          message: resp.message,
          type: 'negative',
          position: 'top',
        })
        return
      }

      alova
        .Post<CommonResponse<CommonNews[] | string>>(
          `${store.server.erp}/post?service=cbis&auth=jwt&path=/api/News/list`,
          {
            class: 1,
            sortBy: 'date',
            descending: true,
            curPage: index - 1,
            maxPage: 1,
          },
        )
        .then((resp) => {
          if (!resp.status) {
            done(true)
            quasar.notify({
              message: resp.message,
              type: 'negative',
              position: 'top',
            })
            return
          }

          list.value.push((resp.data as CommonNews[])[0]!)

          done()
        })
        .catch(() => {
          done(true)
          quasar.notify({
            message: '网络错误，请稍后重试',
            type: 'negative',
            position: 'top',
          })
        })
    })
    .catch(() => {
      done(true)
      quasar.notify({
        message: '网络错误，请稍后重试',
        type: 'negative',
        position: 'top',
      })
    })
}
</script>
