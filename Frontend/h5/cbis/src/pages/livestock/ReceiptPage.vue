<template>
  <q-page>
    <q-table
      square
      :rows="rows"
      :columns="columns"
      row-key="id"
      separator="cell"
      v-model:pagination="pagination"
      virtual-scroll
      :style="`${store.height};width: calc(100vw - 200px)`"
    >
      <template v-slot:top>
        <div class="column fit">
          <div class="col row">
            <q-select
              class="col-4"
              v-model="search.select.model"
              :options="search.select.options"
              label="查询类型"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="search.content" label="查询内容" />
          </div>

          <div class="col-auto" style="height: 8px" />

          <div class="col row">
            <q-input class="col" v-model="search.date" label="创建日期">
              <template v-slot:prepend>
                <q-icon name="event" class="cursor-pointer">
                  <q-popup-proxy transition-show="scale" transition-hide="scale">
                    <q-date v-model="search.date" mask="YYYY-MM-DD HH:mm:ss">
                      <div class="row items-center justify-end">
                        <q-btn v-close-popup label="关闭" color="primary" flat />
                      </div>
                    </q-date>
                  </q-popup-proxy>
                </q-icon>
              </template>

              <template v-slot:append>
                <q-icon name="access_time" class="cursor-pointer">
                  <q-popup-proxy transition-show="scale" transition-hide="scale">
                    <q-time v-model="search.date" mask="YYYY-MM-DD HH:mm:ss" format24h>
                      <div class="row items-center justify-end">
                        <q-btn v-close-popup label="关闭" color="primary" flat />
                      </div>
                    </q-time>
                  </q-popup-proxy>
                </q-icon>
              </template>
            </q-input>

            <div class="col-auto" style="width: 8px" />

            <q-btn class="col-2" color="green" label="导出" @click="onExport" />

            <div class="col-auto" style="width: 8px" />

            <q-btn class="col-2" color="red" label="重置" @click="onReset" />

            <div class="col-auto" style="width: 8px" />

            <q-btn class="col-2" color="primary" label="查询" @click="onQuery" />

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
        <q-td v-if="props.col.name === 'no'" :props="props">
          <q-btn color="primary" :label="props.value" @click="onView(props.row)" />
        </q-td>

        <q-td v-else :props="props">
          {{ props.value }}
        </q-td>
      </template>
    </q-table>
  </q-page>

  <q-dialog v-model="dialog.show" persistent class="my-font">
    <q-card style="min-width: 70%">
      <q-card-section>
        <div class="row">
          <q-input
            class="col"
            v-model="dialog.no"
            label="缴费编号"
            :rules="[(val) => !!val || '此为必填项']"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            v-model="dialog.name"
            label="货主"
            :rules="[(val) => !!val || '此为必填项']"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            v-model="dialog.phone"
            label="电话"
            :rules="[(val) => !!val || '此为必填项']"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            v-model="dialog.idcard"
            label="身份证"
            :rules="[(val) => !!val || '此为必填项']"
          />
        </div>

        <div class="col-auto" style="width: 8px" />

        <div class="row">
          <q-select
            class="col"
            v-model="dialog.livestock_type.model"
            :options="dialog.livestock_type.options"
            label="牲畜类型"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            readonly
            v-model="dialog.livestock_type.model.value"
            label="牲畜单价"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            v-model="dialog.livestock_num"
            label="牲畜数量"
            :rules="[(val) => !!val || '此为必填项']"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            v-model="dialog.livestock_amount"
            label="牲畜金额"
            :rules="[(val) => !!val || '此为必填项']"
          />
        </div>

        <div class="col-auto" style="height: 8px" />

        <div class="row">
          <q-select
            class="col"
            v-model="dialog.car_type.model"
            :options="dialog.car_type.options"
            label="车辆类型"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" readonly v-model="dialog.car_type.model.value" label="车辆单价" />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            v-model="dialog.car_num"
            label="车辆数量"
            :rules="[(val) => !!val || '此为必填项']"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            v-model="dialog.car_amount"
            label="车辆金额"
            :rules="[(val) => !!val || '此为必填项']"
          />
        </div>

        <div class="col-auto" style="height: 8px" />

        <div class="row">
          <q-input class="col" v-model="dialog.amount.cash" label="现金" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" v-model="dialog.amount.qrcode" label="收款码" />

          <div class="col-auto" style="width: 8px" />

          <q-input readonly class="col" v-model="dialog.amount.lower" label="总金额" />

          <div class="col-auto" style="width: 8px" />

          <q-input readonly class="col" v-model="dialog.amount.capital" label="总金额（大写）" />
        </div>

        <div class="col-auto" style="height: 8px" />

        <q-input v-model="dialog.remark" label="备注" />

        <div class="col-auto" style="height: 8px" />

        <div class="row">
          <q-btn class="col" color="negative" label="取消" style="font-size: 20px" v-close-popup />

          <div class="col-auto" style="width: 8px" />

          <q-btn
            class="col"
            color="positive"
            label="保存"
            style="font-size: 20px"
            @click="onSave"
          />
        </div>
      </q-card-section>
    </q-card>
  </q-dialog>

  <q-dialog v-model="dialog.print.show">
    <q-card style="width: 95%">
      <q-card-section>
        <div v-html="dialog.print.preview" />
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
    options: ['', '缴费编号', '货主', '电话'],
  },
});

