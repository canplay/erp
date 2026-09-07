<template>
  <q-dialog v-model="model">
    <q-card style="width: 90vw; height: 90vh">
      <q-card-section>
        <div class="text-h6 text-center text-bold">预约历史</div>
      </q-card-section>

      <q-card-section style="height: calc(100% - 64px)">
        <q-scroll-area class="fit">
          <q-item style="max-width: 300px">
            <q-item-section avatar>
              <q-skeleton size="56px" square />
            </q-item-section>

            <q-item-section>
              <q-item-label>
                <q-skeleton type="text" />
              </q-item-label>
              <q-item-label caption>
                <q-skeleton type="text" width="65%" />
              </q-item-label>
            </q-item-section>
          </q-item>

          <q-item style="max-width: 300px">
            <q-item-section avatar>
              <q-skeleton size="56px" square />
            </q-item-section>

            <q-item-section>
              <q-item-label>
                <q-skeleton type="text" />
              </q-item-label>
              <q-item-label caption>
                <q-skeleton type="text" width="65%" />
              </q-item-label>
            </q-item-section>
          </q-item>

          <q-item style="max-width: 300px">
            <q-item-section avatar>
              <q-skeleton size="56px" square />
            </q-item-section>

            <q-item-section>
              <q-item-label>
                <q-skeleton type="text" />
              </q-item-label>
              <q-item-label caption>
                <q-skeleton type="text" width="65%" />
              </q-item-label>
            </q-item-section>
          </q-item>

          <q-item style="max-width: 300px">
            <q-item-section avatar>
              <q-skeleton size="56px" square />
            </q-item-section>

            <q-item-section>
              <q-item-label>
                <q-skeleton type="text" />
              </q-item-label>
              <q-item-label caption>
                <q-skeleton type="text" width="65%" />
              </q-item-label>
            </q-item-section>
          </q-item>
        </q-scroll-area>
      </q-card-section>
    </q-card>
  </q-dialog>
</template>

<script setup lang="ts">
const model = defineModel<boolean>('show', { required: true });
</script>
