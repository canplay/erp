<template>
  <div class="row q-col-gutter-x-lg q-col-gutter-y-md">
    <div class="col-12 col-md-6">
      <div class="text-subtitle1 q-mb-sm">记分牌控制</div>
      <q-card class="q-pa-md dark-card">
        <div class="q-gutter-sm">
          <q-input
            filled
            :model-value="titleText1"
            label="第一行文字"
            @update:model-value="onTitleText1Change"
          />
          <q-input
            filled
            :model-value="titleText2"
            label="第二行文字"
            @update:model-value="onTitleText2Change"
          />
          <q-input
            filled
            :model-value="titleText3"
            label="第三行文字"
            @update:model-value="onTitleText3Change"
          />

          <q-select
            filled
            v-model.number="internalDisplayScale"
            label="缩放"
            :options="scaleOptions"
            option-value="value"
            option-label="label"
            emit-value
            map-options
          />
        </div>

        <div class="q-mt-md">
          <q-btn color="secondary" label="全屏显示" @click="onEnterFullscreen" class="q-mr-sm" />
          <q-btn color="negative" label="重置记分牌" @click="onResetAll" class="q-mr-sm" />
          <q-btn
            color="primary"
            label="在新窗口中显示记分牌"
            @click="openScoreboardWindow"
            icon="open_in_new"
          />
        </div>
      </q-card>
    </div>

    <div class="col-12 col-md-6">
      <div class="text-subtitle1 q-mb-sm">时钟控制</div>
      <ClockControls
        :current-time="currentTime"
        :is-running="isRunning"
        :is-countdown="isCountdown"
        @toggle-clock="onToggleClock"
        @reset-clock="onResetClock"
        @adjust-clock="onAdjustClock"
        @set-clock-time="onSetClockTime"
        @countdown-change="onCountdownChange"
      />
    </div>

    <div class="col-12 col-md-6">
      <div class="text-subtitle1 q-mb-sm">主队控制</div>
      <TeamControls
        :team-name="homeTeamName"
        :team-score="homeTeamScore"
        :is-home="true"
        @name-change="(value) => onTeamNameChange('homeTeam', value)"
        @score-adjust="(delta) => onScoreAdjust(true, delta)"
        @score-reset="() => onScoreReset(true)"
        @logo-select="() => onSelectLogo(true)"
      />
    </div>

    <div class="col-12 col-md-6">
      <div class="text-subtitle1 q-mb-sm">客队控制</div>
      <TeamControls
        :team-name="guestTeamName"
        :team-score="guestTeamScore"
        :is-home="false"
        @name-change="(value) => onTeamNameChange('guestTeam', value)"
        @score-adjust="(delta) => onScoreAdjust(false, delta)"
        @score-reset="() => onScoreReset(false)"
        @logo-select="() => onSelectLogo(false)"
      />
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue';
import TeamControls from './TeamControls.vue';
import ClockControls from './ClockControls.vue';

interface ScaleOption {
  label: string;
  value: number;
}

interface Props {
  titleText1: string;
  titleText2: string;
  titleText3: string;
  displayScale: number;
  homeTeamName: string;
  guestTeamName: string;
  homeTeamScore: number;
  guestTeamScore: number;
  currentTime: number;
  isRunning: boolean;
  isCountdown: boolean;
}

interface Emits {
  (e: 'update:titleText1', value: string): void;
  (e: 'update:titleText2', value: string): void;
  (e: 'update:titleText3', value: string): void;
  (e: 'update:displayScale', value: number): void;
  (e: 'update:homeTeamName', value: string): void;
  (e: 'update:guestTeamName', value: string): void;
  (e: 'update:homeTeamScore', value: number): void;
  (e: 'update:guestTeamScore', value: number): void;
  (e: 'update:currentTime', value: number): void;
  (e: 'update:isRunning', value: boolean): void;
  (e: 'update:isCountdown', value: boolean): void;
  (e: 'enter-fullscreen'): void;
  (e: 'reset-all'): void;
  (e: 'team-name-change', team: 'homeTeam' | 'guestTeam', value: string): void;
  (e: 'score-adjust', isHome: boolean, delta: number): void;
  (e: 'score-reset', isHome: boolean): void;
  (e: 'select-logo', isHome: boolean): void;
  (e: 'toggle-clock'): void;
  (e: 'reset-clock'): void;
  (e: 'adjust-clock', delta: number): void;
  (e: 'set-clock-time', time: number): void;
  (e: 'countdown-change', value: boolean): void;
}

const props = defineProps<Props>();
const emit = defineEmits<Emits>();

const scaleOptions: ScaleOption[] = [
  { label: '25%', value: 0.25 },
  { label: '50%', value: 0.5 },
  { label: '75%', value: 0.75 },
  { label: '100%', value: 1 },
  { label: '125%', value: 1.25 },
  { label: '150%', value: 1.5 },
  { label: '175%', value: 1.75 },
  { label: '200%', value: 2 },
];

// 内部使用的缩放值，用于处理q-select的选择
const internalDisplayScale = computed({
  get: () => props.displayScale,
  set: (value) => {
    // 确保emit的是一个数值而不是对象
    const numericValue = typeof value === 'object' ? (value as ScaleOption).value : Number(value);
    emit('update:displayScale', numericValue);
  },
});

// 标题文本的事件处理函数，完全参考TeamControls的模式
const onTitleText1Change = (value: string | number | null) => {
  emit('update:titleText1', String(value || ''));
};

const onTitleText2Change = (value: string | number | null) => {
  emit('update:titleText2', String(value || ''));
};

const onTitleText3Change = (value: string | number | null) => {
  emit('update:titleText3', String(value || ''));
};

const onTeamNameChange = (team: 'homeTeam' | 'guestTeam', value: string | number | null) => {
  emit('team-name-change', team, String(value || ''));
};

const onScoreAdjust = (isHome: boolean, delta: number) => {
  emit('score-adjust', isHome, delta);
};

const onScoreReset = (isHome: boolean) => {
  emit('score-reset', isHome);
};

const onSelectLogo = (isHome: boolean) => {
  emit('select-logo', isHome);
};

const onEnterFullscreen = () => {
  emit('enter-fullscreen');
};

const onResetAll = () => {
  emit('reset-all');
};

const onToggleClock = () => {
  emit('toggle-clock');
};

const onResetClock = () => {
  emit('reset-clock');
};

const onAdjustClock = (delta: number) => {
  emit('adjust-clock', delta);
};

const onSetClockTime = (time: number) => {
  emit('set-clock-time', time);
};

const onCountdownChange = (value: boolean) => {
  emit('countdown-change', value);
};

// 打开独立记分牌窗口
const openScoreboardWindow = () => {
  // 构造记分牌页面的URL，使用hash模式，注意要与路由配置中的路径匹配
  // 添加id参数，确保新窗口能获取到正确的记分牌数据
  const url = `${window.location.origin}${window.location.pathname}#/scoreboard-display`;

  // 使用window.open打开新窗口
  window.open(url, 'scoreboard', 'width=1200,height=800,resizable=yes,scrollbars=yes');
};
</script>

<style scoped>
.dark-card {
  background-color: rgba(255, 255, 255, 0.05);
}
</style>
