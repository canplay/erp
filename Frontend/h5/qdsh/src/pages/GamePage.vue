<template>
  <q-page>
    <q-card square>
      <q-card-section>
        <div class="text-h6">筛选</div>
        <q-separator />
        <q-select v-model="select.type.model" :options="select.type.options" label="类型" />
      </q-card-section>
    </q-card>

    <q-infinite-scroll @load="onLoad">
      <div v-for="(item, index) in list" :key="index" style="padding: 4px">
        <q-card @click="onGoto(item)">
          <q-item>
            <q-item-section>
              <q-item-label class="ellipsis" lines="1">
                {{ item.name }}
              </q-item-label>
              <q-item-label class="ellipsis" caption lines="2">
                介绍：{{ item.desc }}
              </q-item-label>
            </q-item-section>

            <q-item-section side top>
              <q-item-label caption>{{ getClassName(item.class) }}</q-item-label>
              <q-item-label>来源: {{ item.src }}</q-item-label>
            </q-item-section>
          </q-item>

          <q-img :src="item.preview" style="width: 100%; height: 250px" />
        </q-card>
      </div>
    </q-infinite-scroll>
  </q-page>
</template>

<script setup lang="ts">
import type { CommonGoto } from 'src/components/models/common'
import type { GameList } from 'src/components/models/game'
import { useStore } from 'src/stores/store'
import { onMounted, ref } from 'vue'
import { useRouter } from 'vue-router'

const router = useRouter()
const store = useStore()

const select = ref({
  type: {
    model: '全部',
    options: ['全部', '游戏', '旅游', '书籍'],
  },
})
const list = ref<GameList[]>([])

const getClassName = (val: string) => {
  switch (val) {
    case 'travel':
      return '旅游'
    case 'book':
      return '书籍'
    default:
      return '游戏'
  }
}

const onGoto = async (item: CommonGoto) => {
  switch (item.type) {
    case 'iframe':
      await router.push({
        path: '/iframe',
        query: {
          title: '首页',
          src: '/',
          target: 'https://qdsh.cm.example.com/game',
        },
      })
      break
    default:
      await router.push(item.url)
      break
  }
}

const onLoad = (index: number, done: (stop?: boolean) => void) => {
  done(true)
}

onMounted(() => {
  store.page = {
    title: '首页',
    current: '/',
  }

  list.value.push({
    id: 1,
    name: '自在修仙',
    desc: '文字类修仙游戏',
    class: 'game',
    preview: 'imgs/noimg.svg',
    type: 'iframe',
    src: '商管公司',
    url: 'https://qdsh.cm.example.com/game',
  })
})
</script>
