<template>
  <q-page>
    <q-table
      square
      :rows="table.rows"
      :columns="table.columns"
      row-key="id"
      separator="cell"
      v-model:pagination="table.pagination"
      selection="multiple"
      v-model:selected="table.selected"
      @request="onRequest"
      virtual-scroll
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

          <q-input class="col" v-model="search.content" label="查询内容" />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col-2" color="green" label="导出" @click="onExport" />

          <div class="col-auto" style="width: 8px" />

          <q-btn
            class="col-2"
            color="secondary"
            label="查询"
            @click="onRequest({ pagination: table.pagination })"
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

        <q-td v-else :props="props">
          {{ props.value }}
        </q-td>
      </template>
    </q-table>
  </q-page>

  <q-dialog v-model="dialog.show">
    <q-card style="width: 80%">
      <q-card-section>
        <div class="row fit">
          <q-input class="col" readonly label="ID" v-model="dialog.id" />
          <div class="col-auto" style="width: 8px" />
          <q-input class="col" readonly label="标题" v-model="dialog.title" />
          <div class="col-auto" style="width: 8px" />
          <q-input class="col" readonly label="联系人" v-model="dialog.contact" />
        </div>

        <div class="col-auto" style="height: 8px" />

        <div class="row fit">
          <q-input class="col" readonly label="联系电话" v-model="dialog.phone" />
          <div class="col-auto" style="width: 8px" />
          <q-input class="col" readonly label="小区" v-model="dialog.zone" />
          <div class="col-auto" style="width: 8px" />
          <q-input class="col" readonly label="幢" v-model="dialog.building" />
        </div>

        <div class="col-auto" style="height: 8px" />

        <div class="row fit">
          <q-input class="col" readonly label="房" v-model="dialog.room" />
          <div class="col-auto" style="width: 8px" />
          <q-input class="col" readonly label="状态" v-model="dialog.status" />
          <div class="col-auto" style="width: 8px" />
          <q-input class="col" readonly label="备注" v-model="dialog.remark" />
        </div>

        <div class="col-auto" style="height: 8px" />

        <div class="row fit">
          <q-input class="col" label="预览图" v-model="dialog.preview" />
          <div class="col-auto" style="width: 8px" />
          <q-input class="col" label="单价" v-model="dialog.price" />
          <div class="col-auto" style="width: 8px" />
          <q-input class="col" label="面积" v-model="dialog.area" />
        </div>

        <div class="col-auto" style="height: 8px" />

        <div class="row fit">
          <q-input class="col" label="总价" v-model="dialog.amount" />
          <div class="col-auto" style="width: 8px" />
          <q-input class="col" label="附加内容" v-model="dialog.additional" />
        </div>
      </q-card-section>

      <q-card-actions>
        <q-btn class="fit" color="primary" label="保存" @click="onSave" />
      </q-card-actions>
    </q-card>
  </q-dialog>
</template>

<script setup lang="ts">
import type { QTableProps } from 'quasar';
import { exportFile } from 'quasar';
import { api } from 'src/boot/axios';
import { useStore } from 'src/stores/store';
import { ref } from 'vue';

interface TableRow {
  [key: string]: unknown;
  id?: string | number;
  name?: string;
  field?: string | ((row: Record<string, unknown>) => unknown);
  title?: string;
  contact?: string;
  phone?: string;
  zone?: string;
  building?: string;
  room?: string;
  status?: string;
  remark?: string;
  preview?: string;
  price?: string | number;
  area?: string | number;
  amount?: string | number;
  create_user?: string;
  create_date?: string;
  update_user?: string;
  update_date?: string;
}

const store = useStore();

const search = ref({
  date: '',
  content: '',
  select: {
    model: '',
    options: ['', '标题', '联系人', '联系电话', '状态'],
  },
});

