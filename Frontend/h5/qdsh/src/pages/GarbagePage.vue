<template>
  <q-page padding>
    <q-card>
      <q-card-section class="row">
        <q-input filled class="col-9" input-class="text-primary" label="户号" v-model="numbering" />

        <div class="col-auto" style="width: 10px" />

        <q-btn class="col" color="primary" label="查询" @click="onSearch" />
      </q-card-section>
    </q-card>

    <div style="height: 8px" />

    <q-card>
      <q-card-section>
        <div class="text-h6">户号信息</div>

        <div style="height: 12px" />

        <div class="row">
          <div class="col">户号: {{ numbering }}</div>
          <div class="col">应缴金额: {{ money }}元</div>
        </div>

        <div class="row">
          <div class="col">联系人: {{ principal === '' ? name : principal }}</div>
          <div class="col">联系电话: {{ telephone }}</div>
        </div>

        <div>地址: {{ address }}</div>
      </q-card-section>
    </q-card>

    <div style="height: 8px" />

    <q-table
      dense
      title="账单明细"
      selection="multiple"
      :rows="table.rows"
      :columns="table.columns"
      row-key="id"
      v-model:selected="table.selected"
      @selection="onSelect"
      v-model:pagination="table.pagination"
      style="box-shadow: 0 0 5px 0 grey"
    />

    <div style="height: 8px" />

    <q-btn
      class="fit"
      color="negative"
      label="立即缴费"
      :disable="table.selected.length <= 0"
      @click="onPay"
    />

    <q-dialog v-model="dialog.show" class="my-font" persistent>
      <q-card style="width: 80vw">
        <q-card-section>
          <q-select
            label="选择房屋所在的区域"
            class="fit"
            v-model="dialog.zone"
            :options="[
              { label: '示例城市A市', value: 'wsjc' },
              { label: '砚山县', value: 'ysjc' },
            ]"
            style="width: 180px"
          />
        </q-card-section>

        <q-card-actions>
          <q-btn class="fit" label="确定" color="primary" v-close-popup />
        </q-card-actions>
      </q-card>
    </q-dialog>
  </q-page>
</template>

<script setup lang="ts">
import { useStore } from 'src/stores/store'
import { ref } from 'vue'
import { type QTableProps, date, uid, useQuasar } from 'quasar'
import BigNumber from 'bignumber.js'
import { alova } from 'src/boot/axios'
import type { CommonResponse, CommonSession } from 'src/components/models/common'
import type { GarbageItem, GarbageQRCode } from 'src/components/models/garbage'

const quasar = useQuasar()
const store = useStore()

const numbering = ref('')
const money = ref(0)
const principal = ref('')
const name = ref('')
const telephone = ref('')
const address = ref('')

const table = ref({
  rows: <GarbageItem[]>[],
  columns: [
    {
      name: 'billYear',
      label: '账单年',
      align: 'center',
      field: 'bill_year',
    },
    {
      name: 'billMonth',
      label: '账单月',
      align: 'center',
      field: 'bill_month',
    },
    {
      name: 'amount',
      label: '计量',
      align: 'center',
      field: 'amount',
    },
    {
      name: 'price',
      label: '单价',
      align: 'center',
      field: 'price',
    },
    {
      name: 'money',
      label: '应缴金额',
      align: 'center',
      field: 'money',
    },
  ] as QTableProps['columns'],
  selected: <GarbageItem[]>[],
  pagination: {
    sortBy: 'billMonth',
    descending: true,
    rowsPerPage: 0,
  },
})

const dialog = ref({
  show: false,
  zone: { label: '示例城市A市', value: 'wsjc' },
})

interface Selected {
  rows: readonly GarbageItem[]
  keys: readonly number[]
  added: boolean
  evt: Event
}

function onSelect(val: Selected) {
  if (val.added) {
    for (let index = 0; index < val.rows.length; index++) {
      const element = val.rows[index]
      if (element) money.value += BigNumber(element.money).toNumber()
    }
  } else {
    for (let index = 0; index < val.rows.length; index++) {
      const element = val.rows[index]
      if (element) money.value -= BigNumber(element.money).toNumber()
    }
  }

  if (money.value <= 0) {
    money.value = 0
    return
  }
}

