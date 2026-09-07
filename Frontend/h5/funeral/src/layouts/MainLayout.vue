<template>
  <q-layout view="hHh Lpr lFf" class="font-ali">
    <q-header elevated>
      <q-toolbar>
        <q-btn flat dense round icon="menu" aria-label="Menu" />

        <q-toolbar-title>数字祭扫</q-toolbar-title>

        <div>v{{ ver }}</div>
      </q-toolbar>
    </q-header>

    <q-page-container>
      <q-scroll-area :style="route.path != '/view'
        ? 'width: 100vw; height: calc(100vh - 56.01px)'
        : 'width: 100vw; height: calc(100vh)'
        ">
        <router-view />
      </q-scroll-area>
    </q-page-container>

    <q-footer elevated style="background-color: #c9ac8e">
      <q-toolbar>
        <q-btn flat stack class="fit" label="首页" icon="img:imgs/index.svg"
          :style="store.page.path === '/' ? 'color: #1976d2; font-size: 16px; font-weight: bolder' : ''" to="/" />
        <q-btn flat stack class="fit" label="预约" icon="img:imgs/order.svg"
          :style="store.page.path === '/order' ? 'color: #1976d2; font-size: 16px; font-weight: bolder' : ''"
          @click="store.order.show = !store.order.show" />
      </q-toolbar>
    </q-footer>


    <q-dialog v-model="store.order.show" persistent>
      <q-card style="width: 80vw; background-color: #e3dfd3">
        <q-card-section>
          <div class="text-h4 text-center">业务预约</div>

          <q-separator spaced="16px" />

          <q-form @submit="onSubmit" @reset="onReset">
            <q-select outlined v-model="store.order.zone.model" :options="store.order.zone.options" emit-value
              map-options label="陵园" />
            <div style="height: 8px" />
            <q-input outlined v-model="store.order.address" label="门牌号" />
            <div style="height: 8px" />
            <q-select outlined v-model="store.order.type.model" :options="store.order.type.options" emit-value
              map-options label="服务" />
            <div style="height: 8px" />
            <q-input outlined v-model="store.order.name" label="联系人" />
            <div style="height: 8px" />
            <q-input outlined v-model="store.order.phone" label="联系电话" />
            <div style="height: 8px" />
            <q-input outlined v-model="store.order.src" label="信息来源" />
            <div style="height: 8px" />
            <q-input outlined v-model="store.order.remark" label="备注" />
            <div style="height: 8px" />

            <div class="row" style="height: 56px">
              <q-btn outline class="col" type="reset" label="我再想想" />
              <div class="col-auto" style="width: 8px" />
              <q-btn outline class="col text-negative" type="submit" label="立即预约" />
            </div>
          </q-form>
        </q-card-section>
      </q-card>
    </q-dialog>
  </q-layout>
</template>

<script setup lang="ts">
import { useQuasar } from 'quasar'
import { useStore } from 'src/stores/store'
import { useRoute } from 'vue-router'
import { version } from '../../package.json'

const ver = version

const quasar = useQuasar()
const route = useRoute()
const store = useStore()

const onReset = () => {
  store.order = {
    show: false,
    name: '',
    phone: '',
    address: '',
    src: '',
    remark: '',
    zone: {
      model: 1,
      options: store.order.zone.options,
    },
    type: {
      model: 1,
      options: store.order.type.options,
    },
  }
}

const onSubmit = () => {
  quasar.notify('预约成功')
}
</script>
