<template>
  <div>
    <q-card class="col">
      <q-card-section class="column">
        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口122监控"
            @click="monitor('入口122')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口120监控"
            @click="monitor('出口120')"
          />
        </div>

        <q-separator class="col-auto" size="5px" spaced="5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口122开闸"
            @click="open('122')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口120开闸"
            @click="open('120')"
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
  name: 'CompXihua',

  setup() {
    const store = useStore();

    const monitor = (val: string) => {
      void api.post(
        store.server +
          '/api/route/post?auth=jwt&service=cbis&path=/api/public_assets/dz_monitor',
        {
          place: '西华公园停车场',
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
          place: '西华公园停车场',
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