const table = ref({
  rows: [] as TableRow[],
  selected: [] as TableRow[],
  pagination: {
    page: 1,
    rowsNumber: 0,
    rowsPerPage: 20,
    sortBy: 'update_date',
    descending: true,
  },
  columns: [
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
      name: 'contact',
      label: '联系人',
      field: 'contact',
      align: 'center',
      sortable: true,
    },
    {
      name: 'phone',
      label: '联系电话',
      field: 'phone',
      align: 'center',
      sortable: true,
    },
    {
      name: 'zone',
      label: '小区',
      field: 'zone',
      align: 'center',
      sortable: true,
    },
    {
      name: 'building',
      label: '幢',
      field: 'building',
      align: 'center',
      sortable: true,
    },
    {
      name: 'room',
      label: '房',
      field: 'room',
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
      name: 'remark',
      label: '备注',
      field: 'remark',
      align: 'center',
      sortable: true,
    },
    {
      name: 'preview',
      label: '预览图',
      field: 'preview',
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
      name: 'area',
      label: '面积',
      field: 'area',
      align: 'center',
      sortable: true,
    },
    {
      name: 'amount',
      label: '总价',
      field: 'amount',
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
      name: 'update_user',
      label: '更新人',
      field: 'update_user',
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
  ] as QTableProps['columns'],
});

const dialog = ref({
  show: false,
  id: '',
  title: '',
  contact: '',
  phone: '',
  zone: '',
  building: '',
  room: '',
  status: '',
  remark: '',
  preview: '',
  price: '',
  area: '',
  amount: '',
  additional: '',
});

interface SellhouseTableRow {
  [key: string]: unknown;
  id?: string | number;
  title?: string;
  contact?: string;
  phone?: string;
  zone?: string;
  building?: string;
  room?: string;
  status?: string;
  remark?: string;
  preview?: string;
  price?: string | number;
  area?: string | number;
  amount?: string | number;
  additional?: string;
  create_user?: unknown;
  create_date?: string;
  update_user?: unknown;
  update_date?: string;
}

const onRequest = (value: { pagination: QTableProps['pagination'] }) => {
  const time = setTimeout(() => {
    store.quasar.loading.show();
    clearTimeout(time);
  }, 120000);

  store.quasar.loading.hide();

  const page = value.pagination?.page ?? 1;
  const rowsPerPage = value.pagination?.rowsPerPage ?? 20;
  const sortBy = value.pagination?.sortBy ?? null;
  const descending = value.pagination?.descending ?? true;
  const rowsNumber = value.pagination?.rowsNumber;

  table.value.rows = [];

  let title = '',
    contact = '',
    phone = '',
    status = '';

  switch (search.value.select.model) {
    case '标题':
      title = search.value.content;
      break;
    case '联系人':
      contact = search.value.content;
      break;
    case '联系电话':
      phone = search.value.content;
      break;
    case '状态':
      status = search.value.content;
      break;
  }

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=cbis&path=/api/sellHouse/count',
      {
        title: title,
        contact: contact,
        phone: phone,
        status: status,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      table.value.pagination.rowsNumber = parseInt(resp.data.data);

      if (resp.data.status === 0 || parseInt(resp.data.data) <= 0) {
        store.quasar.loading.hide();
        clearTimeout(time);
        store.quasar.notify('没有数据');
        return;
      }

      api
        .post(
          store.server + '/api/route/post?auth=jwt&service=cbis&path=/api/sellHouse/list',
          {
            title: title,
            contact: contact,
            phone: phone,
            status: status,
            curPage: (page - 1) * rowsPerPage,
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
              table.value.rows.push({
                id: resp.data.data[i].id,
                title: resp.data.data[i].title,
                contact: resp.data.data[i].contact,
                phone: resp.data.data[i].phone,
                zone: resp.data.data[i].zone,
                building: resp.data.data[i].building,
                room: resp.data.data[i].room,
                status: resp.data.data[i].status,
                remark: resp.data.data[i].remark,
                preview: resp.data.data[i].preview,
                price: resp.data.data[i].price,
                area: resp.data.data[i].area,
                amount: resp.data.data[i].amount,
                create_user: resp.data.data[i].create_user.msg.user
                  ? resp.data.data[i].create_user.msg.user.nickname
                  : resp.data.data[i].create_user.msg,
                create_date: resp.data.data[i].create_date,
                update_user: resp.data.data[i].update_user.msg.user
                  ? resp.data.data[i].update_user.msg.user.nickname
                  : resp.data.data[i].update_user.msg,
                update_date: resp.data.data[i].update_date,
              });
            }
          } else {
            store.quasar.notify('网络错误，请稍后重试');
          }

          store.quasar.loading.hide();
          clearTimeout(time);

          table.value.pagination.page = page;
          table.value.pagination.rowsPerPage = rowsPerPage;
          table.value.pagination.sortBy = sortBy ?? 'update_date';
          table.value.pagination.descending = descending;
        })
        .catch(() => {
          store.quasar.notify('网络错误，请稍后重试');
          store.quasar.loading.hide();
          clearTimeout(time);
        });
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      store.quasar.loading.hide();
      clearTimeout(time);
    });
};

