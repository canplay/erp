<template>
  <q-table
    square
    :rows="rows"
    :columns="columns"
    row-key="id"
    separator="cell"
    v-model:pagination="pagination"
    :filter="filter"
    @request="onRequest"
    virtual-scroll
    :style="`${store.height};width: calc(100vw - 200px)`"
  >
    <template v-slot:top>
      <div class="column fit">
        <div class="col row">
          <q-btn class="col-2" color="primary" label="查询" @click="onQuery" />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col-2" color="green" label="导出" @click="onExport" />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col-2" color="red" label="新增" @click="onNew" />
        </div>
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
          @click="onView(props.row)"
        />
      </q-td>

      <q-td v-else :props="props">
        {{ props.value }}
      </q-td>
    </template>
  </q-table>
</template>

<script setup lang="ts">
import { ref } from 'vue';
import type { QTableProps } from 'quasar';
import { useStore } from 'src/stores/store';

const store = useStore();

const pagination = ref({
  page: 1,
  rowsNumber: 0,
  rowsPerPage: 20,
  sortBy: '',
  descending: true,
} as QTableProps['pagination']);

const filter = ref('');

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
    label: '门牌',
    field: 'name',
    align: 'center',
    sortable: true,
  },
  {
    name: 'master',
    label: '户主',
    field: 'master',
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
    name: 'idcard',
    label: '身份证',
    field: 'idcard',
    align: 'center',
    sortable: true,
  },
  {
    name: 'type',
    label: '户型',
    field: 'type',
    align: 'center',
    sortable: true,
  },
  {
    name: 'parking',
    label: '车位',
    field: 'parking',
    align: 'center',
    sortable: true,
  },
  {
    name: 'pay_level_property',
    label: '物业资费等级',
    field: 'pay_level_property',
    align: 'center',
    sortable: true,
  },
  {
    name: 'pay_level_clean',
    label: '清运资费等级',
    field: 'pay_level_clean',
    align: 'center',
    sortable: true,
  },
  {
    name: 'pay_level_water',
    label: '供水资费等级',
    field: 'pay_level_water',
    align: 'center',
    sortable: true,
  },
  {
    name: 'pay_last_propery',
    label: '最近物业缴费时间',
    field: 'pay_last_propery',
    align: 'center',
    sortable: true,
  },
  {
    name: 'pay_last_clean',
    label: '最近清运缴费时间',
    field: 'pay_last_clean',
    align: 'center',
    sortable: true,
  },
  {
    name: 'pay_last_water',
    label: '最近供水缴费时间',
    field: 'pay_last_water',
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

const rows = ref([] as Record<string, unknown>[]);

const onExport = () => {
};

const onRequest = () => {
};

const onQuery = () => {
};

const onNew = () => {
};

const onView = (val: Record<string, unknown>) => {
  void val;
};
</script>
