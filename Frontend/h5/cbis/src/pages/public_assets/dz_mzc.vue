<template>
  <div>
    <q-card class="col">
      <q-card-section class="column">
        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口1监控"
            @click="monitor('入口100')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口2监控"
            @click="monitor('出口102')"
          />
        </div>

        <div class="col-auto" style="height: 5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口3监控"
            @click="monitor('入口104')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="入口4监控"
            @click="monitor('入口106')"
          />
        </div>

        <div class="col-auto" style="height: 5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="出口5监控"
            @click="monitor('出口108')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="入口6监控"
            @click="monitor('入口110')"
          />
        </div>

        <div class="col-auto" style="height: 5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="出口7监控"
            @click="monitor('出口112')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口8监控"
            @click="monitor('出口114')"
          />
        </div>

        <div class="col-auto" style="height: 5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口9监控"
            @click="monitor('入口116')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="入口10监控"
            @click="monitor('入口118')"
          />
        </div>

        <div class="col-auto" style="height: 5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="出口11监控"
            @click="monitor('出口120')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口12监控"
            @click="monitor('出口122')"
          />
        </div>

        <div class="col-auto" style="height: 5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口13监控"
            @click="monitor('入口124')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口14监控"
            @click="monitor('出口126')"
          />
        </div>

        <q-separator class="col-auto" size="5px" spaced="5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口1开闸"
            @click="open('100')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口2开闸"
            @click="open('102')"
          />
        </div>

        <div class="col-auto" style="height: 5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口3开闸"
            @click="open('104')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="入口4开闸"
            @click="open('106')"
          />
        </div>

        <div class="col-auto" style="height: 5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="出口5开闸"
            @click="open('108')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="入口6开闸"
            @click="open('110')"
          />
        </div>

        <div class="col-auto" style="height: 5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="出口7开闸"
            @click="open('112')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口8开闸"
            @click="open('114')"
          />
        </div>

        <div class="col-auto" style="height: 5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口9开闸"
            @click="open('116')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="入口10开闸"
            @click="open('118')"
          />
        </div>

        <div class="col-auto" style="height: 5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="出口11开闸"
            @click="open('120')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口12开闸"
            @click="open('122')"
          />
        </div>

        <div class="col-auto" style="height: 5px" />

        <div class="col row">
          <q-btn
            class="col"
            color="primary"
            label="入口13开闸"
            @click="open('124')"
          />

          <div class="col-auto" style="width: 5px" />

          <q-btn
            class="col"
            color="primary"
            label="出口14开闸"
            @click="open('126')"
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
  name: 'CompMzc',

  setup() {
    const store = useStore();

    const monitor = (val: string) => {
      void api.post(
        store.server +
          '/api/route/post?auth=jwt&service=cbis&path=/api/public_assets/dz_monitor',
        {
          place: '示例村停车场',
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
          place: '示例村停车场',
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
