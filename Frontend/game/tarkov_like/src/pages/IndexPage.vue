<template>
  <q-page class="row items-center justify-evenly">
    <baidu-map
      class="map"
      :center="center"
      :zoom="zoom"
      :scroll-wheel-zoom="true"
      @ready="handleMapReady"
    >
      <bm-scale anchor="BMAP_ANCHOR_BOTTOM_RIGHT"></bm-scale>
      <bm-geolocation
        anchor="BMAP_ANCHOR_BOTTOM_RIGHT"
        :showAddressBar="true"
        :autoLocation="true"
        @locationSuccess="handleLocationSuccess"
      ></bm-geolocation>
      <bm-marker :position="center" @click="handleMarkerClick"></bm-marker>
    </baidu-map>
  </q-page>
</template>

<style scoped>
.map {
  width: 100%;
  height: calc(100vh - 50px);
}
</style>

<script setup lang="ts">
import type {
  BMapType,
  LocationSuccessEvent,
  MapInstanceType,
  TranslateResult,
} from 'src/components/models';
import { ref } from 'vue';

const center = ref({ lng: 116.404, lat: 39.915 });
const zoom = ref(15);

const bmap = ref<BMapType | null>(null);
const mapInstance = ref<MapInstanceType | null>(null);

const handleMapReady = ({ BMap, map }: { BMap: BMapType; map: MapInstanceType }) => {
  console.log('地图加载完成', BMap, map);
  bmap.value = BMap;
  mapInstance.value = map;
};

const handleMarkerClick = (e: unknown) => {
  console.log('标记点被点击', e);
};

const handleLocationSuccess = (e: LocationSuccessEvent) => {
  console.log('定位成功', e);

  if (bmap.value && mapInstance.value) {
    try {
      const wgsPoint = new bmap.value.Point(e.point.longitude, e.point.latitude);
      const convertor = new bmap.value.Convertor();

      convertor.translate([wgsPoint], 1, 5, (data: TranslateResult) => {
        if (data.status === 0 && data.points.length > 0) {
          const bdPoint = data.points[0];
          center.value = {
            lng: bdPoint!.lng,
            lat: bdPoint!.lat,
          };
          console.log('坐标转换成功:', {
            original: { lng: e.point.longitude, lat: e.point.latitude },
            converted: { lng: bdPoint!.lng, lat: bdPoint!.lat },
          });
        } else {
          console.warn('坐标转换失败，使用原始坐标');
          center.value = {
            lng: e.point.longitude,
            lat: e.point.latitude,
          };
        }
      });
    } catch (error) {
      console.error('坐标转换过程出错:', error);
      center.value = {
        lng: e.point.longitude,
        lat: e.point.latitude,
      };
    }
  } else {
    console.log('百度地图API未加载，使用原始坐标');
    center.value = {
      lng: e.point.longitude,
      lat: e.point.latitude,
    };
  }
};
</script>
