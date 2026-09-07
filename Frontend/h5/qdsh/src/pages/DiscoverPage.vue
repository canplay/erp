<template>
  <q-page>
    <q-card square>
      <q-card-section class="col">
        <div class="text-h6">最常访问</div>
        <q-separator />
        <q-scroll-area style="height: 72px">
          <div class="row no-wrap">
            <q-btn
              v-for="(item, index) in history"
              :key="index"
              class="col-3"
              round
              flat
              :label="item.name"
              :to="item.page"
            >
              <q-avatar>
                <q-img :src="item.avatar" />
              </q-avatar>
            </q-btn>
          </div>
        </q-scroll-area>
      </q-card-section>
    </q-card>

    <div style="height: 8px" />

    <q-infinite-scroll @load="onLoad">
      <div v-for="(item, index) in list" :key="index" style="padding: 4px">
        <q-card>
          <q-item>
            <q-item-section avatar>
              <q-avatar>
                <q-img src="/imgs/noimg.svg" />
              </q-avatar>
            </q-item-section>

            <q-item-section
              @click="
                router.push({
                  path: '/view',
                  query: { id: item.id },
                })
              "
            >
              <q-item-label lines="1">
                {{ item.author }}
              </q-item-label>
              <q-item-label caption lines="2">
                {{ item.date }}
              </q-item-label>
            </q-item-section>

            <q-item-section class="col">
              <div class="row">
                <q-btn class="col" flat round icon="img:imgs/discover/report.svg" />

                <q-btn class="col" flat round icon="img:imgs/discover/unfollow.svg" />

                <q-btn class="col" flat round icon="img:imgs/discover/share.svg" />
              </div>
            </q-item-section>
          </q-item>

          <q-item
            @click="
              router.push({
                path: '/view',
                query: { id: item.id },
              })
            "
          >
            <q-item-section>{{ item.title }}</q-item-section>
          </q-item>

          <q-img
            @click="
              router.push({
                path: '/view',
                query: { id: item.id },
              })
            "
            :src="item.preview"
            style="height: 250px"
          />
        </q-card>
      </div>

      <template v-slot:loading>
        <div class="fixed-center">
          <q-spinner-facebook color="primary" size="40px" />
        </div>
      </template>
    </q-infinite-scroll>
  </q-page>
</template>

<script setup lang="ts">
import { useQuasar } from 'quasar'
import { alova } from 'src/boot/axios'
import type { CommonNews, CommonResponse } from 'src/components/models/common'
import type { DiscoverHistory } from 'src/components/models/discover'
import { useStore } from 'src/stores/store'
import { onMounted, ref } from 'vue'
import { useRouter } from 'vue-router'
// import { marked } from 'marked';

const quasar = useQuasar()
const router = useRouter()
const store = useStore()

const list = ref<CommonNews[]>([])
const history = ref<DiscoverHistory[]>([])

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

          list.value = resp.data as CommonNews[]

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

// const more = () => {
//   quasar
//     .bottomSheet({
//       actions: [
//         {
//           label: '分享',
//           img: 'imgs/discover/share.svg',
//           id: 'share',
//         },
//         {
//           label: '取消关注',
//           img: 'imgs/discover/unfollow.svg',
//           id: 'unfollow',
//         },
//         {
//           label: '举报',
//           img: 'imgs/discover/report.svg',
//           id: 'report',
//         },
//       ],
//     })
//     .onOk(() => {
//       // console.log('Action chosen:', action.id)
//     })
//     .onCancel(() => {
//       // console.log('Dismissed')
//     })
//     .onDismiss(() => {
//       // console.log('I am triggered on both OK and Cancel')
//     });
// };

onMounted(() => {
  store.page = {
    title: '动态',
    current: '/discover',
  }

  history.value.push({
    avatar: 'imgs/noimg.svg',
    name: '示例城市A市商管',
    page: '/my',
  })
})
</script>
