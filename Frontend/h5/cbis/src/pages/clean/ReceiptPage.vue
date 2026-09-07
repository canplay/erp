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
            <q-input class="col" v-model="search.date" label="开票日期">
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

        <q-td v-else-if="props.col.name === 'invalid'" :props="props">
          {{ props.value === true ? '是' : '否' }}
        </q-td>

        <q-td v-else :props="props">
          {{ props.value }}
        </q-td>
      </template>
    </q-table>

    <q-dialog v-model="dialog.view.show">
      <q-card style="min-width: 70%">
        <q-card-section>
          <div class="row">
            <q-input readonly class="col" v-model="dialog.view.no" label="票据号" />

            <div class="col-auto" style="width: 8px" />

            <q-input readonly class="col" v-model="dialog.view.imposing_no" label="执收单位编码" />

            <div class="col-auto" style="width: 8px" />

            <q-input
              readonly
              class="col"
              v-model="dialog.view.imposing_name"
              label="执收单位名称"
            />
          </div>

          <div class="col-auto" style="height: 8px" />

          <div class="row">
            <q-input
              readonly
              class="col"
              v-model="dialog.view.collection_name"
              label="代收单位名称"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input readonly class="col" v-model="dialog.view.create_date" label="开票日期" />

            <div class="col-auto" style="width: 8px" />

            <q-input readonly class="col" v-model="dialog.view.zone" label="区号" />
          </div>

          <div class="col-auto" style="height: 8px" />

          <q-input readonly v-model="dialog.view.payer" label="缴款人" />

          <div class="col-auto" style="height: 8px" />

          <q-table
            square
            :rows="dialog.view.rows"
            :columns="dialog.view.columns"
            row-key="project_code"
            separator="cell"
          />

          <div class="col-auto" style="height: 8px" />

          <div class="row">
            <q-input readonly class="col" v-model="dialog.view.sum" label="合计" />

            <div class="col-auto" style="width: 8px" />

            <q-input readonly class="col" v-model="dialog.view.sum_capital" label="合计（大写）" />

            <div class="col-auto" style="width: 8px" />

            <q-input readonly class="col" v-model="dialog.view.remark" label="备注" />
          </div>

          <div class="col-auto" style="height: 8px" />

          <div class="row">
            <q-input readonly class="col" v-model="dialog.view.review" label="复核人" />

            <div class="col-auto" style="width: 8px" />

            <q-input readonly class="col" v-model="dialog.view.operator" label="经办人" />

            <div class="col-auto" style="width: 8px" />

            <q-input readonly class="col" v-model="dialog.view.create_user" label="开票人" />
          </div>

          <div class="col-auto" style="height: 8px" />

          <div class="row">
            <q-btn
              v-if="!dialog.view.invalid"
              class="col"
              style="font-size: 20px"
              color="negative"
              label="作废"
              @click="onDelete"
            />

            <div class="col-auto" style="width: 8px" />

            <q-btn
              class="col"
              style="font-size: 20px"
              color="positive"
              label="打印"
              @click="onPrint"
            />
          </div>
        </q-card-section>
      </q-card>

      <img
        v-if="dialog.view.invalid"
        class="fixed-center"
        src="/imgs/delete.png"
        style="width: 200px; transform: rotate(30deg)"
      />
    </q-dialog>

    <q-dialog v-model="dialog.create.show" persistent>
      <q-card style="min-width: 70%">
        <q-card-section>
          <div class="row">
            <q-input
              class="col"
              v-model="dialog.create.no"
              label="票据号"
              :rules="[(val) => !!val || '此为必填项']"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="dialog.create.imposing_no" label="执收单位编码" />

            <div class="col-auto" style="width: 8px" />

            <q-input
              class="col"
              v-model="dialog.create.imposing_name"
              label="执收单位名称"
              :rules="[(val) => !!val || '此为必填项']"
            />
          </div>

          <div class="col-auto" style="height: 8px" />

          <div class="row">
            <q-input
              class="col"
              v-model="dialog.create.collection_name"
              label="代收单位名称"
              :rules="[(val) => !!val || '此为必填项']"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="dialog.create.create_date" label="开票日期">
              <template v-slot:prepend>
                <q-icon name="event" class="cursor-pointer">
                  <q-popup-proxy transition-show="scale" transition-hide="scale">
                    <q-date v-model="dialog.create.create_date" mask="YYYY-MM-DD HH:mm:ss">
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
                    <q-time
                      v-model="dialog.create.create_date"
                      mask="YYYY-MM-DD HH:mm:ss"
                      format24h
                    >
                      <div class="row items-center justify-end">
                        <q-btn v-close-popup label="关闭" color="primary" flat />
                      </div>
                    </q-time>
                  </q-popup-proxy>
                </q-icon>
              </template>
            </q-input>

            <div class="col-auto" style="width: 8px" />

            <q-input
              class="col"
              v-model="dialog.create.zone"
              label="区号"
              :rules="[(val) => !!val || '此为必填项']"
            />
          </div>

          <div class="col-auto" style="height: 8px" />

          <q-input
            v-model="dialog.create.payer"
            label="缴款人"
            :rules="[(val) => !!val || '此为必填项']"
          />

          <div class="col-auto" style="height: 8px" />

          <q-table
            square
            :rows="dialog.create.rows"
            :columns="dialog.create.columns"
            row-key="project_code"
            separator="cell"
          >
            <template v-slot:top>
              <div class="fit row">
                <div class="col-8" />

                <q-btn class="col" color="primary" label="添加" @click="addRow" />

                <div class="col-auto" style="width: 8px" />

                <q-btn class="col" color="primary" label="删除" @click="delRow" />
              </div>
            </template>

            <template v-slot:header-cell="props">
              <q-th :props="props" style="font-size: 15px; font-weight: bold">
                {{ props.col.label }}
              </q-th>
            </template>

            <template v-slot:body="props">
              <q-tr :props="props">
                <q-td key="project_code" :props="props">
                  {{ props.row.project_code }}
                  <q-popup-edit v-model="props.row.project_code">
                    <q-input v-model="props.row.project_code" dense autofocus />
                  </q-popup-edit>
                </q-td>

                <q-td key="project_name" :props="props">
                  {{ props.row.project_name }}
                  <q-popup-edit v-model="props.row.project_name">
                    <q-input v-model="props.row.project_name" dense autofocus />
                  </q-popup-edit>
                </q-td>

                <q-td key="unit" :props="props">
                  {{ props.row.unit }}
                  <q-popup-edit v-model="props.row.unit">
                    <q-input v-model="props.row.unit" dense autofocus />
                  </q-popup-edit>
                </q-td>

                <q-td key="num" :props="props">
                  {{ props.row.num }}
                  <q-popup-edit v-model="props.row.num">
                    <q-input v-model="props.row.num" dense autofocus />
                  </q-popup-edit>
                </q-td>

                <q-td key="criteria" :props="props">
                  {{ props.row.criteria }}
                  <q-popup-edit v-model="props.row.criteria">
                    <q-input v-model="props.row.criteria" dense autofocus />
                  </q-popup-edit>
                </q-td>

                <q-td key="sum" :props="props">
                  {{ props.row.sum }}
                  <q-popup-edit v-model="props.row.sum">
                    <q-input
                      v-model="props.row.sum"
                      dense
                      autofocus
                      debounce="1000"
                      @update:model-value="onSum"
                    />
                  </q-popup-edit>
                </q-td>
              </q-tr>
            </template>
          </q-table>

          <div class="col-auto" style="height: 8px" />

          <div class="row">
            <q-input
              readonly
              class="col"
              v-model="dialog.create.sum"
              label="合计"
              :rules="[(val) => !!val || '此为必填项']"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input
              readonly
              class="col"
              v-model="dialog.create.sum_capital"
              label="合计（大写）"
              :rules="[(val) => !!val || '此为必填项']"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input
              class="col"
              v-model="dialog.create.remark"
              label="备注"
              :rules="[(val) => !!val || '此为必填项']"
            />
          </div>

          <div class="col-auto" style="height: 8px" />

          <div class="row">
            <q-input
              class="col"
              v-model="dialog.create.review"
              label="复核人"
              :rules="[(val) => !!val || '此为必填项']"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input
              class="col"
              v-model="dialog.create.operator"
              label="经办人"
              :rules="[(val) => !!val || '此为必填项']"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input
              readonly
              class="col"
              v-model="dialog.create.create_user"
              label="开票人"
              :rules="[(val) => !!val || '此为必填项']"
            />
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
import type { QTableProps } from 'quasar';
import { date, exportFile } from 'quasar';
import { useStore } from 'stores/store';
import { api } from 'src/boot/axios';
import { numberToChineseUppercaseCurrency } from 'pixiu-number-toolkit';

