<template>
  <div
    class="relative-position"
    style="width: 100%; height: calc(100vh - 50px - 56.02px - 7px - 56px)"
  >
    <div class="fit" id="map" />

    <!-- <div id="vditor" /> -->

    <vueDanmaku
      ref="danmukuRef"
      :danmus="danmus"
      autoplay
      loop
      loop-only
      :speeds="50"
      randomChannel
      :zIndex="20"
      style="
        height: calc(100vh - 50px - 56.02px - 7px - 56px);
        position: absolute !important;
        top: 0px;
        pointer-events: none;
      "
    >
      <template v-slot:danmu="{ danmu }">
        <span
          :style="
            'padding: 8px; border-radius: 8px; color: ' +
            danmu.color +
            ';font-size: ' +
            danmu.size +
            'px; background: ' +
            danmu.bg +
            ';'
          "
        >
          {{ danmu.name }}：{{ danmu.text }}
        </span>
      </template>
    </vueDanmaku>

    <q-btn
      class="z-top absolute-bottom-right"
      square
      icon="img:imgs/map/position.png"
      color="white"
      size="20px"
      @click="onLocate"
    />
  </div>

  <q-input outlined label="路牌" placeholder="我来说两句..." v-model="msg">
    <template v-slot:prepend>
      <q-btn color="primary" dense icon="img:imgs/chat/setting.svg" @click="dialog.show = true" />
    </template>
    <template v-slot:append>
      <q-btn
        color="primary"
        label="发送"
        @click="(onSend(store.user.nickName, msg, true), (msg = ''))"
      />
    </template>
  </q-input>

  <q-dialog v-model="dialog.show" class="my-font">
    <q-card style="width: 90%">
      <q-card-section>
        <q-tabs
          v-model="dialog.tab"
          dense
          class="text-grey"
          active-color="primary"
          indicator-color="primary"
          align="justify"
          narrow-indicator
        >
          <q-tab name="my" label="我的设置" />
          <q-tab name="display" label="显示设置" />
        </q-tabs>

        <q-separator />

        <q-tab-panels v-model="dialog.tab" animated>
          <q-tab-panel name="my">
            <div class="row items-center">
              <div class="col-4">弹幕大小</div>
              <q-btn-toggle
                class="col"
                outline
                v-model="dialog.my.size"
                toggle-color="primary"
                :options="[
                  { label: '默认', value: 'default' },
                  { label: '较大', value: 'large' },
                  { label: '较小', value: 'minimal' },
                ]"
              />
            </div>

            <div style="height: 8px" />

            <div class="row items-center">
              <div class="col-4">弹幕颜色</div>
              <q-btn-toggle
                class="col"
                outline
                v-model="dialog.my.color"
                toggle-color="primary"
                :options="[
                  { label: '黑色', value: 'black' },
                  { label: '白色', value: 'white' },
                  { label: '红色', value: 'red' },
                ]"
              />
            </div>

            <div style="height: 8px" />

            <div class="row items-center">
              <div class="col-4">弹幕背景</div>
              <q-btn-toggle
                class="col"
                outline
                v-model="dialog.my.bg"
                toggle-color="primary"
                :options="[
                  { label: '白色', value: 'white' },
                  { label: '黑色', value: 'black' },
                  { label: '红色', value: 'red' },
                ]"
              />
            </div>
          </q-tab-panel>

          <q-tab-panel name="display">
            <div class="row items-center">
              <div class="col-4">弹幕</div>
              <q-btn-toggle
                class="col"
                outline
                v-model="dialog.display.danmaku"
                toggle-color="primary"
                :options="[
                  { label: '显示', value: 'show' },
                  { label: '隐藏', value: 'hide' },
                ]"
              />
            </div>

            <div style="height: 8px" />

            <div class="row items-center">
              <div class="col-4">停车位</div>
              <q-btn-toggle
                class="col"
                outline
                v-model="dialog.display.park"
                toggle-color="primary"
                :options="[
                  { label: '显示', value: 'show' },
                  { label: '隐藏', value: 'hide' },
                ]"
              />
            </div>

            <div style="height: 8px" />

            <div class="row items-center">
              <div class="col-4">闲置资产</div>
              <q-btn-toggle
                class="col fit"
                outline
                v-model="dialog.display.asset"
                toggle-color="primary"
                :options="[
                  { label: '显示', value: 'show' },
                  { label: '隐藏', value: 'hide' },
                ]"
              />
            </div>
          </q-tab-panel>
        </q-tab-panels>
      </q-card-section>
    </q-card>
  </q-dialog>
