<template>
  <q-layout view="hHh lpR fFf" class="my-font bg-grey-2">
    <!-- <BgAnime /> -->

    <q-header class="bg-grey-2" v-if="route.path != '/view' && route.path != '/shop'" elevated>
      <q-toolbar>
        <q-toolbar-title class="row">
          <q-btn class="col-auto" round flat to="/my">
            <q-avatar>
              <q-img
                :src="
                  store.user.avatar === '' || store.user.avatar === null
                    ? 'imgs/noimg.svg'
                    : store.user.avatar
                "
              />
            </q-avatar>
          </q-btn>

          <div class="col-auto" style="width: 8px" />

          <q-input class="col" outlined v-model="search" label="搜索" dense>
            <template v-slot:prepend>
              <q-icon
                v-if="search !== ''"
                name="close"
                @click="search = ''"
                class="cursor-pointer"
              />
              <q-icon name="search" />
            </template>
          </q-input>

          <div class="col-auto" style="width: 8px" />

          <q-btn dense class="col-1" flat round color="grey-8" to="/notify">
            <q-avatar>
              <q-icon name="img:imgs/main/notify.svg" />
            </q-avatar>
            <q-badge
              v-if="store.notify.unread > 0"
              floating
              color="red"
              rounded
              style="margin: 3px 3px 0 0"
            >
              {{ store.notify.unread }}
            </q-badge>
          </q-btn>
        </q-toolbar-title>

        <!-- <q-toolbar-title class="row" v-else>
          <q-btn
            v-if="store.page.return"
            class="col-auto"
            flat
            icon="img:imgs/main/return.svg"
          />
          <div v-else class="col-auto" style="width: 56px" />
          <div
            class="col text-black flex justify-center items-center text-weight-bolder"
          >
            {{ store.page.title }}
          </div>
          <div class="col-auto" style="width: 56px" />
        </q-toolbar-title> -->
      </q-toolbar>
    </q-header>

    <q-drawer side="right" v-model="store.drawer.right" elevated @show="onRightShow">
      <q-card square class="transparent text-black">
        <q-card-section>
          <div class="row">
            <div class="col-auto flex items-center">
              <q-avatar style="width: 48px; height: 48px">
                <q-img src="/imgs/noimg.svg" />
              </q-avatar>
            </div>

            <div class="col-auto" style="width: 8px" />

            <div class="col row">
              <div class="col flex items-center ellipsis-2-lines">
                {{ store.comment.selected.id }}
              </div>
              <div class="col-auto" style="width: 8px" />
              <div class="col-5 text-right ellipsis-2-lines">
                {{ store.comment.selected.create_date }}
              </div>
            </div>
          </div>

          <div>{{ store.comment.selected.content }}</div>
        </q-card-section>
      </q-card>

      <q-separator />

      <div
        style="
          height: calc(100vh - 101px - 56px - 1px);
          overflow-y: auto;
          display: flex;
          flex-direction: column;
        "
        ref="refScroll"
      >
        <q-infinite-scroll reverse :offset="0" @load="onCommentLoad">
          <q-card
            square
            class="text-black"
            v-for="(item, index) in chatCommentDetails"
            :key="index"
          >
            <q-card-section>
              <div class="row">
                <div class="col-auto flex items-center">
                  <q-avatar style="width: 48px; height: 48px">
                    <q-img src="/imgs/noimg.svg" />
                  </q-avatar>
                </div>

                <div class="col-auto" style="width: 8px" />

                <div class="col row">
                  <div class="col flex items-center ellipsis-2-lines">
                    {{ item.id }}
                  </div>
                  <div class="col-auto" style="width: 8px" />
                  <div class="col-5 text-right ellipsis-2-lines">
                    {{ item.create_date }}
                  </div>
                </div>
              </div>

              <div>{{ item.content }}</div>
            </q-card-section>
          </q-card>

          <template v-slot:loading>
            <div class="row justify-center q-my-md">
              <q-spinner-dots color="primary" size="40px" />
            </div>
          </template>
        </q-infinite-scroll>
      </div>

      <q-input label="附言" outlined v-model="postscript">
        <template v-slot:append>
          <q-btn color="primary" label="发送" @click="onSend" />
        </template>
      </q-input>
    </q-drawer>

    <q-page-container>
      <q-scroll-area
        :style="
          route.path != '/view' && route.path != '/shop'
            ? 'width: 100vw; height: calc(100vh - 50px - 62.88px)'
            : 'width: 100vw; height: calc(100vh - 62.88px)'
        "
      >
        <router-view />
      </q-scroll-area>
    </q-page-container>

    <q-footer elevated class="bg-grey-2 text-black">
      <q-toolbar>
        <q-btn
          flat
          stack
          class="fit"
          label="首页"
          :icon="
            store.page.current === '/' ? 'img:imgs/main/index_h.svg' : 'img:imgs/main/index.svg'
          "
          :style="
            store.page.current === '/' ? 'color: #1976d2; font-size: 16px; font-weight: bolder' : ''
          "
          to="/"
        />
        <!-- <q-btn
          flat
          stack
          class="fit"
          label="动态"
          :icon="
            store.page.current === '/discover'
              ? 'img:imgs/main/discover_h.svg'
              : 'img:imgs/main/discover.svg'
          "
          :style="
            store.page.current === '/discover'
              ? 'color: #1976d2; font-size: 16px; font-weight: bolder'
              : ''
          "
          to="/discover"
        /> -->
        <q-btn
          flat
          stack
          class="fit"
          label="圈子"
          icon="img:icons/favicon-128x128.png"
          :style="
            store.page.current == '/chat'
              ? 'color: #1976d2; font-size: 16px; font-weight: bolder'
              : ''
          "
          to="/chat"
        />
        <!-- <q-btn
          flat
          stack
          class="fit"
          label="商城"
          :icon="
            store.page.current === '/shop' ? 'img:imgs/main/shop_h.svg' : 'img:imgs/main/shop.svg'
          "
          :style="
            store.page.current === '/shop'
              ? 'color: #1976d2; font-size: 16px; font-weight: bolder'
              : ''
          "
          to="/shop"
        /> -->
        <q-btn
          flat
          stack
          class="fit"
          label="我的"
          :icon="store.page.current === '/my' ? 'img:imgs/main/my_h.svg' : 'img:imgs/main/my.svg'"
          :style="
            store.page.current === '/my'
              ? 'color: #1976d2; font-size: 16px; font-weight: bolder'
              : ''
          "
          to="/my"
        />
      </q-toolbar>
    </q-footer>
  </q-layout>