const store = useStore();

const loading = ref(false);

const search = ref({
  date: '',
  content: '',
  select: {
    model: '',
    options: [
      '',
      '票据号',
      '执收单位编码',
      '执收单位名称',
      '代收单位名称',
      '缴款人',
      '复核人',
      '经办人',
      '开票人',
    ],
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
    name: 'no',
    label: '票据号',
    field: 'no',
    align: 'center',
    sortable: true,
  },
  {
    name: 'imposing_name',
    label: '执收单位名称',
    field: 'imposing_name',
    align: 'center',
    sortable: true,
  },
  {
    name: 'create_date',
    label: '开票日期',
    field: 'create_date',
    align: 'center',
    sortable: true,
  },
  { name: 'payer', label: '缴款人', field: 'payer', align: 'center' },
  {
    name: 'sum',
    label: '金额',
    field: 'sum',
    align: 'center',
    sortable: true,
  },
  { name: 'remark', label: '备注', field: 'remark', align: 'center' },
  {
    name: 'create_user',
    label: '开票人',
    field: 'create_user',
    align: 'center',
    sortable: true,
  },
  {
    name: 'operator',
    label: '经办人',
    field: 'operator',
    align: 'center',
    sortable: true,
  },
  {
    name: 'review',
    label: '复核人',
    field: 'review',
    align: 'center',
    sortable: true,
  },
  {
    name: 'print',
    label: '打印次数',
    field: 'print',
    align: 'center',
    sortable: true,
  },
  {
    name: 'invalid',
    label: '是否作废',
    field: 'invalid',
    align: 'center',
    sortable: true,
  },
] as QTableProps['columns']);

