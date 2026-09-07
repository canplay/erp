<template>
  <q-card class="q-pa-md dark-card">
    <div class="text-center">
      <div class="text-h4 q-my-md">
        {{ formattedTime }}
      </div>
      <div class="q-gutter-sm">
        <q-btn color="primary" label="启动/停止" @click="onToggleClock" />
        <q-btn color="info" label="重置" @click="onResetClock" />
        <q-btn color="accent" label="+10秒" @click="() => onAdjustClock(10)" />
        <q-btn color="accent" label="-10秒" @click="() => onAdjustClock(-10)" />
        <q-toggle v-model="countdownMode" label="倒计时模式" />
      </div>
    </div>

    <div class="q-mt-md">
      <div class="row q-col-gutter-x-sm">
        <div class="col">
          <q-input filled type="number" v-model.number="minutesInput" label="分钟" :min="0" />
        </div>
        <div class="col-auto self-center">:</div>
        <div class="col">
          <q-input
            filled
            type="number"
            v-model.number="secondsInput"
            label="秒钟"
            :min="0"
            :max="59"
          />
        </div>
        <div class="col-12">
          <q-btn color="secondary" label="设置时间" @click="onSetClockTime" class="full-width" />
        </div>
      </div>
    </div>
  </q-card>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue';

interface Props {
  currentTime: number;
  isRunning: boolean;
  isCountdown: boolean;
}

interface Emits {
  (e: 'toggle-clock'): void;
  (e: 'reset-clock'): void;
  (e: 'adjust-clock', delta: number): void;
  (e: 'set-clock-time', time: number): void;
  (e: 'countdown-change', value: boolean): void;
}

const props = withDefaults(defineProps<Props>(), {
  currentTime: 0,
  isRunning: false,
  isCountdown: false,
});
const emit = defineEmits<Emits>();

const minutesInput = ref(10);
const secondsInput = ref(0);

// 计算属性：格式化时间显示
const formattedTime = computed(() => {
  const totalSeconds = Math.abs(props.currentTime);
  const minutes = Math.floor(totalSeconds / 60);
  const seconds = totalSeconds % 60;
  return `${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`;
});

// 响应属性变化，更新输入框值
const countdownMode = computed<boolean>({
  get: () => props.isCountdown,
  set: (value) => emit('countdown-change', value),
});

const onToggleClock = () => {
  emit('toggle-clock');
};

const onResetClock = () => {
  emit('reset-clock');
};

const onAdjustClock = (delta: number) => {
  emit('adjust-clock', delta);
};

const onSetClockTime = () => {
  const totalTime = minutesInput.value * 60 + secondsInput.value;
  emit('set-clock-time', totalTime);
};
</script>

<style scoped>
.dark-card {
  background-color: rgba(255, 255, 255, 0.05);
}
</style>
