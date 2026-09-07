<template>
  <q-page>
    <q-table
      square
      :rows="table.rows"
      :columns="table.columns"
      row-key="id"
      separator="cell"
      v-model:pagination="table.pagination"
      selection="multiple"
      v-model:selected="table.selected"
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
            @click="onRequest({ pagination: table.pagination })"
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

        <q-td v-else :props="props">
          {{ props.value }}
        </q-td>
      </template>
    </q-table>
  </q-page>

  <q-dialog v-model="dialog.show">
    <q-card style="width: 90%">
      <q-card-section>
        <q-form @submit="onSave">
          <q-input
            class="col"
            v-model="dialog.project"
            filled
            label="项目名称"
            lazy-rules
            :rules="[(val) => (val && val.length > 0) || '必填项']"
          />

          <q-input
            class="col"
            v-model="dialog.company"
            filled
            label="单位名称"
            lazy-rules
            :rules="[(val) => (val && val.length > 0) || '必填项']"
          />

          <q-input
            class="col"
            v-model="dialog.contact"
            filled
            label="联系人"
            lazy-rules
            :rules="[(val) => (val && val.length > 0) || '必填项']"
          />

          <q-input
            class="col"
            v-model="dialog.phone"
            filled
            label="联系电话"
            lazy-rules
            :rules="[(val) => (val && val.length > 0) || '必填项']"
          />

          <q-card bordered flat>
            <div class="text-h6 text-center">商品列表</div>
            <q-table
              :rows="dialog.items.rows"
              :columns="dialog.items.columns"
              grid
              binary-state-sort
            >
              <template v-slot:top>
                <div class="row fit">
                  <q-btn
                    class="col"
                    dense
                    label="添加"
                    color="primary"
                    @click="dialog.items.show = true"
                  />

                  <div class="col-auto" style="width: 8px" />

                  <q-btn class="col" dense label="删除" color="negative" @click="onDelete" />
                </div>
              </template>

              <template v-slot:item="props">
                <q-card class="no-box-shadow fit" style="background: transparent">
                  <q-card-section>
                    <div class="row">
                      <div class="col-auto flex items-center">
                        <q-checkbox v-model="props.row.selected" />
                      </div>

                      <div class="col-4 flex items-center ellipsis">
                        {{ props.row.name }}
                      </div>

                      <div class="col ellipsis">
                        <div>{{ props.row.num }} {{ props.row.unit }}</div>

                        <div>
                          {{ props.row.spec }}
                        </div>

                        <div>
                          {{ props.row.remark }}
                        </div>
                      </div>
                    </div>

                    <q-separator />
                  </q-card-section>
                </q-card>
              </template>
            </q-table>
          </q-card>

          <div style="height: 20px" />

          <q-input
            class="col"
            v-model="dialog.amount"
            filled
            label="总价"
            lazy-rules
            :rules="[(val) => (val && val.length > 0) || '必填项']"
          />

          <q-input filled v-model="dialog.delivery" mask="date" :rules="['date']" label="交付日期">
            <template v-slot:append>
              <q-icon name="event" class="cursor-pointer">
                <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                  <q-date v-model="dialog.delivery">
                    <div class="row items-center justify-end">
                      <q-btn v-close-popup label="关闭" color="primary" flat />
                    </div>
                  </q-date>
                </q-popup-proxy>
              </q-icon>
            </template>
          </q-input>

          <q-select
            class="col"
            v-model="dialog.status"
            :options="['未接单', '未收款', '已完成']"
            filled
            label="状态"
            lazy-rules
            :rules="[(val) => (val && val.length > 0) || '必填项']"
          />

          <q-input
            class="col"
            v-model="dialog.price_range"
            filled
            label="意向价格范围"
            lazy-rules
            :rules="[(val) => (val && val.length > 0) || '必填项']"
          />

          <div style="height: 8px" />

          <q-input class="col" v-model="dialog.remark" filled label="备注" />

          <div style="height: 8px" />

          <q-btn class="fit" color="primary" label="保存" type="submit" />
        </q-form>
      </q-card-section>
    </q-card>
  </q-dialog>

  <q-dialog v-model="dialog.items.show">
    <q-card style="width: 80%">
      <q-card-section>
        <q-input v-model="dialog.items.name" label="商品名称" />
        <q-input v-model="dialog.items.num" label="采购数量" />
        <q-input v-model="dialog.items.unit" label="商品单位" />
        <q-input v-model="dialog.items.spec" label="商品规格" />
        <q-input v-model="dialog.items.remark" label="商品备注" />
      </q-card-section>

      <q-card-actions class="row">
        <q-btn class="col" color="negative" label="取消" v-close-popup />
        <q-btn class="col" color="primary" label="确定" v-close-popup @click="onAddDialog" />
      </q-card-actions>
    </q-card>
  </q-dialog>
