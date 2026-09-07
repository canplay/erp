<template>
  <q-page>
    <q-table
      square
      :rows="rows"
      :columns="columns"
      row-key="id"
      separator="cell"
      v-model:pagination="pagination"
      cell
      selection="multiple"
      v-model:selected="selected"
      virtual-scroll
      @request="onRequest"
      :style="`${store.height};width: calc(100vw - 200px)`"
    >
      <template v-slot:top>
        <div class="fit row">
          <q-select
            class="col-2"
            v-model="search.select.model"
            :options="search.select.options"
            label="查询类型"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col-4" v-model="search.content" label="查询内容" />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col" color="green" label="导出" @click="onExport" />

          <div class="col-auto" style="width: 8px" />

          <q-btn
            class="col"
            color="secondary"
            label="查询"
            @click="onRequest({ pagination: pagination })"
          />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col" color="negative" label="删除" @click="onDelete" />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col" color="primary" label="新增" @click="onNew" />
        </div>
      </template>

      <template v-slot:header-cell="props">
        <q-th :props="props" style="font-size: 15px; font-weight: bold">
          {{ props.col.label }}
        </q-th>
      </template>

      <template v-slot:body-selection="props">
        <q-checkbox v-model="props.selected" />
      </template>

      <template v-slot:body-cell="props">
        <q-td v-if="props.col.name === 'id'" :props="props">
          <q-btn
            color="primary"
            :label="props.value"
            @click="onView(props.row)"
          />
        </q-td>

        <q-td v-else-if="props.col.name === 'pay_need'" :props="props">
          {{ props.value === 'true' ? '是' : '否' }}
        </q-td>

        <q-td
          v-else-if="
            props.col.name === 'desc' ||
            props.col.name === 'remark' ||
            props.col.name === 'address'
          "
          :props="props"
        >
          <q-input
            readonly
            type="textarea"
            v-model="props.value"
            style="width: 250px"
          />
        </q-td>

        <q-td v-else :props="props">
          {{ props.value }}
        </q-td>
      </template>
    </q-table>
  </q-page>

  <q-dialog v-model="dialog.show">
    <q-card style="min-width: 70%">
      <q-card-section class="text-h6"> 票据信息 </q-card-section>

      <q-card-section>
        <div class="row">
          <q-input readonly filled class="col" v-model="dialog.id" label="ID" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.title" label="标题" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.num" label="数量" />
        </div>

        <div style="height: 8px" />

        <div class="row">
          <q-input
            class="col"
            filled
            v-model="dialog.seats.start"
            label="座次前缀"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            filled
            v-model="dialog.seats.end"
            label="座次后缀"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.address" label="地点" />
        </div>

        <div style="height: 8px" />

        <div class="row">
          <q-input class="col" filled v-model="dialog.price" label="单价" />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            filled
            v-model="dialog.date.start"
            label="开始时间"
          >
            <template v-slot:prepend>
              <q-icon name="event" class="cursor-pointer">
                <q-popup-proxy transition-show="scale" transition-hide="scale">
                  <q-date
                    v-model="dialog.date.start"
                    mask="YYYY-MM-DD HH:mm:ss"
                  >
                    <div class="row items-center justify-end">
                      <q-btn v-close-popup label="关闭" color="primary" flat />
                    </div>
                  </q-date>
                </q-popup-proxy>
              </q-icon>
            </template>

            <template v-slot:append>
              <q-icon name="access_time" class="cursor-pointer">
                <q-popup-proxy transition-show="scale" transition-hide="scale">
                  <q-time
                    v-model="dialog.date.start"
                    mask="YYYY-MM-DD HH:mm:ss"
                    format24h
                  >
                    <div class="row items-center justify-end">
                      <q-btn v-close-popup label="关闭" color="primary" flat />
                    </div>
                  </q-time>
                </q-popup-proxy>
              </q-icon>
            </template>
          </q-input>

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            filled
            v-model="dialog.date.end"
            label="结束时间"
          >
            <template v-slot:prepend>
              <q-icon name="event" class="cursor-pointer">
                <q-popup-proxy transition-show="scale" transition-hide="scale">
                  <q-date v-model="dialog.date.end" mask="YYYY-MM-DD HH:mm:ss">
                    <div class="row items-center justify-end">
                      <q-btn v-close-popup label="关闭" color="primary" flat />
                    </div>
                  </q-date>
                </q-popup-proxy>
              </q-icon>
            </template>

            <template v-slot:append>
              <q-icon name="access_time" class="cursor-pointer">
                <q-popup-proxy transition-show="scale" transition-hide="scale">
                  <q-time
                    v-model="dialog.date.end"
                    mask="YYYY-MM-DD HH:mm:ss"
                    format24h
                  >
                    <div class="row items-center justify-end">
                      <q-btn v-close-popup label="关闭" color="primary" flat />
                    </div>
                  </q-time>
                </q-popup-proxy>
              </q-icon>
            </template>
          </q-input>
        </div>

        <div style="height: 8px" />

        <div class="row">
          <q-input class="col" filled v-model="dialog.desc" label="介绍" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.remark" label="备注" />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            filled
            v-model="dialog.num_available"
            label="可领次数/每用户"
          />
        </div>

        <div style="height: 8px" />

        <div class="row">
          <q-input
            class="col"
            filled
            v-model="dialog.num_limit"
            label="可用次数/每票"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            filled
            v-model="dialog.pay_need"
            label="是否需要支付"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            readonly
            filled
            v-model="dialog.order"
            label="支付订单ID"
          />
        </div>

        <div style="height: 8px" />

        <div class="row">
          <q-select
            v-if="dialog.mode === 'view'"
            class="col"
            filled
            v-model="dialog.status"
            :options="['未发放']"
            label="状态"
          />

          <div
            v-if="dialog.mode === 'view'"
            class="col-auto"
            style="width: 8px"
          />

          <q-input
            class="col"
            filled
            v-model="dialog.qrcode"
            label="二维码数据"
            @update:model-value="onQrcodeData"
          />
        </div>

        <div style="height: 8px" />

        <div v-if="dialog.mode === 'view'" class="text-center" ref="qrcode" />
      </q-card-section>

      <q-card-actions class="row">
        <q-btn class="col" color="negative" label="删除" @click="onDelete" />

        <div class="col-auto" style="width: 8px" />

        <q-btn
          v-if="dialog.mode === 'view'"
          class="col"
          color="secondary"
          label="导出二维码"
          @click="onExportQrcode"
        />

        <div class="col-auto" style="width: 8px" />

        <q-btn class="col" color="primary" label="保存" @click="onSave" />
      </q-card-actions>
    </q-card>
  </q-dialog>

  <q-inner-loading :showing="loading">
    <q-spinner size="50px" color="primary" />
  </q-inner-loading>
