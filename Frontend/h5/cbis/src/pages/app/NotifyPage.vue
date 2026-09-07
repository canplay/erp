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

          <q-btn
            class="col-2"
            color="negative"
            label="删除"
            @click="onDelete"
          />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col-2" color="secondary" label="新增" @click="onNew" />
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

        <q-td v-else :props="props">
          {{ props.value }}
        </q-td>
      </template>
    </q-table>
  </q-page>

  <q-dialog v-model="dialog.show">
    <q-card style="min-width: 70%">
      <q-card-section class="text-h6">消息通知</q-card-section>

      <q-card-section>
        <div class="row">
          <q-input readonly filled class="col" v-model="dialog.id" label="ID" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" v-model="dialog.topic" label="主题" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" v-model="dialog.subscriber" label="订阅者" />

          <div class="col-auto" style="width: 8px" />

          <q-select
            class="col"
            v-model="dialog.type"
            :options="dialog.options"
            label="类型"
          />
        </div>

        <div class="row" v-if="dialog.type === '全局支付'">
          <div class="col-auto" style="width: 8px" />

          <q-input class="col" v-model="dialog.msg" label="消息内容" />
        </div>

        <div class="row" v-if="dialog.type === '供排水公司微信订阅通知'">
          <div class="col-auto" style="width: 8px" />

          <q-input class="col" v-model="dialog.msg" label="停水时间" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" v-model="dialog.msg" label="停水区域" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" v-model="dialog.msg" label="停水原因" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" v-model="dialog.msg" label="温馨提示" />
        </div>
      </q-card-section>

      <q-card-actions class="row">
        <q-btn class="col" color="negative" label="删除" @click="onDelete" />

        <div class="col-auto" style="width: 8px" />

        <q-btn class="col" color="primary" label="保存" @click="onSave" />
      </q-card-actions>
    </q-card>
  </q-dialog>
</template>

<script setup lang="ts">
import { ref } from 'vue';
import type { QTableProps} from 'quasar';
import { exportFile } from 'quasar';
import { useStore } from 'src/stores/store';
import { api } from 'src/boot/axios';

const store = useStore();

const search = ref({
  date: '',
  content: '',
  select: {
    model: '',
    options: ['', '消息'],
  },
});

