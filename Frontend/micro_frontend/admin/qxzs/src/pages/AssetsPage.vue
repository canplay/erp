<template>
  <q-page>
    <q-table
      virtual-scroll
      separator="cell"
      :rows="rows"
      :columns="columns"
      row-key="name"
      selection="multiple"
      v-model:selected="selected"
      :style="{
        maxWidth: props.componentSize.width + 'px',
        height: props.componentSize.height + 'px',
      }"
    >
      <template v-slot:top>
        <div class="fit">
          <div class="row" style="height: 60px">
            <q-btn
              class="col"
              color="positive"
              label="新增"
              @click="dialog.model = !dialog.model"
            />

            <div class="col-auto" style="width: 8px" />

            <q-btn class="col" color="negative" label="删除" :disable="selected.length <= 0" />

            <div class="col-auto" style="width: 8px" />

            <q-btn class="col" color="secondary" label="导出本页" @click="onExport" />

            <div class="col-auto" style="width: 8px" />

            <q-btn class="col" color="secondary" label="导出全部" />

            <div class="col-auto" style="width: 8px" />

            <q-toggle class="col-auto" v-model="search.visible" label="筛选" />
          </div>

          <q-slide-transition>
            <q-card v-show="search.visible" style="margin-top: 8px">
              <q-card-section class="row">
                <div class="col">
                  <div class="row">
                    <q-input outlined class="col" v-model="search.content" label="公司" />

                    <div class="col-auto" style="width: 8px" />

                    <q-input outlined class="col" v-model="search.content" label="负责人" />

                    <div class="col-auto" style="width: 8px" />

                    <q-input outlined class="col" v-model="search.content" label="联系电话" />
                  </div>

                  <div style="height: 8px" />

                  <div class="row">
                    <q-input outlined class="col" v-model="search.content" label="状态" />

                    <div class="col-auto" style="width: 8px" />

                    <q-input outlined class="col" v-model="search.content" label="创建人" />

                    <div class="col-auto" style="width: 8px" />

                    <q-input outlined class="col" v-model="search.content" label="更新人" />
                  </div>

                  <div style="height: 8px" />

                  <div class="row">
                    <q-input outlined class="col" v-model="search.content" label="名称" />

                    <div class="col-auto" style="width: 8px" />

                    <q-btn class="col-2" color="primary" label="筛选" />

                    <div class="col-auto" style="width: 8px" />

                    <q-btn class="col-2" color="primary" label="重置" />
                  </div>
                </div>

                <div class="col-auto" style="width: 8px" />

                <div class="col-4 row">
                  <q-card class="col">
                    <q-card-section>
                      <div class="text-h6">创建日期</div>

                      <div style="height: 8px" />

                      <q-input v-model="search.date" label="开始时间">
                        <template v-slot:prepend>
                          <q-icon name="event">
                            <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                              <q-date v-model="search.date" mask="YYYY-MM-DD HH:mm" />
                            </q-popup-proxy>
                          </q-icon>
                        </template>

                        <template v-slot:append>
                          <q-icon name="access_time">
                            <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                              <q-time v-model="search.date" mask="YYYY-MM-DD HH:mm" format24h />
                            </q-popup-proxy>
                          </q-icon>
                        </template>
                      </q-input>

                      <q-input v-model="search.date" label="结束时间">
                        <template v-slot:prepend>
                          <q-icon name="event">
                            <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                              <q-date v-model="search.date" mask="YYYY-MM-DD HH:mm" />
                            </q-popup-proxy>
                          </q-icon>
                        </template>

                        <template v-slot:append>
                          <q-icon name="access_time">
                            <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                              <q-time v-model="search.date" mask="YYYY-MM-DD HH:mm" format24h />
                            </q-popup-proxy>
                          </q-icon>
                        </template>
                      </q-input>
                    </q-card-section>
                  </q-card>

                  <div class="col-auto" style="width: 8px" />

                  <q-card class="col">
                    <q-card-section>
                      <div class="text-h6">更新日期</div>

                      <div style="height: 8px" />

                      <q-input v-model="search.date" label="开始时间">
                        <template v-slot:prepend>
                          <q-icon name="event">
                            <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                              <q-date v-model="search.date" mask="YYYY-MM-DD HH:mm" />
                            </q-popup-proxy>
                          </q-icon>
                        </template>

                        <template v-slot:append>
                          <q-icon name="access_time">
                            <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                              <q-time v-model="search.date" mask="YYYY-MM-DD HH:mm" format24h />
                            </q-popup-proxy>
                          </q-icon>
                        </template>
                      </q-input>

                      <q-input v-model="search.date" label="结束时间">
                        <template v-slot:prepend>
                          <q-icon name="event">
                            <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                              <q-date v-model="search.date" mask="YYYY-MM-DD HH:mm" />
                            </q-popup-proxy>
                          </q-icon>
                        </template>

                        <template v-slot:append>
                          <q-icon name="access_time">
                            <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                              <q-time v-model="search.date" mask="YYYY-MM-DD HH:mm" format24h />
                            </q-popup-proxy>
                          </q-icon>
                        </template>
                      </q-input>
                    </q-card-section>
                  </q-card>
                </div>
              </q-card-section>
            </q-card>
          </q-slide-transition>
        </div>
      </template>

      <template v-slot:header-cell="props">
        <q-th :props="props" style="font-size: 15px; font-weight: bold">
          {{ props.col.label }}
        </q-th>
      </template>
    </q-table>
  </q-page>

  <q-dialog v-model="dialog.model" persistent>
    <q-card style="min-width: 400px; max-width: 90vw">
      <q-toolbar class="bg-primary text-white">
        <q-toolbar-title>新增资产</q-toolbar-title>
        <q-btn flat round dense icon="close" v-close-popup />
      </q-toolbar>

      <q-scroll-area
        :style="{
          width: '100%',
          height: dialog.size.height > 0 ? dialog.size.height + 'px' : 'auto',
          maxHeight: 'calc(90vh - 50px)',
          padding: '8px',
        }"
      >
        <q-resize-observer @resize="onResize" />
        <q-card bordered>
          <q-card-section>
            <div class="text-h6">基本信息</div>

            <div style="height: 8px" />

            <q-input outlined v-model="search.content" label="名称" />

            <div style="height: 8px" />

            <q-input outlined v-model="search.content" label="说明" />

            <div style="height: 8px" />

            <q-input outlined v-model="search.content" label="公司" />
          </q-card-section>
        </q-card>

        <div style="height: 8px" />

        <q-card bordered>
          <q-card-section>
            <div class="text-h6">状态信息</div>

            <div style="height: 8px" />

            <q-input outlined v-model="search.content" label="状态" />

            <div style="height: 8px" />

            <q-input outlined v-model="search.content" label="地理位置" />

            <div style="height: 8px" />

            <q-input outlined v-model="search.content" label="负责人" />

            <div style="height: 8px" />

            <q-input outlined v-model="search.content" label="负责人电话" />
          </q-card-section>
        </q-card>

        <div style="height: 8px" />

        <q-btn color="primary" class="full-width" label="保存" />
      </q-scroll-area>
    </q-card>
  </q-dialog>
