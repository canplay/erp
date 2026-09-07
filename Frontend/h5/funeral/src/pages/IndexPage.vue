<template>
  <q-page>
    <div style="height: calc(100vh - 50px - 62.88px)" ref="view" />
  </q-page>

  <q-dialog v-model="dialog.show">
    <q-card>
      <q-card-section>
        <q-img :src="dialog.img" style="width: 80vw;" />
        <q-separator size="4px" style="margin: 8px 0 8px 0;" />
        <div class="text-h6">{{ dialog.content }}</div>
      </q-card-section>
    </q-card>
  </q-dialog>
</template>

<script setup lang="ts">
import { useStore } from 'src/stores/store'
import { Viewer, utils } from '@photo-sphere-viewer/core'
import { AutorotatePlugin } from '@photo-sphere-viewer/autorotate-plugin'
import '@photo-sphere-viewer/core/index.css'
import { MarkersPlugin } from '@photo-sphere-viewer/markers-plugin';
import '@photo-sphere-viewer/markers-plugin/index.css'
import { onBeforeUnmount, onMounted, ref } from 'vue'

const store = useStore()

store.page = {
  path: '/',
  title: '首页',
}

const dialog = ref({
  show: false,
  content: '',
  img: ''
})

const view = ref(null)
let viewer: Viewer

const animatedValues = {
  pitch: { start: -Math.PI / 2, end: 0 },
  yaw: { start: Math.PI / 2, end: 0 },
  zoom: { start: 0, end: 50 },
  maxFov: { start: 130, end: 90 },
  fisheye: { start: 2, end: 0 },
}

onMounted(() => {
  if (view.value) {
    viewer = new Viewer({
      container: view.value,
      panorama: 'imgs/default.jpg',
      caption: '罗汉山陵园',
      description: '陵园由示例城市A市民政局管理，是示例城市A市目前主要的公墓',
      loadingTxt: '加载中...',
      defaultPitch: animatedValues.pitch.start,
      defaultYaw: animatedValues.yaw.start,
      defaultZoomLvl: animatedValues.zoom.start,
      maxFov: animatedValues.maxFov.start,
      fisheye: animatedValues.fisheye.start,
      navbar: ['autorotate', 'zoom', 'caption'],
      plugins: [
        [
          AutorotatePlugin,
          {
            autostartDelay: null,
            autostartOnIdle: false,
            autorotatePitch: 0,
          },
        ], [MarkersPlugin, {
          markers: [
            {
              id: '1',
              position: { yaw: '3.767217465519149', pitch: '-0.47701203443478724' },
              image: '/icons/favicon-128x128.png',
              size: { width: 32, height: 32 },
            },
            {
              id: '2',
              position: { yaw: '3.9557439732137496', pitch: '-0.41814138399804257' },
              image: '/icons/favicon-128x128.png',
              size: { width: 32, height: 32 },
            },
          ],
        }],
      ],
    })

    const autorotate: AutorotatePlugin = viewer.getPlugin(AutorotatePlugin)
    let isInit = true

    const intro = (pitch: number, yaw: number) => {
      isInit = false
      autorotate.stop()
      viewer.navbar.hide()

      new utils.Animation({
        properties: {
          ...animatedValues,
          pitch: { start: animatedValues.pitch.start, end: pitch },
          yaw: { start: animatedValues.yaw.start, end: yaw },
        },
        duration: 2500,
        easing: 'inOutQuad',
        onTick: (properties) => {
          viewer.setOptions({
            fisheye: properties.fisheye,
            maxFov: properties.maxFov,
          })
          viewer.rotate({ yaw: properties.yaw, pitch: properties.pitch })
          viewer.zoom(properties.zoom)
        },
      })
        .then(() => {
          autorotate.start()
          viewer.navbar.show()
          viewer.setOptions({
            mousemove: true,
            mousewheel: true,
          })
        })
        .catch((e) => {
          console.log(e)
        })
    }

    viewer.addEventListener(
      'ready',
      () => {
        viewer.navbar.hide()

        setTimeout(() => {
          if (isInit) {
            intro(animatedValues.pitch.end, animatedValues.pitch.end)
          }
        }, 5000)
      },
      { once: true },
    )

    viewer.addEventListener(
      'click',
      (e) => {
        const yaw = e.data.yaw;
        const pitch = e.data.pitch;

        console.log('pos:', { yaw, pitch });
      },
    )

    const markersPlugin: MarkersPlugin = viewer.getPlugin(MarkersPlugin);
    markersPlugin.addEventListener('select-marker', (e) => {
      switch (e.marker.id) {
        case '2':
          dialog.value.img = '/imgs/street_2.jpg'
          dialog.value.content = '示例城市A市民政局公墓管理所'
          dialog.value.show = true
          break;

        default:
          dialog.value.img = '/imgs/street_1.jpg'
          dialog.value.content = '罗汉山陵园入口'
          dialog.value.show = true
          break;
      }
    })
  }
})

onBeforeUnmount(() => {
  if (viewer) {
    viewer.destroy()
  }
})
</script>
