<template>
  <q-page>
    <q-inner-loading :showing="!showLoading">
      <q-spinner-gears size="50px" color="primary" />
    </q-inner-loading>

    <iframe class="fixed-full fit" src="map.html" frameborder="0" seamless ref="webview" />

    <div v-if="panel.left" class="absolute-top-left desktop-only panel-left">
      <q-card
        square
        flat
        ref="panel1"
        class="panel-overlay"
      >
        <q-card-section>
          <q-input
            outlined
            square
            bottom-slots
            debounce="500"
            label-color="white"
            class="input-transparent"
            v-model="car.search"
            label="查找"
            @input="onSearch('car')"
          >
            <template v-slot:hint>
              <div class="text-white">车辆数量：{{ car.list.length }}</div>
            </template>
          </q-input>
        </q-card-section>

        <q-card-section>
          <q-virtual-scroll ref="panelscrollarea1" :items="car.filter" separator>
            <template v-slot="{ item, index }">
              <q-item clickable v-ripple :key="index" @click="setCenter(item.gps)">
                <q-item-section side center>
                  <q-checkbox dense v-model="item.check" />
                </q-item-section>

                <q-item-section>
                  <q-item-label v-if="item.status === 0" class="text-grey">
                    {{ item.code }}<br />
                    状态：离线
                  </q-item-label>

                  <q-item-label v-else-if="item.status === 1" class="text-white">
                    {{ item.code }}<br />
                    状态：停止中
                  </q-item-label>

                  <q-item-label v-else-if="item.status === 2" class="text-positive">
                    {{ item.code }}<br />
                    状态：骑行中
                  </q-item-label>

                  <q-item-label v-else-if="item.status === 3" class="text-negative">
                    {{ item.code }}<br />
                    状态：故障
                  </q-item-label>

                  <q-item-label v-else class="text-secondary">
                    {{ item.code }}<br />
                    状态：其他
                  </q-item-label>
                </q-item-section>
              </q-item>
            </template>
          </q-virtual-scroll>
        </q-card-section>
      </q-card>
    </div>

    <div v-if="panel.right" class="absolute-top-right desktop-only panel-right">
      <q-card
        square
        flat
        ref="panel2"
        class="panel-overlay"
      >
        <q-card-section>
          <q-input
            outlined
            square
            bottom-slots
            debounce="500"
            label-color="white"
            class="input-transparent"
            v-model="storage.search"
            label="查找"
            @input="onSearch('storage')"
          >
            <template v-slot:hint>
              <div class="text-white">存放点数量：{{ storage.list.length }}</div>
            </template>
          </q-input>
        </q-card-section>

        <q-card-section>
          <q-virtual-scroll ref="panelscrollarea2" :items="storage.filter" separator>
            <template v-slot="{ item, index }">
              <q-item clickable v-ripple :key="index" @click="setCenter(item.gps)">
                <q-item-section>
                  <q-item-label class="text-white">
                    {{ item.code }}
                  </q-item-label>
                </q-item-section>
              </q-item>
            </template>
          </q-virtual-scroll>
        </q-card-section>
      </q-card>
    </div>

    <div
      class="row absolute-bottom text-right bottom-bar-bg"
    >
      <q-carousel
        v-model="slide"
        vertical
        transition-prev="slide-down"
        transition-next="slide-up"
        swipeable
        animated
        autoplay
        infinite
        control-color="white"
        height="30px"
        class="text-white col no-border"
        style="background-color: rgba(0, 0, 0, 0)"
      >
        <q-carousel-slide
          :name="index"
          v-for="(item, index) in alert"
          :key="index"
          class="text-left flex items-center"
          style="padding: 0"
        >
          {{ item.title }}
        </q-carousel-slide>
      </q-carousel>

      <q-space />

      <div class="text-red flex items-center text-bold">今日警告数：{{ alert.length }}</div>

      <div class="spacer-20" />
    </div>
  </q-page>
</template>

<script setup lang="ts">
import { onBeforeUnmount, onMounted, ref, type Ref, computed } from 'vue';
import { useStore } from 'src/stores/store';
import common from 'src/components/common';
import { api } from 'src/boot/axios';
import { date, QVirtualScroll } from 'quasar';
import gcoord from 'gcoord';
import type { AxiosResponse } from 'axios';

const store = useStore();

interface CustomWindow {
  clear?: () => void;
  setCar?: (
    run: Array<Record<string, unknown>>,
    stop: Array<Record<string, unknown>>,
    fault: Array<Record<string, unknown>>,
    alert: Array<Record<string, unknown>>,
    offline: Array<Record<string, unknown>>,
  ) => void;
  setCenter?: (lng: number | string, lat: number | string, zoom: number) => void;
  setPolygon?: (polys: Array<Record<string, unknown>>) => void;
  setText?: (texts: Array<Record<string, unknown>>) => void;
}

