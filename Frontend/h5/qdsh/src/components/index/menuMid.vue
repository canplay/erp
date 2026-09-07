<template>
  <q-card>
    <q-card-section class="row q-pa-sm">
      <q-btn
        v-for="(item, index) in list"
        :key="index"
        class="col-3"
        flat
        stack
        size="14px"
        :icon="`img:${item.appimg}`"
        :label="item.name"
        @click="
          openURL(`https://sc.ebaoton.cn/front_page/speciallist?specialId=${item.url}&siteId=24305`)
        "
      />
    </q-card-section>

    <q-card-section class="row q-pa-sm">
      <q-btn
        class="col-3"
        flat
        stack
        size="14px"
        href="https://api.wsscxt.com/driver/wechat/home/103"
      >
        <q-img src="imgs/index/parking.svg" width="30.86px" />
        国资车位
      </q-btn>

      <q-btn class="col-3" flat stack size="14px" href="https://swp.wsqxgis.com/Hall">
        <q-img src="imgs/index/water.svg" width="30.86px" />
        城市供水
      </q-btn>

      <q-btn
        class="col-3"
        flat
        stack
        size="14px"
        href="weixin://dl/business/?appid=wx0a9d3adf8de6db7a&path=pages/index"
      >
        <q-img src="imgs/index/sanitation.svg" width="30.86px" />
        环卫清运
      </q-btn>

      <!-- <q-btn class="col-3" flat stack size="14px" to="/game">
        <q-img src="imgs/index/entertainment.svg" width="30.86px" />
        文化娱乐
      </q-btn> -->

      <q-btn class="col-3" flat stack size="14px" to="/assets">
        <q-img src="imgs/index/assets.svg" width="30.86px" />
        资产盘活
      </q-btn>
    </q-card-section>
    <!-- <q-btn class="col" flat stack size="14px" @click="onGoto('mp-weixin', 'wx185c106a0a4f5461')">
        <q-img src="imgs/index/repair.svg" />
        维修
      </q-btn> -->
  </q-card>
</template>

<script setup lang="ts">
import { openURL } from 'quasar'

defineProps(['list'])
</script>
