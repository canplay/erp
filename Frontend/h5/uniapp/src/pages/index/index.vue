<template>
  <web-view :src="url" @error="onError" @message="onMessage"></web-view>
</template>

<script setup lang="ts">
import { onLoad } from '@dcloudio/uni-app';
import { ref } from 'vue';

const url = ref('https://qdsh-n.cm.example.com');
// const url = ref('https://frps.cm.example.com');

const updateManager = uni.getUpdateManager();

updateManager.onCheckForUpdate((res) => {
  console.log(`onCheckForUpdate: ${res.hasUpdate}`);
});

updateManager.onUpdateReady(() => {
  uni.showModal({
    title: '提示',
    content: '新版本已经准备好, 是否重启应用？',
    success(res) {
      if (res.confirm) {
        updateManager.applyUpdate();
      }
    },
  });
});

updateManager.onUpdateFailed(() => {
  uni.showModal({
    title: '提示',
    content: '新版本下载失败, 是否重试？',
    success(res) {
      if (res.confirm) {
        uni.exitMiniProgram();
      }
    },
  });
});

const onError = (ev: any) => {
  uni.showModal({
    title: '提示',
    content: '加载失败, 是否重试？',
    success(res) {
      if (res.confirm) {
        uni.reLaunch({
          url: 'index',
        });
      } else if (res.cancel) {
        uni.exitMiniProgram();
      }
    },
  });
};

const onMessage = (ev: any) => {
  switch (ev.detail.data.action) {
    case 'location':
      uni.getLocation({
        type: 'wgs84',
        success: function (res) {
          console.log(`当前位置的经度: ${res.longitude}`);
          console.log(`当前位置的纬度 ${res.latitude}`);
        },
      });
      break;
  }
};

onLoad((params) => {
  if (params && params.method) {
    switch (params.method) {
      case 'redirect':
        url.value += params.url;
        break;
      case 'orderList':
        url.value += '?method=orderList';
        break;
      default:
        url.value += `?method=${params.method}`;
        break;
    }
  }
});
</script>