</template>

<script setup lang="ts">
import { ref } from 'vue';
import type { QTableProps} from 'quasar';
import { date, exportFile } from 'quasar';
import { useStore } from 'src/stores/store';
import QRCode from 'easyqrcodejs';
import { api } from 'src/boot/axios';

const store = useStore();

const loading = ref(false);

const search = ref({
  date: '',
  content: '',
  select: {
    model: '',
    options: ['', 'ID', '标题', '使用状态', '领取状态'],
  },
});

const pagination = ref({
  page: 1,
  rowsNumber: 0,
  rowsPerPage: 20,
  sortBy: 'create_date',
  descending: true,
} as QTableProps['pagination']);

const selected = ref([] as Record<string, unknown>[]);

interface ColumnItem {
  name: string;
  label: string;
  field: string | ((row: Record<string, unknown>) => unknown);
  align?: 'center' | 'left' | 'right';
  sortable?: boolean;
  required?: boolean;
  format?: (val: unknown) => unknown;
  sort?: (a: unknown, b: unknown, rowA: unknown, rowB: unknown) => number;
  style?: string;
  headerClasses?: string;
  headerStyle?: string;
}

const columns = ref<ColumnItem[]>([
  {
    name: 'id',
    label: 'ID',
    field: 'id',
    align: 'center',
    sortable: true,
  },
  {
    name: 'title',
    label: '标题',
    field: 'title',
    align: 'center',
    sortable: true,
  },
  {
    name: 'desc',
    label: '介绍',
    field: 'desc',
    align: 'center',
    sortable: true,
  },
  {
    name: 'status',
    label: '状态',
    field: 'status',
    align: 'center',
    sortable: true,
  },
  {
    name: 'num',
    label: '数量',
    field: 'num',
    align: 'center',
    sortable: true,
  },
  {
    name: 'price',
    label: '单价',
    field: 'price',
    align: 'center',
    sortable: true,
  },
  {
    name: 'seats',
    label: '座次',
    field: 'seats',
    align: 'center',
    sortable: true,
  },
  {
    name: 'address',
    label: '地点',
    field: 'address',
    align: 'center',
    sortable: true,
  },
  {
    name: 'date_start',
    label: '开始时间',
    field: 'date_start',
    align: 'center',
    sortable: true,
  },
  {
    name: 'date_end',
    label: '结束时间',
    field: 'date_end',
    align: 'center',
    sortable: true,
  },
  {
    name: 'remark',
    label: '备注',
    field: 'remark',
    align: 'center',
    sortable: true,
  },
  {
    name: 'owner',
    label: '拥有人',
    field: 'owner',
    align: 'center',
    sortable: true,
  },
  {
    name: 'num_available',
    label: '可用次数',
    field: 'num_available',
    align: 'center',
    sortable: true,
  },
  {
    name: 'pay_need',
    label: '是否需要支付',
    field: 'pay_need',
    align: 'center',
    sortable: true,
  },
  {
    name: 'order',
    label: '支付订单ID',
    field: 'order',
    align: 'center',
    sortable: true,
  },
  {
    name: 'verify_user',
    label: '核销人',
    field: 'verify_user',
    align: 'center',
    sortable: true,
  },
  {
    name: 'verify_date',
    label: '核销时间',
    field: 'verify_date',
    align: 'center',
    sortable: true,
  },
  {
    name: 'verify_amount',
    label: '核销金额',
    field: 'verify_amount',
    align: 'center',
    sortable: true,
  },
  {
    name: 'create_user',
    label: '创建人',
    field: 'create_user',
    align: 'center',
    sortable: true,
  },
  {
    name: 'create_date',
    label: '创建时间',
    field: 'create_date',
    align: 'center',
    sortable: true,
  },
  {
    name: 'update_date',
    label: '更新人',
    field: 'update_date',
    align: 'center',
    sortable: true,
  },
  {
    name: 'update_date',
    label: '更新时间',
    field: 'update_date',
    align: 'center',
    sortable: true,
  },
]);