</template>

<script setup lang="ts">
import type { QTableProps } from 'quasar';
import { exportFile } from 'quasar';
import { api } from 'src/boot/axios';
import { useStore } from 'src/stores/store';
import { ref } from 'vue';

interface TableRow {
  [key: string]: unknown;
  id?: string | number;
  name?: string;
  field?: string | ((row: Record<string, unknown>) => unknown);
  project?: string;
  company?: string;
  contact?: string;
  phone?: string;
  items?: unknown;
  amount?: string | number;
  delivery?: string;
  status?: string;
  remark?: string;
  price_range?: string;
  create_user?: string;
  create_date?: string;
  update_user?: string;
  update_date?: string;
  num?: string | number;
  unit?: string;
  spec?: string;
  selected?: boolean;
}

interface ItemRow {
  name?: string | undefined;
  num?: string | number | undefined;
  unit?: string | undefined;
  spec?: string | undefined;
  remark?: string | undefined;
  selected?: boolean;
}

const store = useStore();

const search = ref({
  date: '',
  content: '',
  select: {
    model: '',
    options: ['', '项目名称', '单位名称', '联系人', '联系电话', '状态'],
  },
});

const table = ref({
  rows: [] as TableRow[],
  selected: [] as TableRow[],
  pagination: {
    page: 1,
    rowsNumber: 0,
    rowsPerPage: 20,
    sortBy: 'create_date',
    descending: true,
  },
  columns: [
    {
      name: 'id',
      label: 'ID',
      field: 'id',
      align: 'center',
      sortable: true,
    },
    {
      name: 'project',
      label: '项目名称',
      field: 'project',
      align: 'center',
      sortable: true,
    },
    {
      name: 'company',
      label: '单位名称',
      field: 'company',
      align: 'center',
      sortable: true,
    },
    {
      name: 'contact',
      label: '联系人',
      field: 'contact',
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
      name: 'amount',
      label: '总价',
      field: 'amount',
      align: 'center',
      sortable: true,
    },
    {
      name: 'delivery',
      label: '交付时间',
      field: 'delivery',
      align: 'center',
      sortable: true,
    },
    {
      name: 'price_range',
      label: '意向价格范围',
      field: 'price_range',
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
  ] as QTableProps['columns'],
});

const dialog = ref({
  show: false,
  mode: 'view',
  id: '',
  project: '',
  company: '',
  contact: '',
  phone: '',
  info: '',
  amount: '',
  delivery: '',
  status: '',
  remark: '',
  price_range: '',
  items: {
    show: false,
    rows: [] as ItemRow[],
    columns: [
      {
        name: 'name',
        label: '名称',
        align: 'left',
        field: 'name',
        sortable: true,
      },
      {
        name: 'num',
        label: '数量',
        align: 'left',
        field: 'num',
        sortable: true,
      },
      {
        name: 'unit',
        label: '单位',
        align: 'left',
        field: 'unit',
        sortable: true,
      },
      {
        name: 'spec',
        label: '规格型号',
        align: 'left',
        field: 'spec',
        sortable: true,
      },
      {
        name: 'remark',
        label: '备注',
        align: 'left',
        field: 'remark',
        sortable: true,
      },
    ] as QTableProps['columns'],
    name: '',
    num: '',
    unit: '',
    spec: '',
    remark: '',
  },
});

const onRequest = (value: { pagination: QTableProps['pagination'] }) => {
  const time = setTimeout(() => {
    store.quasar.loading.show();
    clearTimeout(time);
  }, 120000);

  store.quasar.loading.hide();

  const page = value.pagination?.page ?? 1;
  const rowsPerPage = value.pagination?.rowsPerPage ?? 20;
  const sortBy = value.pagination?.sortBy ?? null;
  const descending = value.pagination?.descending ?? true;
  const rowsNumber = value.pagination?.rowsNumber;

  table.value.rows = [];

  let project = '',
    company = '',
    contact = '',
    phone = '',
    status = '';

  switch (search.value.select.model) {
    case '项目名称':
      project = search.value.content;
      break;
    case '单位名称':
      company = search.value.content;
      break;
    case '联系人':
      contact = search.value.content;
      break;
    case '联系电话':
      phone = search.value.content;
      break;
    case '状态':
      status = search.value.content;
      break;
  }

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=cbis&path=/api/groupbuy/count',
      {
        project: project,
        company: company,
        contact: contact,
        phone: phone,
        status: status,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      table.value.pagination.rowsNumber = parseInt(resp.data.data);

      if (resp.data.status === 0 || parseInt(resp.data.data) <= 0) {
        store.quasar.loading.hide();
        clearTimeout(time);
        store.quasar.notify('没有数据');
        return;
      }

      api
        .post(
          store.server + '/api/route/post?auth=jwt&service=cbis&path=/api/groupbuy/list',
          {
            project: project,
            company: company,
            contact: contact,
            phone: phone,
            status: status,
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
              table.value.rows.push({
                id: resp.data.data[i].id,
                project: resp.data.data[i].project,
                company: resp.data.data[i].company,
                contact: resp.data.data[i].contact,
                phone: resp.data.data[i].phone,
                items: resp.data.data[i].items,
                amount: resp.data.data[i].amount,
                delivery: resp.data.data[i].delivery,
                status: resp.data.data[i].status,
                remark: resp.data.data[i].remark,
                price_range: resp.data.data[i].price_range,
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

          table.value.pagination.page = page;
          table.value.pagination.rowsPerPage = rowsPerPage;
          table.value.pagination.sortBy = sortBy ?? 'create_date';
          table.value.pagination.descending = descending;
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

const onSave = () => {
  const time = setTimeout(() => {
    store.quasar.loading.hide();
    clearTimeout(time);
  }, 120000);

  store.quasar.loading.show();

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=cbis&path=/api/groupbuy/update',
      {
        method: 'update',
        id: dialog.value.id,
        project: dialog.value.project,
        company: dialog.value.company,
        contact: dialog.value.contact,
        phone: dialog.value.phone,
        delivery: dialog.value.delivery,
        amount: parseInt(dialog.value.amount),
        status: dialog.value.status,
        remark: dialog.value.remark,
        price_range: dialog.value.price_range,
        items: dialog.value.items.rows,
        user: store.user.id,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      store.quasar.loading.hide();
      clearTimeout(time);

      if (resp.data.status !== 1) {
        store.quasar.notify('保存失败，请稍后重试');
        return;
      }

      store.quasar.notify('保存成功');
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      store.quasar.loading.hide();
      clearTimeout(time);
    });
};

const onView = (val: TableRow) => {
  dialog.value.mode = 'view';
  dialog.value.id = String(val.id ?? '');
  dialog.value.project = val.project ?? '';
  dialog.value.company = val.company ?? '';
  dialog.value.contact = val.contact ?? '';
  dialog.value.phone = val.phone ?? '';
   dialog.value.info = val.info == null ? '' : JSON.stringify(val.info);
  dialog.value.delivery = val.delivery ?? '';
  dialog.value.status = val.status ?? '';
  dialog.value.remark = val.remark ?? '';
  dialog.value.price_range = val.price_range ?? '';
  dialog.value.items.rows = val.items as ItemRow[];
  dialog.value.show = true;
};

// eslint-disable-next-line @typescript-eslint/no-unused-vars
const onAdd = (item?: ItemRow) => {
  if (item) {
    dialog.value.items.rows.push({
      name: item.name,
      num: item.num,
      unit: item.unit,
      spec: item.spec,
      remark: item.remark,
      selected: false,
    });
  }
};

const onAddDialog = () => {
  dialog.value.items.rows.push({
    name: dialog.value.items.name,
    num: dialog.value.items.num,
    unit: dialog.value.items.unit,
    spec: dialog.value.items.spec,
    remark: dialog.value.items.remark,
    selected: false,
  });
};

const onDelete = () => {
  for (let index = dialog.value.items.rows.length - 1; index >= 0; index--) {
    const element = dialog.value.items.rows[index];
    if (element?.selected) {
      dialog.value.items.rows.splice(index, 1);
    }
  }
};

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

  const content = [table.value.columns!.map((col) => wrapCsvValue(col.label))]
    .concat(
      table.value.rows.map((row: TableRow) =>
        table.value
          .columns!
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

  const status = exportFile('集采订单.csv', content, 'text/csv');

  if (status !== true) {
    store.quasar.notify('即将开始下载，请稍后...');
  }

  store.quasar.loading.hide();
  clearTimeout(time);
};
</script>
