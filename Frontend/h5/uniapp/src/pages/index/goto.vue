<template>
  <div
    style="
      width: 100vw;
      height: 100vh;
      display: flex;
      justify-content: center;
      align-items: center;
      font-size: 48rpx;
    "
  >
    <div>
      <div>即将打开链接, 请确认</div>

      <div style="height: 32px" />

      <button class="btn" @click="onBack">
        <div>返回</div>
      </button>

      <div style="height: 8px" />

      <button class="btn" @click="onConfirm">
        <div>确认</div>
      </button>
    </div>
  </div>
</template>

<style lang="scss" scoped>
.btn {
  width: 80%;
  border-radius: 20rpx;
  background-color: #1976d2;
  color: #fff;
}
</style>

<script setup lang="ts">
import { onLoad } from '@dcloudio/uni-app';
import { ref } from 'vue';

const url = ref({
  method: '',
  url: '',
});

const onBack = () => {
  uni.redirectTo({ url: 'index' });
};

const onConfirm = () => {
  uni.navigateToMiniProgram({
    appId: url.value.url,
  });
};

onLoad((params) => {
  url.value.method = params!.method;
  url.value.url = decodeURIComponent(params!.url);

  if (url.value.method === 'redirect') {
    uni.redirectTo({
      url: `index?method=redirect&url=${url.value.url}`,
    });
  }
});
</script>
