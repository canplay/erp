<template>
  <q-table
    square
    :rows="rows"
    :columns="columns"
    row-key="id"
    separator="cell"
    v-model:pagination="pagination"
    :rows-per-page-options="[10, 20, 30, 40, 50]"
    selection="multiple"
    v-model:selected="selected"
    @request="onRequest"
    virtual-scroll
    :style="`${store.height};width: calc(100vw - 200px)`"
  >
    <template v-slot:top>
      <div class="row fit">
        <div class="col">
          <div class="row">
            <q-input class="col" v-model="search.id" label="编号" />

            <div class="col-auto" style="width: 8px" />

            <q-select
              class="col-3"
              v-model="search.status"
              :options="['全部', '启用', '停用']"
              label="状态"
            />

            <div class="col-auto" style="width: 8px" />

            <q-select
              class="col-3"
              v-model="search.status"
              :options="['全部', '已租赁', '未租赁']"
              label="租赁"
            />
          </div>

          <div style="height: 8px" />

          <div class="row">
            <q-select
              class="col-3"
              v-model="search.status"
              :options="['全部', '需要维修', '正在维修', '无需维修']"
              label="维修"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="search.remark" label="备注" />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="search.date" label="创建时间" mask="####-##-##">
              <template v-slot:append>
                <q-icon name="event" class="cursor-pointer">
                  <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                    <q-date v-model="search.date" mask="YYYY-MM-DD" />
                  </q-popup-proxy>
                </q-icon>
              </template>
            </q-input>
          </div>
        </div>

        <div class="col-auto" style="width: 8px" />

        <q-btn class="col-1" color="primary" label="导出" />

        <div class="col-auto" style="width: 8px" />

        <q-btn class="col-1" color="negative" label="删除" @click="onDel()" />

        <div class="col-auto" style="width: 8px" />

        <q-btn class="col-1" color="positive" label="添加" @click="onAdd()" />

        <div class="col-auto" style="width: 8px" />

        <q-btn class="col-1" color="primary" label="查询" @click="onQuery()" />
      </div>
    </template>

    <template v-slot:header-cell="props">
      <q-th :props="props" style="font-weight: bold; font-size: 16px">
        {{ props.col.label }}
      </q-th>
    </template>

    <template v-slot:body="props">
      <q-tr :props="props">
        <q-td>
          <q-checkbox v-model="props.selected" />
        </q-td>

        <q-td key="id" :props="props">
          <q-btn color="primary" :label="props.row.id" @click="onView(props.row)" />
        </q-td>

        <q-td key="name" :props="props">
          {{ props.row.name }}
        </q-td>

        <q-td key="locate" :props="props">
          {{ props.row.locate }}
        </q-td>

        <q-td key="status" :props="props">
          {{ props.row.status }}
        </q-td>

        <q-td key="history" :props="props">
          {{ props.row.history_count }}
        </q-td>

        <q-td key="remark" :props="props">
          {{ props.row.remark }}
        </q-td>

        <q-td key="create_name" :props="props">
          {{ props.row.create_name }}
        </q-td>

        <q-td key="create_date" :props="props">
          {{ props.row.create_date }}
        </q-td>

        <q-td key="update_name" :props="props">
          {{ props.row.update_name }}
        </q-td>

        <q-td key="update_date" :props="props">
          {{ props.row.update_date }}
        </q-td>
      </q-tr>
    </template>
  </q-table>

  <q-dialog v-model="dialog.model" persistent>
    <q-card style="min-width: calc(80vw)">
      <q-card-section>
        <div class="row">
          <div class="col text-h6 text-bold">公交车站台</div>

          <q-btn class="col-2" color="negative" label="关闭" @click="dialog.model = false" />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col-2" color="primary" label="保存" @click="onSave()" />
        </div>

        <q-separator />

        <div class="row">
          <q-input class="col" v-model="dialog.info.id" label="编号" />

          <div class="col-auto" style="width: 8px" />

          <q-select
            class="col-4"
            v-model="dialog.info.status"
            :options="['启用', '停用']"
            label="状态"
          />
        </div>

        <div style="height: 8px" />

        <div class="row">
          <q-input class="col" v-model="dialog.info.name" label="名称" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" v-model="dialog.info.locate" label="位置" />
        </div>

        <div style="height: 8px" />

        <q-input class="col" v-model="dialog.info.remark" label="备注" />

        <div style="height: 8px" />

        <q-table
          square
          :rows="dialog.history.rows"
          :columns="dialog.history.columns"
          row-key="id"
          separator="cell"
        >
          <template v-slot:top-left>
            <div class="text-h5">历史记录</div>
          </template>

          <template v-slot:top-right>
            <div class="row">
              <q-btn class="col" color="negative" label="删除" @click="delRow" />

              <div class="col-auto" style="width: 8px" />

              <q-btn class="col" color="primary" label="添加" @click="addRow" />
            </div>
          </template>

          <template v-slot:header-cell="props">
            <q-th :props="props" style="font-size: 15px; font-weight: bold">
              {{ props.col.label }}
            </q-th>
          </template>

          <template v-slot:body="props">
            <q-tr :props="props">
              <q-td key="id" :props="props" style="max-width: 100px; overflow: hidden">
                {{ props.row.id }}
              </q-td>

              <q-td key="type" :props="props">
                <q-select v-model="props.row.type" :options="['租赁', '维修', '其他']" />
              </q-td>

              <q-td key="status" :props="props">
                <q-select v-model="props.row.status" :options="['正常', '停止', '其他']" />
              </q-td>

              <q-td key="date_start" :props="props" style="min-width: 150px">
                <q-input v-model="props.row.date_start" mask="####-##-##" label="开始时间">
                  <template v-slot:append>
                    <q-icon name="event" class="cursor-pointer">
                      <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                        <q-date v-model="props.row.date_start" mask="YYYY-MM-DD" range />
                      </q-popup-proxy>
                    </q-icon>
                  </template>
                </q-input>
              </q-td>

              <q-td key="date_end" :props="props" style="min-width: 150px">
                <q-input v-model="props.row.date_end" mask="####-##-##" label="结束时间">
                  <template v-slot:append>
                    <q-icon name="event" class="cursor-pointer">
                      <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                        <q-date v-model="props.row.date_end" mask="YYYY-MM-DD" />
                      </q-popup-proxy>
                    </q-icon>
                  </template>
                </q-input>
              </q-td>

              <q-td key="remark" :props="props" style="min-width: 500px">
                {{ props.row.remark }}
                <q-popup-edit v-model="props.row.remark">
                  <q-input v-model="props.row.remark" dense autofocus />
                </q-popup-edit>
              </q-td>
            </q-tr>
          </template>
        </q-table>
      </q-card-section>
    </q-card>
  </q-dialog>

  <q-inner-loading :showing="loading">
    <q-spinner size="50px" color="primary" />
  </q-inner-loading>
