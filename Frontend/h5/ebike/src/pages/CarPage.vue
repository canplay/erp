<template>
  <q-page padding class="bg-grey">
    <q-splitter
      v-model="splitter.model"
      :limits="[50, 99]"
      :disable="!splitter.show"
      ref="container"
    >
      <template v-slot:before>
        <q-table
          :rows="table.rows"
          :columns="table.columns"
          row-key="code"
          separator="cell"
          v-model:pagination="table.pagination"
          :rows-per-page-options="[10, 20, 30, 40, 50, 0]"
        >
          <template v-slot:top>
            <div class="fit">
              <div class="row">
                <q-input class="col" v-model="search.code" label="识别码" />

                <div class="col-auto" style="width: 5px" />

                <q-input class="col" v-model="search.provide" label="运营商" />

                <div class="col-auto" style="width: 5px" />

                <q-select
                  class="col"
                  v-model="search.status.model"
                  :options="search.status.options"
                  label="状态"
                />
              </div>

              <div style="height: 5px" />

              <div class="row">
                <q-input
                  class="col"
                  filled
                  v-model="search.time.start"
                  label="启动时间"
                >
                  <template v-slot:append>
                    <q-icon name="event" class="cursor-pointer">
                      <q-popup-proxy
                        ref="qDateProxy"
                        transition-show="scale"
                        transition-hide="scale"
                      >
                        <q-date v-model="search.time.start" mask="YYYY-MM-DD">
                          <div class="row items-center justify-end">
                            <q-btn
                              v-close-popup
                              label="关闭"
                              color="primary"
                              flat
                            />
                          </div>
                        </q-date>
                      </q-popup-proxy>
                    </q-icon>
                  </template>
                </q-input>

                <div class="col-auto" style="width: 5px" />

                <q-input
                  class="col"
                  filled
                  v-model="search.time.end"
                  label="关闭时间"
                >
                  <template v-slot:append>
                    <q-icon name="event" class="cursor-pointer">
                      <q-popup-proxy
                        ref="qDateProxy"
                        transition-show="scale"
                        transition-hide="scale"
                      >
                        <q-date v-model="search.time.end" mask="YYYY-MM-DD">
                          <div class="row items-center justify-end">
                            <q-btn
                              v-close-popup
                              label="关闭"
                              color="primary"
                              flat
                            />
                          </div>
                        </q-date>
                      </q-popup-proxy>
                    </q-icon>
                  </template>
                </q-input>

                <div class="col-auto" style="width: 5px" />

                <q-btn
                  class="col-2"
                  color="red"
                  label="导出"
                  @click="onExport"
                />

                <div class="col-auto" style="width: 5px" />

                <q-btn
                  class="col-2"
                  color="primary"
                  label="查询"
                  @click="onSearch"
                />
              </div>
            </div>
          </template>

          <template v-slot:body="props">
            <q-tr :props="props">
              <q-td key="code" :props="props">
                <q-btn
                  color="primary"
                  :label="props.row.code"
                  @click="onHistory(props.row.code)"
                />
              </q-td>
              <q-td key="provide" :props="props">
                {{ props.row.provide }}
              </q-td>
              <q-td key="status" :props="props">
                {{ props.row.status }}
              </q-td>
              <q-td key="speed" :props="props">
                {{ props.row.speed }}
              </q-td>
              <q-td key="time_start" :props="props">
                {{ props.row.time_start }}
              </q-td>
              <q-td key="time_end" :props="props">
                {{ props.row.time_end }}
              </q-td>
              <q-td key="alert" :props="props">
                {{ props.row.alert }}
              </q-td>
              <q-td key="remark" :props="props">
                {{ props.row.remark }}
              </q-td>
            </q-tr>
          </template>
        </q-table>
      </template>

      <template v-slot:after v-if="splitter.show">
        <q-inner-loading :showing="!init">
          <q-spinner-gears size="50px" color="primary" />
        </q-inner-loading>

        <div
          v-if="init"
          class="absolute full-width"
          style="
            background-color: rgba(255, 255, 255, 0.5);
            backdrop-filter: blur(2px);
          "
        >
          <div class="row">
            <q-input
              class="col"
              filled
              v-model="track.time.start"
              label="开始时间"
            >
              <template v-slot:append>
                <q-icon name="event" class="cursor-pointer">
                  <q-popup-proxy
                    ref="qDateProxy"
                    transition-show="scale"
                    transition-hide="scale"
                  >
                    <q-date v-model="track.time.start" mask="YYYY-M-D">
                      <div class="row items-center justify-end">
                        <q-btn
                          v-close-popup
                          label="关闭"
                          color="primary"
                          flat
                        />
                      </div>
                    </q-date>
                  </q-popup-proxy>
                </q-icon>
              </template>
            </q-input>

            <div class="col-auto" style="width: 5px" />

            <q-input
              class="col"
              filled
              v-model="track.time.end"
              label="结束时间"
            >
              <template v-slot:append>
                <q-icon name="event" class="cursor-pointer">
                  <q-popup-proxy
                    ref="qDateProxy"
                    transition-show="scale"
                    transition-hide="scale"
                  >
                    <q-date v-model="track.time.end" mask="YYYY-M-D">
                      <div class="row items-center justify-end">
                        <q-btn
                          v-close-popup
                          label="关闭"
                          color="primary"
                          flat
                        />
                      </div>
                    </q-date>
                  </q-popup-proxy>
                </q-icon>
              </template>
            </q-input>
          </div>

          <q-select
            v-model="track.model"
            :options="track.options"
            label="历史记录"
            @input="onTrackChange"
          />
        </div>

        <div class="fit">
          <iframe
            class="fit"
            src="map.html"
            frameborder="0"
            seamless
            ref="webview"
          />
        </div>
      </template>
    </q-splitter>
  </q-page>
