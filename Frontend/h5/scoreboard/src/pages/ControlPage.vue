<template>
  <q-page>
    <div class="q-pa-md" v-if="!isFullscreenMode">
      <!-- 标签页导航 -->
      <div class="tab-container">
        <q-tabs
          v-model="currentTab"
          dense
          active-color="primary"
          indicator-color="primary"
          class="text-teal"
          align="left"
          narrow-indicator
          inline-label
        >
          <q-tab name="display" label="记分牌显示" />
          <q-tab name="control" label="控制面板" />
        </q-tabs>
      </div>

      <!-- 标签页内容 -->
      <div class="content q-mt-lg">
        <q-tab-panels v-model="currentTab" animated class="bg-transparent full-height">
          <!-- 记分牌显示页面 -->
          <q-tab-panel name="display" class="q-pa-none">
            <ScoreboardDisplay
              :titleText1="currentScoreboardText1"
              :titleText2="currentScoreboardText2"
              :titleText3="currentScoreboardText3"
              :home-team-name="currentScoreboard!.homeTeam.name || '主队'"
              :guest-team-name="currentScoreboard!.guestTeam.name || '客队'"
              :home-team-score="currentHomeTeamScore"
              :guest-team-score="currentGuestTeamScore"
              :home-team-logo-url="currentScoreboard!.homeTeam.logoUrl || ''"
              :guest-team-logo-url="currentScoreboard!.guestTeam.logoUrl || ''"
              :current-time="currentScoreboardCurrentTime"
              :display-scale="currentScoreboardDisplayScale"
            />
          </q-tab-panel>

          <!-- 控制面板页面 -->
          <q-tab-panel name="control" class="q-pa-lg">
            <ControlPanel
              :title-text1="currentScoreboardText1"
              :title-text2="currentScoreboardText2"
              :title-text3="currentScoreboardText3"
              :display-scale="currentScoreboardDisplayScale"
              :home-team-name="currentScoreboard?.homeTeam.name || '主队'"
              :guest-team-name="currentScoreboard?.guestTeam.name || '客队'"
              :home-team-score="currentHomeTeamScore"
              :guest-team-score="currentGuestTeamScore"
              :current-time="currentScoreboardCurrentTime"
              :is-running="currentIsRunning"
              :is-countdown="currentIsCountdown"
              @update:title-text1="updateCurrentScoreboardProperty('titleText1', $event)"
              @update:title-text2="updateCurrentScoreboardProperty('titleText2', $event)"
              @update:title-text3="updateCurrentScoreboardProperty('titleText3', $event)"
              @update:display-scale="updateCurrentScoreboardProperty('displayScale', $event)"
              @team-name-change="onTeamNameChange"
              @score-adjust="adjustScore"
              @score-reset="resetScore"
              @select-logo="selectLogo"
              @enter-fullscreen="enterFullscreen"
              @reset-all="resetAll"
              @toggle-clock="toggleClock"
              @reset-clock="resetClock"
              @adjust-clock="adjustClock"
              @set-clock-time="setClockTime"
              @countdown-change="updateCurrentScoreboardProperty('isCountdown', $event)"
            />
          </q-tab-panel>
        </q-tab-panels>
      </div>
    </div>

    <!-- 全屏显示记分牌 -->
    <div
      v-else
      class="fullscreen bg-black text-white text-center"
      ref="fullscreenDiv"
      style="display: flex; justify-content: center; align-items: center"
    >
      <ScoreboardDisplay
        :titleText1="currentScoreboardText1"
        :titleText2="currentScoreboardText2"
        :titleText3="currentScoreboardText3"
        :home-team-name="currentScoreboard?.homeTeam.name || '主队'"
        :guest-team-name="currentScoreboard?.guestTeam.name || '客队'"
        :home-team-score="currentHomeTeamScore"
        :guest-team-score="currentGuestTeamScore"
        :home-team-logo-url="currentScoreboard?.homeTeam.logoUrl || ''"
        :guest-team-logo-url="currentScoreboard?.guestTeam.logoUrl || ''"
        :current-time="currentScoreboardCurrentTime"
        :display-scale="currentScoreboardDisplayScale"
      />

      <!-- 全屏退出按钮 -->
      <q-btn
        class="absolute-top-right q-ma-md"
        color="white"
        text-color="black"
        label="退出全屏"
        @click="exitFullscreen"
      />
    </div>
  </q-page>
