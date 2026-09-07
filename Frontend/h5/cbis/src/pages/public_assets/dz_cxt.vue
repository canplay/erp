<template>
  <div>
    <q-card class="col">
      <q-card-section class="column">
        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口116监控"
            @click="monitor('116')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口125监控"
            @click="monitor('125')"
          />
        </div>

        <q-separator class="col-auto" size="5px" spaced="5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口116开闸"
            @click="open('116')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口125开闸"
            @click="open('125')"
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
  name: 'CompCxt',

  setup() {
    const store = useStore();

    const monitor = (val: string) => {
      void api.post(
        store.server +
          '/api/route/post?auth=jwt&service=cbis&path=/api/public_assets/dz_monitor',
        {
          place: '城乡投停车场',
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
          place: '城乡投停车场',
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
