<template>
  <q-page class="bg-grey">
    <q-card square style="padding: 20px">
      <q-card-section>
        <div class="text-h6">通用设置</div>
        <q-separator />
        <q-input
          v-model="store.options.system"
          label="系统刷新时间（秒）"
          outlined
          dense
        />
      </q-card-section>
      <q-separator />
      <q-card-section>
        <div class="text-h6">报警设置</div>
        <q-separator />
        <q-input
          v-model="store.options.alert"
          label="违停超时报警（秒）"
          outlined
          dense
        />
      </q-card-section>
    </q-card>
  </q-page>
</template>

<script setup lang="ts">
import { useStore } from 'src/stores/store';

const store = useStore();
</script>
