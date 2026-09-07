<template>
  <div class="q-pa-sm">
    <div class="row">
      <q-card class="col">
        <q-card-section class="row">
          <div class="col text-primary text-h4">今日订单</div>

          <div class="col-2 text-primary text-h5 flex-center">0</div>
        </q-card-section>
      </q-card>

      <div class="col-auto" style="width: 8px" />

      <q-card class="col">
        <q-card-section class="row">
          <div class="col text-primary text-h4">今日收入</div>

          <div class="col-2 text-primary text-h5 flex-center">0</div>
        </q-card-section>
      </q-card>

      <div class="col-auto" style="width: 8px" />

      <q-card class="col">
        <q-card-section class="row">
          <div class="col text-primary text-h4">今日支出</div>

          <div class="col-2 text-primary text-h5 flex-center">0</div>
        </q-card-section>
      </q-card>

      <div class="col-auto" style="width: 8px" />

      <q-card class="col">
        <q-card-section class="row">
          <div class="col text-primary text-h4">今日工单</div>

          <div class="col-2 text-primary text-h5 flex-center">0</div>
        </q-card-section>
      </q-card>
    </div>

    <div style="height: 8px" />

    <div class="row">
      <q-card class="col">
        <q-card-section class="row">
          <div class="col text-primary text-h4">本月订单</div>

          <div class="col-2 text-primary text-h5 flex-center">0</div>
        </q-card-section>
      </q-card>

      <div class="col-auto" style="width: 8px" />

      <q-card class="col">
        <q-card-section class="row">
          <div class="col text-primary text-h4">本月收入</div>

          <div class="col-2 text-primary text-h5 flex-center">0</div>
        </q-card-section>
      </q-card>

      <div class="col-auto" style="width: 8px" />

      <q-card class="col">
        <q-card-section class="row">
          <div class="col text-primary text-h4">本月支出</div>

          <div class="col-2 text-primary text-h5 flex-center">0</div>
        </q-card-section>
      </q-card>

      <div class="col-auto" style="width: 8px" />

      <q-card class="col">
        <q-card-section class="row">
          <div class="col text-primary text-h4">本月工单</div>

          <div class="col-2 text-primary text-h5 flex-center">0</div>
        </q-card-section>
      </q-card>
    </div>

    <div style="height: 8px" />

    <div class="row">
      <q-card class="col">
        <q-card-section>
          <div class="col text-primary text-h4">一周订单走势</div>

          <q-separator />

          <div id="weekOrder" style="min-height: 300px" />
        </q-card-section>
      </q-card>

      <div class="col-auto" style="width: 8px" />

      <q-card class="col">
        <q-card-section>
          <div class="col text-primary text-h4">一周收入</div>

          <q-separator />

          <div id="weekIncome" style="min-height: 300px" />
        </q-card-section>
      </q-card>
    </div>

    <div style="height: 8px" />

    <q-card>
      <q-card-section>
        <div class="col text-primary text-h4">地图</div>

        <q-separator />

        <bmap style="min-height: 500px" />
      </q-card-section>
    </q-card>
  </div>
</template>

<script lang="ts">
import { defineComponent, onMounted } from 'vue';
import * as echarts from 'echarts';

export default defineComponent({
  name: 'CompPublicIndex',

  setup() {
    type EChartsOption = echarts.EChartsOption;

    let weekOrderChart: echarts.ECharts;
    let weekIncomeChart: echarts.ECharts;

    const initWeekOrder = () => {
      const dom = document.getElementById('weekOrder') as HTMLElement;
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
      const dom = document.getElementById('weekIncome') as HTMLElement;
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

    return {};
  },
});
</script>