</template>

<script setup lang="ts">
import { ref } from 'vue';
import type { QTableProps } from 'quasar';
import { exportFile, useQuasar } from 'quasar';

// 接收主应用传递的可用空间
const props = defineProps<{
  componentSize: {
    width: number;
    height: number;
  };
}>();

const $q = useQuasar();

const columns = <QTableProps['columns']>[
  {
    name: 'name',
    required: true,
    label: '名称',
    align: 'center',
    field: 'name',
    sortable: true,
  },
  {
    name: 'company',
    label: '公司',
    align: 'center',
    field: 'company',
    sortable: true,
  },
  { name: 'contacts', label: '负责人', align: 'center', field: 'contacts', sortable: true },
  { name: 'phone', label: '联系电话', align: 'center', field: 'phone', sortable: true },
  { name: 'status', label: '状态', align: 'center', field: 'status', sortable: true },
  { name: 'create_date', label: '创建日期', align: 'center', field: 'create_date', sortable: true },
  { name: 'create_user', label: '创建人', align: 'center', field: 'create_user', sortable: true },
  { name: 'update_date', label: '更新日期', align: 'center', field: 'update_date', sortable: true },
  { name: 'update_user', label: '更新人', align: 'center', field: 'update_user', sortable: true },
];

const rows = ref([
  {
    name: 'Frozen Yogurt',
    desc: 159,
    company: 6.0,
    contacts: 24,
    create_date: 4.0,
    create_user: 87,
    update_date: '14%',
    update_user: '1%',
  },
]);

interface RowData {
  name: string;
  desc: number | string;
  company: number | string;
  contacts: number | string;
  create_date: number | string;
  create_user: number | string;
  update_date: number | string;
  update_user: number | string;
  [key: string]: number | string;
}

const selected = ref([]);

const search = ref({
  visible: false,
  date: '',
  content: '',
});

const dialog = ref({
  model: false,
  size: {
    width: 0,
    height: 0,
  },
});

const wrapCsvValue = (
  val: unknown,
  formatFn?: (val: unknown, row?: unknown) => unknown,
  row?: unknown,
) => {
  let formatted = formatFn !== void 0 ? formatFn(val, row) : val;

  formatted = formatted === void 0 || formatted === null ? '' : String(formatted as string);

  formatted = (formatted as string).split('"').join('""');

  return `"${formatted as string}"`;
};

const onExport = () => {
  const content = [columns!.map((col) => wrapCsvValue(col.label))]
    .concat(
      rows.value.map((row: RowData) =>
        columns!
          .map((col) =>
            wrapCsvValue(
              typeof col.field === 'function'
                ? col.field(row)
                : row[col.field === void 0 ? col.name : col.field],
              col.format,
              row,
            ),
          )
          .join(','),
      ),
    )
    .join('\r\n');

  const status = exportFile('业务列表.csv', content, 'text/csv');

  if (status !== true) {
    $q.notify({ type: 'positive', message: '即将开始下载' });
  }
};

const onResize = (size: { width: number; height: number }) => {
  if (dialog.value.size.height <= 0) {
    // 计算合适的滚动区域高度（考虑工具栏高度和一些边距）
    const toolbarHeight = 50; // 工具栏高度
    const padding = 16; // padding值
    const maxHeight = props.componentSize.height - toolbarHeight - padding;
    const maxWidth = props.componentSize.width - padding;

    // 设置滚动区域高度，不超过最大高度
    dialog.value.size.height = Math.min(size.height + padding, maxHeight);
    dialog.value.size.width = Math.min(size.width + padding, maxWidth);
  }
};
</script>