interface CarItem {
  code: string;
  status: number;
  gps: { lng: number | string; lat: number | string };
  check: boolean;
  time?: Record<string, string>;
  speed?: unknown;
}

interface StorageItem {
  code: string;
  status: number;
  cur: number;
  gps: { lng: number | string; lat: number | string };
  check: boolean;
  points: Array<Array<number>>;
}

interface AlertItem {
  date: string;
  title: string;
  link: string;
}

interface StatusMap {
  [key: number]: string;
}

const slide = ref(0);
const showLoading = ref(true);
const car = ref({
  search: '',
  list: [] as CarItem[],
  filter: [] as CarItem[],
  num: 0,
});
const storage = ref({
  search: '',
  list: [] as StorageItem[],
  filter: [] as StorageItem[],
  num: 0,
});
const panel = ref({
  left: true,
  right: true,
  height: 100,
});
const alert = ref<AlertItem[]>([]);

// 状态映射表，减少 switch 判断
const statusMap = computed<StatusMap>(() => ({
  0: '离线',
  1: '正常',
  2: '骑行',
  3: '故障',
  4: '其他',
}));

const webview = ref<InstanceType<typeof QVirtualScroll> & CustomWindow | null>(null);
const panel1 = ref<InstanceType<typeof QVirtualScroll> | null>(null);
const panelscrollarea1 = ref<InstanceType<typeof QVirtualScroll> | null>(null);
const panelscrollarea2 = ref<InstanceType<typeof QVirtualScroll> | null>(null);

const baiduSearch = ref('');

const onResize = (): void => {
  const containerHeight = webview.value?.$el?.parentElement?.parentElement?.clientHeight ?? 0;
  const panelHeight = containerHeight - 30;

  if (panel1.value?.$el) {
    panel1.value.$el.style.height = `${panelHeight}px`;
  }
  if (panelscrollarea1.value?.$el) {
    panelscrollarea1.value.$el.style.height = `${panelHeight - 140}px`;
  }
};

const setCenter = (gps: { lng: number | string; lat: number | string }, zoom: number = 19): void => {
  if (webview.value) {
    webview.value.setCenter?.(gps.lng, gps.lat, zoom);
  }
};

const initDraw = (): void => {
  if (webview.value) {
    webview.value.clear?.();
  }

  const createCarPayload = (method: string, token?: string) => ({
    method,
    code: '',
    provide: '',
    status: -1,
    time: { start: '', end: '' },
    ...(token ? { token } : {}),
  });

   const processCarData = (resp: AxiosResponse, listRef: Ref<{ list: CarItem[] }>) => {
    const car_run_list: Array<Record<string, unknown>> = [];
    const car_stop_list: Array<Record<string, unknown>> = [];
    const car_fault_list: Array<Record<string, unknown>> = [];
    const car_alert_list: Array<Record<string, unknown>> = [];
    const car_offline_list: Array<Record<string, unknown>> = [];
    alert.value = [];

    for (const element of resp.data) {
      if (element.code === '') continue;

      const pt =
        element.gps_type === 1
          ? gcoord.transform(
              [parseFloat(element.gps.lng), parseFloat(element.gps.lat)],
              gcoord.WGS84,
              gcoord.BD09,
            )
          : [parseFloat(element.gps.lng), parseFloat(element.gps.lat)];

      const status = statusMap.value[element.status] ?? '未知';

      const info = `识别码：${element.code}<br />运营商：${element.provide}<br />状态：${status}<br />最近启动时间：${element.time.start}<br />最近关闭时间：${element.time.end}<br />警告：${element.alert ?? ''}<br />备注：${element.remark ?? ''}`;

      const statusList = [
        car_offline_list,
        car_stop_list,
        car_run_list,
        car_fault_list,
        car_alert_list,
      ][element.status];
      statusList?.push({
        geometry: { type: 'Point', coordinates: [pt[0], pt[1]] },
        properties: { code: element.code, info },
      });

      if (listRef.value.list.length !== resp.data.length) {
        listRef.value.list.push({
          code: `${element.code} - ${element.provide}`,
          status: element.status,
          gps: { lng: pt[0] ?? 0, lat: pt[1] ?? 0 },
          time: element.time,
          speed: element.speed,
          check: true,
        });
      }

      if (element.alert && element.alert !== '') {
        alert.value.push({
          date: date.formatDate(element.update_date, 'YYYY-MM-DD HH:mm:ss'),
          title: `${element.code} [${element.provide}] - ${element.alert}`,
          link: `/alert?title=${element.alert}&date=${element.time.start}&code=${element.code}`,
        });
      }
    }

    onSearch('car');
    if (webview.value) {
      webview.value.setCar?.(
        car_run_list,
        car_stop_list,
        car_fault_list,
        car_alert_list,
        car_offline_list,
      );
    }
  };

  const processStorageData = (resp: AxiosResponse) => {
    const polys: Array<Record<string, unknown>> = [];
    const text: Array<Record<string, unknown>> = [];

    for (const element of resp.data) {
      if (element.code === '' || element.points === null || element.points === '') continue;

      const pts = element.points.split(';');
      const poly: Array<Array<number>> = [];
      const p: Array<Array<number>> = [];

      for (const pt of pts) {
        const [lng, lat] = pt.split(',');
        const coords =
          element.gps_type === 1
            ? gcoord.transform([parseFloat(lng), parseFloat(lat)], gcoord.WGS84, gcoord.BD09)
            : [parseFloat(lng), parseFloat(lat)];
        poly.push(coords);
        p.push(coords);
      }

      polys.push({
        geometry: { type: 'Polygon', coordinates: [poly] },
        properties: {
          height: 0,
          code: element.code,
          info: `识别码：${element.code}<br />警告：${element.alert ?? ''}<br />备注：${element.remark ?? ''}`,
        },
      });

      const ptc = common.getPolygonCenter(poly);
      text.push({
        geometry: { type: 'Point', coordinates: ptc },
        properties: { text: element.code },
      });

      if (poly[0]) poly.push(poly[0]);

      if (storage.value.list.length !== resp.data.length) {
        storage.value.list.push({
          code: element.code,
          status: element.status,
          cur: element.cur,
          gps: { lng: ptc[0] ?? 0, lat: ptc[1] ?? 0 },
          check: true,
          points: poly,
        });
      }

      if (element.alert && element.alert !== '') {
        alert.value.push({
          date: date.formatDate(element.update_date, 'YYYY-MM-DD HH:mm:ss'),
          title: `${element.code}[${element.provide}] - ${element.alert}`,
          link: `/alert?title=${element.alert}&date=${element.time.start}&code=${element.code}`,
        });
      }
    }

    onSearch('storage');
    if (webview.value) {
      webview.value.setPolygon?.(polys);
      webview.value.setText?.(text);
    }
  };

  // 车辆数据
  void api.post(store.backend.private + '/car', createCarPayload('query')).then((resp) => {
    processCarData(resp, car);
  });

  // 存放点数据
  void api.post(store.backend.private + '/storage', createCarPayload('query')).then((resp) => {
    processStorageData(resp);
  });
};

