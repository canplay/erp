<template>
  <q-page padding>
    <q-card>
      <q-card-section class="row">
        <q-input filled class="col-9" input-class="text-primary" label="车牌" v-model="plate" />

        <div class="col-auto" style="width: 10px" />

        <q-btn class="col" color="primary" label="查询" @click="onSearch" />
      </q-card-section>
    </q-card>

    <div style="height: 8px" />
  </q-page>
</template>

<script setup lang="ts">
import { useQuasar } from 'quasar'
import { ref } from 'vue'

const quasar = useQuasar()

const plate = ref('')

const onSearch = () => {
  quasar.notify({
    message: '无法获取拖车信息，请稍后重试',
    type: 'negative',
    position: 'top',
  })
}
</script>
