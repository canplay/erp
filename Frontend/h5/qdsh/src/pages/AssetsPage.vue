<template>
  <q-page>
    <q-card square>
      <q-card-section>
        <div class="text-h6">筛选</div>
        <q-separator />
        <q-select v-model="select.type.model" :options="select.type.options" label="资产类型" />
        <q-select v-model="select.status.model" :options="select.status.options" label="资产状态" />
      </q-card-section>
    </q-card>

    <div style="height: 8px" />

    <q-infinite-scroll @load="onLoad">
      <div
        v-for="(item, index) in list"
        :key="index"
        style="width: calc(100vw - 16px); padding: 4px"
      >
        <q-card>
          <q-item>
            <q-item-section
              @click="
                router.push({
                  path: '/view',
                  query: { id: item.id },
                })
              "
            >
              <q-item-label class="ellipsis" lines="1">
                {{ item.name }}
              </q-item-label>
              <q-item-label class="ellipsis" caption lines="2">
                所属：{{ item.deed }}
              </q-item-label>
              <q-item-label class="ellipsis" caption lines="1">
                面积：{{ item.area }}
              </q-item-label>
            </q-item-section>
          </q-item>

          <!-- <iframe
            seamless
            frameborder="0"
            sandbox="allow-same-origin allow-scripts allow-top-navigation allow-popups"
            :src="item.preview"
            style="width: 100%; height: 250px"
          /> -->
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
import type { AssetsItem } from 'src/components/models/assets'
import type { CommonResponse } from 'src/components/models/common'
import { useStore } from 'src/stores/store'
import { onMounted, ref } from 'vue'
import { useRouter } from 'vue-router'

const quasar = useQuasar()
const router = useRouter()
const store = useStore()

const list = ref<AssetsItem[]>([])

const select = ref({
  type: {
    model: '全部',
    options: ['全部', '固定资产', '经营权'],
  },
  status: {
    model: '全部',
    options: ['全部', '出售', '租赁', '待处置'],
  },
})

const onLoad = (index: number, done: (stop?: boolean) => void) => {
  alova
    .Post<CommonResponse<AssetsItem[] | string>>(
      `${store.server.erp}/post?service=cbis&auth=public&path=/api/project/public/list`,
      {
        curPage: index - 1,
        maxPage: 1,
      },
    )
    .then((resp) => {
      if (index - 1 >= (resp.data as AssetsItem[]).length) {
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

      list.value = resp.data as AssetsItem[]
      list.value.sort((a: AssetsItem, b: AssetsItem) => {
        return parseInt(b.id) - parseInt(a.id)
      })

      done()
    })
    .catch(() => {
      quasar.notify({
        message: '网络错误，请稍后重试',
        type: 'negative',
        position: 'top',
      })
    })
}

onMounted(() => {
  store.page = {
    title: '',
    current: '/',
  }
})
</script>