const onMessage = (event: MessageEvent): void => {
  switch (event.data.method) {
    case 'init': {
      showLoading.value = false;
      initDraw();
      store.addSchedule({ level: 'system', key: 'query_all', value: initDraw });
      break;
    }
    case 'search':
      baiduSearch.value = event.data.params;
      break;
    case 'notify':
      store.quasar.notify(event.data.msg);
      break;
    case 'startDraw':
      store.runPauseSchedule({ level: 'system', value: false });
      break;
    case 'endDraw': {
      const points = event.data.params
        .map((p: { lng: string; lat: string }) => `${p.lng},${p.lat}`)
        .join(';');
      const data = prompt('设置区域名称', '');

      if (data) {
        api
          .post(store.backend.private + '/storage', {
            method: 'add',
            code: data,
            provide: '喜骑出行',
            status: '正常',
            sum: 0,
            cur: 0,
            gps: { lng: '', lat: '' },
            type: 'BD',
            alert: '',
            remark: '',
            points,
          })
          .then((resp) => {
            if (resp.data) store.quasar.notify(`添加区域 ${data} 成功`);
            else store.quasar.notify(`添加区域 ${data} 失败`);
          })
          .catch(() => {});
        store.runPauseSchedule({ level: 'system', value: true });
      }
      break;
    }
    case 'delete': {
      if (!store.showDrawPanel) return;
      store.runPauseSchedule({ level: 'system', value: false });

      const isConfirmed = window.confirm(`确认删除 ${event.data.params} 吗？`);
      if (isConfirmed) {
        api
          .post(store.backend.private + '/storage', {
            method: 'delete',
            code: event.data.params,
          })
          .then((resp) => {
            if (resp.data) store.quasar.notify(`删除区域 ${event.data.params} 成功`);
            else store.quasar.notify(`删除区域 ${event.data.params} 失败`);
          })
          .catch(() => {});
        store.runPauseSchedule({ level: 'system', value: true });
      }
      break;
    }
  }
};

const onSearch = (type: 'car' | 'storage'): void => {
  const list = type === 'car' ? car.value : storage.value;
  const search = list.search;

  if (search !== '') {
    list.filter = list.list.filter((item) => item.code.search(search) !== -1);
  } else {
    list.filter = list.list;
  }
  list.num = list.filter.length;
};

onMounted(() => {
  window.addEventListener('resize', onResize);
  window.addEventListener('message', onMessage);
  onResize();
});

onBeforeUnmount(() => {
  store.delSchedule({ level: 'system', key: 'query_all' });
  window.removeEventListener('resize', onResize);
  window.removeEventListener('message', onMessage);
});
</script>
