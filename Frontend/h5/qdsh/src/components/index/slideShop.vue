<template>
  <q-card style="margin: 0 8px 0 8px">
    <q-card-section class="text-h6 text-center row">
      <div class="col flex items-center">
        <div style="border-top: 1px solid #000; width: 100%" />
      </div>
      <div class="col">{{ title }}</div>
      <div class="col flex items-center">
        <div style="border-top: 1px solid #000; width: 100%" />
      </div>
    </q-card-section>
    <q-card-section>
      <q-scroll-area style="height: 130px">
        <div class="row no-wrap">
          <q-btn
            v-for="item in list"
            :key="item.id"
            flat
            square
            style="width: 120px; margin-right: 8px"
            @click="
              openURL(`https://sc.ebaoton.cn/front_page/productdetail/${item.id}?siteId=24305`)
            "
          >
            <q-avatar class="bg-grey-4" rounded size="120px">
              <q-img :src="item.photo" />
            </q-avatar>
            <div
              class="absolute-bottom"
              style="
                font-size: 10px;
                text-align: center;
                padding: 4px;
                color: white;
                background-color: rgba(0, 0, 0, 0.6);
              "
            >
              {{ item.itemName }}
            </div>
          </q-btn>
        </div>
      </q-scroll-area>
    </q-card-section>
  </q-card>
</template>

<script setup lang="ts">
import { openURL } from 'quasar'
import type { ShopIndexItem } from '../models/shop'

defineProps<{
  title: string
  list: ShopIndexItem[]
}>()
</script>
