<template>
  <q-page class="bg-info">
    <q-scroll-area :style="store.height">
      <div class="row flex fixed-top z-top" style="margin: 8px">
        <q-card class="col" style="background-color: rgba(255, 255, 255, 0.9)">
          <q-card-section class="row">
            <div class="col text-primary text-h4">今日订单</div>

            <div class="col-2 text-primary text-h5 flex-center">0</div>
          </q-card-section>
        </q-card>

        <div class="col-auto" style="width: 8px" />

        <q-card class="col" style="background-color: rgba(255, 255, 255, 0.9)">
          <q-card-section class="row">
            <div class="col text-primary text-h4">今日收入</div>

            <div class="col-2 text-primary text-h5 flex-center">0</div>
          </q-card-section>
        </q-card>

        <div class="col-auto" style="width: 8px" />

        <q-card class="col" style="background-color: rgba(255, 255, 255, 0.9)">
          <q-card-section class="row">
            <div class="col text-primary text-h4">今日支出</div>

            <div class="col-2 text-primary text-h5 flex-center">0</div>
          </q-card-section>
        </q-card>

        <div class="col-auto" style="width: 8px" />

        <q-card class="col" style="background-color: rgba(255, 255, 255, 0.9)">
          <q-card-section class="row">
            <div class="col text-primary text-h4">今日工单</div>

            <div class="col-2 text-primary text-h5 flex-center">0</div>
          </q-card-section>
        </q-card>
      </div>

      <div class="row flex fixed-top z-top" style="margin: 88px 8px 8px 8px">
        <q-card class="col" style="background-color: rgba(255, 255, 255, 0.9)">
          <q-card-section class="row">
            <div class="col text-primary text-h4">本月订单</div>

            <div class="col-2 text-primary text-h5 flex-center">0</div>
          </q-card-section>
        </q-card>

        <div class="col-auto" style="width: 8px" />

        <q-card class="col" style="background-color: rgba(255, 255, 255, 0.9)">
          <q-card-section class="row">
            <div class="col text-primary text-h4">本月收入</div>

            <div class="col-2 text-primary text-h5 flex-center">0</div>
          </q-card-section>
        </q-card>

        <div class="col-auto" style="width: 8px" />

        <q-card class="col" style="background-color: rgba(255, 255, 255, 0.9)">
          <q-card-section class="row">
            <div class="col text-primary text-h4">本月支出</div>

            <div class="col-2 text-primary text-h5 flex-center">0</div>
          </q-card-section>
        </q-card>

        <div class="col-auto" style="width: 8px" />

        <q-card class="col" style="background-color: rgba(255, 255, 255, 0.9)">
          <q-card-section class="row">
            <div class="col text-primary text-h4">本月工单</div>

            <div class="col-2 text-primary text-h5 flex-center">0</div>
          </q-card-section>
        </q-card>
      </div>

      <div class="row flex fixed-bottom z-top" style="margin: 8px">
        <q-card class="col" style="background-color: rgba(255, 255, 255, 0.9)">
          <q-card-section>
            <div class="col text-primary text-h4">一周订单走势</div>

            <q-separator />

            <div id="weekOrder" style="min-height: 300px" />
          </q-card-section>
        </q-card>

        <div class="col-auto" style="width: 8px" />

        <q-card class="col" style="background-color: rgba(255, 255, 255, 0.9)">
          <q-card-section>
            <div class="col text-primary text-h4">一周收入</div>

            <q-separator />

            <div id="weekIncome" style="min-height: 300px" />
          </q-card-section>
        </q-card>
      </div>

      <bmap style="height: calc(100vh - 50px)" />
    </q-scroll-area>
  </q-page>
</template>

<script setup lang="ts">
import { onMounted } from 'vue';
import * as echarts from 'echarts';
import { useStore } from 'src/stores/store';

const store = useStore();

type EChartsOption = echarts.EChartsOption;

let weekOrderChart: echarts.ECharts;
let weekIncomeChart: echarts.ECharts;

// ECharts DOM 元素类型
type EChartsDOM = HTMLElement & { __echarts_instance__?: echarts.ECharts };

const initWeekOrder = () => {
  const dom = document.getElementById('weekOrder') as EChartsDOM;
  weekOrderChart = echarts.init(dom);

  const option: EChartsOption = {
    tooltip: {
      trigger: 'axis',
      axisPointer: {
        type: 'shadow',
      },
    },
    grid: {
      top: '10%',
      left: '1%',
      right: '1%',
      bottom: '1%',
      containLabel: true,
    },
    xAxis: {
      type: 'category',
      data: ['周一', '周二', '周三', '周四', '周五', '周六', '周日'],
    },
    yAxis: {
      type: 'value',
    },
    series: [
      {
        data: [150, 230, 224, 218, 135, 147, 260],
        type: 'line',
      },
    ],
  };

  if (option && typeof option === 'object') {
    weekOrderChart.setOption(option);
  }
};

const initWeekIncome = () => {
  const dom = document.getElementById('weekIncome') as EChartsDOM;
  weekIncomeChart = echarts.init(dom);
  const option: EChartsOption = {
    tooltip: {
      trigger: 'axis',
      axisPointer: {
        type: 'shadow',
      },
    },
    grid: {
      top: '10%',
      left: '1%',
      right: '1%',
      bottom: '1%',
      containLabel: true,
    },
    xAxis: [
      {
        type: 'category',
        data: ['周一', '周二', '周三', '周四', '周五', '周六', '周日'],
        axisTick: {
          alignWithLabel: true,
        },
      },
    ],
    yAxis: [
      {
        type: 'value',
      },
    ],
    series: [
      {
        name: 'Direct',
        type: 'bar',
        barWidth: '60%',
        data: [10, 52, 200, 334, 390, 330, 220],
      },
    ],
  };

  if (option && typeof option === 'object') {
    weekIncomeChart.setOption(option);
  }
};

const onResize = () => {
  weekOrderChart.resize();
  weekIncomeChart.resize();
};

addEventListener('resize', onResize);

onMounted(() => {
  initWeekOrder();
  initWeekIncome();
});
</script>