const onSearch = async () => {
  numbering.value = numbering.value.toUpperCase()

  if (numbering.value === '' || numbering.value.length < 8) {
    quasar.notify({
      message: '输入户号位数不能小于8位',
      type: 'negative',
      position: 'top',
    })
    return
  }

  if (
    numbering.value.substring(0, 1) != 'J' &&
    numbering.value.substring(0, 1) != 'D' &&
    numbering.value.substring(0, 1) != 'M'
  ) {
    quasar.notify({
      message: '户号格式不正确, 户号以J、D或M开头',
      type: 'negative',
      position: 'top',
    })
    return
  }

  const time = setTimeout(() => {
    quasar.loading.hide()
  }, 30000)

  quasar.loading.show({
    message: '正在处理，请稍后...',
  })

  money.value = 0
  table.value.rows = []
  table.value.selected = []

  const token: CommonSession = quasar.cookies.get('qdsh_session')

  await alova
    .Post<CommonResponse<GarbageItem[] | string>>(
      `${store.server.erp}/post?service=clean&auth=jwt&path=/api/order/legacy/info`,
      {
        numbering: numbering.value,
      },
      {
        headers: {
          Authorization: token.access_token,
        },
      },
    )
    .then((resp) => {
      quasar.loading.hide()
      clearTimeout(time)

      if (resp.message === 'no numbering') {
        quasar.notify({
          message: '户号错误',
          type: 'negative',
          position: 'top',
        })
      } else if (resp.message === 'no data') {
        quasar.notify({
          message: '没有欠费记录',
          type: 'negative',
          position: 'top',
        })
      } else if (resp.status === 0) {
        quasar.notify({
          message: resp.message,
          type: 'negative',
          position: 'top',
        })
      } else {
        const data = resp.data![0] as GarbageItem

        if (data.address.length === 2) {
          address.value = `${data.address.substr(0, 1)}*`
        } else if (data.address.length > 2) {
          let char = ''
          for (let i = 0; i < data.address.length / 2; i++) {
            char += '*'
          }

          address.value =
            data.address.substr(0, 1) +
            char +
            data.address.substr(char.length + 1, data.address.length - char.length)
        } else {
          address.value = data.address
        }

        if (data.name.length === 2) {
          name.value = `${data.name.substr(0, 1)}*`
        } else if (data.name.length > 2) {
          let char = ''
          for (let i = 0; i < data.name.length / 2; i++) {
            char += '*'
          }

          name.value =
            data.name.substr(0, 1) +
            char +
            data.name.substr(char.length + 1, data.name.length - char.length)
        } else {
          name.value = data.name
        }

        if (data.principal.length === 2) {
          principal.value = `${data.principal.substr(0, 1)}*`
        } else if (data.principal.length > 2) {
          let char = ''
          for (let i = 0; i < data.principal.length / 2; i++) {
            char += '*'
          }

          principal.value =
            data.principal.substr(0, 1) +
            char +
            data.principal.substr(char.length + 1, data.principal.length - char.length)
        } else {
          principal.value = data.principal
        }

        if (data.telephone.length === 2) {
          telephone.value = `${data.telephone.substr(0, 1)}*`
        } else if (data.telephone.length > 2) {
          let char = ''
          for (let i = 0; i < data.telephone.length / 2; i++) {
            char += '*'
          }

          telephone.value =
            data.telephone.substr(0, 1) +
            char +
            data.telephone.substr(char.length + 1, data.telephone.length - char.length)
        } else {
          telephone.value = data.telephone
        }

        table.value.rows = resp.data as GarbageItem[]
      }
    })
}

const onPay = async () => {
  const time = setTimeout(() => {
    quasar.loading.hide()
  }, 30000)

  quasar.loading.show({
    message: '正在处理，请稍后...',
  })

  let money = 0
  let billID = '{"id":['
  for (let index = 0; index < table.value.selected.length; index++) {
    const element = table.value.selected[index]

    if (element) {
      money += parseFloat(element.money)
      billID += `"${element.id}"`
      if (index != table.value.selected.length - 1) {
        billID += ','
      }
    }
  }
  billID += ']}'

  await alova
    .Post<CommonResponse<GarbageQRCode>>(
      `${store.server.erp}/post?service=pay&auth=public&path=/api/ums/order`,
      {
        time: date.formatDate(Date.now(), 'YYYY-MM-DD hh:mm:ss'),
        desc: numbering.value,
        no: `31GK${date.formatDate(Date.now(), 'YYYYMMDDhhmmss')}${uid().substring(0, 8)}`,
        no_pay: JSON.parse(billID),
        amount: money,
        return: 'https://qdsh_n.cm.example.com',
        notify: 'https://jcbackend.cm.example.com/api/order/legacy/pay',
        zone: dialog.value.zone.value,
      },
    )
    .then((resp) => {
      if (resp.data!.errMsg === '查询二维码成功') {
        quasar.loading.hide()
        clearTimeout(time)
        window.document.location.href = resp.data!.billQRCode!
      }
    })
}
</script>