</template>

<script setup lang="ts">
import { useStore } from 'src/stores/store'
import { onBeforeUnmount, onMounted, ref } from 'vue'
import { useRoute } from 'vue-router'
// import BgAnime from 'src/components/BgAnime.vue'
import useUtility from 'src/components/utility'
import { alova } from 'src/boot/axios'
import type { CommonResponse, CommonSession, CommonUser } from 'src/components/models/common'
import type { ChatCommentDetail } from 'src/components/models/chat'
import { date, useQuasar } from 'quasar'

const quasar = useQuasar()
const route = useRoute()
const store = useStore()
const utility = useUtility()

const search = ref('')

const refScroll = ref<HTMLElement>()
const chatCommentDetails = ref<ChatCommentDetail[]>([])

const postscript = ref('')

const onRightShow = () => {
  if (store.comment.selected.id === undefined) {
    return
  }

  const time = setTimeout(() => {
    quasar.loading.hide()
  }, 30000)

  quasar.loading.show({
    message: '正在处理，请稍后...',
  })

  alova
    .Get<CommonResponse<number>>(
      `${store.server.erp}/get?service=chat&auth=jwt&path=/api/detail/count/${store.comment.selected.id}`,
      // `http://127.0.0.1:51530/api/detail/count/${store.comment.selected.id}`,
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

      alova
        .Post<CommonResponse<ChatCommentDetail[]>>(
          `${store.server.erp}/post?service=chat&auth=jwt&path=/api/detail/list`,
          // `http://127.0.0.1:51530/api/detail/list`,
          {
            parent: store.comment.selected.id,
            sortBy: 'create_date',
            descending: true,
            curPage: 0,
            maxPage: 10,
          },
        )
        .then((resp) => {
          if (!resp.status) {
            quasar.notify({
              message: resp.message,
              type: 'negative',
              position: 'top',
            })
            return
          }

          chatCommentDetails.value = resp.data!

          refScroll.value!.scrollTop = refScroll.value!.scrollHeight
        })
        .catch(() => {
          quasar.notify({
            message: '网络错误，请稍后重试',
            type: 'negative',
            position: 'top',
          })
        })
    })
    .catch(() => {
      quasar.loading.hide()
      clearTimeout(time)
      quasar.notify({
        message: '请求评论详情失败',
        type: 'negative',
        position: 'top',
      })
    })
}