</template>

<style lang="scss" scoped>
.anchorBL {
  display: none;
}

.BMap_bubble_pop {
  background-color: #f5f5f5 !important;
}
.BMap_bubble_pop .BMap_bubble_top .BMap_bubble_title {
  text-overflow: ellipsis;
  color: #000 !important;
  font-weight: bold;
  font-size: 18px !important;
}
.BMap_bubble_pop .BMap_bubble_center .BMap_bubble_content {
  color: #000 !important;
  font-size: 18px !important;
}

.animeMarker {
  -webkit-animation: heartbeat 1.5s ease-in-out infinite both;
  animation: heartbeat 1.5s ease-in-out infinite both;
  z-index: 20;
}

@-webkit-keyframes heartbeat {
  from {
    -webkit-transform: scale(1);
    transform: scale(1);
    -webkit-transform-origin: center center;
    transform-origin: center center;
    -webkit-animation-timing-function: ease-out;
    animation-timing-function: ease-out;
  }
  10% {
    -webkit-transform: scale(0.91);
    transform: scale(0.91);
    -webkit-animation-timing-function: ease-in;
    animation-timing-function: ease-in;
  }
  17% {
    -webkit-transform: scale(0.98);
    transform: scale(0.98);
    -webkit-animation-timing-function: ease-out;
    animation-timing-function: ease-out;
  }
  33% {
    -webkit-transform: scale(0.87);
    transform: scale(0.87);
    -webkit-animation-timing-function: ease-in;
    animation-timing-function: ease-in;
  }
  45% {
    -webkit-transform: scale(1);
    transform: scale(1);
    -webkit-animation-timing-function: ease-out;
    animation-timing-function: ease-out;
  }
}
@keyframes heartbeat {
  from {
    -webkit-transform: scale(1);
    transform: scale(1);
    -webkit-transform-origin: center center;
    transform-origin: center center;
    -webkit-animation-timing-function: ease-out;
    animation-timing-function: ease-out;
  }
  10% {
    -webkit-transform: scale(0.91);
    transform: scale(0.91);
    -webkit-animation-timing-function: ease-in;
    animation-timing-function: ease-in;
  }
  17% {
    -webkit-transform: scale(0.98);
    transform: scale(0.98);
    -webkit-animation-timing-function: ease-out;
    animation-timing-function: ease-out;
  }
  33% {
    -webkit-transform: scale(0.87);
    transform: scale(0.87);
    -webkit-animation-timing-function: ease-in;
    animation-timing-function: ease-in;
  }
  45% {
    -webkit-transform: scale(1);
    transform: scale(1);
    -webkit-animation-timing-function: ease-out;
    animation-timing-function: ease-out;
  }
}

.infoBox {
  background-color: #fff;
  border-radius: 4px;
  box-shadow: 4px 4px 4px #cccccc;
  padding: 8px;
}
.infoBoxContent {
  margin: 0px;
  word-break: keep-all;
  font-size: 24px;
}
.infoBoxContent:before {
  content: '';
  border: 10px solid transparent;
  border-top-color: #fff;
  position: absolute;
  left: calc(50% + 10px);
  top: 100%;
  margin-left: -20px;
}
</style>

