<template>
  <q-page>
    <q-table
      square
      :rows="rows"
      :columns="columns"
      row-key="id"
      separator="cell"
      v-model:pagination="pagination"
      selection="multiple"
      v-model:selected="selected"
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
            @click="onRequest({ pagination: pagination })"
          />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col-2" color="negative" label="删除" @click="onDelete" />
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
    </q-table>
  </q-page>

  <q-dialog v-model="dialog.show">
    <q-card style="min-width: 70%">
      <q-card-section class="text-h6"> 预约信息 </q-card-section>

      <q-card-section>
        <div class="row">
          <q-input readonly filled class="col" v-model="dialog.id" label="ID" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.name" label="姓名" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.sex" label="性别" />
        </div>
      </q-card-section>

      <q-card-actions class="row">
        <q-btn class="col" color="negative" label="删除" @click="onDelete" />

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
    options: ['', '姓名'],
  },
});

const pagination = ref({
  page: 1,
  rowsNumber: 0,
  rowsPerPage: 20,
  sortBy: 'create_date',
  descending: true,
} as QTableProps['pagination']);

const selected = ref([] as TableRow[]);

const columns = ref([
  {
    name: 'id',
    label: 'ID',
    field: 'id',
    align: 'center',
    sortable: true,
  },
  {
    name: 'name',
    label: '姓名',
    field: 'name',
    align: 'center',
    sortable: true,
  },
  {
    name: 'sex',
    label: '性别',
    field: 'sex',
    align: 'center',
    sortable: true,
  },
  {
    name: 'idcard',
    label: '身份证',
    field: 'idcard',
    align: 'center',
    sortable: true,
  },
  {
    name: 'appointment',
    label: '预约时间',
    field: 'appointment',
    align: 'center',
    sortable: true,
  },
  {
    name: 'age',
    label: '年龄',
    field: 'age',
    align: 'center',
    sortable: true,
  },
  {
    name: 'birthday',
    label: '出生日期',
    field: 'birthday',
    align: 'center',
    sortable: true,
  },
  {
    name: 'marital',
    label: '已婚',
    field: 'marital',
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
    name: 'org1',
    label: '公司',
    field: 'org1',
    align: 'center',
    sortable: true,
  },
  {
    name: 'org2',
    label: '部门',
    field: 'org2',
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
] as QTableProps['columns']);

const rows = ref([] as TableRow[]);

const dialog = ref({
  show: false,
  mode: 'new',
  id: '',
  name: '',
  sex: '',
  idcard: '',
  age: '',
  birthday: '',
  marital: '',
  phone: '',
  org1: '',
  org2: '',
  remark: '',
  user: '',
});

interface TableRow {
  [key: string]: unknown;
  id?: string | number;
  name?: string;
  field?: string | ((row: Record<string, unknown>) => unknown);
  sex?: string;
  idcard?: string;
  appointment?: string;
  age?: string | number;
  birthday?: string;
  marital?: string;
  phone?: string;
  org1?: string;
  org2?: string;
  remark?: string;
  create_user?: string;
  create_date?: string;
  update_user?: string;
  update_date?: string;
}

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

  const content = [columns.value!.map((col) => wrapCsvValue(col.label))]
    .concat(
      rows.value.map((row: TableRow) =>
        columns
          .value!
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

  const status = exportFile('体检预约.csv', content, 'text/csv');

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

  const page = props.pagination?.page ?? 1;
  const rowsPerPage = props.pagination?.rowsPerPage ?? 20;
  const sortBy = props.pagination?.sortBy ?? null;
  const descending = props.pagination?.descending ?? true;
  const rowsNumber = props.pagination?.rowsNumber;

  rows.value = [];

  let name = '',
    phone = '';

  switch (search.value.select.model) {
    case '姓名':
      name = search.value.content;
      break;
    case '联系电话':
      phone = search.value.content;
      break;
  }

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=cbis&path=/api/healthex/count',
      {
        name: name,
        phone: phone,
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
          store.server + '/api/route/post?auth=jwt&service=cbis&path=/api/healthex/list',
          {
            name: name,
            phone: phone,
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
              rows.value.push({
                id: resp.data.data[i].id,
                name: resp.data.data[i].name,
                sex: resp.data.data[i].sex,
                appointment: resp.data.data[i].appointment,
                idcard: resp.data.data[i].idcard,
                age: resp.data.data[i].age,
                birthday: resp.data.data[i].birthday,
                marital: resp.data.data[i].marital,
                phone: resp.data.data[i].phone,
                org1: resp.data.data[i].org1,
                org2: resp.data.data[i].org2,
                remark: resp.data.data[i].remark,
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

const onDelete = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  if (selected.value.length === 0) {
    if (dialog.value.id === '') return;

    api
      .post(
        store.server + '/api/route/post?auth=jwt&service=cbis&path=/api/healthex/update',
        {
          method: 'update',
          id: dialog.value.id,
          user: store.user.id,
        },
        {
          headers: {
            Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
          },
        },
      )
      .then((resp) => {
        loading.value = false;
        clearTimeout(time);

        if (resp.data.status !== 1) {
          store.quasar.notify('删除失败，请稍后重试');
          return;
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

      api
        .post(
          store.server +
            '/api/route/post?auth=jwt&service=cbis&path=/api/healthex/update?id=' +
            (element?.id ?? '') +
            '&user=' +
            store.user.id,

          {
            method: 'delete',
            id: element?.id ?? 0,
            user: store.user.id,
          },
          {
            headers: {
              Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
            },
          },
        )
        .then((resp) => {
          if (resp.data.status !== 1) {
            store.quasar.notify('删除失败，请稍后重试');
            return;
          }

          store.quasar.notify('删除成功');
        })
        .catch(() => {
          store.quasar.notify('网络错误，请稍后重试');
          loading.value = false;
          clearTimeout(time);
        });
    }
  }

  loading.value = false;
  clearTimeout(time);
  onRequest({ pagination: pagination.value });
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
      store.server + '/api/route/post?auth=jwt&service=cbis&path=/api/healthex/update',
      {
        method: 'add',
        id: dialog.value.id,
        name: dialog.value.name,
        sex: dialog.value.sex,
        idcard: dialog.value.idcard,
        age: dialog.value.age,
        birthday: dialog.value.birthday,
        marital: dialog.value.marital,
        phone: dialog.value.phone,
        org1: dialog.value.org1,
        org2: dialog.value.org2,
        remark: dialog.value.remark,
        user: store.user.id,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
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