const rows = ref([] as Record<string, unknown>[]);

const qrcode = ref({} as HTMLElement);

const dialog = ref({
  show: false,
  mode: 'new',
  id: '',
  title: '',
  address: '',
  num: '',
  date: {
    start: '',
    end: '',
  },
  remark: '',
  status: '',
  seats: {
    start: '',
    mid: '',
    end: '',
  },
  qrcode: '',
  owner: '',
  price: '',
  desc: '',
  num_available: '',
  num_limit: '',
  pay_need: '',
  order: '',
});

const onQrcodeData = () => {
  if (dialog.value.mode !== 'view') return;
  if (qrcode.value.children.length > 0) {
    qrcode.value.removeChild(qrcode.value.lastChild!);
  }
  new QRCode(qrcode.value, dialog.value.qrcode);
};
const wrapCsvValue = <T = unknown>(val: unknown, formatFn?: (val: unknown) => T) => {
  let formatted = formatFn !== void 0 ? formatFn(val) : val;

  if (formatted === void 0 || formatted === null) {
    return '';
  }

  if (typeof formatted === 'object') {
    formatted = JSON.stringify(formatted);
  }

  formatted = String(formatted).split('"').join('""');

  return String(formatted);
};

const onExport = () => {
  const time = setTimeout(() => {
    store.quasar.loading.hide();
    clearTimeout(time);
  }, 120000);

  store.quasar.loading.show();

  const content = [columns.value.map((col) => wrapCsvValue(col.label))]
    .concat(
      rows.value.map((row: Record<string, unknown>) =>
        columns
          .value.map((col) =>
            wrapCsvValue(
              typeof col.field === 'function'
                ? col.field(row)
                : (col.field === void 0 ? col.name : col.field),
              col.format
            )
          )
          .join(',')
      )
    )
    .join('\r\n');

  const status = exportFile('票据信息.csv', content, 'text/csv');

  if (status !== true) {
    store.quasar.notify('即将开始下载，请稍后...');
  }

  store.quasar.loading.hide();
  clearTimeout(time);
};

