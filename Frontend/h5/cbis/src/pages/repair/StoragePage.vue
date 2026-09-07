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
        <div class="column fit">
          <div class="col row">
            <q-select
              class="col"
              v-model="search.select.model"
              :options="search.select.options"
              label="查询类型"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="search.content" label="查询内容" />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="search.date.in" label="入库日期">
              <template v-slot:append>
                <q-icon name="event" class="cursor-pointer">
                  <q-popup-proxy
                    transition-show="scale"
                    transition-hide="scale"
                  >
                    <q-date
                      v-model="search.date.in"
                      range
                      mask="YYYY-MM-DD"
                      @update:model-value="onDateIn"
                    >
                      <div class="row items-center justify-end">
                        <q-btn
                          v-close-popup
                          label="关闭"
                          color="primary"
                          flat
                        />
                      </div>
                    </q-date>
                  </q-popup-proxy>
                </q-icon>
              </template>
            </q-input>

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="search.date.out" label="出库日期">
              <template v-slot:append>
                <q-icon name="event" class="cursor-pointer">
                  <q-popup-proxy
                    transition-show="scale"
                    transition-hide="scale"
                  >
                    <q-date
                      v-model="search.date.out"
                      range
                      mask="YYYY-MM-DD"
                      @update:model-value="onDateOut"
                    >
                      <div class="row items-center justify-end">
                        <q-btn
                          v-close-popup
                          label="关闭"
                          color="primary"
                          flat
                        />
                      </div>
                    </q-date>
                  </q-popup-proxy>
                </q-icon>
              </template>
            </q-input>
          </div>

          <div class="col-auto" style="height: 8px" />

          <div class="col row">
            <q-btn class="col-2" color="green" label="导出" @click="onExport" />

            <div class="col-auto" style="width: 8px" />

            <q-btn class="col-2" color="red" label="重置" @click="onReset" />

            <div class="col-auto" style="width: 8px" />

            <q-btn
              class="col-2"
              color="primary"
              label="查询"
              @click="onQuery"
            />

            <div class="col-auto" style="width: 8px" />

            <q-btn class="col-2" color="red" label="新增" @click="onCreate" />
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

    <q-dialog v-model="dialog.show" persistent>
      <q-card style="min-width: 70%">
        <q-card-section>
          <div class="row">
            <q-btn
              class="col-3"
              size="30px"
              :label="dialog.img === '' ? '照片' : ''"
              @click="onUpload"
              style="min-height: 110px"
            >
              <img
                v-if="dialog.img != ''"
                :src="dialog.img"
                style="max-width: 194px; max-height: 160px"
              />
            </q-btn>

            <div class="col-auto" style="width: 8px" />

            <div class="col">
              <div class="row">
                <q-input
                  class="col"
                  v-model="dialog.code"
                  label="编码"
                  :rules="[(val) => !!val || '此为必填项']"
                />

                <div class="col-auto" style="width: 8px" />

                <q-input
                  class="col"
                  v-model="dialog.name"
                  label="名称"
                  :rules="[(val) => !!val || '此为必填项']"
                />

                <div class="col-auto" style="width: 8px" />

                <q-input
                  class="col"
                  v-model="dialog.tag"
                  label="标志"
                  :rules="[(val) => !!val || '此为必填项']"
                />
              </div>

              <div class="row">
                <q-input
                  class="col"
                  v-model="dialog.num"
                  label="数量"
                  :rules="[(val) => !!val || '此为必填项']"
                />

                <div class="col-auto" style="width: 8px" />

                <q-input
                  class="col"
                  v-model="dialog.unit"
                  label="单位"
                  :rules="[(val) => !!val || '此为必填项']"
                />

                <div class="col-auto" style="width: 8px" />

                <q-input
                  class="col"
                  v-model="dialog.price"
                  label="单价"
                  :rules="[(val) => !!val || '此为必填项']"
                />
              </div>

              <div class="row">
                <q-input
                  class="col"
                  v-model="dialog.date_in"
                  label="入库日期"
                  :rules="[(val) => !!val || '此为必填项']"
                >
                  <template v-slot:prepend>
                    <q-icon name="event" class="cursor-pointer">
                      <q-popup-proxy
                        transition-show="scale"
                        transition-hide="scale"
                      >
                        <q-date
                          v-model="dialog.date_in"
                          mask="YYYY-MM-DD HH:mm:ss"
                        >
                          <div class="row items-center justify-end">
                            <q-btn
                              v-close-popup
                              label="关闭"
                              color="primary"
                              flat
                            />
                          </div>
                        </q-date>
                      </q-popup-proxy>
                    </q-icon>
                  </template>

                  <template v-slot:append>
                    <q-icon name="access_time" class="cursor-pointer">
                      <q-popup-proxy
                        transition-show="scale"
                        transition-hide="scale"
                      >
                        <q-time
                          v-model="dialog.date_in"
                          mask="YYYY-MM-DD HH:mm:ss"
                          format24h
                        >
                          <div class="row items-center justify-end">
                            <q-btn
                              v-close-popup
                              label="关闭"
                              color="primary"
                              flat
                            />
                          </div>
                        </q-time>
                      </q-popup-proxy>
                    </q-icon>
                  </template>
                </q-input>

                <div class="col-auto" style="width: 8px" />

                <q-input class="col" v-model="dialog.date_out" label="出库日期">
                  <template v-slot:prepend>
                    <q-icon name="event" class="cursor-pointer">
                      <q-popup-proxy
                        transition-show="scale"
                        transition-hide="scale"
                      >
                        <q-date
                          v-model="dialog.date_in"
                          mask="YYYY-MM-DD HH:mm:ss"
                        >
                          <div class="row items-center justify-end">
                            <q-btn
                              v-close-popup
                              label="关闭"
                              color="primary"
                              flat
                            />
                          </div>
                        </q-date>
                      </q-popup-proxy>
                    </q-icon>
                  </template>

                  <template v-slot:append>
                    <q-icon name="access_time" class="cursor-pointer">
                      <q-popup-proxy
                        transition-show="scale"
                        transition-hide="scale"
                      >
                        <q-time
                          v-model="dialog.date_in"
                          mask="YYYY-MM-DD HH:mm:ss"
                          format24h
                        >
                          <div class="row items-center justify-end">
                            <q-btn
                              v-close-popup
                              label="关闭"
                              color="primary"
                              flat
                            />
                          </div>
                        </q-time>
                      </q-popup-proxy>
                    </q-icon>
                  </template>
                </q-input>

                <div class="col-auto" style="width: 8px" />

                <q-select
                  class="col"
                  v-model="dialog.status.model"
                  :options="dialog.status.options"
                  label="状态"
                />
              </div>
            </div>
          </div>

          <div class="col-auto" style="height: 8px" />

          <div class="row">
            <q-input class="col" v-model="dialog.remark" label="备注" />
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
              @click="onSave"
            />

            <div class="col-auto" style="width: 8px" />

            <q-btn
              class="col"
              color="secondary"
              label="打印"
              style="font-size: 20px"
              @click="onPrint"
            />
          </div>
        </q-card-section>
      </q-card>
    </q-dialog>
  </q-page>

  <q-inner-loading :showing="loading">
    <q-spinner size="50px" color="primary" />
  </q-inner-loading>