</template>

<script setup lang="ts">
import { ref } from 'vue';
import type { QTableProps } from 'quasar';
import { uid, date } from 'quasar';
import { useStore } from 'stores/store';
import { api } from 'src/boot/axios';

interface ASSERTS {
  id: string;
  name: string;
  locate: string;
  status: string;
  history: string;
  history_count: number;
  remark: string;
  create_name: string;
  create_date: string;
  create_user: string;
  update_name: string;
  update_date: string;
  update_user: string;
  delete: boolean;
  selected: boolean;
}

interface HISTORY {
  id: string;
  type: string;
  status: string;
  date_start: string;
  date_end: string;
  remark: string;
  create_name: string;
  create_date: string;
  create_user: string;
  update_name: string;
  update_date: string;
  update_user: string;
  delete: boolean;
  model: string;
}

const store = useStore();

const loading = ref(false);

const dialog = ref({
  model: false,
  type: 'add',
  info: {} as ASSERTS,
  history: {
    rows: [] as HISTORY[],
    columns: [
      {
        name: 'id',
        label: '编号',
        field: 'id',
        align: 'center',
      },
      {
        name: 'type',
        label: '类型',
        field: 'type',
        align: 'center',
      },
      {
        name: 'status',
        label: '状态',
        field: 'status',
        align: 'center',
      },
      {
        name: 'date_start',
        label: '开始时间',
        field: 'date_start',
        align: 'center',
      },
      {
        name: 'date_end',
        label: '结束时间',
        field: 'date_end',
        align: 'center',
      },
      {
        name: 'remark',
        label: '备注',
        field: 'remark',
        align: 'center',
      },
    ] as QTableProps['columns'],
  },
});