</template>

<script setup lang="ts">
import { ref, onMounted, computed, nextTick, watch } from 'vue';
import { useRoute } from 'vue-router';
import { useScoreboardStore, type ScoreboardData } from 'src/stores/scoreboard-store';
import { useQuasar } from 'quasar';
import ScoreboardDisplay from '../components/ScoreboardDisplay.vue';
import ControlPanel from '../components/ControlPanel.vue';

const scoreboardStore = useScoreboardStore();
const $q = useQuasar();
const route = useRoute();
const currentTab = ref<string>('control'); // 默认显示控制面板
const isFullscreenMode = ref(false);
const fullscreenDiv = ref<HTMLDivElement | null>(null);

// 根据路由参数设置初始标签页
onMounted(() => {
  // 检查路由参数中是否有指定标签页
  if (route.query.tab === 'display') {
    currentTab.value = 'display'; // 切换到记分牌显示标签页
  } else {
    currentTab.value = 'control'; // 默认控制面板标签页
  }

  // 确保至少有一个记分牌
  if (scoreboardStore.scoreboards.length === 0) {
    scoreboardStore.createScoreboard();
  }
});

// 监听路由参数变化，如果在控制面板中切换标签页后路由参数变化，则更新标签页
watch(
  () => route.query.tab,
  (newTab) => {
    if (newTab === 'display' || newTab === 'control') {
      currentTab.value = newTab;
    }
  },
);

// 计算属性：获取当前记分牌
const currentScoreboard = computed(() => {
  return scoreboardStore.currentScoreboard();
});

// 提取常用数据为计算属性，减少模板中的重复引用
const currentScoreboardText1 = computed({
  get: () => currentScoreboard.value?.titleText1 || '',
  set: (value) => scoreboardStore.updateScoreboardProperty('titleText1', value),
});

const currentScoreboardText2 = computed({
  get: () => currentScoreboard.value?.titleText2 || '',
  set: (value) => scoreboardStore.updateScoreboardProperty('titleText2', value),
});

const currentScoreboardText3 = computed({
  get: () => currentScoreboard.value?.titleText3 || '',
  set: (value) => scoreboardStore.updateScoreboardProperty('titleText3', value),
});

// 添加watch监听器确保ControlPanel的更新能触发store同步
const updateCurrentScoreboardProperty = (
  property: keyof ScoreboardData,
  value: ScoreboardData[keyof ScoreboardData],
) => {
  scoreboardStore.updateScoreboardProperty(property, value);
};

const currentScoreboardDisplayScale = computed({
  get: () => {
    const scale = currentScoreboard.value?.displayScale || 1;
    return scale;
  },
  set: (value) => {
    scoreboardStore.updateScoreboardProperty('displayScale', value);
  },
});

const currentHomeTeamScore = computed(() => currentScoreboard.value?.homeTeam.score || 0);
const currentGuestTeamScore = computed(() => currentScoreboard.value?.guestTeam.score || 0);

const currentIsRunning = computed(() => currentScoreboard.value?.isRunning || false);
const currentIsCountdown = computed({
  get: () => currentScoreboard.value?.isCountdown || false,
  set: (value) => scoreboardStore.updateScoreboardProperty('isCountdown', value),
});

const currentScoreboardCurrentTime = computed(() => currentScoreboard.value?.currentTime || 0);

const adjustScore = (isHome: boolean, delta: number) => {
  scoreboardStore.updateTeamScore(isHome, delta);
};

const resetScore = (isHome: boolean) => {
  scoreboardStore.resetTeamScore(isHome);
};

const adjustClock = (delta: number) => {
  scoreboardStore.adjustClock(delta);
};

const toggleClock = () => {
  scoreboardStore.toggleClock();
};

const resetClock = () => {
  scoreboardStore.resetClock();
};

const setClockTime = () => {
  const minutes = Number(
    +(document.querySelector('input[type="number"][aria-label="分钟"]') as HTMLInputElement)
      ?.value || 10,
  );
  const seconds = Number(
    +(document.querySelector('input[type="number"][aria-label="秒钟"]') as HTMLInputElement)
      ?.value || 0,
  );
  const totalTime = minutes * 60 + seconds;
  scoreboardStore.setClockTime(totalTime);
};

