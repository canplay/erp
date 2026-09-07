<template>
  <q-page style="width: 100vw">
    <div
      v-if="route.query.redirect && route.query.redirect != ''"
      class="fullscreen text-center absolute-center"
    >
      <q-img src="icons/favicon-128x128.png" width="128px" />
      <div class="text-h4">七都商合</div>
      <div style="height: 8px" />
      <div class="text-h6 text-negetive">载入中...</div>
    </div>

    <div v-else>
      <q-card
        v-if="news.list.length <= 0"
        class="flex items-center justify-center"
        square
        style="height: 250px"
      >
        <q-spinner color="primary" size="64px" />
      </q-card>

      <q-carousel
        v-else
        v-model="news.model"
        animated
        infinite
        autoplay
        arrows
        transition-prev="slide-right"
        transition-next="slide-left"
        style="height: 250px"
      >
        <q-carousel-slide
          :name="item.id"
          class="flex items-center"
          v-for="item in news.list"
          :key="item.id"
          style="padding: 0"
        >
          <q-img
            class="fit"
            fit="cover"
            :src="item.preview"
            @click="
              item.id === '0'
                ? ''
                : router.push({
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
              background-color: rgba(0, 0, 0, 0.6);
            "
          >
            <div style="font-size: 18px">{{ item.title }}</div>
          </div>
        </q-carousel-slide>
      </q-carousel>

      <div style="height: 8px" />

      <slideShop title="新品上市" :list="shop.new.list" />

      <div style="height: 16px" />

      <menuMid :list="shop.menu" style="margin: 0 8px 0 8px" />

      <div style="height: 8px" />

      <!-- <BaiduMap />

    <div style="height: 8px" /> -->

      <slideShop title="好物优选" :list="shop.recommend.list" />

      <div style="height: 8px" />

      <!-- <q-card class="transparent" style="margin: 0 8px 0 8px">
        <q-carousel
          v-model="slide.text.model"
          animated
          infinite
          autoplay
          transition-prev="slide-right"
          transition-next="slide-left"
          style="height: 50px"
        >
          <q-carousel-slide name="1" class="text-negative"> 滚动链接1 </q-carousel-slide>
          <q-carousel-slide name="2" class="text-secondary"> 滚动链接2 </q-carousel-slide>
          <q-carousel-slide name="3" class="text-accent"> 滚动链接3 </q-carousel-slide>
          <q-carousel-slide name="4" class="text-info"> 滚动链接4 </q-carousel-slide>
        </q-carousel>
      </q-card>

      <div style="height: 8px" /> -->

      <div style="margin: 0 8px 0 8px">
        <FlowInfo />
      </div>
    </div>
  </q-page>

  <div style="height: 8px" />

  <!-- <q-page-sticky position="bottom-right" :offset="[18, 18]">
    <q-btn round color="accent" label="Debug" @click="onDebug" />
  </q-page-sticky> -->
</template>

<script setup lang="ts">
import { onMounted, ref } from 'vue'
import FlowInfo from 'src/components/index/FlowInfo.vue'
import { useStore } from 'src/stores/store'
import { alova } from 'src/boot/axios'
import { useQuasar } from 'quasar'
import { useRoute, useRouter } from 'vue-router'
import menuMid from 'src/components/index/menuMid.vue'
import type { CommonResponse, CommonNews, CommonMenu } from 'src/components/models/common'
import type { ShopIndexItem, ShopResponse } from 'src/components/models/shop'
import slideShop from 'src/components/index/slideShop.vue'
// import { generateSignature, generateNonce, generateTimestamp } from 'src/components/Signature'

const quasar = useQuasar()
const router = useRouter()
const route = useRoute()
const store = useStore()

const news = ref({
  model: '1',
  list: <CommonNews[]>[],
})

const shop = ref({
  menu: <CommonMenu[]>[],
  recommend: {
    model: '1',
    list: <ShopIndexItem[]>[],
  },
  new: {
    model: '1',
    list: <ShopIndexItem[]>[],
  },
})

// const onDebug = () => {
//   window.uniWeb.redirectTo({
//     url: 'shop',
//   });
// };

onMounted(async () => {
  store.page = {
    title: '',
    current: '/',
  }

  if (route.query.method && route.query.method != '') {
    if (route.query.method === 'orderList') {
      await router.push({
        path: '/shop',
        query: {
          method: 'orderList',
        },
      })
    }
  }

  alova
    .Post<CommonResponse<CommonNews[]>>(
      `${store.server.erp}/post?service=cbis&auth=log&path=/api/News/latest`,
      {
        class: 1,
      },
    )
    .then((resp) => {
      if (resp.data && resp.status != 0) {
        resp.data.forEach((element) => {
          if (element.hot) {
            news.value.model = resp.data![0]?.id ?? '1'
            news.value.list = resp.data!
          }
        })
      } else {
        news.value.list.push({
          id: '0',
          title: '暂无新闻',
          content: '',
          date: '',
          hot: false,
          author: '',
          class: 0,
          create_date: '',
          create_user: '',
          preview: '/imgs/noimg.svg',
          update_date: '',
          update_user: '',
        })
      }
    })
    .catch(() => {
      quasar.notify({
        message: '无法获取新闻信息，请稍后重试',
        type: 'negative',
        position: 'top',
      })
    })

  alova
    .Post<ShopResponse<ShopIndexItem>>(
      store.server.shop + '/site/frontJson/getSiteJson',
      {
        siteId: '24305',
      },
      {
        headers: {
          'Content-Type': 'multipart/form-data',
        },
      },
    )
    .then((resp) => {
      if (resp.code != 0) {
        quasar.notify({
          message: '无法获取商城信息',
          type: 'negative',
          position: 'top',
        })
        return
      }

      const shopInfo = JSON.parse(resp.siteFrontJson!.content)
      let shopNew
      let shopRecommend

      console.log(shopInfo)

      for (let i = 0; i < shopInfo.length; i++) {
        const element = shopInfo[i]

        if (element.id === 7) {
          for (let n = 0; n < element.list.length; n++) {
            shop.value.menu.push({
              name: element.list[n].jump.data.name,
              appimg: element.list[n].imageUrl,
              url: element.list[n].jump.data.id,
            })
          }
        }

        if (element.id === 12 && element.jump.data.name === '冬季上新') {
          shopNew = element.list
        }

        if (element.id === 11) {
          shopRecommend = element.list
        }
      }

      alova
        .Post<ShopResponse<ShopIndexItem>>(store.server.shop + '/api/siteProduct/batchInfo', {
          items: shopNew.join(','),
          pxFlag: '',
          siteId: '24305',
        })
        .then((resp) => {
          if (resp.code != 0) {
            quasar.notify({
              message: '无法获取新品上市',
              type: 'negative',
              position: 'top',
            })
            return
          }

          shop.value.new.list = resp.siteProductList!
        })
        .catch(() => {
          quasar.notify({
            message: '无法获取新品上市，请稍后重试',
            type: 'negative',
            position: 'top',
          })
        })

      alova
        .Post<ShopResponse<ShopIndexItem>>(store.server.shop + '/api/siteProduct/batchInfo', {
          items: shopRecommend.join(','),
          pxFlag: '',
          siteId: '24305',
        })
        .then((resp) => {
          if (resp.code != 0) {
            quasar.notify({
              message: '无法获取好物优选',
              type: 'negative',
              position: 'top',
            })
            return
          }

          shop.value.recommend.list = resp.siteProductList!
        })
        .catch(() => {
          quasar.notify({
            message: '无法获取好物优选，请稍后重试',
            type: 'negative',
            position: 'top',
          })
        })
    })
    .catch(() => {
      quasar.notify({
        message: '无法获取好物优选，请稍后重试',
        type: 'negative',
        position: 'top',
      })
    })

  // alova
  //   .Post<ShopResponse<ShopIndexItem>>(
  //     store.server.shop,
  //     {
  //       access_id: '',
  //       language: 'zh',
  //       module: 'app',
  //       action: 'index',
  //       page: 1,
  //       app: 'recommend',
  //     },
  //     {
  //       headers: {
  //         'Content-Type': 'multipart/form-data',
  //       },
  //     },
  //   )
  //   .then((resp) => {
  //     if (resp.code != '200') {
  //       quasar.notify({
  //         message: '无法获取好物优选',
  //         type: 'negative',
  //         position: 'top',
  //       })
  //       return
  //     }

  //     slide.value.shop.recommend.list = resp.data
  //   })
  //   .catch(() => {
  //     quasar.notify({
  //       message: '无法获取好物优选，请稍后重试',
  //       type: 'negative',
  //       position: 'top',
  //     })
  //   })

  // const formData = new FormData()
  // formData.append('access_id', 'user')
  // formData.append('language', 'zh')
  // formData.append('module', 'app')
  // formData.append('action', 'index')
  // formData.append('app', 'index')
  // formData.append('longitude', '112.951227')
  // formData.append('latitude', '28.227965')

  // alova
  //   .Post<ShopResponse<ShopIndex>>(`${store.server.shop}`, formData)
  //   .then((resp) => {
  //     shop.value = resp.data
  //     tab.value = resp.data.list2[0]?.cid.toString() ?? ''
  //   })
  //   .catch((e) => console.log(e))

  // const appid = 'example-appid'
  // const nonce = generateNonce()
  // const timestamp = generateTimestamp()
  // const phone = '13300000001'
  // const amount = 1
  // const start = 1380000000100
  // const end = '1380000000000'

  // const sign = await generateSignature('7c2149b53aa0798c9f778eb487177cd8', {
  //   appid: appid,
  //   nonce: nonce,
  //   timestamp: timestamp,
  //   phone: phone,
  //   amount: amount,
  //   start: start,
  //   end: end,
  // })

  // alova
  //   .Post<CommonResponse<string>>(
  //     `${store.server.erp}/post?service=hik&auth=key&path=/api/hik/coupon/send/car`,
  //     {
  //       appid: appid,
  //       nonce: nonce,
  //       timestamp: timestamp,
  //       phone: phone,
  //       amount: amount,
  //       start: start,
  //       end: end,
  //     },
  //     {
  //       headers: {
  //         Authorization: `Bearer ${sign}`,
  //       },
  //     },
  //   )
  //   .then((resp) => {
  //     console.log(resp)
  //   })
  //   .catch((e) => console.log(e))
})
</script>