const wrapCsvValue = (val: string, formatFn?: (...args: unknown[]) => unknown) => {
  let formatted: string = formatFn !== void 0 ? String(formatFn(val)) : val;

  formatted = formatted === void 0 || formatted === null ? '' : String(formatted);

  formatted = formatted.split('"').join('""');

  return formatted;
};

const onExport = () => {
  const time = setTimeout(() => {
    store.quasar.loading.hide();
    clearTimeout(time);
  }, 120000);

  store.quasar.loading.show();

  const content = [table.value.columns!.map((col) => wrapCsvValue(col.label))]
    .concat(
      table.value.rows.map((row: TableRow) =>
        table.value
          .columns!
          .map((col) =>
            wrapCsvValue(
              typeof col.field === 'function'
                ? col.field(row) as string
                : row[col.field === void 0 ? col.name : col.field] as string,
              col.format,
            ),
          )
          .join(','),
      ),
    )
    .join('\r\n');

  const status = exportFile('房产预售.csv', content, 'text/csv');

  if (status !== true) {
    store.quasar.notify('即将开始下载，请稍后...');
  }

  store.quasar.loading.hide();
  clearTimeout(time);
};

const onView = (val: SellhouseTableRow) => {
  dialog.value.id = String(val.id ?? '');
  dialog.value.title = val.title ?? '';
  dialog.value.contact = val.contact ?? '';
  dialog.value.phone = val.phone ?? '';
  dialog.value.zone = val.zone ?? '';
  dialog.value.building = val.building ?? '';
  dialog.value.room = val.room ?? '';
  dialog.value.status = val.status ?? '';
  dialog.value.remark = val.remark ?? '';
  dialog.value.preview = val.preview ?? '';
  dialog.value.price = String(val.price ?? '');
  dialog.value.area = String(val.area ?? '');
  dialog.value.amount = String(val.amount ?? '');
  dialog.value.additional = val.additional ?? '';
  dialog.value.show = true;
};

const onSave = () => {
  const time = setTimeout(() => {
    store.quasar.loading.hide();
    clearTimeout(time);
  }, 120000);

  store.quasar.loading.show();

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=cbis&path=/api/sellHouse/update',
      {
        method: 'update',
        id: dialog.value.id,
        title: dialog.value.title,
        contact: dialog.value.contact,
        phone: dialog.value.phone,
        zone: dialog.value.zone,
        building: dialog.value.building,
        room: dialog.value.room,
        status: dialog.value.status,
        remark: dialog.value.remark,
        preview: dialog.value.preview,
        price: dialog.value.price,
        area: dialog.value.area,
        amount: dialog.value.amount,
        additional: dialog.value.additional,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      store.quasar.loading.hide();
      clearTimeout(time);

       if (resp.data.status === 0) {
         store.quasar.notify('网络错误，请稍后重试');
       }

      dialog.value.show = false;
      store.quasar.notify('保存成功');
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      store.quasar.loading.hide();
      clearTimeout(time);
    });
};
</script>