const pagination = ref<QTableProps['pagination']>({
  page: 1,
  rowsNumber: 0,
  rowsPerPage: 20,
  sortBy: 'create_date',
  descending: true,
});

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
    name: 'topic',
    label: '主题',
    field: 'topic',
    align: 'center',
    sortable: true,
  },
  {
    name: 'subscriber',
    label: '订阅者',
    field: 'subscriber',
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
    name: 'msg',
    label: '消息内容',
    field: 'msg',
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

const dialog = ref({
  show: false,
  mode: 'new',
  options: ['全局支付', '供排水公司微信订阅通知'],
  id: '',
  topic: '',
  subscriber: '',
  type: '全局支付',
  msg: '',
  time: '',
  location: '',
  reason: '',
  remark: '',
});

const wrapCsvValue = (val: string, formatFn?: (val: string) => string) => {
  let formatted = formatFn !== void 0 ? formatFn(val) : val;

  formatted =
    formatted === void 0 || formatted === null ? '' : String(formatted);

  formatted = formatted.split('"').join('""');

  return `${formatted}`;
};

const onExport = () => {
  const time = setTimeout(() => {
    store.quasar.loading.hide();
    clearTimeout(time);
  }, 120000);

  store.quasar.loading.show();

  const content = [columns.value!.map((col) => wrapCsvValue(col.label))]
    .concat(
      rows.value.map((row: Record<string, unknown>) =>
        columns
          .value!.map((col: Record<string, unknown>) =>
            wrapCsvValue(
              typeof col.field === 'function'
                ? col.field(row)
                : row[col.field == null ? (col.name as string) : (col.field as string)],
              col.format as ((val: unknown) => string) | undefined
            )
          )
          .join(',')
      )
    )
    .join('\r\n');

  const status = exportFile('通知消息.csv', content, 'text/csv');

  if (status !== true) {
    store.quasar.notify('即将开始下载，请稍后...');
  }

  store.quasar.loading.hide();
  clearTimeout(time);
};

const onRequest = (props: Record<string, unknown>) => {
  const time = setTimeout(() => {
    store.quasar.loading.hide();
    clearTimeout(time);
  }, 120000);

  store.quasar.loading.show();

  const pagination = ref(props.pagination as { page: number; rowsPerPage: number; sortBy: string; descending: boolean; rowsNumber: number });
  const { page, rowsPerPage, sortBy, descending, rowsNumber } =
    pagination.value;

  rows.value = [];

  const name = '';
  let content = '';

  switch (search.value.select.model) {
    case '消息':
      content = search.value.content;
      break;
  }

  api
    .post(
      store.server +
        '/api/route/post?auth=jwt&service=notify&path=/api/notify/count',
      {
        name: name,
        msg: content,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      }
    )
    .then((resp) => {
      pagination.value.rowsNumber = parseInt(resp.data.data);

      if (resp.data.status === 0 || parseInt(resp.data.data) <= 0) {
        store.quasar.loading.hide();
        clearTimeout(time);
        store.quasar.notify('没有数据');
        return;
      }

      api
        .post(
          store.server +
            '/api/route/post?auth=jwt&service=notify&path=/api/notify/list',
          {
            name: name,
            msg: content,
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
                topic: resp.data.data[i].topic,
                subscriber: resp.data.data[i].subscriber,
                type: resp.data.data[i].type,
                msg: resp.data.data[i].msg,
                status: resp.data.data[i].status,
                create_user: resp.data.data[i].create_user.msg.user.nickname,
                create_date: resp.data.data[i].create_date,
                update_user: resp.data.data[i].update_user.msg.user.nickname,
                update_date: resp.data.data[i].update_date,
              });
            }
          } else {
            store.quasar.notify('网络错误，请稍后重试');
          }

          store.quasar.loading.hide();
          clearTimeout(time);

           pagination.value.page = page;
           pagination.value.rowsPerPage = rowsPerPage;
           pagination.value.sortBy = sortBy;
           pagination.value.descending = descending;
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

interface NotifyRow {
  id: string | number | null;
  topic: string | number | null;
  type: string | number | null;
  msg: string | number | null;
}

const onView = (val: NotifyRow) => {
  dialog.value.mode = 'view';
  dialog.value.id = val.id == null ? '' : String(val.id);
  dialog.value.topic = val.topic == null ? '' : String(val.topic);
  dialog.value.type = val.type == null ? '' : String(val.type);
  dialog.value.msg = val.msg == null ? '' : String(val.msg);
  dialog.value.show = true;
};

const onNew = () => {
  dialog.value.mode = 'new';
  dialog.value.id = '';
  dialog.value.topic = '';
  dialog.value.type = '';
  dialog.value.msg = '';
  dialog.value.show = true;
};

const onDelete = () => {
  const time = setTimeout(() => {
    store.quasar.loading.hide();
    clearTimeout(time);
  }, 120000);

  store.quasar.loading.show();

  if (selected.value.length === 0) {
    if (dialog.value.id === '') return;

    api
      .post(
        store.server +
          '/api/route/post?auth=jwt&service=notify&path=/api/notify/update?id=' +
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
        store.quasar.loading.hide();
        clearTimeout(time);

        if (resp.data.status !== 1) {
          store.quasar.notify('删除失败，请稍后重试');
          return;
        }

        store.quasar.notify('删除成功');
      })
      .catch(() => {
        store.quasar.notify('网络错误，请稍后重试');
        store.quasar.loading.hide();
        clearTimeout(time);
      });
  } else {
    for (let index = 0; index < selected.value.length; index++) {
      const element = selected.value[index];

      api
        .post(
          store.server +
            '/api/route/post?auth=jwt&service=notify&path=/api/notify/update?id=' +
            String(element!.id) +
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
            return;
          }

          store.quasar.notify('删除成功');
        })
        .catch(() => {
          store.quasar.notify('网络错误，请稍后重试');
          store.quasar.loading.hide();
          clearTimeout(time);
        });
    }
  }

  store.quasar.loading.hide();
  clearTimeout(time);
  onRequest({ pagination: pagination.value });
};

const onSave = () => {
  dialog.value.show = false;

  if (dialog.value.mode === 'new') {
    const time = setTimeout(() => {
      store.quasar.loading.hide();
      clearTimeout(time);
    }, 120000);

    store.quasar.loading.show();

    api
      .post(
        store.server +
          '/api/route/post?auth=jwt&service=notify&path=/api/notify/update',
        {
          id: dialog.value.id,
          topic: dialog.value.topic,
          type: dialog.value.type,
          msg: dialog.value.msg,
          user: store.user.id,
        },
        {
          headers: {
            Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
          },
        }
      )
      .then((resp) => {
        store.quasar.loading.hide();
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
        store.quasar.loading.hide();
        clearTimeout(time);
      });
  } else {
    // intentionally empty
  }
};
</script>