const onRequest = (props: { pagination: QTableProps['pagination'] }) => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  const { page = 1, rowsPerPage = 20, sortBy = 'update_date', descending = true, rowsNumber = 0 } =
    props.pagination ?? {};

  rows.value = [];

  let id = '',
    title = '',
    status_use = '',
    status_recive = '';

  if (search.value.select.model === 'ID') {
    id = search.value.content;
  } else if (search.value.select.model === '标题') {
    title = search.value.content;
  } else if (search.value.select.model === '使用状态') {
    status_use = search.value.content;
  } else if (search.value.select.model === '领取状态') {
    status_recive = search.value.content;
  }

  api
    .post(
      store.server +
        '/api/route/post?auth=jwt&service=cbis&path=/api/ticket/count',
      {
        id: id,
        title: title,
        status_use: status_use,
        status_recive: status_recive,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      }
    )
    .then((resp) => {
      pagination.value!.rowsNumber = parseInt(resp.data.data);

      if (resp.data.status === 0 || parseInt(resp.data.data) <= 0) {
        loading.value = false;
        clearTimeout(time);
        store.quasar.notify('没有数据');
        return;
      }

      api
        .post(
          store.server +
            '/api/route/post?auth=jwt&service=cbis&path=/api/ticket/list',
          {
            id: id,
            title: title,
            status_use: status_use,
            status_recive: status_recive,
            curPage: (page - 1) * rowsPerPage,
            maxPage: rowsPerPage === 0 ? rowsNumber : rowsPerPage,
            sortBy: sortBy,
            descending: descending,
          },
          {
            headers: {
              Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
            },
          }
        )
        .then((resp) => {
        if (resp.data.status !== 0) {
            for (let i = 0; i < resp.data.data.length; i++) {
              rows.value.push({
                id: resp.data.data[i].id,
                title: resp.data.data[i].title,
                address: resp.data.data[i].address,
                num: resp.data.data[i].num,
                date_start: resp.data.data[i].date_start,
                date_end: resp.data.data[i].date_end,
                remark: resp.data.data[i].remark,
                status: resp.data.data[i].status,
                seats_start: resp.data.data[i].seats_start,
                seats_mid: resp.data.data[i].seats_mid,
                seats_end: resp.data.data[i].seats_end,
                seats:
                  resp.data.data[i].seats_start +
                  resp.data.data[i].seats_mid +
                  resp.data.data[i].seats_end,
                qrcode: resp.data.data[i].qrcode,
                price: resp.data.data[i].price,
                owner:
                  resp.data.data[i].owner.msg === '无有效用户'
                    ? ''
                    : resp.data.data[i].owner.msg.user.nickname,
                desc: resp.data.data[i].desc,
                num_limit: resp.data.data[i].num_limit,
                num_available: resp.data.data[i].num_available,
                pay_need: resp.data.data[i].pay_need,
                order: resp.data.data[i].order,
                verify_user:
                  resp.data.data[i].verify_user.msg === '无有效用户'
                    ? ''
                    : resp.data.data[i].verify_user.msg.user.nickname,
                verify_date: resp.data.data[i].verify_date,
                verify_amount: resp.data.data[i].verify_amount,
                create_user: resp.data.data[i].create_user.msg.user.nickname,
                create_date: resp.data.data[i].create_date,
                update_user: resp.data.data[i].update_user.msg.user.nickname,
                update_date: resp.data.data[i].update_date,
              });
            }
          } else {
            store.quasar.notify('网络错误，请稍后重试');
          }

          loading.value = false;
          clearTimeout(time);

          pagination.value!.page = page;
          pagination.value!.rowsPerPage = rowsPerPage;
          pagination.value!.sortBy = sortBy;
          pagination.value!.descending = descending;
        })
        .catch(() => {
          store.quasar.notify('网络错误，请稍后重试');
          loading.value = false;
          clearTimeout(time);
        });
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      loading.value = false;
      clearTimeout(time);
    });
};

const onNew = () => {
  dialog.value.mode = 'new';
  dialog.value.id = '';
  dialog.value.title = '';
  dialog.value.address = '';
  dialog.value.num = '0';
  dialog.value.date = {
    start: date.formatDate(Date.now(), 'YYYY-MM-DD HH:mm:ss'),
    end: date.formatDate(Date.now(), 'YYYY-MM-DD HH:mm:ss'),
  };
  dialog.value.remark = '';
  dialog.value.status = '待使用';
  dialog.value.seats = {
    start: '',
    mid: '',
    end: '',
  };
  dialog.value.qrcode =
    store.server + '/api/route/get?auth=jwt&service=cbis&path=/api/ticket/info';
  dialog.value.owner = '';
  dialog.value.price = '0';
  dialog.value.desc = '';
  dialog.value.num_limit = '1';
  dialog.value.num_available = '1';
  dialog.value.pay_need = 'true';
  dialog.value.order = '';
  dialog.value.show = true;
  setTimeout(() => {
    if (dialog.value.mode !== 'view') return;
    if (qrcode.value != null && qrcode.value !== undefined) {
      new QRCode(qrcode.value, dialog.value.qrcode);
    }
  }, 1000);
};

interface SeatsOrderRow {
  id?: string | number;
  title?: string;
  address?: string;
  num?: string | number;
  date_start?: string;
  date_end?: string;
  remark?: string;
  status?: string;
  seats_start?: string;
  seats_mid?: string;
  seats_end?: string;
  qrcode?: string;
  owner?: string;
  price?: string | number;
  desc?: string;
  num_limit?: string | number;
  num_available?: string | number;
  pay_need?: string | number;
  order?: string | number;
}