const rows = ref([] as Record<string, unknown>[]);

const dialog = ref({
  mode: 'create',
  create: {
    id: '',
    show: false,
    no: '',
    imposing_no: '',
    imposing_name: '',
    collection_name: '',
    fingerprint: '',
    create_date: '',
    zone: '',
    payer: '',
    sum: '',
    sum_capital: '',
    remark: '',
    review: '',
    operator: '',
    create_user: '',
    print: 0,
    invalid: false,
    columns: [
      {
        name: 'project_code',
        label: '项目代码',
        field: 'project_code',
        align: 'center',
      },
      {
        name: 'project_name',
        label: '项目名称',
        field: 'project_name',
        align: 'center',
      },
      {
        name: 'unit',
        label: '计量单位',
        field: 'unit',
        align: 'center',
      },
      {
        name: 'num',
        label: '数量',
        field: 'num',
        align: 'center',
      },
      {
        name: 'criteria',
        label: '标准',
        field: 'criteria',
        align: 'center',
      },
      {
        name: 'sum',
        label: '金额',
        field: 'sum',
        align: 'center',
      },
    ] as QTableProps['columns'],
    rows: [] as Record<string, unknown>[],
    selected: [],
  },
  view: {
    id: '',
    show: false,
    no: '',
    imposing_no: '',
    imposing_name: '',
    collection_name: '',
    fingerprint: '',
    create_date: '',
    zone: '',
    payer: '',
    sum: '',
    sum_capital: '',
    remark: '',
    review: '',
    operator: '',
    create_user: '',
    print: 0,
    invalid: false,
    columns: [
      {
        name: 'project_code',
        label: '项目代码',
        field: 'project_code',
        align: 'center',
      },
      {
        name: 'project_name',
        label: '项目名称',
        field: 'project_name',
        align: 'center',
      },
      {
        name: 'unit',
        label: '计量单位',
        field: 'unit',
        align: 'center',
      },
      {
        name: 'num',
        label: '数量',
        field: 'num',
        align: 'center',
      },
      {
        name: 'criteria',
        label: '标准',
        field: 'criteria',
        align: 'center',
      },
      {
        name: 'sum',
        label: '金额',
        field: 'sum',
        align: 'center',
      },
    ] as QTableProps['columns'],
    rows: [] as Record<string, unknown>[],
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

const onRequest = (props: { pagination: QTableProps['pagination'] }) => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  const { page, rowsPerPage, sortBy, descending, rowsNumber } = props.pagination!;

  rows.value = [];

  let no = '';
  let imposing_no = 0;
  let imposing_name = '';
  let collection_name = '';
  let payer = '';
  let remark = '';
  let review = '';
  let operator = '';
  let create_user = '';

  switch (search.value.select.model) {
    case '票据号':
      no = search.value.content;
      break;
    case '执收单位编码':
      imposing_no = parseInt(search.value.content);
      break;
    case '执收单位名称':
      imposing_name = search.value.content;
      break;
    case '代收单位名称':
      collection_name = search.value.content;
      break;
    case '缴款人':
      payer = search.value.content;
      break;
    case '备注':
      remark = search.value.content;
      break;
    case '复核人':
      review = search.value.content;
      break;
    case '经办人':
      operator = search.value.content;
      break;
    case '开票人':
      create_user = search.value.content;
      break;
  }

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=clean&path=/api/invoice/info',
      {
        id: '',
        no: no,
        imposing_no: imposing_no,
        imposing_name: imposing_name,
        collection_name: collection_name,
        fingerprint: '',
        create_date: search.value.date,
        zone: '',
        payer: payer,
        sum: '',
        sum_capital: '',
        remark: remark,
        review: review,
        operator: operator,
        create_user: create_user,
        invalid: false,
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
          if (resp.data.data[i].id === '') break;

          rows.value.push({
            id: resp.data.data[i].id,
            no: resp.data.data[i].no,
            imposing_no: resp.data.data[i].imposing_no,
            imposing_name: resp.data.data[i].imposing_name,
            collection_name: resp.data.data[i].collection_name,
            fingerprint: resp.data.data[i].fingerprint,
            create_date: date.formatDate(resp.data.data[i].create_date, 'YYYY-MM-DD HH:mm:ss'),
            zone: resp.data.data[i].zone,
            payer: resp.data.data[i].payer,
            project: JSON.parse(resp.data.data[i].project),
            sum: resp.data.data[i].sum,
            sum_capital: resp.data.data[i].sum_capital,
            remark: resp.data.data[i].remark,
            review: resp.data.data[i].review,
            operator: resp.data.data[i].operator,
            create_user: resp.data.data[i].create_user,
            update_date: date.formatDate(resp.data.data[i].update_date, 'YYYY-MM-DD HH:mm:ss'),
            update_user: resp.data.data[i].update_user,
            invalid: resp.data.data[i].invalid,
            print: resp.data.data[i].print,
          });
        }
      } else {
        store.quasar.notify('网络错误，请稍后重试');
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

  let no = '';
  let imposing_no = 0;
  let imposing_name = '';
  let collection_name = '';
  let payer = '';
  let remark = '';
  let review = '';
  let operator = '';
  let create_user = '';

  switch (search.value.select.model) {
    case '票据号':
      no = search.value.content;
      break;
    case '执收单位编码':
      imposing_no = parseInt(search.value.content);
      break;
    case '执收单位名称':
      imposing_name = search.value.content;
      break;
    case '代收单位名称':
      collection_name = search.value.content;
      break;
    case '缴款人':
      payer = search.value.content;
      break;
    case '备注':
      remark = search.value.content;
      break;
    case '复核人':
      review = search.value.content;
      break;
    case '经办人':
      operator = search.value.content;
      break;
    case '开票人':
      create_user = search.value.content;
      break;
  }

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=clean&path=/api/invoice/count',
      {
        id: '',
        no: no,
        imposing_no: imposing_no,
        imposing_name: imposing_name,
        collection_name: collection_name,
        fingerprint: '',
        create_date: search.value.date,
        zone: '',
        payer: payer,
        sum: '',
        sum_capital: '',
        remark: remark,
        review: review,
        operator: operator,
        create_user: create_user,
        invalid: false,
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
  dialog.value.view.id = val.id as string;
  dialog.value.view.no = val.no as string;
  dialog.value.view.imposing_no = val.imposing_no as string;
  dialog.value.view.imposing_name = val.imposing_name as string;
  dialog.value.view.collection_name = val.collection_name as string;
  dialog.value.view.fingerprint = val.fingerprint as string;
  dialog.value.view.create_date = val.create_date as string;
  dialog.value.view.zone = val.zone as string;
  dialog.value.view.payer = val.payer as string;
  dialog.value.view.rows = val.project as Record<string, unknown>[];
  dialog.value.view.sum = val.sum as string;
  dialog.value.view.sum_capital = val.sum_capital as string;
  dialog.value.view.remark = val.remark as string;
  dialog.value.view.review = val.review as string;
  dialog.value.view.operator = val.operator as string;
  dialog.value.view.create_user = val.create_user as string;
  dialog.value.view.invalid = val.invalid as boolean;
  dialog.value.view.print = val.print as number;

  dialog.value.view.show = true;
};

const onNew = () => {
  dialog.value.mode = 'create';
  dialog.value.create.id = '';
  dialog.value.create.no = '';
  dialog.value.create.imposing_no = '';
  dialog.value.create.imposing_name = '国家税务总局示例城市A市税务局';
  dialog.value.create.collection_name = '示例城市A市洁城环卫有限公司';
  dialog.value.create.fingerprint = '';
  dialog.value.create.create_date = date.formatDate(Date.now(), 'YYYY-MM-DD H:mm:ss');
  dialog.value.create.zone = '县级';
  dialog.value.create.payer = '';
  dialog.value.create.rows = [];
  dialog.value.create.rows.push({
    project_code: 304331300,
    project_name: '城镇垃圾处理费',
    unit: '',
    num: '',
    criteria: '',
    sum: '',
  });
  dialog.value.create.sum = '';
  dialog.value.create.sum_capital = '';
  dialog.value.create.remark = '';
  dialog.value.create.review = '';
  dialog.value.create.operator = '';
  dialog.value.create.create_user = store.user.name;
  dialog.value.create.invalid = false;
  dialog.value.create.print = 0;

  dialog.value.create.show = true;
};

const onSum = () => {
  dialog.value.create.sum = '0.0';
  dialog.value.create.rows.forEach((val: Record<string, unknown>) => {
    dialog.value.create.sum = (
      parseFloat(dialog.value.create.sum) + parseFloat(val.sum as string)
    ).toString();
  });
  dialog.value.create.sum_capital = numberToChineseUppercaseCurrency(
    parseFloat(dialog.value.create.sum),
  ).replace('欠', '（负数）');
};

const addRow = () => {
  dialog.value.create.rows.push({
    project_code: dialog.value.create.rows.length,
    project_name: '',
    unit: '',
    num: '',
    criteria: '',
    sum: '',
  });
};

const delRow = () => {
  dialog.value.create.rows.splice(dialog.value.create.rows.length - 1);
  onSum();
};

const onDelete = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=clean&path=/api/invoice/update',
      {
        id: dialog.value.view.id,
        no: dialog.value.view.no,
        imposing_no: parseInt(dialog.value.view.imposing_no),
        imposing_name: dialog.value.view.imposing_name,
        collection_name: dialog.value.view.collection_name,
        fingerprint: dialog.value.view.fingerprint,
        create_date: dialog.value.view.create_date,
        zone: dialog.value.view.zone,
        payer: dialog.value.view.payer,
        project: JSON.stringify(dialog.value.view.rows),
        sum: parseFloat(dialog.value.view.sum).toFixed(2).toString(),
        sum_capital: dialog.value.view.sum_capital,
        remark: dialog.value.view.remark,
        review: dialog.value.view.review,
        operator: dialog.value.view.operator,
        create_user: dialog.value.view.create_user,
        print: dialog.value.view.print,
        invalid: true,
        delete: false,
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
      dialog.value.view.show = false;
      onQuery();
    })
    .catch(() => {
      loading.value = false;
      store.quasar.notify('网络错误，请稍后重试');
    });
};

const onSave = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=clean&path=/api/invoice/update',
      {
        id: '',
        no: dialog.value.create.no,
        imposing_no: parseInt(dialog.value.create.imposing_no),
        imposing_name: dialog.value.create.imposing_name,
        collection_name: dialog.value.create.collection_name,
        fingerprint: dialog.value.create.fingerprint,
        create_date: dialog.value.create.create_date,
        zone: dialog.value.create.zone,
        payer: dialog.value.create.payer,
        project: JSON.stringify(dialog.value.create.rows),
        sum: parseFloat(dialog.value.create.sum).toFixed(2).toString(),
        sum_capital: dialog.value.create.sum_capital,
        remark: dialog.value.create.remark,
        review: dialog.value.create.review,
        operator: dialog.value.create.operator,
        create_user: dialog.value.create.create_user,
        print: dialog.value.create.print,
        invalid: dialog.value.create.invalid,
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
      dialog.value.create.show = false;
      onQuery();
    })
    .catch(() => {
      loading.value = false;
      store.quasar.notify('网络错误，请稍后重试');
    });
};

