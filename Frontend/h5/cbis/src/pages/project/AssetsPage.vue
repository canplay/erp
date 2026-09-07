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

          <q-input class="col-4" v-model="search.content" label="查询内容" />

          <div class="col-auto" style="width: 8px" />

          <q-btn
            class="col"
            color="secondary"
            label="查询"
            @click="onRequest({ pagination: pagination })"
          />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col" color="primary" label="新增" @click="onAdd" />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col" color="negative" label="删除" @click="onDelete" />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col" color="green" label="导出" @click="onExport" />
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
    <q-card style="min-width: 70%">
      <q-card-section class="text-h6"> 资产信息 </q-card-section>

      <q-card-section>
        <div class="row">
          <q-input readonly filled class="col" v-model="dialog.id" label="ID" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.name" label="名称" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.class" label="类别" />
        </div>

        <div style="height: 8px" />

        <div class="row">
          <q-input filled class="col" v-model="dialog.license" label="证件" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.pledge" label="抵押" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.cost" label="价值" />
        </div>

        <div style="height: 8px" />

        <div class="row">
          <q-input filled class="col" v-model="dialog.deed" label="产权证" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.type" label="性质" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.purpose" label="用途" />
        </div>

        <div style="height: 8px" />

        <div class="row">
          <q-input filled class="col" v-model="dialog.reply" label="批复" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.status" label="使用情况" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.area" label="面积" />
        </div>

        <div style="height: 8px" />

        <div class="row">
          <q-input filled class="col" v-model="dialog.street" label="位置" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.location" label="地理坐标" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" filled v-model="dialog.income" label="收益" />
        </div>

        <div style="height: 8px" />

        <div class="row">
          <q-input filled class="col" v-model="dialog.remark" label="备注" />

          <div class="col-auto" style="width: 8px" />

          <q-select
            filled
            class="col"
            v-model="dialog.public"
            :options="['是', '否']"
            label="信息公开"
          />
        </div>

        <div style="height: 8px" />

        <q-uploader
          class="fit"
          multiple
          url="http://localhost:4444/upload"
          :headers="[{ name: 'Authorization', value: '' }]"
          label="附件"
        />
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
import { useStore } from 'src/stores/store';
import { api } from 'src/boot/axios';

const store = useStore();

const loading = ref(false);

const search = ref({
  date: '',
  content: '',
  select: {
    model: '',
    options: ['', '名称'],
  },
});

const pagination = ref({
  page: 1,
  rowsNumber: 0,
  rowsPerPage: 20,
  sortBy: 'create_date',
  descending: true,
} as QTableProps['pagination']);

interface TableRow {
  id?: string | number;
  name?: string;
  class?: string;
  license?: string;
  pledge?: string;
  cost?: string;
  deed?: string;
  type?: string;
  purpose?: string;
  reply?: string;
  status?: string;
  area?: string;
  street?: string;
  location?: string;
  income?: string;
  remark?: string;
  attachment?: string;
  public?: string;
  [key: string]: unknown;
}

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
    label: '名称',
    field: 'name',
    align: 'center',
    sortable: true,
  },
  {
    name: 'class',
    label: '类别',
    field: 'class',
    align: 'center',
    sortable: true,
  },
  {
    name: 'license',
    label: '证件',
    field: 'license',
    align: 'center',
    sortable: true,
  },
  {
    name: 'pledge',
    label: '抵押',
    field: 'pledge',
    align: 'center',
    sortable: true,
  },
  {
    name: 'cost',
    label: '价值',
    field: 'cost',
    align: 'center',
    sortable: true,
  },
  {
    name: 'deed',
    label: '产权',
    field: 'deed',
    align: 'center',
    sortable: true,
  },
  {
    name: 'type',
    label: '性质',
    field: 'type',
    align: 'center',
    sortable: true,
  },
  {
    name: 'purpose',
    label: '用途',
    field: 'purpose',
    align: 'center',
    sortable: true,
  },
  {
    name: 'reply',
    label: '批复',
    field: 'reply',
    align: 'center',
    sortable: true,
  },
  {
    name: 'status',
    label: '使用情况',
    field: 'status',
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
    name: 'street',
    label: '位置',
    field: 'street',
    align: 'center',
    sortable: true,
  },
  {
    name: 'location',
    label: '地理坐标',
    field: 'location',
    align: 'center',
    sortable: true,
  },
  {
    name: 'income',
    label: '收益',
    field: 'income',
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
    name: 'attachment',
    label: '附件',
    field: 'attachment',
    align: 'center',
    sortable: true,
  },
  {
    name: 'public',
    label: '信息公开',
    field: 'public',
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
  class: '',
  license: '',
  pledge: '',
  cost: '',
  deed: '',
  type: '',
  purpose: '',
  reply: '',
  status: '',
  area: '',
  street: '',
  location: '',
  income: '',
  remark: '',
  attachment: '',
  public: '',
});

const onExport = () => {
  
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
                type: resp.data.data[i].type,
                info: resp.data.data[i].info,
                contract_user: resp.data.data[i].create_user.msg.user.nickname,
                contract_phone: resp.data.data[i].create_user.msg.user.nickname,
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

const onView = (val: TableRow) => {
  dialog.value.mode = 'view';
  dialog.value.id = val.id?.toString() ?? '';
  dialog.value.name = val.name ?? '';
  dialog.value.class = val.class ?? '';
  dialog.value.license = val.license ?? '';
  dialog.value.pledge = val.pledge ?? '';
  dialog.value.cost = val.cost ?? '';
  dialog.value.deed = val.deed ?? '';
  dialog.value.type = val.type ?? '';
  dialog.value.purpose = val.purpose ?? '';
  dialog.value.reply = val.reply ?? '';
  dialog.value.status = val.status ?? '';
  dialog.value.area = val.area ?? '';
  dialog.value.street = val.street ?? '';
  dialog.value.location = val.location ?? '';
  dialog.value.income = val.income ?? '';
  dialog.value.remark = val.remark ?? '';
  dialog.value.attachment = val.attachment ?? '';
  dialog.value.public = val.public ?? '';
  dialog.value.show = true;
};

const onAdd = () => {
  dialog.value.mode = 'add';
  dialog.value.id = '';
  dialog.value.name = '';
  dialog.value.class = '';
  dialog.value.license = '';
  dialog.value.pledge = '';
  dialog.value.cost = '';
  dialog.value.deed = '';
  dialog.value.type = '';
  dialog.value.purpose = '';
  dialog.value.reply = '';
  dialog.value.status = '';
  dialog.value.area = '';
  dialog.value.street = '';
  dialog.value.location = '';
  dialog.value.income = '';
  dialog.value.remark = '';
  dialog.value.attachment = '';
  dialog.value.public = '';
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
            (element?.id ?? '') +
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