const onView = (val: SeatsOrderRow) => {
  dialog.value.mode = 'view';
  dialog.value.id = val.id != null ? String(val.id) : '';
  dialog.value.title = val.title != null ? String(val.title) : '';
  dialog.value.address = val.address != null ? String(val.address) : '';
  dialog.value.num = val.num != null ? String(val.num) : '';
  dialog.value.date = {
    start: val.date_start != null ? String(val.date_start) : '',
    end: val.date_end != null ? String(val.date_end) : '',
  };
  dialog.value.remark = val.remark != null ? String(val.remark) : '';
  dialog.value.status = val.status != null ? String(val.status) : '';
  dialog.value.seats = {
    start: val.seats_start != null ? String(val.seats_start) : '',
    mid: val.seats_mid != null ? String(val.seats_mid) : '',
    end: val.seats_end != null ? String(val.seats_end) : '',
  };
  dialog.value.qrcode = val.qrcode != null ? String(val.qrcode) : '';
  dialog.value.owner = val.owner != null ? String(val.owner) : '';
  dialog.value.price = val.price != null ? String(val.price) : '';
  dialog.value.desc = val.desc != null ? String(val.desc) : '';
  dialog.value.num_limit = val.num_limit != null ? String(val.num_limit) : '';
  dialog.value.num_available = val.num_available != null ? String(val.num_available) : '';
  dialog.value.pay_need = val.pay_need != null ? String(val.pay_need) : '';
  dialog.value.order = val.order != null ? String(val.order) : '';
  dialog.value.show = true;
  setTimeout(() => {
    if (qrcode.value != null && qrcode.value !== undefined) {
      if (dialog.value.mode !== 'view') return;
      new QRCode(qrcode.value, dialog.value.qrcode);
    }
  }, 1000);
};

const onExportQrcode = () => {
  
};

const onDelete = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  if (selected.value.length === 0 && dialog.value.id !== '') {
    api
      .post(
        store.server +
          '/api/route/post?auth=jwt&service=cbis&path=/api/ticket/update?id=' +
          dialog.value.id +
          '&user=' +
          store.user.id,
        {
          headers: {
            Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
          },
        }
      )
      .then((resp) => {
        loading.value = false;
        clearTimeout(time);

        if (resp.data.status !== 1) {
          store.quasar.notify('删除失败，请稍后重试');
        }

        store.quasar.notify('删除成功');
      })
      .catch(() => {
        store.quasar.notify('网络错误，请稍后重试');
        loading.value = false;
        clearTimeout(time);
      });
  } else {
    for (let index = 0; index < selected.value.length; index++) {
      const element = selected.value[index];
      if (!element) continue;

      api
        .post(
          store.server +
            '/api/route/post?auth=jwt&service=cbis&path=/api/ticket/update?id=' +
            String(element.id) +
            '&user=' +
            String(store.user.id),
          {
            headers: {
              Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
            },
          }
        )
        .then((resp) => {
          if (resp.data.status !== 1) {
            store.quasar.notify('删除失败，请稍后重试');
          }

          store.quasar.notify('删除成功');
        })
        .catch(() => {
          store.quasar.notify('网络错误，请稍后重试');
          loading.value = false;
          clearTimeout(time);
        });
    }

    loading.value = false;
    clearTimeout(time);
    onRequest({ pagination: pagination.value });
  }
};

const onSave = () => {
  dialog.value.show = false;

  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  api
    .post(
      store.server +
        '/api/route/post?auth=jwt&service=cbis&path=/api/ticket/update',
      {
        title: dialog.value.title,
        num: parseInt(dialog.value.num),
        seats_start: dialog.value.seats.start,
        seats_mid: dialog.value.seats.mid,
        seats_end: dialog.value.seats.end,
        date_start: dialog.value.date.start,
        date_end: dialog.value.date.end,
        remark: dialog.value.remark,
        status: dialog.value.status,
        qrcode: dialog.value.qrcode,
        address: dialog.value.address,
        price: dialog.value.price,
        desc: dialog.value.desc,
        num_limit: dialog.value.num_limit,
        num_available: dialog.value.num_available,
        pay_need: dialog.value.pay_need === 'true',
        create_user: store.user.id,
        update_user: store.user.id,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      }
    )
    .then((resp) => {
      loading.value = false;
      clearTimeout(time);

        if (resp.data.status !== 1) {
          store.quasar.notify('保存失败，请稍后重试');
        return;
      }

      store.quasar.notify('保存成功');
      onRequest({ pagination: pagination.value });
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      loading.value = false;
      clearTimeout(time);
    });
};
</script>