</template>

<script setup lang="ts">
import type { QTableProps } from 'quasar';
import { date, exportFile } from 'quasar';
import gcoord from 'gcoord';
import { useStore } from 'src/stores/store';
import { onBeforeUnmount, onMounted, ref, computed } from 'vue';
import { api } from 'src/boot/axios';

const store = useStore();

const init = ref(false);
const splitter = ref({
  model: 100,
  show: false,
});
const search = ref({
  code: '',
  provide: '',
  status: {
    model: '',
    options: ['', '离线', '停止中', '骑行中', '故障', '其他'],
  },
  time: {
    start: '',
    end: '',
  },
});
const track = ref({
  model: '',
  options: [] as string[],
  list: new Map<string, unknown>(),
  time: {
    start: '',
    end: '',
  },
});
const table = ref({
  pagination: {
    rowsPerPage: 10,
    sortBy: 'time_start' as const,
    descending: false,
    options: [10, 20, 30, 40, 50, '全部'],
  },
  columns: [
    {
      name: 'code',
      align: 'center',
      label: '识别码',
      field: 'code',
      sortable: true,
    },
    {
      name: 'provide',
      align: 'center',
      label: '运营商',
      field: 'provide',
      sortable: true,
    },
    {
      name: 'status',
      align: 'center',
      label: '状态',
      field: 'status',
      sortable: true,
    },
    {
      name: 'speed',
      align: 'center',
      label: '速度',
      field: 'speed',
      sortable: true,
    },
    {
      name: 'time_start',
      align: 'center',
      label: '启动时间',
      field: 'time_start',
      sortable: true,
    },
    {
      name: 'time_end',
      align: 'center',
      label: '关闭时间',
      field: 'time_end',
      sortable: true,
    },
    {
      name: 'alert',
      align: 'center',
      label: '警告',
      field: 'alert',
    },
    {
      name: 'remark',
      align: 'center',
      label: '备注',
      field: 'remark',
    },
  ] as QTableProps['columns'],
  rows: [] as Array<Record<string, unknown>>,
});
interface CarInfo {
  code: string;
  provide: string;
  speed: number;
  status: string;
  gps: { lng: string; lat: string };
  time: { start: string; end: string };
  alert: string;
  remark: string;
}

interface CustomWindow {
  clear?: () => void;
  setCar?: (...args: unknown[]) => void;
  setCenter?: (...args: unknown[]) => void;
}

// 状态映射表，统一处理 0-4 的状态枚举
const statusMap = computed<Record<number, string>>(() => ({
  0: '离线',
  1: '停止中',
  2: '骑行中',
  3: '故障',
  4: '其他',
}));

const webview = ref<CustomWindow | null>(null);

const onMessage = (event: MessageEvent) => {
  switch (event.data.method) {
    case 'init':
      init.value = true;
      break;
  }
};

const wrapCsvValue = (val: unknown, formatFn?: ((val: unknown, row?: unknown) => unknown)) => {
  const formatted = formatFn !== undefined ? formatFn(val) : val;

  const result =
    formatted === undefined || formatted === null ? '' : JSON.stringify(formatted);

  const escaped = result.split('"').join('""');

  return `${escaped}`;
};