<script setup lang="ts">
import { onBeforeUnmount, onMounted, ref } from 'vue'
import useHikParking from 'src/components/index/HikParking'
import { date, useQuasar } from 'quasar'
import { useStore } from 'src/stores/store'
import vueDanmaku, { type Danmu } from 'vue-danmaku'
import type { ChatComment } from 'src/components/models/chat'
// import Vditor from 'vditor'
// import 'vditor/dist/index.css'
import { alova } from 'src/boot/axios'
import type { CommonResponse, CommonSession, CommonUser } from 'src/components/models/common'
import AMapLoader from '@amap/amap-jsapi-loader'

const quasar = useQuasar()
const store = useStore()

const msg = ref('')
const danmukuRef = ref<typeof vueDanmaku>()
const danmus = ref<Danmu>([])

// const vditor = ref<Vditor | null>(null)

const dialog = ref({
  show: false,
  tab: 'my',
  my: {
    size: 'default',
    color: 'black',
    bg: 'white',
    flag: {
      border: '',
      img: '',
    },
    detail: {
      bg: '',
    },
  },
  display: {
    danmaku: 'show',
    park: 'show',
    asset: 'show',
  },
})

const map = ref<AMap.Map>()
const location = ref({
  locate: {
    lng: 0,
    lat: 0,
  },
  marker: <BMapGL.CustomHtmlLayer>{},
  label: <BMapGL.Label>{},
  info: '',
})

// const bd_encrypt = (lng: number, lat: number) => {
//   const X_PI = (Math.PI * 3000.0) / 180.0
//   const x = lng
//   const y = lat
//   const z = Math.sqrt(x * x + y * y) + 0.00002 * Math.sin(y * X_PI)
//   const theta = Math.atan2(y, x) + 0.000003 * Math.cos(x * X_PI)
//   const bd_lng = z * Math.cos(theta) + 0.0065
//   const bd_lat = z * Math.sin(theta) + 0.006
//   return {
//     lat: bd_lat,
//     lng: bd_lng,
//   }
// }