const onCommentLoad = (index: number, done: (stop?: boolean) => void) => {
  if (store.comment.selected.id === undefined) {
    done(true)
    return
  }

  const time = setTimeout(() => {
    quasar.loading.hide()
  }, 30000)

  quasar.loading.show({
    message: '正在处理，请稍后...',
  })

  alova
    .Get<CommonResponse<number>>(
      `${store.server.erp}/get?service=chat&auth=jwt&path=/api/detail/count/${store.comment.selected.id}`,
      // `http://127.0.0.1:51530/api/detail/count/${store.comment.selected.id}`,
    )
    .then((resp) => {
      quasar.loading.hide()
      clearTimeout(time)

      if (index - 1 >= resp.data!) {
        done(true)
        return
      }

      if (!resp.status) {
        done(true)
        quasar.notify({
          message: resp.message,
          type: 'negative',
          position: 'top',
        })
        return
      }

      alova
        .Post<CommonResponse<ChatCommentDetail[]>>(
          `${store.server.erp}/post?service=chat&auth=jwt&path=/api/detail/list`,
          // `http://127.0.0.1:51530/api/detail/list`,
          {
            parent: store.comment.selected.id,
            sortBy: 'create_date',
            descending: true,
            curPage: index - 1,
            maxPage: 1,
          },
        )
        .then((resp) => {
          if (!resp.status) {
            done(true)
            quasar.notify({
              message: resp.message,
              type: 'negative',
              position: 'top',
            })
            return
          }

          resp.data!.forEach((element) => {
            const token: CommonSession = quasar.cookies.get('qdsh_session')

            alova
              .Get<CommonResponse<CommonUser>>(
                `${store.server.usercenter}/usercenter/user/userinfo?userId=${element.create_user}`,
                {
                  headers: {
                    Authorization: token.access_token,
                  },
                },
              )
              .then((resp) => {
                if (resp === undefined || resp.message != '请求成功') {
                  quasar.loading.hide()
                  clearTimeout(time)
                  quasar.notify({
                    message: '网络错误, 请稍后重试',
                    type: 'negative',
                    position: 'top',
                  })
                  return
                }

                chatCommentDetails.value.push({
                  id: resp.data!.nickName,
                  content: element.content,
                  parent: element.parent,
                  like: element.like,
                  dislike: element.dislike,
                  create_date: element.create_date,
                  create_user: element.create_user,
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
          })

          done()
        })
        .catch(() => {
          done(true)
          quasar.notify({
            message: '网络错误，请稍后重试',
            type: 'negative',
            position: 'top',
          })
        })
    })
    .catch(() => {
      done(true)
      quasar.loading.hide()
      clearTimeout(time)
      quasar.notify({
        message: '请求评论详情失败',
        type: 'negative',
        position: 'top',
      })
    })
}

const onSend = () => {
  const time = setTimeout(() => {
    quasar.loading.hide()
  }, 30000)

  quasar.loading.show({
    message: '正在处理，请稍后...',
  })

  const token: CommonSession = quasar.cookies.get('qdsh_session')

  alova
    .Post<CommonResponse<ChatCommentDetail[]>>(
      `${store.server.erp}/post?service=chat&auth=jwt&path=/api/detail/update`,
      // `http://127.0.0.1:51530/api/detail/update`,
      {
        content: postscript.value,
        parent: store.comment.selected.id,
        create_user: store.user.id,
      },
      {
        headers: {
          Authorization: `Bearer ${token.access_token}`,
        },
      },
    )
    .then((resp) => {
      if (!resp.status) {
        quasar.loading.hide()
        clearTimeout(time)
        quasar.notify({
          message: resp.message,
          type: 'negative',
          position: 'top',
        })
        return
      }

      alova
        .Get<CommonResponse<CommonUser>>(
          `${store.server.usercenter}/usercenter/user/userinfo?userId=${store.user.id}`,
          {
            headers: {
              Authorization: token.access_token,
            },
          },
        )
        .then((resp) => {
          if (resp === undefined || resp.message != '请求成功') {
            quasar.loading.hide()
            clearTimeout(time)
            quasar.notify({
              message: '网络错误, 请稍后重试',
              type: 'negative',
              position: 'top',
            })
            return
          }

          chatCommentDetails.value.push({
            id: store.user.nickName,
            content: postscript.value,
            parent: store.comment.selected.id,
            like: 0,
            dislike: 0,
            create_date: date.formatDate(Date.now(), 'YYYY-MM-DD hh:mm:ss'),
            create_user: store.user.id,
          })

          refScroll.value!.scrollTop = refScroll.value!.scrollHeight
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
}

onMounted(async () => {
  if (utility.isPlatform('mp-weixin') && route.query.method) {
    await store.signin('mp-wechat-callback', route.query.method.toString())
  }

  store.userInfo()
  // await store.nakama_start()
})

onBeforeUnmount(async () => {
  // await store.nakama_close()
})
</script>