</template>

<script setup lang="ts">
import { ref } from 'vue';
import type { QTableProps} from 'quasar';
import { exportFile, date } from 'quasar';
import { useStore } from 'stores/store';
import { api } from 'src/boot/axios';

const store = useStore();

const loading = ref(false);

const search = ref({
  date: {
    in: '',
    out: '',
  },
  content: '',
  select: {
    model: '',
    options: ['', '编码', '名称'],
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
    name: 'code',
    label: '编码',
    field: 'code',
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
    name: 'tag',
    label: '标志',
    field: 'tag',
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
    name: 'unit',
    label: '单位',
    field: 'unit',
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
    name: 'date_in',
    label: '入库日期',
    field: 'date_in',
    align: 'center',
    sortable: true,
  },
  {
    name: 'date_out',
    label: '出库日期',
    field: 'date_out',
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
    label: '创建日期',
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
    label: '更新日期',
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

interface TableRow {
  [key: string]: unknown;
  id?: string | number;
  name?: string;
  field?: string | ((row: Record<string, unknown>) => unknown);
  code?: string;
  num?: number | string;
  unit?: string;
  price?: number | string;
  date_in?: string;
  date_out?: string;
  img?: string;
  status?: string;
  remark?: string;
}

interface DateValue {
  from: string;
  to: string;
}

const rows = ref([] as TableRow[]);

const dialog = ref({
  mode: 'create',
  show: false,
  id: '',
  code: '',
  name: '',
  tag: '',
  num: '',
  unit: '',
  price: '',
  img: '',
  date_in: '',
  date_out: '',
  create_date: '',
  create_user: '',
  create_id: '',
  update_date: '',
  update_user: '',
  update_id: '',
  status: {
    model: '正常',
    options: ['正常', '缺货'],
  },
  remark: '',
  selected: [],
});

const onReset = () => {
  search.value.select.model = '';
  search.value.date.in = '';
  search.value.date.out = '';
  search.value.content = '';
};

const onDateIn = (val: DateValue | null) => {
  if (val === null) {
    search.value.date.in = '';
    return;
  }

  if (typeof val === 'object') {
    search.value.date.in = val.from + ' - ' + val.to;
  }
};

const onDateOut = (val: DateValue | null) => {
  if (val === null) {
    search.value.date.out = '';
    return;
  }

  if (typeof val === 'object') {
    search.value.date.out = val.from + ' - ' + val.to;
  }
};

const wrapCsvValue = (val: string, formatFn?: (...args: unknown[]) => unknown) => {
  let formatted: string = formatFn !== void 0 ? String(formatFn(val)) : val;

  formatted =
    formatted === void 0 || formatted === null ? '' : String(formatted);

  formatted = formatted.split('"').join('""');

  return formatted;
};

const onExport = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

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

  const status = exportFile('库存列表.csv', content, 'text/csv');

  if (status !== true) {
    store.quasar.notify('即将开始下载，请稍后...');
  }

  loading.value = false;
  clearTimeout(time);
};

const onUpload = () => {
  
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

  rows.value = [];

  let code = '';
  let name = '';

  switch (search.value.select.model) {
    case '编码':
      code = search.value.content;
      break;
    case '名称':
      name = search.value.content;
      break;
  }

  api
    .post(
      store.server +
        '/api/route/post?auth=jwt&service=repair&path=/api/storage/list',
      {
        code: code,
        name: name,
        curPage: (page - 1) * rowsPerPage,
        maxPage: rowsPerPage === 0 ? pagination.value!.rowsNumber : rowsPerPage,
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
          if (resp.data.data[i].id === '') break;

          rows.value.push({
            id: resp.data.data[i].id,
            code: resp.data.data[i].code,
            name: resp.data.data[i].name,
            tag: resp.data.data[i].tag,
            num: resp.data.data[i].num,
            unit: resp.data.data[i].unit,
            price: resp.data.data[i].price,
            date_in: resp.data.data[i].date_in,
            date_out: resp.data.data[i].date_out,
            img: resp.data.data[i].img,
            status: resp.data.data[i].status,
            remark: resp.data.data[i].remark,
            create_date: resp.data.data[i].create_date,
            create_user: resp.data.data[i].create_user,
            create_id: resp.data.data[i].create_id,
            update_date: resp.data.data[i].update_date,
            update_user: resp.data.data[i].update_user,
            update_id: resp.data.data[i].update_id,
          });
        }
      } else if (resp.data.message === 'no data') {
        store.quasar.notify('无数据');
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

  let code = '';
  let name = '';

  switch (search.value.select.model) {
    case '编码':
      code = search.value.content;
      break;
    case '名称':
      name = search.value.content;
      break;
  }

  api
    .post(
      store.server +
        '/api/route/post?auth=jwt&service=repair&path=/api/storage/count',
      {
        code: code,
        name: name,
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

const onCreate = () => {
  dialog.value.date_in = date.formatDate(Date.now(), 'YYYY-MM-DD H:mm:ss');
  dialog.value.date_out = date.formatDate(Date.now(), 'YYYY-MM-DD H:mm:ss');
  dialog.value.create_user = store.user.name;
  dialog.value.create_id = store.user.id;
  dialog.value.update_user = store.user.name;
  dialog.value.update_id = store.user.id;
  dialog.value.mode = 'create';
  dialog.value.show = true;
};

const onView = (val: TableRow) => {
  dialog.value.id = val.id as string;
  dialog.value.code = val.code as string;
  dialog.value.name = val.name as string;
  dialog.value.tag = val.tag as string;
  dialog.value.num = val.num as string;
  dialog.value.unit = val.unit as string;
  dialog.value.price = val.price as string;
  dialog.value.date_in = val.date_in as string;
  dialog.value.date_out = val.date_out as string;
  dialog.value.img = val.img as string;
  dialog.value.create_user = val.create_user as string;
  dialog.value.create_id = val.create_id as string;
  dialog.value.update_user = store.user.name;
  dialog.value.update_id = store.user.id;
  dialog.value.status.model = val.status as string;
  dialog.value.remark = val.remark as string;
  dialog.value.mode = 'view';
  dialog.value.show = true;
};

const onSave = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  switch (dialog.value.mode) {
    case 'create':
      api
        .put(
          store.server +
            '/api/route/post?auth=jwt&service=repair&path=/api/storage/info',
          {
            id: dialog.value.id,
            code: dialog.value.code,
            name: dialog.value.name,
            tag: dialog.value.tag,
            num: parseInt(dialog.value.num),
            unit: dialog.value.unit,
            price: parseFloat(dialog.value.price),
            img: dialog.value.img,
            date_in: dialog.value.date_in,
            date_out: dialog.value.date_out,
            status: dialog.value.status.model,
            remark: dialog.value.remark,
            create_user: dialog.value.create_user,
            create_id: dialog.value.create_id,
            update_user: dialog.value.update_user,
            update_id: dialog.value.update_id,
          },
          {
            headers: {
              Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
            },
          }
        )
        .then((resp) => {
          loading.value = false;
          clearTimeout(time);

          if (resp.data.status !== 1) {
            store.quasar.notify('保存失败，请稍后重试');
            
          } else {
            store.quasar.notify('保存成功');
            dialog.value.show = false;
            onQuery();
          }
        })
        .catch(() => {
          store.quasar.notify('网络错误，请稍后重试');
          loading.value = false;
          clearTimeout(time);
        });

      break;
    case 'view':
      api
        .post(
          store.server +
            '/api/route/post?auth=jwt&service=repair&path=/api/storage/info',
          {
            id: '',
            code: dialog.value.code,
            name: dialog.value.name,
            tag: dialog.value.tag,
            num: parseInt(dialog.value.num),
            unit: dialog.value.unit,
            price: parseFloat(dialog.value.price),
            img: dialog.value.img,
            date_in: dialog.value.date_in,
            date_out: dialog.value.date_out,
            status: dialog.value.status.model,
            remark: dialog.value.remark,
            create_user: dialog.value.create_user,
            create_id: dialog.value.create_id,
            update_user: dialog.value.update_user,
            update_id: dialog.value.update_id,
          },
          {
            headers: {
              Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
            },
          }
        )
        .then((resp) => {
          loading.value = false;
          clearTimeout(time);

          if (resp.data.status !== 1) {
            store.quasar.notify('保存失败，请稍后重试');
          } else {
            store.quasar.notify('保存成功');
            dialog.value.show = false;
            onQuery();
          }
        })
        .catch(() => {
          store.quasar.notify('网络错误，请稍后重试');
          loading.value = false;
          clearTimeout(time);
        });

      break;
  }
};

const onPrint = () => {
  window.open(
    'print/repair_settle_print.html?id=' +
      encodeURI(dialog.value.id) +
      '&user=' +
      encodeURI(store.user.name) +
      '&code=' +
      encodeURI(dialog.value.code) +
      '&name=' +
      encodeURI(dialog.value.name) +
      '&tag=' +
      encodeURI(dialog.value.tag) +
      '&num=' +
      encodeURI(dialog.value.num.toString()) +
      '&unit=' +
      encodeURI(dialog.value.unit) +
      '&price=' +
      encodeURI(dialog.value.price.toString()) +
      '&img=' +
      encodeURI(dialog.value.img) +
      '&date_in=' +
      encodeURI(dialog.value.date_in) +
      '&date_out=' +
      encodeURI(dialog.value.date_out) +
      '&status=' +
      encodeURI(dialog.value.status.model) +
      '&remark=' +
      encodeURI(dialog.value.remark) +
      '&create_date=' +
      encodeURI(dialog.value.create_date) +
      '&create_user=' +
      encodeURI(dialog.value.create_user) +
      '&create_id=' +
      encodeURI(dialog.value.create_id) +
      '&update_date=' +
      encodeURI(dialog.value.update_date) +
      '&update_user=' +
      encodeURI(dialog.value.update_user) +
      '&update_id=' +
      encodeURI(dialog.value.update_id)
  );
};
</script>
