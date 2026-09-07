<template>
  <q-page>
    <q-table
      :rows="rows"
      :columns="columns"
      row-key="id"
      separator="cell"
      v-model:pagination="pagination"
      :filter="filterInput"
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

          <q-btn class="col-2" color="primary" label="查询" @click="onQuery" />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col-2" color="red" label="新增" @click="onNew" />
        </div>
      </template>

      <template v-slot:header-cell="props">
        <q-th :props="props" style="font-size: 15px; font-weight: bold">
          {{ props.col.label }}
        </q-th>
      </template>

      <template v-slot:body-cell="props">
        <q-td v-if="props.col.name === 'id'" :props="props">
          <q-btn
            color="primary"
            :label="props.value"
            @click="onShow(props.row)"
          />
        </q-td>

        <q-td v-else-if="props.col.name === 'delete'" :props="props">
          {{ props.value === true ? '是' : '否' }}
        </q-td>

        <q-td v-else :props="props">
          {{ props.value }}
        </q-td>
      </template>
    </q-table>
  </q-page>

  <q-dialog v-model="dialog.show">
    <q-card style="min-width: 70%">
      <q-card-section>
        <div class="row">
          <q-input class="col" v-model="dialog.id" readonly label="ID" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" v-model="dialog.name" label="名称" />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col-2" color="primary" label="权限" />
        </div>

        <div class="col-auto" style="height: 8px" />

        <div class="row">
          <q-input
            class="col"
            v-model="dialog.create_user"
            readonly
            label="创建人"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            v-model="dialog.create_date"
            readonly
            label="创建时间"
          />
        </div>

        <div class="col-auto" style="height: 8px" />

        <div class="row">
          <q-input
            class="col"
            v-model="dialog.update_user"
            readonly
            label="更新人"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            v-model="dialog.update_date"
            readonly
            label="更新时间"
          />
        </div>

        <div class="col-auto" style="height: 8px" />

        <div class="row">
          <q-btn
            class="col"
            color="negative"
            label="取消"
            style="font-size: 20px"
            v-close-popup
          />

          <div class="col-auto" style="width: 8px" />

          <q-btn
            class="col"
            color="positive"
            label="保存"
            style="font-size: 20px"
            @click="onUpdate"
          />
        </div>
      </q-card-section>
    </q-card>
  </q-dialog>

  <q-inner-loading :showing="loading">
    <q-spinner size="50px" color="primary" />
  </q-inner-loading>
</template>

<script setup lang="ts">
import { ref } from 'vue';
import type { QTableProps} from 'quasar';
import { exportFile } from 'quasar';
import { useStore } from 'stores/store';
import { api } from 'src/boot/axios';

const store = useStore();

const loading = ref(false);

const search = ref({
  date: '',
  content: '',
  select: {
    model: '',
    options: ['', '名称', '是否删除'],
  },
});

const pagination = ref({
  page: 1,
  rowsNumber: 0,
  rowsPerPage: 20,
  sortBy: 'update_date',
  descending: true,
} as QTableProps['pagination']);

const filterInput = ref('');

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
    label: '名称',
    field: 'name',
    align: 'center',
    sortable: true,
  },
  {
    name: 'access',
    label: '权限',
    field: 'access',
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
    label: '更新日期',
    field: 'update_date',
    align: 'center',
    sortable: true,
  },
  {
    name: 'delete',
    label: '是否删除',
    field: 'delete',
    align: 'center',
    sortable: true,
  },
] as NonNullable<QTableProps['columns']>);

const rows = ref([] as Record<string, unknown>[]);

const dialog = ref({
  show: false,
  id: '',
  name: '',
  access: '',
  delete: '',
  create_user: '',
  create_date: '',
  update_user: '',
  update_date: '',
});

