<template>
  <q-page padding class="bg-grey row">
    <q-card
      class="col"
      style="background-color: rgba(0, 0, 0, 0.5); backdrop-filter: blur(2px)"
    >
      <q-card-section>
        <div id="echarts1" ref="echarts1" />
      </q-card-section>
    </q-card>

    <div class="col-auto" style="width: 10px" />

    <q-card
      class="col"
      style="background-color: rgba(0, 0, 0, 0.5); backdrop-filter: blur(2px)"
    >
      <q-card-section>
        <div id="echarts4" ref="echarts4" />
      </q-card-section>
    </q-card>
  </q-page>
</template>

<script setup lang="ts">
import { onMounted, ref } from 'vue';
import { date } from 'quasar';
import * as echarts from 'echarts';
import { api } from 'src/boot/axios';
import { useStore } from 'src/stores/store';

const store = useStore();

const echarts1 = ref<echarts.ECharts | null>(null);
const echarts4 = ref<echarts.ECharts | null>(null);

const draw = async () => {
  await initechart1();
  initechart4();
};

const initechart1 = async () => {
  await api.post(store.backend.private + '/car', {
    method: 'query',
    code: '',
    provide: '',
    status: -1,
    time: {
      start: '',
      end: '',
    },
  });

  const s = await api.post(store.backend.private + '/storage', {
    method: 'query',
    code: '',
    provide: '',
    status: -1,
  });

  const echarts_storages: Array<{ value: number; name: string }> = [];
  for (const element of s.data) {
    if (element.code) {
      const codeStr = typeof element.code === 'string' ? element.code : String(element.code);
      echarts_storages.push({
        value: Math.random(),
        name: codeStr,
      });
    }
  }

  const dom = document.getElementById('echarts1');
  if (dom) {
    echarts1.value = echarts.init(dom);
    echarts1.value?.setOption({
      title: {
        text: '今日提车区域占比',
        left: 'center',
        textStyle: { color: '#ffffff' },
      },
      tooltip: {
        trigger: 'item',
      },
      series: [
        {
          name: '区域',
          type: 'pie',
          radius: '50%',
          data: echarts_storages,
          emphasis: {
            itemStyle: {
              shadowBlur: 10,
              shadowOffsetX: 0,
              shadowColor: 'rgba(0, 0, 0, 0.5)',
            },
          },
        },
      ],
    });
  }
};

const initechart4 = () => {
  const dom = document.getElementById('echarts4');
  if (!dom) return;
  const echartsChart = echarts.init(dom);

  const option = {
    tooltip: {
      trigger: 'axis',
      axisPointer: {
        type: 'cross',
      },
    },
    grid: {
      left: '3%',
      right: '4%',
      bottom: '3%',
      containLabel: true,
    },
    xAxis: {
      type: 'category',
      boundaryGap: false,
      data: [] as string[],
      axisLine: {
        lineStyle: {
          color: 'white',
        },
      },
    },
    yAxis: {
      type: 'value',
      name: '元',
      boundaryGap: [0, '100%'],
      axisLine: {
        lineStyle: {
          color: 'white',
        },
      },
    },
    legend: {
      data: ['喜骑出行', '哈啰出行', '美团出行', '收益'],
      textStyle: {
        color: 'white',
      },
    },
    series: [
      { name: '喜骑出行', type: 'line', data: [] as number[] },
      { name: '哈啰出行', type: 'line', data: [] as number[] },
      { name: '美团出行', type: 'line', data: [] as number[] },
      { name: '收益', type: 'line', data: [] as number[] },
    ],
  };

  const intervalId = setInterval(() => {
    if (option.series[0] && option.series[1] && option.series[2] && option.series[3]) {
      const val = parseFloat((Math.random() * 5).toString()).toFixed(2);
      option.series[0].data.push(Number(val));
      option.series[1].data.push(0);
      option.series[2].data.push(0);
      option.series[3].data.push(0);
    }

    const axisData = date.formatDate(Date.now(), 'YYYY-MM-DD H:mm:ss');
    option.xAxis.data.push(axisData);

    echartsChart.setOption(option);
  }, 5000);

  return () => clearInterval(intervalId);
};

onMounted(async () => {
  await draw();
});
</script>
