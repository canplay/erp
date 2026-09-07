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
      virtual-scroll
      :style="`${store.height};width: calc(100vw - 200px)`"
    >
      <template v-slot:top>
        <div class="col row">
          <q-select
            class="col-1"
            v-model="search.select.model"
            :options="search.select.options"
            label="查询类型"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col-4" v-model="search.content" label="查询内容" />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col" color="green" label="导出" @click="onExport" />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col" color="red" label="重置" @click="onReset" />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col" color="primary" label="查询" @click="onQuery" />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col" color="red" label="新增" @click="onNew" />
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

    <q-dialog v-model="dialog.show">
      <q-card style="min-width: 70%">
        <q-card-section> </q-card-section>
        <q-card-actions class="row">
          <q-btn class="col" color="negative" label="取消" style="font-size: 20px" v-close-popup />

          <div class="col-auto" style="width: 8px" />

          <q-btn
            class="col"
            color="positive"
            label="保存"
            style="font-size: 20px"
            @click="onSave"
          />
        </q-card-actions>
      </q-card>
    </q-dialog>

    <q-inner-loading :showing="loading">
      <q-spinner size="50px" color="primary" />
    </q-inner-loading>
  </q-page>
</template>

<script setup lang="ts">
import { ref } from 'vue';
import type { QTableProps } from 'quasar';
import { date, exportFile } from 'quasar';
import { useStore } from 'stores/store';
import { api } from 'src/boot/axios';

const store = useStore();

const loading = ref(false);