const onExport = () => {
  const time = setTimeout(() => {
    store.quasar.loading.hide();
    clearTimeout(time);
  }, 120000);

  store.quasar.loading.show();

  const content = [table.value.columns!.map((col) => wrapCsvValue(col.label))]
    .concat(
      table.value.rows.map((row) =>
        table.value
          .columns!.map((col) =>
            wrapCsvValue(
              typeof col.field === 'function'
                ? col.field(row)
                : row[col.field === void 0 ? col.name : col.field],
              col.format
            )
          )
          .join(',')
      )
    )
    .join('\r\n');

  const status = exportFile('车辆列表.xlsx', content, 'text/xlsx');

  if (status !== true) {
    store.quasar.notify('正在准备下载，请稍后...');
  }

  store.quasar.loading.hide();
  clearTimeout(time);
};

const onSearch = () => {
  table.value.rows = [];

   let statusValue = -1;
   const statusValueMap = statusMap.value;
   // 使用字符串比较，与 search.status.options 的值匹配
   switch (search.value.status.model) {
     case '离线':
       statusValue = 0;
       break;
     case '停止中':
       statusValue = 1;
       break;
     case '骑行中':
       statusValue = 2;
       break;
     case '故障':
       statusValue = 3;
       break;
     case '其他':
       statusValue = 4;
       break;
   }

  api
    .post(store.backend.private + '/car', {
      method: 'query',
      code: search.value.code,
      provide: search.value.provide,
      status: statusValue,
      time: {
        start: search.value.time.start,
        end: search.value.time.end,
      },
    })
    .then((resp) => {
      if (resp.data != '' && resp.data != 'error') {
        for (let i = 0; i < resp.data.length; i++) {
          const element = resp.data[i];

          if (element.code === '') break;

           const statusStr = statusValueMap[element.status] ?? '未知';

           table.value.rows.push({
             code: element.code,
             provide: element.provide,
             speed: element.speed,
             status: statusStr,
            time_start: element.time.start,
            time_end: element.time.end,
            alert: element.alert,
            remark: element.remark,
          });
        }
      }
    })
    .catch(() => {});
};

const onHistory = (code: string) => {
  splitter.value.show = true;
  splitter.value.model = 50;

  track.value.time.start = '';
  track.value.time.end = '';
  track.value.model = '';
  track.value.options = [];
  track.value.list = new Map();

  if (webview.value != null && webview.value != undefined) {
    webview.value.clear?.();
  }

  api
    .post(store.backend.private + '/car', {
      method: 'history',
      code: code,
    })
    .then((resp) => {
      if (resp.data != '' && resp.data != 'error') {
        for (let i = 0; i < resp.data.length; i++) {
          const element = resp.data[i];

          if (element.code === '') break;

          let pt: Array<number>;
          if (element.gps_type === 1) {
            pt = gcoord.transform(
              [parseFloat(element.gps.lng), parseFloat(element.gps.lat)],
              gcoord.WGS84,
              gcoord.BD09
            );
          } else {
            pt = [parseFloat(element.gps.lng), parseFloat(element.gps.lat)];
          }

          track.value.options.push(
            date.formatDate(element.create_date, 'YYYY-MM-DD HH:mm:ss') +
              ' (' +
              i +
              ')'
          );

           const statusValueMap = statusMap.value;
           const statusStr = statusValueMap[element.status] ?? '未知';

            track.value.list.set(
            date.formatDate(element.create_date, 'YYYY-MM-DD HH:mm:ss') +
              ' (' +
              i +
              ')',
            {
              code: element.code,
              provide: element.provide,
              speed: element.speed,
              status: statusStr,
              gps: pt,
              time: element.time,
              alert: element.alert,
              remark: element.remark,
            }
          );
        }
      }
    })
    .catch(() => {});
};

const onTrackChange = (val: string) => {
  const v = track.value.list.get(val) as CarInfo;
  const ww = webview.value as CustomWindow;
  ww.clear?.();
  ww.setCar?.(
    [
      {
        geometry: {
          type: 'Point',
          coordinates: [v.gps.lng, v.gps.lat],
        },
        properties: {
          code: v.code,
          info:
            '识别码：' +
            v.code +
            '<br />' +
            '状态：' +
            v.status +
            '<br />' +
            '运营商：' +
            v.provide +
            '<br />' +
            '速度：' +
            v.speed +
            '<br />' +
            '最近启动时间：' +
            v.time.start +
            '<br />' +
            '最近关闭时间：' +
            v.time.end +
            '<br />' +
            '警告：' +
            v.alert +
            '<br />' +
            '备注：' +
            v.remark,
        },
      },
    ],
    [],
    [],
    []
  );
  ww.setCenter?.(v.gps.lng, v.gps.lat, 18);
};


onMounted(() => {
  window.addEventListener('message', onMessage);
});

onBeforeUnmount(() => {
  window.removeEventListener('message', onMessage);
});
</script>