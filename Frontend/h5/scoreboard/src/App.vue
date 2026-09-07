<template>
  <router-view />
</template>

<script setup lang="ts">
import { useScoreboardStore } from 'src/stores/scoreboard-store';
import { onMounted, onUnmounted } from 'vue';

const scoreboardStore = useScoreboardStore();

// 全局键盘事件处理函数
const handleKeyDown = (e: KeyboardEvent) => {
  // 只有在非输入状态下才处理快捷键
  if ((e.target as Element).tagName === 'INPUT' || (e.target as Element).tagName === 'TEXTAREA') {
    return;
  }

  switch (e.key.toLowerCase()) {
    case 'i': // 主队加分
    case 'h':
      e.preventDefault();
      scoreboardStore.increaseScore('home');
      break;
    case 'k': // 主队减分
      e.preventDefault();
      scoreboardStore.decreaseScore('home');
      break;
    case 'o': // 客队加分
    case 'g':
      e.preventDefault();
      scoreboardStore.increaseScore('guest');
      break;
    case 'l': // 客队减分
      e.preventDefault();
      scoreboardStore.decreaseScore('guest');
      break;
    case 't': // 切换时钟
      e.preventDefault();
      scoreboardStore.toggleClock();
      break;
    case 'u': // 增加时间
      e.preventDefault();
      scoreboardStore.adjustTime(1);
      break;
    case 'j': // 减少时间
      e.preventDefault();
      scoreboardStore.adjustTime(-1);
      break;
    case 'f': // 全屏切换
      e.preventDefault();
      toggleFullscreen();
      break;
    case '+': // 增大显示比例
    case '=':
      e.preventDefault();
      adjustDisplayScale(0.1);
      break;
    case '-': // 缩小显示比例
      e.preventDefault();
      adjustDisplayScale(-0.1);
      break;
    case '0': // 重置显示比例
      e.preventDefault();
      resetDisplayScale();
      break;
    case 'r': // 重置分数
      e.preventDefault();
      scoreboardStore.resetScores();
      break;
    // 移除了与多记分牌相关的快捷键功能，现在只有一个记分牌
  }
};

// 切换全屏
const toggleFullscreen = () => {
  if (!document.fullscreenElement) {
    document.documentElement
      .requestFullscreen()
      .then(() => {})
      .catch(() => {});
  } else {
    if (document.exitFullscreen) {
      document
        .exitFullscreen()
        .then(() => {})
        .catch(() => {});
    }
  }
};

// 调整显示比例
const adjustDisplayScale = (delta: number) => {
  const current = scoreboardStore.currentScoreboard();
  if (current) {
    let newScale = current.displayScale + delta;
    // 限制缩放范围
    newScale = Math.max(0.25, Math.min(2.0, newScale));
    scoreboardStore.setDisplayScale(newScale);
  }
};

// 重置显示比例
const resetDisplayScale = () => {
  const current = scoreboardStore.currentScoreboard();
  if (current) {
    scoreboardStore.setDisplayScale(1.0);
  }
};

onMounted(() => {
  window.addEventListener('keydown', handleKeyDown);
});

onUnmounted(() => {
  window.removeEventListener('keydown', handleKeyDown);
});
</script>