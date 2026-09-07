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

              <div class="row" style="height: 56px">
                <div class="col" />

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
              <q-td key="sum" :props="props">
                {{ props.row.sum }}
              </q-td>
              <q-td key="cur" :props="props">
                {{ props.row.cur }}
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
import common from 'components/common';
import { useStore } from 'src/stores/store';
import gcoord from 'gcoord';
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
    options: ['', '停用', '启用', '维护', '其他'],
  },
});
const track = ref({
  model: '',
  options: [] as string[],
  list: new Map<string, unknown>(),
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
      name: 'sum',
      align: 'center',
      label: '车辆总数',
      field: 'sum',
      sortable: true,
    },
    {
      name: 'cur',
      align: 'center',
      label: '当前数量',
      field: 'cur',
      sortable: true,
    },
  ] as QTableProps['columns'],
  rows: [] as Array<Record<string, unknown>>,
});
interface WebViewApi {
  clear?: () => void;
  setPolygon?: (data: Array<Record<string, unknown>>) => void;
  setText?: (data: Array<Record<string, unknown>>) => void;
  setCenter?: (lng: string | number, lat: string | number, zoom: number) => void;
}

// 状态映射表，统一处理 0-4 的状态枚举
const statusMap = computed<Record<number, string>>(() => ({
  0: '离线',
  1: '停止中',
  2: '骑行中',
  3: '故障',
  4: '其他',
}));

const webview = ref<WebViewApi>({});

const onMessage = (event: MessageEvent) => {
  switch (event.data.method) {
    case 'init':
      init.value = true;
      break;
  }
};

const wrapCsvValue = (val: unknown, formatFn?: ((val: unknown, row?: unknown) => unknown)): string => {
  const formatted = formatFn !== void 0 ? formatFn(val) : val;

  let result: string;
  if (formatted === void 0 || formatted === null) {
    result = '';
  } else if (typeof formatted === 'object') {
    result = JSON.stringify(formatted);
  } else {
    const typed = formatted as unknown as string | number | boolean;
    result = String(typed);
  }

  const escaped = result.split('"').join('""');

  return `\uFEFF${escaped}`;
};

const onExport = (): void => {
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

  const status = exportFile('存放点列表.xlsx', content, 'text/xlsx');

  if (status !== true) {
    store.quasar.notify('正在准备下载，请稍后...');
  }

  store.quasar.loading.hide();
  clearTimeout(time);
};

const onSearch = (): void => {
  table.value.rows = [];

  let statusValue = -1;
  const statusValueMap = statusMap.value;
  switch (search.value.status.model) {
    case '停用':
      statusValue = 0;
      break;
    case '启用':
      statusValue = 1;
      break;
    case '维护':
      statusValue = 2;
      break;
    case '其他':
      statusValue = 3;
      break;
  }

  api
    .post(store.backend.private + '/storage', {
      method: 'query',
      code: search.value.code,
      provide: search.value.provide,
      status: statusValue,
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
            status: statusStr,
            sum: element.sum,
            cur: element.cur,
            alert: element.alert,
            remark: element.remark,
          });
        }
      }
    })
    .catch(() => {});
};

const onHistory = (code: string): void => {
  splitter.value.show = true;
  splitter.value.model = 50;

  track.value.model = '';
  track.value.options = [];
  track.value.list = new Map();

  if (webview.value != null && webview.value != undefined) {
    webview.value.clear?.();
  }

  api
    .post(store.backend.private + '/storage', {
      method: 'history',
      code: code,
    })
    .then((resp) => {
      if (resp.data != '' && resp.data != 'error') {
        for (let index = 0; index < resp.data.length; index++) {
          const element = resp.data[index];

          if (element.code === '') break;

          const pts = element.points.split(';');
          const poly: Array<Array<number>> = [];
          for (let index = 0; index < pts.length; index++) {
            const pt = pts[index].split(',');
            if (element.gps_type === 1) {
              const t = gcoord.transform(
                [parseFloat(pt[0]), parseFloat(pt[1])],
                gcoord.WGS84,
                gcoord.BD09
              );
              poly.push([t[0], t[1]]);
            } else {
              poly.push([parseFloat(pt[0]), parseFloat(pt[1])]);
            }
          }

          track.value.options.push(
            date.formatDate(element.create_date, 'YYYY-MM-DD HH:mm:ss') +
              ' (' +
              index +
              ')'
          );

           const statusMapObj = statusMap.value;
           const statusStr = statusMapObj[element.status] ?? '未知';

           track.value.list.set(
             date.formatDate(element.create_date, 'YYYY-MM-DD HH:mm:ss') +
              ' (' +
              index +
              ')',
            {
              code: element.code,
              provide: element.provide,
              status: statusStr,
              gps: {
                lng: parseFloat(element.gps.lng),
                lat: parseFloat(element.gps.lat),
              },
              sum: element.sum,
              cur: element.cur,
              alert: element.alert,
              remark: element.remark,
              points: poly,
            }
          );
        }
      }
    })
    .catch(() => {});
};

interface StorageInfo {
  code: string;
  provide: string;
  status: string;
  gps: { lng: string; lat: string };
  sum: number;
  cur: number;
  alert: string;
  remark: string;
  points: Array<Array<number>>;
}

const onTrackChange = (val: string): void => {
  const v = track.value.list.get(val) as StorageInfo;
  webview.value.clear?.();

  webview.value.setPolygon?.([
    {
      geometry: {
        type: 'Polygon',
        coordinates: [v.points],
      },
      properties: {
        height: 0,
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
          '投放数量：' +
          v.sum +
          '<br />' +
          '当前数量：' +
          v.cur +
          '<br />' +
          '警告：' +
          v.alert +
          '<br />' +
          '备注：' +
          v.remark,
      },
    },
  ]);

  webview.value.setText?.([
    {
      geometry: {
        type: 'Point',
        coordinates: common.getPolygonCenter(v.points),
      },
      properties: {
        text: v.code,
      },
    },
  ]);

  const center = common.getPolygonCenter(v.points);
  webview.value.setCenter?.(
    center[0] ?? 0,
    center[1] ?? 0,
    18
  );
};

onMounted(() => {
  window.addEventListener('message', onMessage);
});

onBeforeUnmount(() => {
  window.removeEventListener('message', onMessage);
});
</script>