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

      <template v-slot:body-cell="props">
        <q-td v-if="props.col.name === 'id'" :props="props">
          <q-btn color="primary" :label="props.value" @click="onView(props.row)" />
        </q-td>

        <q-td v-else-if="props.col.name === 'info'" :props="props">
          <q-list bordered>
            <q-item v-if="props.row.type === 'store_register'">
              <q-item-section class="text-left">
                <div>
                  名称:
                  <div class="text-primary" style="display: inline-block">
                    {{ props.row.info.name }}
                  </div>
                </div>
                <div>类型: {{ props.row.info.type }}</div>
                <div>面积: {{ props.row.info.area }}平米</div>
                <div>单价: {{ props.row.info.price }}元</div>
                <div>价格: {{ props.row.info.price * props.row.info.area }}元</div>
                <div>经营范围: {{ props.row.info.manage }}</div>
                <div>健康证: {{ props.row.info.health }}</div>
                <div>备注: {{ props.row.info.remark }}</div>
              </q-item-section>
            </q-item>
          </q-list>
        </q-td>

        <q-td v-else :props="props">
          {{ props.value }}
        </q-td>
      </template>
    </q-table>
  </q-page>

  <q-dialog v-model="dialog.show">
    <q-card style="min-width: 70%">
      <q-card-section class="text-h6"> 报名信息 </q-card-section>

      <q-card-section>
        <div class="row">
          <q-input readonly filled class="col" v-model="dialog.id" label="ID" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.name" label="名称" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.type" label="类型" />
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
    options: ['', '名称', '联系电话'],
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
    name: 'name',
    label: '名称',
    field: 'name',
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
    name: 'info',
    label: '信息',
    field: 'info',
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
] as QTableProps['columns']);

const rows = ref([] as Record<string, unknown>[]);

const dialog = ref({
  show: false,
  mode: 'new',
  id: '',
  name: '',
  type: '',
  info: {},
  user: '',
  phone: '',
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
                  row[col.field == null ? (col.name as string) : (col.field as string)],
              col.format as ((val: unknown) => string) | undefined,
            ),
          )
          .join(','),
      ),
    )
    .join('\r\n');

  const status = exportFile('信息收集.csv', content, 'text/csv');

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

  const { page, rowsPerPage, sortBy, descending, rowsNumber } = props.pagination!;

  rows.value = [];

  let name = '',
    phone = '';

  switch (search.value.select.model) {
    case '名称':
      name = search.value.content;
      break;
    case '联系电话':
      phone = search.value.content;
      break;
  }

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=cbis&path=/api/publicinfo/count',
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
          store.server + '/api/route/post?auth=jwt&service=cbis&path=/api/publicinfo/list',
          {
            name: name,
            phone: phone,
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
                name: resp.data.data[i].name,
                type: resp.data.data[i].type,
                info: resp.data.data[i].info,
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
  dialog.value.mode = 'view';
  dialog.value.id = val.id as string;
  dialog.value.name = val.name as string;
  dialog.value.type = val.type as string;
  dialog.value.info = val.info as string;
  dialog.value.show = true;
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
        store.server +
          '/api/route/post?auth=jwt&service=cbis&path=/api/publicinfo/update?id=' +
          dialog.value.id +
          '&user=' +
          store.user.id,
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
            '/api/route/post?auth=jwt&service=cbis&path=/api/publicinfo/update?id=' +
            (element!.id as string) +
            '&user=' +
            store.user.id,
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
      store.server + '/api/route/post?auth=jwt&service=cbis&path=/api/ticket/update',
      {
        id: dialog.value.id,
        name: dialog.value.name,
        type: dialog.value.type,
        info: dialog.value.info,
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