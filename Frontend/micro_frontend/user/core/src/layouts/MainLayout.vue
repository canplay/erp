<template>
  <q-layout view="hHh Lpr fFf">
    <q-header elevated style="height: 64px">
      <div class="row items-center" style="margin: 8px">
        <q-input class="col" dark standout dense label="搜索" v-model="search" />

        <div class="col-auto" style="width: 8px" />

        <q-btn class="col-auto" round color="white" @click="toggleLeftDrawer">
          <q-avatar>
            <q-img
              src="https://gips3.baidu.com/it/u=2475428638,3303591043&fm=3028&app=3028&f=JPEG&fmt=auto?w=1920&h=2560"
            />
          </q-avatar>
        </q-btn>
      </div>
    </q-header>

    <q-page-container>
      <router-view />
    </q-page-container>

    <q-footer class="row" elevated style="height: 64px">
      <q-btn class="col fit" flat stack square color="white" label="1" />
      <q-btn class="col fit" flat stack square color="white" label="2" />
      <q-btn class="col fit" flat stack square color="white" label="3" />
    </q-footer>

    <q-drawer v-model="leftDrawerOpen" show-if-above bordered>
      <q-scroll-area class="fit">
        <q-list>
          <q-item-label header> Essential Links </q-item-label>
        </q-list>
      </q-scroll-area>
    </q-drawer>
  </q-layout>
</template>

<script setup lang="ts">
import { ref } from 'vue';

const leftDrawerOpen = ref(false);

const toggleLeftDrawer = () => {
  leftDrawerOpen.value = !leftDrawerOpen.value;
};

const search = ref('');
</script>
