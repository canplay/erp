<template>
  <div>
    <q-card class="col">
      <q-card-section class="column">
        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口28监控"
            @click="monitor('入口28')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口30监控"
            @click="monitor('出口30')"
          />
        </div>

        <q-separator class="col-auto" size="5px" spaced="5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口28开闸"
            @click="open('28')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口30开闸"
            @click="open('30')"
          />
        </div>
      </q-card-section>
    </q-card>
  </div>
</template>

<script lang="ts">
import { api } from 'src/boot/axios';
import { useStore } from 'stores/store';

export default {
  name: 'CompSixiao',

  setup() {
    const store = useStore();

    const monitor = (val: string) => {
      void api.post(
        store.server +
          '/api/route/post?auth=jwt&service=cbis&path=/api/public_assets/dz_monitor',
        {
          place: '示例学校停车场',
          name: val,
        },
        {
          headers: {
            Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
          },
        }
      );
    };

    const open = (val: string) => {
      void api.post(
        store.server +
          '/api/route/post?auth=jwt&service=cbis&path=/api/public_assets/dz_open',
        {
          place: '示例学校停车场',
          name: val,
        },
        {
          headers: {
            Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
          },
        }
      );
    };

    return {
      monitor,
      open,
    };
  },
};
</script>
