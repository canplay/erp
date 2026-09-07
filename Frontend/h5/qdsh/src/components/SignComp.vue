<template>
  <q-card>
    <div class="text-center">
      <q-img src="icons/favicon-128x128.png" width="96px" />
      <div class="text-h5">七都商合</div>
    </div>

    <div style="height: 8px" />

    <q-card-section>
      <q-tabs v-model="tabs" active-color="primary" indicator-color="primary">
        <q-tab name="phone" label="手机" />
        <q-tab name="username" label="用户名" />
      </q-tabs>

      <q-tab-panels class="transparent" v-model="tabs" animated>
        <q-tab-panel name="phone">
          <q-input outlined v-model="inputs.username" label="手机号">
            <template v-slot:append>
              <q-btn
                :disable="verify.btn.disable"
                color="primary"
                :label="verify.btn.label"
                @click="onSendVerify"
              />
            </template>
          </q-input>
          <div style="height: 8px" />
          <q-input outlined v-model="inputs.password" label="验证码" />
        </q-tab-panel>

        <q-tab-panel name="username">
          <q-input v-model="inputs.username" label="用户名" />
          <q-input v-model="inputs.password" label="密码" />
        </q-tab-panel>
      </q-tab-panels>

      <div style="height: 8px" />

      <div class="row">
        <q-checkbox class="col-auto" v-model="checkboxs.licence">
          我已阅读并同意
          <q-btn dense flat color="primary" to="/licence" label="《许可及服务协议》" />
          和
          <q-btn dense flat color="primary" to="/privacy" label="《隐私政策》" />
        </q-checkbox>
      </div>
    </q-card-section>

    <q-card-actions vertical>
      <q-btn class="fit" color="primary" label="登录 / 注册" @click="onSignin('')" />

      <div v-if="utility.isPlatform('mp-weixin')" style="height: 8px" />

      <q-btn class="fit" color="primary" label="微信快捷登录" @click="onSignin('mp-wechat')" />
    </q-card-actions>
  </q-card>
</template>

<script setup lang="ts">
import { useStore } from 'src/stores/store'
import { date, useQuasar } from 'quasar'
import { alova } from 'src/boot/axios'
import { ref } from 'vue'
import useUtility from './utility'
import type { CommonResponse } from './models/common'
import { useLogto } from '@logto/vue'

const quasar = useQuasar()
const store = useStore()
const utility = useUtility()
const logto = useLogto()

const tabs = ref('phone')
const inputs = ref({
  username: '',
  password: '',
})
const checkboxs = ref({
  licence: false,
})
const verify = ref({
  model: '',
  btn: {
    label: '发送',
    disable: false,
  },
})

const onSendVerify = () => {
  if (!checkboxs.value.licence) {
    quasar.notify({
      message: '请勾选我已阅读并同意《隐私政策》和《服务协议》',
      type: 'negative',
      position: 'top',
    })
    return
  }

  if (!utility.isPhone(inputs.value.username)) {
    quasar.notify({
      message: '请输入正确的手机号',
      type: 'negative',
      position: 'top',
    })
    return
  }

  const time = setTimeout(() => {
    quasar.loading.hide()
  }, 30000)

  quasar.loading.show({
    message: '正在处理，请稍后...',
  })

  alova
    .Get<CommonResponse<unknown>>(
      `${store.server.usercenter}/usercenter/login/mobile/sms?mobile=${inputs.value.username}`,
    )
    .then((resp) => {
      quasar.loading.hide()
      clearTimeout(time)

      if (resp.status.toString() === 'success') {
        verify.value.btn.disable = true

        const targetTime = date.addToDate(Date.now(), { seconds: 30 })
        const timer = setInterval(() => {
          const nowTime = Date.now()
          const times = date.getDateDiff(targetTime, nowTime, 'seconds')
          verify.value.btn.label = `发送(${times})`

          if (times <= 0) {
            clearInterval(timer)
            verify.value.btn.disable = false
            verify.value.btn.label = '发送'
          }
        }, 1000)

        quasar.notify({
          message: '验证码发送成功, 请注意查收',
          type: 'info',
          position: 'top',
        })

        return
      }

      quasar.notify({
        message: resp.message,
        type: 'negative',
        position: 'top',
      })
    })
    .catch(() => {
      quasar.loading.hide()
      clearTimeout(time)
      quasar.notify({
        message: '验证码发送失败, 请稍后重试',
        type: 'negative',
        position: 'top',
      })
    })
}

const onSignin = async (type: string) => {
  if (!checkboxs.value.licence) {
    quasar.notify({
      message: '请勾选我已阅读并同意《隐私政策》和《服务协议》',
      type: 'negative',
      position: 'top',
    })
    return
  }

  switch (type) {
    case 'mp-wechat':
      await store.signin('mp-wechat')
      break
    default:
      switch (tabs.value) {
        case 'phone':
          if (!utility.isPhone(inputs.value.username)) {
            quasar.notify({
              message: '请输入正确的手机号',
              type: 'negative',
              position: 'top',
            })
            return
          }

          if (inputs.value.password === '') {
            quasar.notify({
              message: '请填写验证码',
              type: 'negative',
              position: 'top',
            })
            return
          }

          await store.signin('phone', inputs.value.username, inputs.value.password)
          break

        case 'username':
          // if (inputs.value.username === '') {
          //   quasar.notify({
          //     message: '请输入用户名',
          //     type: 'negative',
          //     position: 'top',
          //   })
          //   return
          // }

          // if (inputs.value.password === '') {
          //   quasar.notify({
          //     message: '请输入密码',
          //     type: 'negative',
          //     position: 'top',
          //   })
          //   return
          // }

          await logto.signIn('http://localhost:9000/callback')
          break
      }
      break
  }
}
</script>