const selectLogo = (isHome: boolean) => {
  // 在Web环境中，我们需要使用标准的文件输入对话框
  const input = document.createElement('input');
  input.type = 'file';
  input.accept = 'image/*';
  input.onchange = (e) => {
    const target = e.target as HTMLInputElement;
    if (target.files && target.files.length > 0) {
      const file = target.files[0];
      if (file) {
        // 确保file不为undefined
        const reader = new FileReader();
        reader.onload = (event) => {
          if (event.target?.result) {
            scoreboardStore.setTeamLogo(isHome, event.target.result as string);
          }
        };
        reader.readAsDataURL(file);
      }
    }
  };
  input.click();
};

const onTeamNameChange = (team: 'homeTeam' | 'guestTeam', value: string) => {
  const currentTeam = { ...currentScoreboard.value?.[team] };
  if (currentTeam) {
    currentTeam.name = value;
    scoreboardStore.updateScoreboardProperty(team, {
      name: currentTeam.name,
      score: currentTeam.score ?? 0,
      logo: currentTeam.logo ?? null,
      logoUrl: currentTeam.logoUrl ?? '',
    });
  }
};

const enterFullscreen = async () => {
  // 直接设置全屏模式为true，这样会显示全屏的记分牌视图
  isFullscreenMode.value = true;
  await nextTick(); // 确保DOM已更新

  try {
    // 检查 Quasar 全屏 API 是否可用
    if ($q.fullscreen && typeof $q.fullscreen.request === 'function') {
      // 尝试使用专门的全屏div
      if (fullscreenDiv.value) {
        await $q.fullscreen.request(fullscreenDiv.value);
      } else {
        // 如果全屏div不存在，则查找记分牌显示区域
        const element = document.querySelector('.fullscreen.bg-black.text-white.text-center');
        if (element) {
          await $q.fullscreen.request(element);
        }
      }
    } else {
      // 如果 Quasar 全屏 API 不可用，回退到原生 API
      let element: HTMLElement | null = fullscreenDiv.value;
      if (!element) {
        element = document.querySelector(
          '.fullscreen.bg-black.text-white.text-center',
        ) as HTMLElement;
      }

      if (element) {
        await element.requestFullscreen();
      } else {
        // 如果找不到特定元素，则全屏 body
        await document.body.requestFullscreen();
      }
    }
  } catch (error) {
    console.warn('进入全屏时发生错误，使用备选方案:', error);
    // 即使全屏API失败，也要确保UI状态正确
    try {
      if (!document.fullscreenElement) {
        let element: HTMLElement | null = fullscreenDiv.value;
        if (!element) {
          element =
            (document.querySelector(
              '.fullscreen.bg-black.text-white.text-center',
            ) as HTMLElement) || document.body;
        }
        await element.requestFullscreen();
      }
    } catch (fallbackError) {
      console.error('全屏请求失败:', fallbackError);
      // 即使全屏失败，也要保持 isFullscreenMode 为 true，以显示全屏记分牌
    }
  }
};

const exitFullscreen = async () => {
  try {
    // 检查 Quasar 全屏 API 是否可用
    if ($q.fullscreen && typeof $q.fullscreen.exit === 'function' && $q.fullscreen.isActive) {
      await $q.fullscreen.exit();
    } else if (document.fullscreenElement) {
      // 如果Quasar全屏未激活，尝试使用原生API退出全屏
      await document.exitFullscreen();
    }
  } catch (error) {
    console.warn('退出全屏时发生错误:', error);
    // 如果API调用失败，仍然将状态设置为非全屏模式
    try {
      // 尝试使用原生API强制退出全屏
      if (document.fullscreenElement) {
        await document.exitFullscreen();
      }
    } catch (nativeError) {
      console.error('原生API退出全屏也失败:', nativeError);
    }
  }
  isFullscreenMode.value = false;
};

const resetAll = () => {
  scoreboardStore.resetAll();
};

// 初始化状态恢复
onMounted(() => {
  // 确保页面加载时从store获取最新状态
  scoreboardStore.loadFromLocalStorage();
});
</script>

<style scoped>
.dark-card {
  background-color: rgba(255, 255, 255, 0.05);
}

.tab-container {
  display: flex;
  align-items: center;
  flex-wrap: wrap;
  gap: 10px;
}

.tabs {
  flex: 1;
}
</style>
