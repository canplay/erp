<template>
  <q-page>
    <q-table
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

          <q-btn class="col-2" color="primary" label="查询" @click="onQuery" />

          <div class="col-auto" style="width: 8px" />

          <q-btn class="col-2" color="red" label="新增" @click="onNew" />
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
            @click="onShow(props.row)"
          />
        </q-td>

        <q-td v-else-if="props.col.name === 'delete'" :props="props">
          {{ props.value === true ? '是' : '否' }}
        </q-td>

        <q-td v-else :props="props">
          {{ props.value }}
        </q-td>
      </template>
    </q-table>
  </q-page>

  <q-dialog v-model="dialog.show">
    <q-card style="min-width: 70%">
      <q-card-section>
        <div class="row">
          <q-input class="col" v-model="dialog.id" readonly label="ID" />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            v-model="dialog.username"
            readonly
            label="账号"
          />

          <div class="col-auto" style="width: 8px" />

          <q-btn
            class="col-2"
            color="primary"
            label="修改密码"
            @click="onPassword"
          />
        </div>

        <div class="col-auto" style="height: 8px" />

        <div class="row">
          <q-input class="col" v-model="dialog.name" label="昵称" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" v-model="dialog.head" label="头像" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" v-model="dialog.phone" label="联系电话" />
        </div>

        <div class="col-auto" style="height: 8px" />

        <div class="row">
          <q-input class="col" v-model="dialog.wechat" label="微信" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" v-model="dialog.alipay" label="支付宝" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" v-model="dialog.weibo" label="微博" />
        </div>

        <div class="col-auto" style="height: 8px" />

        <div class="row">
          <q-input class="col" v-model="dialog.email" label="邮箱" />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            v-model="dialog.last_login"
            label="最后上线时间"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" v-model="dialog.last_ip" label="最后上线IP" />
        </div>

        <div class="col-auto" style="height: 8px" />

        <div class="row">
          <q-input class="col" v-model="dialog.permissions" label="权限" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" v-model="dialog.options" label="设置" />

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" v-model="dialog.delete" label="是否删除" />
        </div>

        <div class="col-auto" style="height: 8px" />

        <div class="row">
          <q-input class="col" v-model="dialog.idcard" label="身份证号" />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            v-model="dialog.create_user"
            readonly
            label="创建人"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            v-model="dialog.create_date"
            readonly
            label="创建时间"
          />
        </div>

        <div class="col-auto" style="height: 8px" />

        <div class="row">
          <q-input
            class="col"
            v-model="dialog.update_user"
            readonly
            label="更新人"
          />

          <div class="col-auto" style="width: 8px" />

          <q-input
            class="col"
            v-model="dialog.update_date"
            readonly
            label="更新时间"
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
            @click="onUpdate"
          />
        </div>
      </q-card-section>
    </q-card>
  </q-dialog>

  <q-inner-loading :showing="loading">
    <q-spinner size="50px" color="primary" />
  </q-inner-loading>
</template>

<script setup lang="ts">
import { ref } from 'vue';
import type { QTableProps} from 'quasar';
import { exportFile } from 'quasar';
import { useStore } from 'stores/store';
import { api } from 'src/boot/axios';

const store = useStore();

const loading = ref(false);

const search = ref({
  date: '',
  content: '',
  select: {
    model: '',
    options: ['', 'ID', '账号', '昵称', '姓名', '联系电话', '是否删除'],
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
    name: 'username',
    label: '账号',
    field: 'username',
    align: 'center',
    sortable: true,
  },
  {
    name: 'name',
    label: '昵称',
    field: 'name',
    align: 'center',
    sortable: true,
  },
  {
    name: 'head',
    label: '头像',
    field: 'head',
    align: 'center',
  },
  {
    name: 'phone',
    label: '手机号',
    field: 'phone',
    align: 'center',
    sortable: true,
  },
  {
    name: 'idcard',
    label: '身份证号',
    field: 'idcard',
    align: 'center',
    sortable: true,
  },
  {
    name: 'wechat',
    label: '微信',
    field: 'wechat',
    align: 'center',
    sortable: true,
  },
  {
    name: 'alipay',
    label: '支付宝',
    field: 'alipay',
    align: 'center',
    sortable: true,
  },
  {
    name: 'weibo',
    label: '微博',
    field: 'weibo',
    align: 'center',
    sortable: true,
  },
  {
    name: 'email',
    label: '电子邮箱',
    field: 'email',
    align: 'center',
    sortable: true,
  },
  {
    name: 'permissions',
    label: '权限',
    field: 'permissions',
    align: 'center',
    sortable: true,
  },
  {
    name: 'options',
    label: '设置',
    field: 'options',
    align: 'center',
    sortable: true,
  },
  {
    name: 'last_login',
    label: '最近上线时间',
    field: 'last_login',
    align: 'center',
    sortable: true,
  },
  {
    name: 'last_ip',
    label: '最近上线IP',
    field: 'last_ip',
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
    label: '更新日期',
    field: 'update_date',
    align: 'center',
    sortable: true,
  },
  {
    name: 'delete',
    label: '是否删除',
    field: 'delete',
    align: 'center',
    sortable: true,
  },
] as NonNullable<QTableProps['columns']>);