const pagination = ref({
  page: 1,
  rowsNumber: 0,
  rowsPerPage: 20,
  sortBy: 'update_date',
  descending: true,
} as QTableProps['pagination']);

const columns = ref([
  {
    name: 'no',
    label: '缴费编号',
    field: 'no',
    align: 'center',
    sortable: true,
  },
  {
    name: 'name',
    label: '货主',
    field: 'name',
    align: 'center',
    sortable: true,
  },
  {
    name: 'phone',
    label: '电话',
    field: 'phone',
    align: 'center',
    sortable: true,
  },
  { name: 'idcard', label: '身份证', field: 'idcard', align: 'center' },
  {
    name: 'livestock_type',
    label: '牲畜类型',
    field: 'livestock_type',
    align: 'center',
    sortable: true,
  },
  {
    name: 'livestock_price',
    label: '牲畜单价',
    field: 'livestock_price',
    align: 'center',
    sortable: true,
  },
  {
    name: 'livestock_num',
    label: '牲畜数量',
    field: 'livestock_num',
    align: 'center',
  },
  {
    name: 'livestock_amount',
    label: '牲畜金额',
    field: 'livestock_amount',
    align: 'center',
    sortable: true,
  },
  {
    name: 'car_type',
    label: '车辆类型',
    field: 'car_type',
    align: 'center',
    sortable: true,
  },
  {
    name: 'car_price',
    label: '车辆单价',
    field: 'car_price',
    align: 'center',
    sortable: true,
  },
  {
    name: 'car_num',
    label: '车辆数量',
    field: 'car_num',
    align: 'center',
    sortable: true,
  },
  {
    name: 'car_amount',
    label: '车辆金额',
    field: 'car_amount',
    align: 'center',
    sortable: true,
  },
  {
    name: 'cash',
    label: '现金',
    field: 'cash',
    align: 'center',
    sortable: true,
  },
  {
    name: 'qrcode',
    label: '收款码',
    field: 'qrcode',
    align: 'center',
    sortable: true,
  },
  {
    name: 'amount',
    label: '总金额',
    field: 'amount',
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
    label: '修改时间',
    field: 'update_date',
    align: 'center',
    sortable: true,
  },
  {
    name: 'update_user',
    label: '修改人',
    field: 'update_user',
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
] as QTableProps['columns']);

const rows = ref([] as Record<string, unknown>[]);

const dialog = ref({
  mode: 'create',
  id: '',
  show: false,
  no: '',
  name: '',
  phone: '',
  idcard: '',
  livestock_type: {
    model: {
      label: '牛',
      value: '10',
    },
    options: [
      {
        label: '牛',
        value: '10',
      },
      {
        label: '马/驴',
        value: '10',
      },
    ],
  },
  livestock_num: '0',
  livestock_amount: '0.0',
  car_type: {
    model: {
      label: '6米至9.8米车辆',
      value: '20',
    },
    options: [
      {
        label: '摩托车',
        value: '3',
      },
      {
        label: '三轮摩托车',
        value: '10',
      },
      {
        label: '7座及6米以下车辆',
        value: '10',
      },
      {
        label: '6米至9.8米车辆',
        value: '20',
      },
      {
        label: '挂车',
        value: '50',
      },
    ],
  },
  car_num: '0',
  car_amount: '0.00',
  amount: {
    cash: '0.00',
    qrcode: '0.00',
    lower: '0.00',
    capital: '零元零角零分',
  },
  remark: '',
  create_date: '',
  print: {
    show: false,
    preview: '',
  },
});

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
                ? // if field is a function, call it with the row
                  col.field(row)
                : // otherwise index the row with a string key; assert key is string
                  row[col.field === void 0 ? (col.name as string) : (col.field as string)],
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

const onQuery = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  let no = '';
  let name = '';
  let phone = '';

  switch (search.value.select.model) {
    case '缴费编号':
      no = search.value.content;
      break;
    case '货主':
      name = search.value.content;
      break;
    case '电话':
      phone = search.value.content;
      break;
  }

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=clean&path=/api/invoice/count',
      {
        id: '',
        no: no,
        name: name,
        phone: phone,
        status: 0,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      pagination.value!.rowsNumber = parseInt(resp.data.data);

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
  dialog.value.no = val.no as string;
  dialog.value.name = val.name as string;
  dialog.value.phone = val.phone as string;
  dialog.value.idcard = val.idcard as string;
  dialog.value.livestock_type.model = val.livestock_type as { label: string; value: string };
  dialog.value.livestock_num = val.livestock_num as string;
  dialog.value.livestock_amount = val.livestock_amount as string;
  dialog.value.car_type.model = val.car_type as { label: string; value: string };
  dialog.value.car_num = val.car_num as string;
  dialog.value.car_amount = val.car_amount as string;
  dialog.value.amount.cash = val.amount as string;
  dialog.value.amount.qrcode = val.amount as string;
  dialog.value.amount.lower = val.amount as string;
  dialog.value.remark = val.remark as string;
  dialog.value.create_date = val.create_date as string;
  dialog.value.show = true;
};

const onNew = () => {
  dialog.value.mode = 'create';
  dialog.value.id = '';
  dialog.value.no = '';
  dialog.value.name = '';
  dialog.value.phone = '';
  dialog.value.idcard = '';
  dialog.value.livestock_type.model = {
    label: '牛',
    value: '10',
  };
  dialog.value.livestock_num = '0';
  dialog.value.livestock_amount = '0.00';
  dialog.value.car_type.model = {
    label: '6米至9.8米车辆',
    value: '20',
  };
  dialog.value.car_num = '0';
  dialog.value.car_amount = '0.00';
  dialog.value.amount = {
    cash: '0.00',
    qrcode: '0.00',
    lower: '0.00',
    capital: '零元零角零分',
  };
  dialog.value.remark = '';
  dialog.value.create_date = date.formatDate(Date.now(), 'YYYY-MM-DD');
  dialog.value.show = true;
};

const onSave = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  onPrint();

  loading.value = false;
  clearTimeout(time);
};

