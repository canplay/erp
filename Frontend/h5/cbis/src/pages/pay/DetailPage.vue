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
      class="my-table"
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
          <q-btn color="primary" :label="props.value" @click="onView(props.row)" />
        </q-td>

        <q-td v-else-if="props.col.name === 'order_pay'" :props="props">
          <q-input readonly type="textarea" v-model="props.value" style="width: 500px" />
        </q-td>

        <q-td v-else-if="props.col.name === 'status'" :props="props">
          {{ props.value === 'paid' ? '已支付' : '待支付' }}
        </q-td>

        <q-td v-else-if="props.col.name === 'type'" :props="props">
          {{ props.value === 'ums' ? '银联商务' : '建行' }}
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

          <q-input class="col" filled v-model="dialog.order" label="订单号" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.status" label="状态" />
        </div>

        <div style="height: 8px" />

        <div class="row">
          <q-input class="col" filled v-model="dialog.type" label="类型" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.order_pay" label="商户订单号" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.amount" label="金额" />
        </div>

        <div style="height: 8px" />

        <div class="row">
          <q-input class="col" filled v-model="dialog.remark" label="备注" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.create_service" label="创建来源" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.create_params" label="创建参数" />
        </div>

        <div style="height: 8px" />

        <div class="row">
          <q-input class="col" filled v-model="dialog.create_date" label="创建时间" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.update_date" label="更新时间" />
        </div>
      </q-card-section>
    </q-card>
  </q-dialog>

  <q-inner-loading :showing="loading">
    <q-spinner size="50px" color="primary" />
  </q-inner-loading>
</template>

<style lang="sass">
.my-table
  tr th
    position: sticky
    z-index: 2
    background: #ffffff

  thead tr:last-child th
    top: 48px
    z-index: 3

  thead tr:first-child th
    top: 0
    z-index: 1

  tr:first-child th:first-child
    z-index: 3

  td:first-child
    background-color: #ffffff

  td:first-child
    z-index: 1

  td:first-child, th:first-child
    position: sticky
    left: 0

  tbody
    scroll-margin-top: 48px
</style>

<script setup lang="ts">
import { ref } from 'vue';
import type { QTableProps } from 'quasar';
import { exportFile } from 'quasar';
import { useStore } from 'src/stores/store';
import { api } from 'src/boot/axios';

const store = useStore();

const loading = ref(false);