const rows = ref([] as Record<string, unknown>[]);

const dialog = ref({
  show: false,
  id: '',
  username: '',
  password: '',
  name: '',
  head: '',
  phone: '',
  idcard: '',
  wechat: '',
  alipay: '',
  weibo: '',
  email: '',
  last_login: '',
  last_ip: '',
  delete: '',
  permissions: '',
  options: '',
  create_user: '',
  create_date: '',
  update_user: '',
  update_date: '',
});

const wrapCsvValue = (val: unknown, formatFn?: (val: unknown) => unknown) => {
  let formatted = formatFn !== void 0 ? formatFn(val) : val;

  formatted =
    formatted == null ? '' : String(formatted);

  formatted = String(formatted).split('"').join('""');

  return String(formatted);
};

const onExport = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  const content = [columns.value.map((col) => wrapCsvValue(col.label))]
    .concat(
      rows.value.map((row: Record<string, unknown>) =>
        columns
          .value.map((col) =>
            wrapCsvValue(
              typeof col.field === 'function'
                ? col.field(row)
                : (col.field === void 0 ? col.name : col.field),
              col.format as ((val: unknown) => unknown) | undefined
            )
          )
          .join(',')
      )
    )
    .join('\r\n');

  const status = exportFile('用户列表.csv', content, 'text/csv');

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

  const { page = 1, rowsPerPage = 20, sortBy = 'update_date', descending = true, rowsNumber = 0 } = props.pagination ?? {};

  rows.value = [];

  let id,
    username = '',
    nickname = '',
    name = '',
    phone = '';
  let isDelete = false;

  switch (search.value.select.model) {
    case 'ID':
      id = search.value.content;
      break;
    case '账号':
      username = search.value.content;
      break;
    case '昵称':
      nickname = search.value.content;
      break;
    case '姓名':
      name = search.value.content;
      break;
    case '联系电话':
      phone = search.value.content;
      break;
    case '是否删除':
      isDelete = search.value.content === '是' ? true : false;
      break;
  }

  api
    .post(
      store.server +
        '/api/route/post?auth=jwt&service=cbis&path=/api/user/list',
      {
        id: id,
        username: username,
        nickname: nickname,
        name: name,
        phone: phone,
        isDelete: isDelete,
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
          if (resp.data.data[i].user.id === '') break;

          rows.value.push({
            id: resp.data.data[i].user.id,
            username: resp.data.data[i].user.username,
            name: resp.data.data[i].user.name,
            head: resp.data.data[i].user.head,
            phone: resp.data.data[i].user.phone,
            idcard: resp.data.data[i].user.idcard,
            wechat: resp.data.data[i].user.wechat,
            alipay: resp.data.data[i].user.alipay,
            weibo: resp.data.data[i].user.weibo,
            email: resp.data.data[i].user.email,
            last_login: resp.data.data[i].user.last_login,
            last_ip: resp.data.data[i].user.last_ip,
            delete: resp.data.data[i].user.delete,
            permissions: resp.data.data[i].permissions.name,
            options: resp.data.data[i].options.name,
            create_user: resp.data.data[i].user.create_user.msg.user.nickname,
            create_date: resp.data.data[i].user.create_date,
            update_user: resp.data.data[i].user.update_user.msg.user.nickname,
            update_date: resp.data.data[i].user.update_date,
          });
        }
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

  let id,
    username = '',
    nickname = '',
    name = '',
    phone = '';
  let isDelete = false;

  switch (search.value.select.model) {
    case 'ID':
      id = search.value.content;
      break;
    case '账号':
      username = search.value.content;
      break;
    case '昵称':
      nickname = search.value.content;
      break;
    case '姓名':
      name = search.value.content;
      break;
    case '联系电话':
      phone = search.value.content;
      break;
    case '是否删除':
      isDelete = search.value.content === '是' ? true : false;
      break;
  }

  api
    .post(
      store.server +
        '/api/route/post?auth=jwt&service=cbis&path=/api/user/count',
      {
        id: id,
        username: username,
        nickname: nickname,
        name: name,
        phone: phone,
        isDelete: isDelete,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      }
    )
    .then((resp) => {
      pagination.value!.rowsNumber = parseInt(resp.data.message);
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

const onNew = () => {
  
};

const onShow = (val: {
  id?: string | number;
  username?: string | number;
  name?: string | number;
  head?: string | number;
  phone?: string | number;
  idcard?: string | number;
  wechat?: string | number;
  alipay?: string | number;
  weibo?: string | number;
  email?: string | number;
  last_login?: string | number;
  last_ip?: string | number;
  delete?: string | number | boolean;
  permissions?: string | number;
  options?: string | number;
  create_user?: string | number;
  create_date?: string | number;
  update_user?: string | number;
  update_date?: string | number;
}) => {
  dialog.value.id = val.id != null ? String(val.id) : '';
  dialog.value.username = val.username != null ? String(val.username) : '';
  dialog.value.name = val.name != null ? String(val.name) : '';
  dialog.value.head = val.head != null ? String(val.head) : '';
  dialog.value.phone = val.phone != null ? String(val.phone) : '';
  dialog.value.idcard = val.idcard != null ? String(val.idcard) : '';
  dialog.value.wechat = val.wechat != null ? String(val.wechat) : '';
  dialog.value.alipay = val.alipay != null ? String(val.alipay) : '';
  dialog.value.weibo = val.weibo != null ? String(val.weibo) : '';
  dialog.value.email = val.email != null ? String(val.email) : '';
  dialog.value.last_login = val.last_login != null ? String(val.last_login) : '';
  dialog.value.last_ip = val.last_ip != null ? String(val.last_ip) : '';
  dialog.value.delete = val.delete != null ? String(val.delete) : '';
  dialog.value.permissions = val.permissions != null ? String(val.permissions) : '';
  dialog.value.options = val.options != null ? String(val.options) : '';
  dialog.value.create_user = val.create_user != null ? String(val.create_user) : '';
  dialog.value.create_date = val.create_date != null ? String(val.create_date) : '';
  dialog.value.update_user = val.update_user != null ? String(val.update_user) : '';
  dialog.value.update_date = val.update_date != null ? String(val.update_date) : '';
  dialog.value.show = true;
};

const onPassword = () => {
  store.quasar
    .dialog({
      title: '修改密码',
      message: '请输入新密码',
      prompt: {
        model: '',
        type: 'text',
      },
      cancel: true,
      persistent: true,
    })
    .onOk((data) => {
      const time = setTimeout(() => {
        loading.value = false;
        clearTimeout(time);
      }, 120000);

      loading.value = true;

      api
        .post(
          store.server +
            '/api/route/post?auth=jwt&service=cbis&path=/api/user/password',
          {
            id: dialog.value.id,
            password: data,
            update_user: store.user.id,
            update_name: store.user.name,
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

          if (resp.data.status === 0) {
            store.quasar.notify('修改成功');
            return;
          }

          store.quasar.notify(resp.data.message);
        })
        .catch(() => {
          store.quasar.notify('网络错误，请稍后重试');
          loading.value = false;
          clearTimeout(time);
        });
    })
    .onCancel(() => {
      
    })
    .onDismiss(() => {
      
    });
};

const onUpdate = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  api
    .post(
      store.server +
        '/api/route/post?auth=jwt&service=cbis&path=/api/user/update',
      {
        id: dialog.value.id,
        username: dialog.value.username,
        name: dialog.value.name,
        head: dialog.value.head,
        phone: dialog.value.phone,
        idcard: dialog.value.idcard,
        wechat: dialog.value.wechat,
        alipay: dialog.value.alipay,
        weibo: dialog.value.weibo,
        email: dialog.value.email,
        delete: dialog.value.delete !== 'true' ? true : false,
        permissions: dialog.value.permissions,
        options: dialog.value.options,
        update_user: store.user.id,
        update_name: store.user.name,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      }
    )
    .then((resp) => {
      if (resp.data.status !== 1) {
        store.quasar.notify(resp.data.message);
        loading.value = false;
        clearTimeout(time);
        return;
      }

      store.quasar.notify('修改成功');
      loading.value = false;
      clearTimeout(time);
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      loading.value = false;
      clearTimeout(time);
    });
};
</script>
