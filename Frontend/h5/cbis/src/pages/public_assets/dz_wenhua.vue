<template>
  <div>
    <q-card class="col">
      <q-card-section class="column">
        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口13休闲广场监控"
            @click="monitor('入口13')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口12休闲广场监控"
            @click="monitor('出口12')"
          />
        </div>

        <div class="col-auto" style="height: 5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口14和谐世纪城监控"
            @click="monitor('入口14')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口16和谐世纪城监控"
            @click="monitor('出口16')"
          />
        </div>

        <q-separator class="col-auto" size="5px" spaced="5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口13休闲广场开闸"
            @click="open('13')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口12休闲广场开闸"
            @click="open('12')"
          />
        </div>

        <div class="col-auto" style="height: 5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口14和谐世纪城开闸"
            @click="open('14')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口16和谐世纪城开闸"
            @click="open('16')"
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
  name: 'CompWenhua',

  setup() {
    const store = useStore();

    const monitor = (val: string) => {
      void api.post(
        store.server +
          '/api/route/post?auth=jwt&service=cbis&path=/api/public_assets/dz_monitor',
        {
          place: '示例广场停车场',
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
          place: '示例广场停车场',
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
