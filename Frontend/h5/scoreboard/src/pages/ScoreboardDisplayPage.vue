<template>
  <q-page class="fullscreen bg-black text-white text-center fit">
    <ScoreboardDisplay
      :titleText1="currentScoreboard()?.titleText1 || '标题1'"
      :titleText2="currentScoreboard()?.titleText2 || '标题2'"
      :titleText3="currentScoreboard()?.titleText3 || '标题3'"
      :home-team-name="currentScoreboard()?.homeTeam.name || '主队'"
      :guest-team-name="currentScoreboard()?.guestTeam.name || '客队'"
      :home-team-score="currentScoreboard()?.homeTeam.score || 0"
      :guest-team-score="currentScoreboard()?.guestTeam.score || 0"
      :home-team-logo-url="currentScoreboard()?.homeTeam.logoUrl || ''"
      :guest-team-logo-url="currentScoreboard()?.guestTeam.logoUrl || ''"
      :current-time="currentScoreboard()?.currentTime || 0"
      :display-scale="currentScoreboard()?.displayScale || 1"
    />
  </q-page>
</template>

<script setup lang="ts">
import { onMounted, onUnmounted } from 'vue';
import {
  useScoreboardStore,
  type ScoreboardData,
  type StoredScoreboardData,
} from 'src/stores/scoreboard-store';
import ScoreboardDisplay from 'components/ScoreboardDisplay.vue';

const scoreboardStore = useScoreboardStore();
const currentScoreboard = scoreboardStore.currentScoreboard;

// 监听storage事件，确保多标签页间状态同步
const handleStorageChange = (event: StorageEvent) => {
  if (event.key === 'scoreboardState' && event.newValue) {
    try {
      // 从localStorage加载最新的状态
      const savedState = localStorage.getItem('scoreboardState');
      if (savedState) {
        const parsedState = JSON.parse(savedState);

        // 更新store中的数据
        const loadedScoreboards: ScoreboardData[] = parsedState.scoreboards.map(
          (sb: StoredScoreboardData) => ({
            ...sb,
            timerInterval: null, // 重新开始时没有活动的定时器
            homeTeam: {
              ...sb.homeTeam,
              logo: null, // File 对象无法序列化，设为 null
            },
            guestTeam: {
              ...sb.guestTeam,
              logo: null, // File 对象无法序列化，设为 null
            },
          }),
        );

        // 替换store中的数据
        scoreboardStore.scoreboards.splice(0);
        loadedScoreboards.forEach((sb: ScoreboardData) => scoreboardStore.scoreboards.push(sb));
        scoreboardStore.activeScoreboardId = parsedState.activeScoreboardId;

        // 如果记分牌之前正在运行，需要重启定时器
        const current = scoreboardStore.currentScoreboard();
        if (current && current.isRunning) {
          // 重新启动定时器
          scoreboardStore.startTimer();
        }
      }
    } catch (error) {
      console.error('更新记分牌状态失败:', error);
    }
  }
};

// 初始化键盘事件监听器
onMounted(() => {
  // 监听storage变化，确保多标签页状态同步
  window.addEventListener('storage', handleStorageChange);

  const handleKeyDown = (event: KeyboardEvent) => {
    // 防止在输入框等元素上触发快捷键
    if ((event.target as Element).tagName === 'INPUT') {
      return;
    }

    switch (event.key.toLowerCase()) {
      // 主队得分控制
      case 'h':
        scoreboardStore.increaseScore('home');
        break;
      case 'k':
        scoreboardStore.decreaseScore('home');
        break;

      // 客队得分控制
      case 'g':
        scoreboardStore.increaseScore('guest');
        break;
      case 'l':
        scoreboardStore.decreaseScore('guest');
        break;

      // 时钟控制
      case 't':
        scoreboardStore.toggleClock();
        break;
      case 'u':
        scoreboardStore.adjustTime(10);
        break;
      case 'j':
        scoreboardStore.adjustTime(-10);
        break;

      // 全屏控制
      case 'f':
        toggleFullscreen().catch(() => {});
        break;

      // 缩放控制
      case '0':
        scoreboardStore.setDisplayScale(1);
        break;
      case '+':
      case '=':
        scoreboardStore.setDisplayScale(
          Math.min(3, scoreboardStore.currentScoreboard()!.displayScale + 0.1),
        );
        break;
      case '-':
        scoreboardStore.setDisplayScale(
          Math.max(0.1, scoreboardStore.currentScoreboard()!.displayScale - 0.1),
        );
        break;
    }
  };

  // 添加键盘事件监听器
  window.addEventListener('keydown', handleKeyDown);

  // 页面卸载时移除事件监听器
  onUnmounted(() => {
    window.removeEventListener('keydown', handleKeyDown);
    window.removeEventListener('storage', handleStorageChange);
  });
});

// 全屏切换函数
const toggleFullscreen = async () => {
  if (!document.fullscreenElement) {
    await document.documentElement.requestFullscreen();
  } else {
    await document.exitFullscreen();
  }
};
</script>
