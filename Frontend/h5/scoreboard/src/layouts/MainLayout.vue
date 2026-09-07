<template>
  <q-layout view="hHh lpR fff">
    <q-header elevated>
      <q-toolbar>
        <q-toolbar-title>
          记分牌系统
        </q-toolbar-title>

        <q-tabs align="right">
          <q-route-tab to="/" label="首页" />
          <q-route-tab to="/control" label="控制面板" />
          <q-route-tab to="/about" label="关于" />
        </q-tabs>
      </q-toolbar>
    </q-header>

    <q-page-container>
      <router-view />
    </q-page-container>
  </q-layout>
</template>

<script setup lang="ts">
// 移除了对scoreboardStore的引用，因为不再需要打开新标签页
</script>