<template>
  <q-page padding>
    <q-toolbar>
      <q-tabs
        v-model="tab"
        active-color="primary"
        indicator-color="primary"
        align="justify"
        narrow-indicator
      >
        <q-tab name="all" label="所有" />
        <q-tab name="system" label="系统" />
        <q-tab name="social" label="社交" />
      </q-tabs>

      <q-space />

      <q-btn label="清除未读消息" color="primary" @click="onReadAll" />
    </q-toolbar>

    <q-separator />

    <q-tab-panels v-model="tab" animated>
      <q-tab-panel name="all">
        <q-list>
          <q-item clickable v-ripple v-for="(item, index) in store.notify.list" :key="index">
            <q-item-section avatar>
              <q-avatar>
                <q-img :src="item.img" />
              </q-avatar>
            </q-item-section>

            <q-item-section>
              <q-item-label lines="1">{{ item.title }}</q-item-label>
              <q-item-label caption lines="2">
                {{ item.content }}
              </q-item-label>
            </q-item-section>

            <q-item-section side top>
              {{ item.date }}
            </q-item-section>
          </q-item>
        </q-list>
      </q-tab-panel>

      <q-tab-panel name="system">
        <q-list>
          <q-item clickable v-ripple v-for="(item, index) in store.notify.list" :key="index">
            <q-item-section avatar>
              <q-avatar>
                <q-img :src="item.img" />
              </q-avatar>
            </q-item-section>

            <q-item-section>
              <q-item-label lines="1">{{ item.title }}</q-item-label>
              <q-item-label caption lines="2">
                {{ item.content }}
              </q-item-label>
            </q-item-section>

            <q-item-section side top>
              {{ item.date }}
            </q-item-section>
          </q-item>
        </q-list>
      </q-tab-panel>

      <q-tab-panel name="social">
        <q-list>
          <q-item clickable v-ripple v-for="(item, index) in store.notify.list" :key="index">
            <q-item-section avatar>
              <q-avatar>
                <q-img :src="item.img" />
              </q-avatar>
            </q-item-section>

            <q-item-section>
              <q-item-label lines="1">{{ item.title }}</q-item-label>
              <q-item-label caption lines="2">
                {{ item.content }}
              </q-item-label>
            </q-item-section>

            <q-item-section side top>
              {{ item.date }}
            </q-item-section>
          </q-item>
        </q-list>
      </q-tab-panel>
    </q-tab-panels>
  </q-page>
</template>

<script setup lang="ts">
import { useStore } from 'src/stores/store'
import { onMounted, ref } from 'vue'

const store = useStore()

const tab = ref('all')

const onReadAll = () => {
  for (let index = 0; index < store.notify.list.length; index++) {
    const element = store.notify.list[index]
    if (element!.status === 'unread') {
      element!.status = 'read'
      store.notify.unread -= 1
    }
  }
}

onMounted(() => {
  store.page = {
    title: '通知',
    current: '/',
  }
})
</script>
