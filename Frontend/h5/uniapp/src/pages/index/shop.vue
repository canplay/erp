<template>
  <div
    style="
      width: 100vw;
      height: 100vh;
      display: flex;
      justify-content: center;
      align-items: center;
      font-size: 48px;
    "
  >
    <div>
      <div style="display: flex; justify-content: center; align-items: center">
        <image
          style="width: 128px; height: 128px"
          src="../../static/logo.png"
        />
      </div>
      <div style="color: #333">七都商合</div>
      <div style="height: 16px" />
      <div style="display: flex; justify-content: center; align-items: center">
        <div style="color: #dd524d; font-size: 32px">载入中...</div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { onLoad } from '@dcloudio/uni-app';

onLoad(() => {
  setTimeout(() => {
    uni.redirectTo({
      url: 'index?method=orderList',
    });
  }, 1000);
});
</script>
