<template>
  <q-page>
    <q-table
      square
      :rows="rows"
      :columns="columns"
      row-key="id"
      separator="cell"
      v-model:pagination="pagination"
      :filter="filterInput"
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

          <q-input class="col" v-model="search.content" label="查询内容" />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col-2" color="green" label="导出" @click="onExport" />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col-2" color="red" label="重置" @click="onReset" />

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
        <q-td v-if="props.col.name === 'no'" :props="props">
          <q-btn color="primary" :label="props.value" @click="onView(props.row)" />
        </q-td>

        <q-td v-else-if="props.col.name === 'invalid'" :props="props">
          {{ props.value === true ? '是' : '否' }}
        </q-td>

        <q-td v-else :props="props">
          {{ props.value }}
        </q-td>
      </template>
    </q-table>
  </q-page>

  <q-inner-loading :showing="loading">
    <q-spinner size="50px" color="primary" />
  </q-inner-loading>
</template>

<script setup lang="ts">
import { ref } from 'vue';
import type { QTableProps } from 'quasar';
import { exportFile } from 'quasar';
import { useStore } from 'src/stores/store';

const store = useStore();
const loading = ref(false);

const search = ref({
  date: '',
  content: '',
  select: {
    model: '',
    options: ['', '名称', '类型', '状态', '备注'],
  },
});

const pagination = ref({
  page: 1,
  rowsNumber: 0,
  rowsPerPage: 20,
  sortBy: '',
  descending: true,
} as QTableProps['pagination']);

const filterInput = ref('');

const columns = ref([
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
    name: 'num',
    label: '数量',
    field: 'num',
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
    name: 'create_date',
    label: '创建时间',
    field: 'create_date',
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
    name: 'update_date',
    label: '更新时间',
    field: 'update_date',
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
] as QTableProps['columns']);

const rows = ref([] as Record<string, unknown>[]);

const onReset = () => {
  search.value.select.model = '';
  search.value.date = '';
  search.value.content = '';
};

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
                ? col.field(row)
                : row[col.field === void 0 ? (col.name as string) : (col.field as string)],
              col.format as ((val: string) => string) | undefined,
            ),
          )
          .join(','),
      ),
    )
    .join('\r\n');

  const status = exportFile('票据列表.csv', content, 'text/csv');

  if (status !== true) {
    store.quasar.notify('即将开始下载，请稍后...');
  }

  loading.value = false;
  clearTimeout(time);
};

const onRequest = (props: Record<string, unknown>) => {
  void props;
};

const onQuery = () => {
};

const onNew = (val: Event) => {
  void val;
};

const onView = (val: Record<string, unknown>) => {
  void val;
};
</script>
