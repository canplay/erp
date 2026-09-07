<template>
  <div
    class="bg-white text-black text-center flex justify-center items-center"
    :style="{
      width: '100%',
      height: '100%',
      minHeight: '800px',
    }"
  >
    <!-- 添加一个包装层用于缩放变换，避免在Grid容器上应用transform -->
    <div
      class="scale-wrapper"
      :style="{ transform: `scale(${displayScale})`, transformOrigin: 'center center' }"
    >
      <div class="scoreboard-content">
        <div class="row">
          <div class="col flex justify-end">
            <q-img src="/fudian.png" width="160px" fit="contain" />
          </div>
          <div style="width: 20px" />
          <div class="col flex justify-start">
            <q-img src="/ticai.png" width="175px" fit="contain" />
          </div>
        </div>

        <div style="height: 20px" />

        <div>
          <div class="text-h3 text-positive">2025/2026 {{ titleText1 }}</div>
          <span class="text-h2 text-warning text-bold">
            富滇银行<span class="text-positive">示例省份省城市足球联赛</span>
            {{ titleText2 }}
          </span>
          <div class="text-h5 text-positive">
            FUDIAN BANK - 2026/2026 YUNNAN FOOTBALL CITY LEAGUE
            {{ titleText3 }}
          </div>
        </div>

        <div style="height: 60px" />

        <div class="row">
          <div class="col">
            <!-- 主队 logo -->
            <div class="flex flex-center">
              <q-img
                v-if="homeTeamLogoUrl"
                :src="homeTeamLogoUrl"
                style="max-width: 200px; max-height: 200px"
                class="team-logo"
              />
              <q-img
                v-else
                src="/defaultlogo.png"
                style="max-width: 200px; max-height: 200px"
                class="team-logo"
              />
            </div>

            <span id="home-score" class="flex column items-center">
              <h1 class="text-h3">
                <div>
                  {{ homeTeamName }}
                </div>
              </h1>
            </span>
          </div>

          <div class="col">
            <!-- 分数字 -->
            <div class="flex justify-center">
              <img :src="getDigitImage(homeTeamScoreFirstDigit)" :width="'40%'" />
              <img :src="getDigitImage(homeTeamScoreSecondDigit)" :width="'40%'" />
            </div>
          </div>

          <div class="col flex items-center justify-center">
            <div class="text-h1">vs</div>

            <div style="height: 60%" />

            <span class="flex flex-center">
              <div class="text-h4">下半场</div>

              <div style="height: 120px" />

              <!-- 时钟数字 -->
              <div class="flex justify-center">
                <img :src="getDigitImage(Math.floor((timeInMinutes || 0) / 10))" :width="'16%'" />
                <img :src="getDigitImage((timeInMinutes || 0) % 10)" :width="'16%'" />
                <img src="/clock/colon.svg" :width="'8%'" />
                <img :src="getDigitImage(Math.floor((timeInSeconds || 0) / 10))" :width="'16%'" />
                <img :src="getDigitImage((timeInSeconds || 0) % 10)" :width="'16%'" />
              </div>
            </span>
          </div>

          <div class="col">
            <!-- 分数字 -->
            <div class="flex justify-center">
              <img :src="getDigitImage(guestTeamScoreFirstDigit)" :width="'40%'" />
              <img :src="getDigitImage(guestTeamScoreSecondDigit)" :width="'40%'" />
            </div>
          </div>

          <div class="col">
            <!-- 客队 logo -->
            <div class="flex flex-center">
              <q-img
                v-if="guestTeamLogoUrl"
                :src="guestTeamLogoUrl"
                style="max-width: 200px; max-height: 200px"
              />
              <q-img v-else src="/defaultlogo.png" style="max-width: 200px; max-height: 200px" />
            </div>

            <span class="flex column items-center">
              <h1 class="text-h3">
                <div>
                  {{ guestTeamName }}
                </div>
              </h1>
            </span>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue';

interface Props {
  titleText1: string;
  titleText2: string;
  titleText3: string;
  homeTeamName: string;
  guestTeamName: string;
  homeTeamScore: number;
  guestTeamScore: number;
  homeTeamLogoUrl?: string;
  guestTeamLogoUrl?: string;
  currentTime: number;
  displayScale: number;
}

const props = defineProps<Props>();

// 计算属性：将秒数转换为分钟和秒
const timeInMinutes = computed(() => {
  const totalSeconds = Math.abs(props.currentTime);
  return Math.floor(totalSeconds / 60);
});

const timeInSeconds = computed(() => {
  const totalSeconds = Math.abs(props.currentTime);
  return totalSeconds % 60;
});

// 分数字计算
const homeTeamScoreFirstDigit = computed(() => Math.floor(props.homeTeamScore / 10));
const homeTeamScoreSecondDigit = computed(() => props.homeTeamScore % 10);
const guestTeamScoreFirstDigit = computed(() => Math.floor(props.guestTeamScore / 10));
const guestTeamScoreSecondDigit = computed(() => props.guestTeamScore % 10);

// 获取数字图像路径
const getDigitImage = (num: number): string => {
  // 确保数字在0-9范围内
  num = Math.max(0, Math.min(9, Math.floor(num)));
  return `/clock/${num}.svg`;
};
</script>

<style scoped>
.scale-wrapper {
  /* 为缩放包装器设置相对定位 */
  position: relative;
}

.scoreboard-content {
  width: 100%;
  height: 100%;
  position: relative;
  padding: 0 20px 60px 20px;
}
</style>