const search = ref({
  date: '',
  content: '',
  select: {
    model: '',
    options: ['', 'ID', '账号,联系电话', '状态', '类型', '备注', '创建日期', '更新日期'],
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

const columns = ref([
  {
    name: 'id',
    label: 'ID',
    field: 'id',
    align: 'center',
    sortable: true,
  },
  {
    name: 'order',
    label: '订单号',
    field: 'order',
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
    name: 'type',
    label: '类型',
    field: 'type',
    align: 'center',
    sortable: true,
  },
  {
    name: 'order_pay',
    label: '商家订单号',
    field: 'order_pay',
    align: 'center',
    sortable: true,
  },
  {
    name: 'amount',
    label: '金额',
    field: 'amount',
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
    name: 'create_service',
    label: '创建来源',
    field: 'create_service',
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
    label: '更新时间',
    field: 'update_date',
    align: 'center',
    sortable: true,
  },
] as QTableProps['columns']);

const rows = ref([] as Record<string, unknown>[]);

const dialog = ref({
  show: false,
  id: '',
  order: '',
  status: '',
  type: '',
  order_pay: '',
  amount: 0,
  remark: '',
  create_service: '',
  create_params: '',
  create_date: '',
  update_date: '',
});

const wrapCsvValue = (val: string, formatFn?: (val: string) => string) => {
  let formatted = formatFn !== void 0 ? formatFn(val) : val;

  formatted = formatted === void 0 || formatted === null ? '' : String(formatted);

  formatted = formatted.split('"').join('""');

  return `${formatted}`;
};

const onExport = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  const content = [columns.value!.map((col) => wrapCsvValue(col.label))]
    .concat(
      rows.value.map((row: Record<string, unknown>) =>
        columns
          .value!.map((col: Record<string, unknown>) =>
            wrapCsvValue(
              typeof col.field === 'function'
                ? // if field is a function, call it with the row
                  col.field(row)
                : // otherwise index the row with a string key; assert key is string
                  row[col.field === void 0 ? (col.name as string) : (col.field as string)],
              col.format as ((val: string) => string) | undefined,
            ),
          )
          .join(','),
      ),
    )
    .join('\r\n');

  const status = exportFile('支付列表.csv', content, 'text/csv');

  if (status !== true) {
    store.quasar.notify('即将开始下载，请稍后...');
  }

  loading.value = false;
  clearTimeout(time);
};

const onRequest = async (props: { pagination: QTableProps['pagination'] }) => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  const { page, rowsPerPage, sortBy, descending, rowsNumber } = props.pagination!;

  rows.value = [];

  let type = '',
    status = '',
    remark = '';

  if (search.value.select.model === 'ID') {
    return;
  } else if (search.value.select.model === '账号,联系电话') {
    const a = search.value.content.split(',');
    const b = await api.post(
      store.server + '/api/route/post?auth=jwt&service=cbis&path=/api/user/public/info',
      {
        username: a[0],
        phone: a[1],
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    );
    remark = b.data.msg;
  } else if (search.value.select.model === '状态') {
    status = search.value.content;
  } else if (search.value.select.model === '类型') {
    type = search.value.content;
  } else if (search.value.select.model === '备注') {
    remark = search.value.content;
  } else if (search.value.select.model === '创建日期') {
    return;
  } else if (search.value.select.model === '更新日期') {
    return;
  }

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=pay&path=/api/pay/count',
      {
        status: status,
        type: type,
        remark: remark,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
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
          store.server + '/api/route/post?auth=jwt&service=pay&path=/api/pay/list',
          {
            status: status,
            type: type,
            remark: remark,
            curPage: (page! - 1) * rowsPerPage!,
            maxPage: rowsPerPage === 0 ? rowsNumber : rowsPerPage,
            sortBy: sortBy,
            descending: descending,
          },
          {
            headers: {
              Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
            },
          },
        )
        .then((resp) => {
          if (resp.data.status !== 0) {
            for (let i = 0; i < resp.data.data.length; i++) {
              rows.value.push({
                id: resp.data.data[i].id,
                order: resp.data.data[i].order,
                status: resp.data.data[i].status,
                type: resp.data.data[i].type,
                order_pay: JSON.stringify(resp.data.data[i].order_pay),
                amount: (resp.data.data[i].amount / 100).toFixed(2),
                remark: resp.data.data[i].remark,
                create_service: resp.data.data[i].create_service,
                create_params: JSON.stringify(resp.data.data[i].create_params),
                create_date: resp.data.data[i].create_date,
                update_date: resp.data.data[i].update_date,
              });
            }
          } else {
            store.quasar.notify('网络错误，请稍后重试');
          }

          loading.value = false;
          clearTimeout(time);

          pagination.value!.page = page!;
          pagination.value!.rowsPerPage = rowsPerPage!;
          pagination.value!.sortBy = sortBy!;
          pagination.value!.descending = descending!;
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

const onView = (val: Record<string, unknown>) => {
  dialog.value.id = val.id as string;
  dialog.value.order = val.order as string;
  dialog.value.status = val.status as string;
  dialog.value.type = val.type as string;
  dialog.value.order_pay = val.order_pay as string;
  dialog.value.amount = val.amount as number;
  dialog.value.remark = val.remark as string;
  dialog.value.create_service = val.create_service as string;
  dialog.value.create_params = val.create_params as string;
  dialog.value.create_date = val.create_date as string;
  dialog.value.update_date = val.update_date as string;
  dialog.value.show = true;
};
</script>