const setMarker = (
  id: string,
  icon: string,
  point: { lng: number; lat: number },
  label: string,
  type?: string,
  // info?: { title: string; content: string },
) => {
  const p = new BMapGL.Point(point.lng, point.lat)

  const myIcon = () => {
    // const img = document.createElement('img')
    // img.style.height = '64px'
    // img.style.width = '64px'
    // img.src = icon
    // img.className = 'animeMarker'
    // img.draggable = false
    // return img
    const container = document.createElement('div')
    container.style.cssText = `
          width: 64px;
          height: 64px;
          border: 4px solid #fff;
          border-radius: 50%;
          overflow: hidden;
          display: flex;
          align-items: center;
          justify-content: center;
          background: #fff;
          z-index: 99;
        `

    const img = document.createElement('img')
    img.style.cssText = `
          width: 100%;
          height: 100%;
          object-fit: cover;
          z-index: 99;
        `
    img.src = icon
    img.draggable = false

    container.appendChild(img)
    return container
  }

  const parkIcon = () => {
    const img = document.createElement('img')
    img.style.cssText = `
          width: 32px;
          height: 32px;
        `
    img.src = icon
    img.draggable = false
    return img
  }

  let marker
  if (type === 'my' || type === 'comment') {
    marker = new BMapGL.CustomHtmlLayer(myIcon, {
      point: p,
    })
  } else {
    marker = new BMapGL.CustomHtmlLayer(parkIcon, {
      point: p,
    })
  }

  marker.setData({
    type: 'FeatureCollection',
    features: [
      {
        type: 'Feature',
        geometry: {
          type: 'Point',
          coordinates: [point.lng, point.lat],
        },
      },
    ],
  })

  // if (map.value && (type === 'my' || type === 'comment')) {
  //   if (location.value.marker != null) map.value.removeOverlay(location.value.marker)

  //   location.value.marker = marker
  //   map.value.addCustomHtmlLayer(location.value.marker)

  //   location.value.marker.addEventListener('click', () => {
  //     if (!map.value) return

  //     if (type === 'comment') {
  //       const commnet = store.comment.list.find((item) => item.id === id)
  //       if (commnet) store.comment.selected = commnet
  //       else store.comment.selected = <ChatComment>{}

  //       store.drawer.right = !store.drawer.right
  //     }
  //   })
  // } else if (map.value) {
  //   if (marker != null) map.value.removeOverlay(marker)

  //   map.value.addCustomHtmlLayer(marker)
  // }

  // if (label != '') {
  //   const labelWidget = new BMapGL.Label(label, {
  //     position: p,
  //     offset: new BMapGL.Size(0, -65),
  //   })

  //   if (map.value && (type === 'my' || type === 'comment')) {
  //     if (location.value.label != null) map.value.removeOverlay(location.value.label)
  //     location.value.label = labelWidget
  //     map.value.addOverlay(location.value.label)
  //   }

  //   labelWidget.setStyle({
  //     color: '#fff',
  //     backgroundColor: 'rgba(0, 0, 0, 0.5)',
  //     borderRadius: '10px',
  //     padding: '0 10px',
  //     fontSize: '18px',
  //     lineHeight: '24px',
  //     border: '0',
  //     transform: 'translateX(-50%)',
  //   })
  // }

  // if (info != undefined) {
  //   const infoWidget = new BMapGL.InfoWindow(info.content, {
  //     width: 300,
  //     title: info.title,
  //   })
  //   infoWidget.setMaxContent(info.content)
  //   infoWidget.addEventListener('open', () => {
  //     const btn = document.getElementById('preview')
  //     btn?.addEventListener('touchend', (e) => {
  //       if (!e.target) return

  //       const target = e.target as HTMLAnchorElement
  //       if (target && target.href) {
  //         window.location.href = target.href
  //       }
  //     })
  //   })

  //   marker.addEventListener('click', () => {
  //     if (!map.value) return
  //     map.value.openInfoWindow(infoWidget, p)
  //   })
  // }
}

const onMessage = (e: MessageEvent) => {
  if (e.data.channel == 'location') {
    location.value.locate = {
      lng: e.data.location.longitude === null ? '104.0000' : e.data.location.longitude,
      lat: e.data.location.latitude === null ? '23.0000' : e.data.location.latitude,
    }
    setMarker('', store.user.avatar, location.value.locate, '我的位置', 'my')

    if (map.value) {
      // map.value.centerAndZoom(location.value.locate, 17)
      // map.value.panTo(location.value.locate)
    }
  }
}

const onLocate = () => {
  quasar.notify({
    message: '正在获取实时位置...',
    type: 'info',
    position: 'top',
  })

  // const geolocation = new AMap.Geolocation()
  // geolocation.getCurrentPosition((r) => {
  //   if (geolocation.getStatus() === 0) {
  //     location.value.locate = { lng: r.point.lng, lat: r.point.lat }
  //     setMarker('', store.user.avatar, location.value.locate, '我的位置', 'my')

  //     if (map.value) {
  // map.value.centerAndZoom(r.point, 17)
  // map.value.panTo(r.point)
  //     }
  //   } else {
  //     location.value.locate = { lng: 104.0000, lat: 23.0000 }
  //   }
  // })
}

