<template>
  <q-page>
    <q-tabs align="left" v-model="tab" class="bg-primary text-white">
      <q-tab name="tenement" icon="mail" label="住户" />
      <q-tab name="community" icon="alarm" label="小区" />
      <q-tab name="property" icon="alarm" label="物业" />
      <q-tab name="street" icon="movie" label="街道" />
      <q-tab name="zone" icon="movie" label="区域" />
    </q-tabs>

    <q-tab-panels v-model="tab" animated>
      <q-tab-panel name="tenement" style="padding: 0">
        <TenementTable />
      </q-tab-panel>

      <q-tab-panel name="community" style="padding: 0">
        <CommunityTable />
      </q-tab-panel>

      <q-tab-panel name="property" style="padding: 0">
        <PropertyTable />
      </q-tab-panel>

      <q-tab-panel name="street" style="padding: 0">
        <StreetTable />
      </q-tab-panel>

      <q-tab-panel name="zone" style="padding: 0">
        <ZoneTable />
      </q-tab-panel>
    </q-tab-panels>
  </q-page>
</template>

<script setup lang="ts">
import { ref } from 'vue';
import TenementTable from 'src/components/property/TenementTable.vue';
import CommunityTable from 'src/components/property/CommunityTable.vue';
import PropertyTable from 'src/components/property/PropertyTable.vue';
import StreetTable from 'src/components/property/StreetTable.vue';
import ZoneTable from 'src/components/property/ZoneTable.vue';

const tab = ref('tenement');
</script>
