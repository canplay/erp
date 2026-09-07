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
          :filter="table.filter"
        >
          <template v-slot:top>
            <div class="fit">
              <div class="row">
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
                <q-input class="col" filled v-model="search.time.start" label="时间">
                  <template v-slot:append>
                    <q-icon name="event" class="cursor-pointer">
                      <q-popup-proxy
                        ref="qDateProxy"
                        transition-show="scale"
                        transition-hide="scale"
                      >
                        <q-date v-model="search.time.start" mask="YYYY-MM-DD">
                          <div class="row items-center justify-end">
                            <q-btn v-close-popup label="关闭" color="primary" flat />
                          </div>
                        </q-date>
                      </q-popup-proxy>
                    </q-icon>
                  </template>
                </q-input>

                <div class="col-auto" style="width: 5px" />

                <q-input class="col" v-model="search.alert" label="警告" />

                <div class="col-auto" style="width: 5px" />

                <q-input class="col" v-model="search.remark" label="备注" />

                <div class="col-auto" style="width: 5px" />

                <q-btn class="col-2" color="red" label="导出" @click="onExport" />

                <div class="col-auto" style="width: 5px" />

                <q-btn class="col-2" color="primary" label="查询" @click="onSearch" />
              </div>
            </div>
          </template>

          <template v-slot:body="props">
            <q-tr :props="props">
              <q-td key="code" :props="props">
                <q-btn color="primary" :label="props.row.code" @click="onHistory(props.row.code)" />
              </q-td>
              <q-td key="provide" :props="props">
                {{ props.row.provide }}
              </q-td>
              <q-td key="type" :props="props">
                {{ props.row.gps_type }}
              </q-td>
              <q-td key="status" :props="props">
                {{ props.row.status }}
              </q-td>
              <q-td key="speed" :props="props">
                {{ props.row.speed }}
              </q-td>
              <q-td key="time" :props="props">
                {{ props.row.time }}
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
        <div class="fit">
          <iframe class="fit" src="map.html" frameborder="0" seamless ref="webview" />
        </div>
      </template>
    </q-splitter>
  </q-page>
</template>

<script setup lang="ts">
import type { QTableProps } from 'quasar';
import { date, exportFile } from 'quasar';
import { api } from 'src/boot/axios';
import { useStore } from 'src/stores/store';
import { ref, computed } from 'vue';

const store = useStore();

const splitter = ref({
  model: 100,
  show: false,
});
const search = ref({
  code: '',
  type: {
    model: '',
    options: ['', '车辆', '存放点'],
  },
  order: '',
  provide: '',
  status: {
    model: '',
    options: ['', '正常运营', '关闭', '离线'],
  },
  time: {
    start: '',
    end: '',
  },
  alert: '',
  remark: '',
  paystatus: -1,
  paytype: -1,
  paytime: '',
});
const track = ref<{
  model: string;
  options: string[];
  list: Map<string, Record<string, unknown>>;
  time: { start: string; end: string };
}>({
  model: '',
  options: [],
  list: new Map(),
  time: {
    start: '',
    end: '',
  },
});
const table = ref<{
  filter: string;
  columns: QTableProps['columns'];
  rows: Record<string, unknown>[];
}>({
  filter: '',
  columns: [
    {
      name: 'code',
      align: 'center',
      label: '识别码',
      field: 'code',
      sortable: true,
    },
    {
      name: 'type',
      align: 'center',
      label: '类型',
      field: 'type',
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
      name: 'time',
      align: 'center',
      label: '时间',
      field: 'time',
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
interface CustomWindow extends Window {
  clear?: () => void;
  setCar?: (...args: unknown[]) => void;
}

// 状态映射表，统一处理 0-2 的状态枚举
const statusMap = computed<Record<number, string>>(() => ({
  0: '正常运营',
  1: '关闭',
  2: '离线',
}));

const webview = ref<CustomWindow | null>(null);

const wrapCsvValue = (
  val: unknown,
  formatFn?: (val: unknown, row?: unknown) => unknown,
): string => {
  let formatted = formatFn !== void 0 ? formatFn(val) : val;

  formatted = formatted === void 0 || formatted === null ? '' : JSON.stringify(formatted);

  formatted = String(formatted).split('"').join('""');

  return `\uFEFF${String(formatted)}`;
};

const onExport = (): void => {
  const time = setTimeout(() => {
    store.quasar.loading.hide();
    clearTimeout(time);
  }, 120000);

  store.quasar.loading.show();

  const content = [table.value.columns!.map((col) => wrapCsvValue(col.label))]
    .concat(
      table.value.rows.map((row: Record<string, unknown>) =>
        table.value
          .columns!.map((col) =>
            wrapCsvValue(
              typeof col.field === 'function'
                ? col.field(row)
                : row[col.field === void 0 ? col.name : col.field],
              col.format,
            ),
          )
          .join(','),
      ),
    )
    .join('\r\n');

  const status = exportFile('订单列表.xlsx', content, 'text/xlsx');

  if (status !== true) {
    store.quasar.notify('正在准备下载，请稍后...');
  }

  store.quasar.loading.hide();
  clearTimeout(time);
};

const onSearch = (): void => {
  table.value.rows = [];

  let s = -1;
  const statusMapObj = statusMap.value;
  switch (search.value.status.model) {
    case '正常运营':
      s = 0;
      break;
    case '关闭':
      s = 1;
      break;
    case '离线':
      s = 2;
      break;
  }

  api
    .post(store.backend.private + '/order', {
      method: 'query',
      code: search.value.code,
      provide: search.value.provide,
      status: s,
      time: {
        start: search.value.time.start,
        end: search.value.time.end,
      },
      order: search.value.order,
      paystatus: search.value.paystatus,
      paytype: search.value.paytype,
      paytime: search.value.paytime,
    })
    .then((resp) => {
      if (resp.data != '' && resp.data != 'error') {
        for (let i = 0; i < resp.data.length; i++) {
          const element = resp.data[i];

          if (resp.data[i].code === '') break;

           const statusStr = statusMapObj[element.status as unknown as number] ?? '未知';

          table.value.rows.push({
            code: element.code,
            type: '车辆',
            provide: element.provide,
            speed: element.speed,
            status: statusStr,
            time: date.formatDate(element.create_date, 'YYYY-MM-DD HH:mm:ss'),
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

  if (webview.value !== null && webview.value !== undefined) {
    (
      webview.value as unknown as { contentWindow: CustomWindow | undefined }
    ).contentWindow?.clear?.();
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

          track.value.options.push(
            element.powertime.start + ' - ' + element.gps.lng + ', ' + element.gps.lat,
          );

          track.value.list.set(
            element.powertime.start + ' - ' + element.gps.lng + ', ' + element.gps.lat,
            {
              code: element.code,
              provide: element.provide,
              speed: element.speed,
              status: element.status,
              gps: element.gps,
              powertime: element.powertime,
              alert: element.alert,
              remark: element.remark,
            },
          );
        }
      }
    })
    .catch(() => {});
};
</script>