const onSend = (name: string, msg: string, save?: boolean) => {
  if (danmukuRef.value) {
    let size = 24
    let color = '#000'
    let bg = '#fff'

    switch (dialog.value.my.size) {
      case 'default':
        size = 24
        break
      case 'large':
        size = 32
        break
      case 'minimal':
        size = 18
        break
    }

    switch (dialog.value.my.color) {
      case 'black':
        color = '#000'
        break
      case 'white':
        color = '#fff'
        break
      case 'red':
        color = '#f00'
        break
    }

    switch (dialog.value.my.bg) {
      case 'black':
        bg = '#000'
        break
      case 'white':
        bg = '#fff'
        break
      case 'red':
        bg = '#f00'
        break
    }

    if (save) {
      const time = setTimeout(() => {
        quasar.loading.hide()
      }, 30000)

      quasar.loading.show({
        message: '正在处理，请稍后...',
      })

      const token: CommonSession = quasar.cookies.get('qdsh_session')

      alova
        .Post<CommonResponse<ChatComment[]>>(
          `${store.server.erp}/post?service=chat&auth=jwt&path=/api/comments/update`,
          // `http://127.0.0.1:51530/api/comments/update`,
          {
            content: msg,
            create_user: store.user.id,
            update_user: store.user.id,
            lng: location.value.locate.lng,
            lat: location.value.locate.lat,
            end_date: date.formatDate(
              date.addToDate(Date.now(), { days: 3 }),
              'YYYY-MM-DD hh:mm:ss',
            ),
          },
          {
            headers: {
              Authorization: `Bearer ${token.access_token}`,
            },
          },
        )
        .then((resp) => {
          quasar.loading.hide()
          clearTimeout(time)

          if (!resp.status) {
            quasar.notify({
              message: resp.message,
              type: 'negative',
              position: 'top',
            })
            return
          }

          danmukuRef.value!.addDanmu({
            name: name,
            text: msg,
            color: color,
            size: size,
            bg: bg,
          })
        })
        .catch(() => {
          quasar.loading.hide()
          clearTimeout(time)
          quasar.notify({
            message: '发送路牌失败',
            type: 'negative',
            position: 'top',
          })
        })
    } else {
      danmukuRef.value.addDanmu({
        name: name,
        text: msg,
        color: color,
        size: size,
        bg: bg,
      })
    }
  }
}

