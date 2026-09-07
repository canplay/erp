<template>
  <q-page padding>
    <div v-if="!isAuthenticated" class="fixed-center">
      <p v-if="isLoading" class="fit text-h3 text-bold text-center">正在跳转...</p>
      <q-card v-else style="width: 80vw">
        <q-card-section class="text-h6 text-center">七都置业</q-card-section>

        <q-separator />

        <q-card-section>
          <q-btn label="立即注册/登录" color="primary" class="fit" @click="onSignin" />
        </q-card-section>
      </q-card>
    </div>
    <div v-else>
      <q-card>
        <q-card-section class="text-h6">个人信息</q-card-section>

        <q-separator />

        <q-card-section>123</q-card-section>
      </q-card>

      <div style="height: 8px" />

      <q-card>
        <q-card-section class="text-h6">预约历史</q-card-section>

        <q-separator />

        <q-card-section>
          <q-list>
            <q-item>
              <q-item-section>
                <q-item-label>预约时间</q-item-label>
                <q-item-label caption>预约地点</q-item-label>
              </q-item-section>
              <q-item-section side>
                <q-item-label>预约状态</q-item-label>
              </q-item-section>
            </q-item>
          </q-list>
        </q-card-section>
      </q-card>
    </div>
  </q-page>
</template>

<script setup lang="ts">
import { useStore } from 'src/stores/store'
import { onMounted } from 'vue'
import { useLogto, useHandleSignInCallback } from '@logto/vue'
import { useQuasar } from 'quasar'

const quasar = useQuasar()
const store = useStore()

const { isAuthenticated } = useLogto()
const { signIn, getIdTokenClaims } = useLogto()
const { isLoading } = useHandleSignInCallback(() => {
  ;(async () => {
    const token = quasar.localStorage.getItem(`logto:${store.logto.clientId}:idToken`)
    quasar.cookies.set('qdsh_session', JSON.stringify(token).replace(/"/g, ''))

    const claims = await getIdTokenClaims()
    store.user = {
      id: claims!.sub,
      nickname: claims?.name,
      username: claims?.username,
      email: claims?.email,
      avatar: claims?.picture,
      qq: '',
      wechat: '',
      phone: claims?.phone_number,
      orgs: claims?.organizations,
      roles: claims?.roles,
      orgRoles: claims?.organization_roles,
      permission: [],
    }
  })().catch((e) => {
    console.log(e)
  })
})

const onSignin = async () => {
  await signIn(store.server.self)
}

onMounted(() => {
  store.page = {
    path: '/my',
    title: '我的',
  }
})
</script>