const wrapCsvValue = (val: unknown, formatFn?: (val: unknown) => unknown) => {
  let formatted = formatFn !== void 0 ? formatFn(val) : val;

  if (formatted === void 0 || formatted === null) {
    return '';
  }

  if (typeof formatted === 'object') {
    formatted = JSON.stringify(formatted);
  }

  formatted = String(formatted).split('"').join('""');

  return typeof formatted === 'string' ? formatted : String(formatted);
};

const onExport = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  const content = [columns.value.map((col) => wrapCsvValue(col.label))]
    .concat(
rows.value.map((row: Record<string, unknown>) =>
        columns
.value.map((col: { field: string | ((row: Record<string, unknown>) => unknown); format?: (...args: unknown[]) => unknown; name: string }) =>
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

  const status = exportFile('权限列表.csv', content, 'text/csv');

  if (status !== true) {
    store.quasar.notify('即将开始下载，请稍后...');
  }

  loading.value = false;
  clearTimeout(time);
};

const onRequest = (props: { pagination: QTableProps['pagination'] }) => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  const { page = 1, rowsPerPage = 20, sortBy = 'update_date', descending = true, rowsNumber = 0 } = props.pagination ?? {};

  rows.value = [];

  let name = '';
  let isDelete = false;

  switch (search.value.select.model) {
    case '名称':
      name = search.value.content;
      break;
    case '是否删除':
      isDelete = search.value.content === '是' ? true : false;
      break;
  }

  api
    .post(
      store.server +
        '/api/route/post?auth=jwt&service=cbis&path=/api/permission/list',
      {
        name: name,
        isDelete: isDelete,
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
            name: resp.data.data[i].name,
            access: resp.data.data[i].access,
            delete: resp.data.data[i].delete,
            create_user: resp.data.data[i].create_user.msg.user.nickname,
            create_date: resp.data.data[i].create_date,
            update_user: resp.data.data[i].update_user.msg.user.nickname,
            update_date: resp.data.data[i].update_date,
          });
        }
      } else {
        store.quasar.notify('网络错误，请稍后重试');
      }

      pagination.value!.page = page;
      pagination.value!.rowsPerPage = rowsPerPage;
      pagination.value!.sortBy = sortBy;
      pagination.value!.descending = descending;

      loading.value = false;
      clearTimeout(time);
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      loading.value = false;
      clearTimeout(time);
    });
};

const onQuery = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  let name = '';
  let isDelete = false;

  switch (search.value.select.model) {
    case '名称':
      name = search.value.content;
      break;
    case '是否删除':
      isDelete = search.value.content === '是' ? true : false;
      break;
  }

  api
    .post(
      store.server +
        '/api/route/post?auth=jwt&service=cbis&path=/api/permission/count',
      {
        name: name,
        isDelete: isDelete,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      }
    )
    .then((resp) => {
      pagination.value!.rowsNumber = parseInt(resp.data.data);
      onRequest({ pagination: pagination.value });

      loading.value = false;
      clearTimeout(time);
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      loading.value = false;
      clearTimeout(time);
    });
};

const onNew = () => {
  
};

interface PermissionRowData {
  id: string | number | null;
  name: string | number | null;
  access: string | number | null;
  delete: boolean | string | null;
  create_user: { msg: { user: { nickname: string | null } } } | null;
  create_date: string | number | null;
  update_user: { msg: { user: { nickname: string | null } } } | null;
  update_date: string | number | null;
}

const onShow = (val: PermissionRowData) => {
  dialog.value.id = val.id == null ? '' : String(val.id);
  dialog.value.name = val.name == null ? '' : String(val.name);
  dialog.value.access = val.access == null ? '' : String(val.access);
  dialog.value.delete = val.delete == null ? '' : String(val.delete);
  dialog.value.create_user = val.create_user?.msg?.user?.nickname ?? '';
  dialog.value.create_date = val.create_date == null ? '' : String(val.create_date);
  dialog.value.update_user = val.update_user?.msg?.user?.nickname ?? '';
  dialog.value.update_date = val.update_date == null ? '' : String(val.update_date);
  dialog.value.show = true;
};

const onUpdate = () => {
  
};
</script>
