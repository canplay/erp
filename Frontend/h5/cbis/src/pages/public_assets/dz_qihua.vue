<template>
  <div>
    <q-card class="col">
      <q-card-section class="column">
        <div class="col text-center text-warning text-bold text-h5">
          龙海路方向
        </div>

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口监控"
            @click="monitor('示例站点A-龙海路停车场-入口')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口监控"
            @click="monitor('示例站点A-龙海路停车场-出口')"
          />
        </div>

        <div class="col-auto" style="height: 5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口开闸"
            @click="open('示例站点A-龙海路停车场-入口')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口开闸"
            @click="open('示例站点A-龙海路停车场-出口')"
          />
        </div>
      </q-card-section>
    </q-card>

    <q-separator class="col-auto" size="5px" spaced="5px" />

    <q-card class="col bg-secondary">
      <q-card-section class="column">
        <div class="col text-center text-warning text-bold text-h5">
          金石路方向
        </div>

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口监控"
            @click="monitor('示例站点A-金石路-入口')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口监控"
            @click="monitor('示例站点A-金石路-出口')"
          />
        </div>

        <div class="col-auto" style="height: 5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口开闸"
            @click="open('示例站点A-金石路-入口')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口开闸"
            @click="open('示例站点A-金石路-出口')"
          />
        </div>
      </q-card-section>
    </q-card>

    <div class="col-auto" style="height: 5px" />

    <q-card class="col bg-secondary">
      <q-card-section class="column">
        <div class="col text-center text-warning text-bold text-h5">
          七花北路方向
        </div>

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口监控"
            @click="monitor('示例站点A-公厕旁-入口')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口监控"
            @click="monitor('示例站点A-公厕旁-出口')"
          />
        </div>

        <div class="col-auto" style="height: 5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口开闸"
            @click="open('示例站点A-公厕旁-入口')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口开闸"
            @click="open('示例站点A-公厕旁-出口')"
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
  name: 'CompQihua',

  setup() {
    const store = useStore();

    const monitor = (val: string) => {
      void api.post(
        store.server +
          '/api/route/post?auth=jwt&service=cbis&path=/api/public_assets/dz_monitor',
        {
          place: '示例站点A停车场',
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
          place: '示例站点A停车场',
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