const search = ref({
  status: '全部',
  id: ref(''),
  name: ref(''),
  date: ref(''),
  remark: ref(''),
});

const columns = ref([
  {
    name: 'id',
    label: '编号',
    field: 'id',
    align: 'center',
    sortable: true,
  },
  {
    name: 'name',
    label: '名称',
    field: 'name',
    align: 'center',
    required: true,
    sortable: true,
  },
  {
    name: 'locate',
    label: '位置',
    field: 'locate',
    align: 'center',
    required: true,
    sortable: true,
  },
  {
    name: 'status',
    label: '状态',
    field: 'status',
    align: 'center',
    required: true,
    sortable: true,
  },
  {
    name: 'history',
    label: '记录',
    field: 'history',
    align: 'center',
    required: true,
    sortable: true,
  },
  {
    name: 'remark',
    label: '备注',
    field: 'remark',
    align: 'center',
  },
  {
    name: 'create_name',
    label: '创建人',
    field: 'create_name',
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
    name: 'update_name',
    label: '修改人',
    field: 'update_name',
    align: 'center',
    sortable: true,
  },
  {
    name: 'update_date',
    label: '修改时间',
    field: 'update_date',
    align: 'center',
    sortable: true,
  },
] as QTableProps['columns']);

const rows = ref([] as ASSERTS[]);

const selected = ref([]);

const pagination = ref({
  page: 1,
  rowsNumber: 0,
  rowsPerPage: 20,
  sortBy: 'update_date',
  descending: true,
  options: [10, 20, 30, 40, 50],
} as QTableProps['pagination']);

const onQuery = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 180000);

  loading.value = true;

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=public_assets&path=/api/assets/count',
      {
        id: search.value.id,
        status: search.value.status === '全部' ? '' : search.value.status,
        create_date: search.value.date,
        remark: search.value.remark,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      if (resp.data.status === 0) {
        clearTimeout(time);
        loading.value = false;
        store.quasar.notify(resp.data.message);
        return;
      }

      pagination.value!.rowsNumber = parseInt(resp.data.data);

      loading.value = false;
      clearTimeout(time);

      onRequest({ pagination: pagination.value });
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      loading.value = false;
      clearTimeout(time);
    });
};