const onPrint = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  window.open(
    'print/livestock_print.html?no=' +
      encodeURI(dialog.value.no) +
      '&name=' +
      encodeURI(dialog.value.name) +
      '&phone=' +
      encodeURI(dialog.value.phone) +
      '&idcard=' +
      encodeURI(dialog.value.idcard) +
      '&livestock_type=' +
      encodeURI(dialog.value.livestock_type.model.label) +
      '&livestock_price=' +
      encodeURI(dialog.value.livestock_type.model.value) +
      '&livestock_num=' +
      encodeURI(dialog.value.livestock_num) +
      '&livestock_amount=' +
      encodeURI(dialog.value.livestock_amount) +
      '&car_type=' +
      encodeURI(dialog.value.car_type.model.label) +
      '&car_price=' +
      encodeURI(dialog.value.car_type.model.value) +
      '&car_num=' +
      encodeURI(dialog.value.car_num) +
      '&car_amount=' +
      encodeURI(dialog.value.car_amount) +
      '&car_amount=' +
      encodeURI(dialog.value.car_amount) +
      '&amount_cash=' +
      encodeURI(dialog.value.amount.cash) +
      '&amount_qrcode=' +
      encodeURI(dialog.value.amount.qrcode) +
      '&amount_lower=' +
      encodeURI(dialog.value.amount.lower) +
      '&amount_capital=' +
      encodeURI(dialog.value.amount.capital) +
      '&create_date=' +
      encodeURI(dialog.value.create_date),
  );

  loading.value = false;
  clearTimeout(time);
  // hiprint.print({
  //   templates: [{ template: hiprintTemplate, data: printData1 }],
  // });
};
</script>