const onPrint = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=clean&path=/api/invoice/update',
      {
        id: dialog.value.view.id,
        no: dialog.value.view.no,
        imposing_no: parseInt(dialog.value.view.imposing_no),
        imposing_name: dialog.value.view.imposing_name,
        collection_name: dialog.value.view.collection_name,
        fingerprint: dialog.value.view.fingerprint,
        create_date: dialog.value.view.create_date,
        zone: dialog.value.view.zone,
        payer: dialog.value.view.payer,
        project: JSON.stringify(dialog.value.view.rows),
        sum: parseFloat(dialog.value.view.sum).toFixed(2).toString(),
        sum_capital: dialog.value.view.sum_capital,
        remark: dialog.value.view.remark,
        review: dialog.value.view.review,
        operator: dialog.value.view.operator,
        create_user: dialog.value.view.create_user,
        print: dialog.value.view.print + 1,
        invalid: dialog.value.view.invalid,
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
        store.quasar.notify('打印失败，请稍后重试');
        return;
      }

      store.quasar.notify('打印成功');
      onQuery();
    })
    .catch(() => {
      loading.value = false;
      store.quasar.notify('网络错误，请稍后重试');
    });

  if (dialog.value.mode === 'create') {
    window.open(
      'print/clean_invoice_print.html?no=' +
        encodeURI(dialog.value.create.no) +
        '&imposing_no=' +
        encodeURI(dialog.value.create.imposing_no) +
        '&imposing_name=' +
        encodeURI(dialog.value.create.imposing_name) +
        '&collection_name=' +
        encodeURI(dialog.value.create.collection_name) +
        '&create_date=' +
        encodeURI(dialog.value.create.create_date) +
        '&payer=' +
        encodeURI(dialog.value.create.payer) +
        '&zone=' +
        encodeURI(dialog.value.create.zone) +
        '&project=' +
        encodeURI(JSON.stringify(dialog.value.create.rows)) +
        '&sum=' +
        encodeURI(dialog.value.create.sum) +
        '&sum_capital=' +
        encodeURI(dialog.value.create.sum_capital) +
        '&remark=' +
        encodeURI(dialog.value.create.remark) +
        ', 票据号码：' +
        encodeURI(dialog.value.create.no) +
        '(如与右上角印刷码不一致，票据无效！)' +
        '&review=' +
        encodeURI(dialog.value.create.review) +
        '&operator=' +
        encodeURI(dialog.value.create.operator) +
        '&create_user=' +
        encodeURI(dialog.value.create.create_user) +
        '&fingerprint=' +
        encodeURI(dialog.value.create.fingerprint),
    );
  }
  if (dialog.value.mode === 'view') {
    window.open(
      'print/clean_invoice_print.html?no=' +
        encodeURI(dialog.value.view.no) +
        '&imposing_no=' +
        encodeURI(dialog.value.view.imposing_no) +
        '&imposing_name=' +
        encodeURI(dialog.value.view.imposing_name) +
        '&collection_name=' +
        encodeURI(dialog.value.view.collection_name) +
        '&create_date=' +
        encodeURI(dialog.value.view.create_date) +
        '&payer=' +
        encodeURI(dialog.value.view.payer) +
        '&zone=' +
        encodeURI(dialog.value.view.zone) +
        '&project=' +
        encodeURI(JSON.stringify(dialog.value.view.rows)) +
        '&sum=' +
        encodeURI(dialog.value.view.sum) +
        '&sum_capital=' +
        encodeURI(dialog.value.view.sum_capital) +
        '&remark=' +
        encodeURI(dialog.value.view.remark) +
        ', 票据号码：' +
        encodeURI(dialog.value.view.no) +
        '(如与右上角印刷码不一致，票据无效！)' +
        '&review=' +
        encodeURI(dialog.value.view.review) +
        '&operator=' +
        encodeURI(dialog.value.view.operator) +
        '&create_user=' +
        encodeURI(dialog.value.view.create_user) +
        '&fingerprint=' +
        encodeURI(dialog.value.view.fingerprint),
    );
  }
};
</script>