const onRequest = (props: { pagination: QTableProps['pagination'] }) => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 180000);

  loading.value = true;

  rows.value = [];

  const page = props.pagination?.page ?? 1;
  const rowsPerPage = props.pagination?.rowsPerPage ?? 20;
  const sortBy = props.pagination?.sortBy ?? null;
  const descending = props.pagination?.descending ?? true;

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=public_assets&path=/api/assets/info',
      {
        id: search.value.id,
        status: search.value.status === '全部' ? '' : search.value.status,
        create_date: search.value.date,
        remark: search.value.remark,
        curPage: (page - 1) * rowsPerPage,
        maxPage: rowsPerPage,
        sortBy: sortBy,
        descending: descending ? 'ASC' : 'DESC',
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      if (resp.data.status === 0) {
        clearTimeout(time);
        loading.value = false;
        store.quasar.notify(resp.data.message);
        return;
      }

      if (resp.data.message === 'no data') {
        clearTimeout(time);
        loading.value = false;
        return;
      }

      rows.value = resp.data.data;

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

const onAdd = () => {
  dialog.value.info = {
    id: uid(),
    name: '',
    locate: '',
    remark: '',
    status: '启用',
    history: '',
    history_count: 0,
    create_name: store.user.name,
    create_date: '',
    create_user: store.user.id,
    update_name: store.user.name,
    update_date: '',
    update_user: store.user.id,
    selected: false,
    delete: false,
  };
  dialog.value.type = 'add';
  dialog.value.history.rows = [];
  dialog.value.model = true;
};

const onView = (row: ASSERTS) => {
  dialog.value.info = row;
  dialog.value.info.update_name = store.user.name;
  dialog.value.info.update_user = store.user.id;
  dialog.value.type = 'view';
  dialog.value.history.rows = [];

  const id = dialog.value.info.history.split(',');

  for (let index = 0; index < id.length; index++) {
    const element = id[index];
    if (element === '') continue;

    api
      .post(
        store.server + '/api/route/post?auth=jwt&service=public_assets&path=/api/history/count',
        {
          id: element,
        },
        {
          headers: {
            Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
          },
        },
      )
      .then((resp) => {
        if (resp.data.status === 0) {
          store.quasar.notify(resp.data.message);
          return;
        }

        if (resp.data.message === 'no data') {
          return;
        }

        dialog.value.history.rows.push(resp.data.data);

        if (element === id[id.length - 1]) {
          dialog.value.model = true;
        }
      })
      .catch(() => {
        store.quasar.notify('网络错误，请稍后重试');
      });
  }
};

const onDel = () => {
  selected.value.forEach((element: ASSERTS) => {
    const e = rows.value.filter((e: ASSERTS) => {
      return e.id === element.id;
    });
    
    if (e.length === 0) {
      store.quasar.notify('未找到该记录');
      return;
    }
    
    const target = e[0];
    if (!target) return;
    target.delete = true;

    const time = setTimeout(() => {
      loading.value = false;
      clearTimeout(time);
    }, 180000);

    loading.value = true;

    api
      .post(
        store.server + '/api/route/post?auth=jwt&service=public_assets&path=/api/assets/update',
        target,
        {
          headers: {
            Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
          },
        },
      )
      .then((resp) => {
        if (resp.data.status === 0) {
          clearTimeout(time);
          loading.value = false;
          store.quasar.notify(resp.data.message);
          return;
        }

        const targetItem = e[0];
        if (!targetItem) return;
        const index = rows.value.indexOf(targetItem);
        if (index > -1) {
          rows.value.splice(index, 1);
        }
        selected.value = [];

        loading.value = false;
        clearTimeout(time);
      })
      .catch(() => {
        store.quasar.notify('网络错误，请稍后重试');
        loading.value = false;
        clearTimeout(time);
      });
  });
};

const onSave = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 180000);

  loading.value = true;

  dialog.value.info.history = '';
  for (let index = 0; index < dialog.value.history.rows.length; index++) {
    const element = dialog.value.history.rows[index];

    dialog.value.info.history = dialog.value.info.history + ',' + (element?.id ?? '');
  }

  api
    .post(
      store.server +
        '/api/route/post?auth=jwt&service=public_assets&path=/api/assets/' +
        (dialog.value.type === 'add' ? 'add' : 'update'),
      dialog.value.info,
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      if (resp.data.status === 0) {
        clearTimeout(time);
        loading.value = false;
        store.quasar.notify(resp.data.message);
        return;
      }

      for (let index = 0; index < dialog.value.history.rows.length; index++) {
        const element = dialog.value.history.rows[index];
        if (!element) continue;

        api
          .post(
            store.server +
              '/api/route/post?auth=jwt&service=public_assets&path=/api/history/' +
              (element.model === 'add' ? 'add' : 'update'),
            element,
            {
              headers: {
                Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
              },
            },
          )
          .then((resp) => {
            if (resp.data.status === 0) {
              clearTimeout(time);
              loading.value = false;
              store.quasar.notify(resp.data.message);
              return;
            }

            loading.value = false;
            clearTimeout(time);

            dialog.value.model = false;

            onRequest({ pagination: pagination.value });
          })
          .catch(() => {
            store.quasar.notify('网络错误，请稍后重试');
            loading.value = false;
            clearTimeout(time);
          });
      }
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      loading.value = false;
      clearTimeout(time);
    });
};

const addRow = () => {
  dialog.value.history.rows.push({
    id: uid(),
    type: '其他',
    status: '正常',
    date_start: date.formatDate(Date.now(), 'YYYY-MM-DD'),
    date_end: date.formatDate(Date.now(), 'YYYY-MM-DD'),
    remark: '',
    create_name: store.user.name,
    create_date: '',
    create_user: store.user.id,
    update_name: store.user.name,
    update_date: '',
    update_user: store.user.id,
    delete: false,
    model: 'add',
  });
};

const delRow = () => {
  dialog.value.history.rows.splice(dialog.value.history.rows.length - 1);
};
</script>
