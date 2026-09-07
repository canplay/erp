<template>
  <q-page>
    <q-carousel
      v-model="slide.news.model"
      animated
      infinite
      autoplay
      arrows
      transition-prev="slide-right"
      transition-next="slide-left"
      style="height: 240px"
    >
      <q-carousel-slide
        :name="item.id"
        class="flex items-center"
        v-for="item in slide.news.list"
        :key="item.id"
        style="padding: 0"
      >
        <q-img
          class="fit"
          fit="contain"
          :src="item.preview"
          @click="
            item.id === '0'
              ? ''
              : $router.push({
                  path: '/view',
                  query: { id: item.id },
                })
          "
        />
        <div
          class="absolute-bottom"
          style="
            text-align: center;
            padding: 12px;
            color: white;
            background-color: rgba(0, 0, 0, 0.3);
          "
        >
          <div style="font-size: 18px">{{ item.title }}</div>
        </div>
      </q-carousel-slide>
    </q-carousel>

    <q-card style="margin: 8px 8px 0 8px; background-color: rgba(255, 255, 255, 0.8)">
      <q-card-section>
        <q-skeleton height="170px" square animation="fade" />

        <div class="row items-start no-wrap q-mt-sm">
          <q-skeleton size="56px" square animation="fade" />

          <div class="col q-pl-sm">
            <q-skeleton type="text" square width="30%" animation="fade" />
            <q-skeleton type="text" square height="12px" animation="fade" />
            <q-skeleton type="text" square height="12px" width="75%" animation="fade" />
          </div>
        </div>
      </q-card-section>
    </q-card>

    <q-card style="margin: 8px 8px 0 8px; background-color: rgba(255, 255, 255, 0.8)">
      <q-card-section>
        <q-item style="max-width: 300px">
          <q-item-section avatar>
            <q-skeleton type="QAvatar" />
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
            <q-skeleton type="QAvatar" />
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
            <q-skeleton type="QAvatar" />
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
            <q-skeleton type="QAvatar" />
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
      </q-card-section>
    </q-card>

    <q-card style="margin: 8px 8px 0 8px; background-color: rgba(255, 255, 255, 0.8)">
      <q-card-section>
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
      </q-card-section>
    </q-card>
  </q-page>
</template>

<script setup lang="ts">
import { ref } from 'vue';

const slide = ref({
  news: {
    model: '0',
    list: [
      {
        id: '0',
        title: '1',
        preview: 'https://cdn.quasar.dev/img/quasar-logo-full.svg',
      },
      {
        id: '1',
        title: '2',
        preview: 'https://cdn.quasar.dev/img/quasar-logo-full.svg',
      },
      {
        id: '2',
        title: '3',
        preview: 'https://cdn.quasar.dev/img/quasar-logo-full.svg',
      },
    ],
  },
});
</script>