const search = ref({
  date: '',
  content: '',
  select: {
    model: '',
    options: ['', '名称', '法人', '经办人', '手机号', '备注'],
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
    name: 'name',
    label: '名称',
    field: 'name',
    align: 'center',
    sortable: true,
  },
  {
    name: 'representative',
    label: '法人',
    field: 'representative',
    align: 'center',
    sortable: true,
  },
  {
    name: 'address',
    label: '地址',
    field: 'address',
    align: 'center',
    sortable: true,
  },
  { name: 'capital', label: '注册资本金', field: 'capital', align: 'center' },
  {
    name: 'agent',
    label: '经办人',
    field: 'agent',
    align: 'center',
    sortable: true,
  },
  { name: 'phone', label: '经办人电话', field: 'phone', align: 'center' },
  {
    name: 'class1',
    label: '行业大类',
    field: 'class1',
    align: 'center',
    sortable: true,
  },
  {
    name: 'class2',
    label: '行业小类',
    field: 'class2',
    align: 'center',
    sortable: true,
  },
  {
    name: 'upload',
    label: '附件',
    field: 'upload',
    align: 'center',
    sortable: true,
  },
  {
    name: 'type1',
    label: '类别',
    field: 'type1',
    align: 'center',
    sortable: true,
  },
  {
    name: 'type2',
    label: '等级',
    field: 'type2',
    align: 'center',
    sortable: true,
  },
  {
    name: 'score',
    label: '最近评分',
    field: 'score',
    align: 'center',
    sortable: true,
  },
  {
    name: 'level',
    label: '最近评级',
    field: 'level',
    align: 'center',
    sortable: true,
  },
  {
    name: 'reward',
    label: '最近奖惩',
    field: 'reward',
    align: 'center',
    sortable: true,
  },
  {
    name: 'examine',
    label: '最近评分附件',
    field: 'examine',
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

interface SupplierRow {
  id: string;
  name: string;
  representative: string;
  address: string;
  capital: string;
  agent: string;
  phone: string;
  class1: string;
  class2: string;
  create_user: string;
  create_date: string;
  update_user: string;
  update_date: string;
  upload: string;
  type1: string;
  type2: string;
  score: string;
  level: string;
  reward: string;
  examine: string;
  remark: string;
}

const rows = ref<SupplierRow[]>([]);

const dialog = ref({
  show: false,
  mode: 'create',
  id: '',
  name: '',
  representative: '',
  address: '',
  capital: '',
  agent: '',
  phone: '',
  class1: '',
  class2: '',
  upload: [
    {
      name: 'license',
      label: '营业执照',
      field: 'license',
      align: 'center',
    },
    {
      name: 'id_card1',
      label: '身份证正面',
      field: 'id_card1',
      align: 'center',
    },
    {
      name: 'id_card2',
      label: '身份证反面',
      field: 'id_card2',
      align: 'center',
    },
    {
      name: 'authorization',
      label: '代理人授权书',
      field: 'authorization',
      align: 'center',
    },
    {
      name: 'id_card3',
      label: '代理人身份证正面',
      field: 'id_card3',
      align: 'center',
    },
    {
      name: 'id_card4',
      label: '代理人身份证反面',
      field: 'id_card4',
      align: 'center',
    },
  ] as QTableProps['columns'],
  type1: '',
  type2: '',
  score: '',
  level: '',
  reward: '',
  examine: '',
  remark: '',
});

const onReset = () => {
  search.value.select.model = '';
  search.value.date = '';
  search.value.content = '';
};

const wrapCsvValue = (
  val: string,
  formatFn?: (val: string, row?: QTableProps['rows'][0]) => string,
  row?: QTableProps['rows'][0],
) => {
  let formatted = formatFn !== void 0 ? formatFn(val, row) : val;

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
      rows.value.map((row) =>
        columns
          .value!.map((col) =>
            wrapCsvValue(
              typeof col.field === 'function'
                ? col.field(row)
                : (row as Record<string, unknown>)[col.field === void 0 ? col.name : col.field] as string,
              col.format,
              row,
            ),
          )
          .join(','),
      ),
    )
    .join('\r\n');

  const status = exportFile('供应商列表.csv', content, 'text/csv');

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

  let name = '';
  let representative = '';
  let agent = '';
  let phone = '';
  let remark = '';

  switch (search.value.select.model) {
    case '名称':
      name = search.value.content;
      break;
    case '法人':
      representative = search.value.content;
      break;
    case '经办人':
      agent = search.value.content;
      break;
    case '手机号':
      phone = search.value.content;
      break;
    case '备注':
      remark = search.value.content;
      break;
  }

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=cbis&path=/api/customer/supplier/list',
      {
        id: '',
        name: name,
        representative: representative,
        agent: agent,
        phone: phone,
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
            name: resp.data.data[i].name,
            representative: resp.data.data[i].representative,
            address: resp.data.data[i].address,
            capital: resp.data.data[i].capital,
            agent: resp.data.data[i].agent,
            phone: resp.data.data[i].phone,
            class1: resp.data.data[i].class1,
            class2: resp.data.data[i].class2,
            upload: JSON.parse(resp.data.data[i].upload),
            type1: resp.data.data[i].type1,
            type2: resp.data.data[i].type2,
            score: resp.data.data[i].score,
            level: resp.data.data[i].level,
            reward: resp.data.data[i].reward,
            examine: resp.data.data[i].examine,
            remark: resp.data.data[i].remark,
            create_user: resp.data.data[i].create_user,
            create_date: date.formatDate(resp.data.data[i].create_date, 'YYYY-MM-DD HH:mm:ss'),
            update_user: resp.data.data[i].update_user,
            update_date: date.formatDate(resp.data.data[i].update_date, 'YYYY-MM-DD HH:mm:ss'),
          });
        }
      } else {
        store.quasar.notify(resp.data.message);
      }

      pagination.value!.page = page!;
      pagination.value!.rowsPerPage = rowsPerPage!;
      pagination.value!.sortBy = sortBy!;
      pagination.value!.descending = descending!;

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
  let representative = '';
  let agent = '';
  let phone = '';
  let remark = '';

  switch (search.value.select.model) {
    case '名称':
      name = search.value.content;
      break;
    case '法人':
      representative = search.value.content;
      break;
    case '经办人':
      agent = search.value.content;
      break;
    case '手机号':
      phone = search.value.content;
      break;
    case '备注':
      remark = search.value.content;
      break;
  }

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=cbis&path=/api/customer/supplier/count',
      {
        id: '',
        name: name,
        representative: representative,
        agent: agent,
        phone: phone,
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

const onView = (val: Record<string, unknown>) => {
  dialog.value.mode = 'view';
  dialog.value.id = val.id as string;
  dialog.value.name = val.name as string;
  dialog.value.representative = val.representative as string;
  dialog.value.address = val.address as string;
  dialog.value.capital = val.capital as string;
  dialog.value.agent = val.agent as string;
  dialog.value.phone = val.phone as string;
  dialog.value.class1 = val.class1 as string;
  dialog.value.class2 = val.class2 as string;
  dialog.value.upload = val.upload as QTableProps['columns'];
  dialog.value.type1 = val.type1 as string;
  dialog.value.type2 = val.type2 as string;
  dialog.value.score = val.score as string;
  dialog.value.level = val.level as string;
  dialog.value.reward = val.reward as string;
  dialog.value.examine = val.examine as string;
  dialog.value.remark = val.remark as string;

  dialog.value.show = true;
};

const onNew = () => {
  dialog.value.mode = 'create';
  dialog.value.id = '';
  dialog.value.name = '';
  dialog.value.representative = '';
  dialog.value.address = '';
  dialog.value.capital = '';
  dialog.value.agent = '';
  dialog.value.phone = '';
  dialog.value.class1 = '';
  dialog.value.class2 = '';
  dialog.value.upload = [];
  dialog.value.type1 = '';
  dialog.value.type2 = '';
  dialog.value.score = '';
  dialog.value.level = '';
  dialog.value.reward = '';
  dialog.value.examine = '';
  dialog.value.remark = '';

  dialog.value.show = true;
};

const onSave = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;
};
</script>