onMounted(async () => {
  store.page = {
    title: '',
    current: '/chat',
  }

  // vditor.value = new Vditor('vditor', {
  //   after: () => {
  //     vditor.value!.setValue('Vue Composition API + Vditor + TypeScript Minimal Example')
  //   },
  // })

  try {
    window._AMapSecurityConfig = {
      securityJsCode: 'YOUR_AMAP_SECURITY_CODE',
    }
    AMapLoader.load({
      key: 'YOUR_AMAP_KEY',
      version: '2.0',
    })
      .then((AMap) => {
        map.value = new AMap.Map('map', {
          center: [104.0000, 23.0000],
          zoom: 17,
        })

        AMap.plugin('AMap.Scale', function () {
          const scale = new AMap.Scale()
          map.value?.addControl(scale)
        })

        AMap.plugin('AMap.ControlBar', function () {
          const controlbar = new AMap.ControlBar()
          map.value?.addControl(controlbar)
        })

        AMap.plugin('AMap.MapType', function () {
          const maptype = new AMap.MapType()
          map.value?.addControl(maptype)
        })

        AMap.plugin('AMap.Driving', function () {
          const driving = new AMap.Driving()
          map.value?.addControl(driving)
        })

        AMap.plugin('AMap.Geolocation', function () {
          const geolocation: AMap.Geolocation = new AMap.Geolocation()
          geolocation.getCurrentPosition((status, result) => {
            if (status === 'complete') {
              location.value.locate = { lng: result.position.lng!, lat: result.position.lat! }

              if (map.value) {
                map.value.setZoomAndCenter(17, location.value.locate, false)
                map.value.panTo(location.value.locate)
              }
            } else {
              location.value.locate = { lng: 104.0000, lat: 23.0000 }
            }
          })
        })
      })
      .catch((e) => {
        console.log(e)
      })

    // map.value = new BMapGL.Map('map', {
    //   enableMapClick: false,
    //   enableIconClick: false,
    //   showControls: true,
    // })

    // map.value.centerAndZoom(new BMapGL.Point(104.0000, 23.0000), 17)
    // map.value.panTo(new BMapGL.Point(104.0000, 23.0000))
    // map.value.enableScrollWheelZoom()

    window.addEventListener('message', onMessage)

    store.comment.list = []

    await alova
      .Get<CommonResponse<ChatComment[]>>(
        `${store.server.erp}/get?service=chat&auth=jwt&path=/api/comments/latest`,
        // `http://127.0.0.1:51530/api/comments/latest`,
      )
      .then((resp) => {
        if (resp.data === null || resp.data === undefined) return

        for (let index = 0; index < resp.data.length; index++) {
          const element = resp.data[index]

          const token: CommonSession = quasar.cookies.get('qdsh_session')

          alova
            .Get<CommonResponse<CommonUser>>(
              `${store.server.usercenter}/usercenter/user/userinfo?userId=${element!.create_user}`,
              {
                headers: {
                  Authorization: token.access_token,
                },
              },
            )
            .then((resp) => {
              if (resp === undefined || resp.message != '请求成功') {
                quasar.notify({
                  message: '网络错误, 请稍后重试',
                  type: 'negative',
                  position: 'top',
                })
                return
              }

              store.comment.list.push({
                id: resp.data!.nickName,
                content: element!.content,
                like: element!.like,
                dislike: element!.dislike,
                lng: element!.lng,
                lat: element!.lat,
                end_date: element!.end_date,
                create_date: element!.create_date,
                create_user: element!.create_user,
              })

              // setMarker(
              //   element!.id,
              //   'imgs/map/position.svg',
              //   {
              //     lng: parseFloat(element!.lng),
              //     lat: parseFloat(element!.lat),
              //   },
              //   resp.data!.nickName,
              //   'comment',
              // )

              danmukuRef.value!.addDanmu({
                name: resp.data!.nickName,
                text: element!.content,
                color: '#000',
                size: 24,
                bg: '#fff',
              })
            })
            .catch(() => {
              quasar.notify({
                message: '获取路牌失败',
                type: 'negative',
                position: 'top',
              })
            })
        }
      })

    const getRandomDelay = () => Math.floor(Math.random() * (5000 - 1000 + 1)) + 1000
    for (let index = 0; index < store.comment.list.length; index++) {
      const element = store.comment.list[index]

      if (element) {
        const delay = getRandomDelay()
        await new Promise((resolve) => setTimeout(resolve, delay))
        onSend(element.create_user, element.content)
      }
    }

    await useHikParking()
      .park_infos(1, 1)
      .then(async (resp) => {
        if (!resp) return

        let curPage = 1

        for (let index = 0; index < Math.ceil(resp.data.totalRecord! / 100); index++) {
          await useHikParking()
            .park_infos(curPage, 100)
            .then((resp) => {
              if (!resp) return

              for (let index = 0; index < resp.data.results!.length; index++) {
                const element = resp.data.results![index]

                if (
                  element &&
                  !element.parkName.includes('包期') &&
                  !element.parkName.includes('取消') &&
                  !element.parkName.includes('旧')
                ) {
                  // setMarker(
                  //   '',
                  //   'imgs/map/parking.svg',
                  //   bd_encrypt(parseFloat(element.parkLongitude), parseFloat(element.parkLatitude)),
                  //   '',
                  //   '',
                  //   {
                  //     title: element.parkName,
                  //     content: `<div style="color: green">剩余车位: ${element.leftParkingSpaceNum}</div>
                  //     <div style="color: red">总车位: ${element.totalParkingSpaceNum}</div>`,
                  //   },
                  // )
                }
              }
            })

          curPage += 100
        }
      })
  } catch (e) {
    console.log(e)
  }
})

onBeforeUnmount(() => {
  window.removeEventListener('message', onMessage)
  map.value?.destroy()
})
</script>
