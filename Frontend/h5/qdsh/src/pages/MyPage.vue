<template>
  <q-page>
    <div v-if="store.user.signed">
      <q-card class="row" square>
        <q-item class="col">
          <q-item-section avatar>
            <q-avatar size="96px">
              <q-img
                :src="
                  store.user.avatar === '' || store.user.avatar === null
                    ? 'imgs/noimg.svg'
                    : store.user.avatar
                "
              />
            </q-avatar>
          </q-item-section>

          <q-item-section>
            <q-item-label class="text-h5">
              {{
                store.user.nickName === '' || store.user.nickName === null
                  ? '未设置昵称'
                  : store.user.nickName
              }}
            </q-item-label>
            <q-item-label style="font-size: 14px">
              {{
                store.user.remark === '' || store.user.remark === null
                  ? '未设置个性签名'
                  : store.user.remark
              }}
            </q-item-label>
            <q-item-label class="text-red row" style="font-size: 14px">
              <div
                class="col-auto"
                style="border-style: solid; border-width: 1px; border-color: red"
              >
                &nbsp; 普通用户 &nbsp;
              </div>
            </q-item-label>
            <q-item-label class="row" style="font-size: 14px">
              <div class="col">余额: 0.0</div>
              <div class="col">积分: 0.0</div>
            </q-item-label>
          </q-item-section>
        </q-item>

        <q-space />

        <q-btn class="col-2 text-h6" flat round icon="img:imgs/my/options.svg" />
      </q-card>

      <div style="height: 8px" />

      <q-card square class="row">
        <q-btn class="col" flat size="18px" stack>
          <div>0</div>
          <div>动态</div>
        </q-btn>

        <q-separator vertical inset />

        <q-btn class="col" flat size="18px" stack>
          <div>0</div>
          <div>关注</div>
        </q-btn>

        <q-separator vertical inset />

        <q-btn class="col" flat size="18px" stack>
          <div>0</div>
          <div>粉丝</div>
        </q-btn>
      </q-card>

      <div style="height: 8px" />

      <q-banner rounded inline-actions class="text-white bg-red" style="margin: 0 8px 0 8px">
        开通会员享受超多权益
        <template v-slot:action>
          <q-btn flat color="white" label="立即开通 >" />
        </template>
      </q-banner>

      <div style="height: 8px" />

      <q-card square>
        <q-card-section class="text-h6 text-center row">
          <div class="col flex items-center">
            <div style="border-top: 1px solid #000; width: 100%" />
          </div>
          <div class="col">订单中心</div>
          <div class="col flex items-center">
            <div style="border-top: 1px solid #000; width: 100%" />
          </div>
        </q-card-section>
        <div class="row">
          <q-btn
            class="col"
            flat
            stack
            icon="img:imgs/my/wait-pay.svg"
            label="待付款"
            size="12px"
          />
          <div class="col-auto" style="width: 8px" />
          <q-btn
            class="col"
            flat
            stack
            icon="img:imgs/my/wait-sent.svg"
            label="待发货"
            size="12px"
          />
          <div class="col-auto" style="width: 8px" />
          <q-btn
            class="col"
            flat
            stack
            icon="img:imgs/my/wait-receive.svg"
            label="待收货"
            size="12px"
          />
          <div class="col-auto" style="width: 8px" />
          <q-btn
            class="col"
            flat
            stack
            icon="img:imgs/my/wait-reply.svg"
            label="待评价"
            size="12px"
          />
          <div class="col-auto" style="width: 8px" />
          <q-btn class="col" flat stack icon="img:imgs/my/refund.svg" label="退款" size="12px" />
        </div>
      </q-card>

      <div style="height: 8px" />

      <q-card square>
        <q-card-section class="text-h6 text-center row">
          <div class="col flex items-center">
            <div style="border-top: 1px solid #000; width: 100%" />
          </div>
          <div class="col">我的服务</div>
          <div class="col flex items-center">
            <div style="border-top: 1px solid #000; width: 100%" />
          </div>
        </q-card-section>
        <div class="row">
          <q-btn
            class="col"
            flat
            stack
            icon="img:imgs/my/assets.svg"
            label="资产管理"
            size="13px"
          />
          <div class="col-auto" style="width: 8px" />
          <q-btn
            class="col"
            flat
            stack
            icon="img:imgs/my/member.svg"
            label="会员中心"
            size="13px"
          />
          <div class="col-auto" style="width: 8px" />
          <q-btn
            class="col"
            flat
            stack
            icon="img:imgs/my/integral.svg"
            label="积分中心"
            size="13px"
          />
          <div class="col-auto" style="width: 8px" />
          <q-btn class="col" flat stack icon="img:imgs/my/coupon.svg" label="优惠券" size="13px" />
        </div>

        <div style="height: 8px" />

        <div class="row">
          <q-btn
            class="col"
            flat
            stack
            icon="img:imgs/my/history-expense.svg"
            label="消费记录"
            size="13px"
          />
          <div class="col-auto" style="width: 8px" />
          <q-btn
            class="col"
            flat
            stack
            icon="img:imgs/my/customer.svg"
            label="联系客服"
            size="13px"
          />
          <div class="col-auto" style="width: 8px" />
          <q-btn class="col" flat stack icon="img:imgs/my/aging.svg" label="操作简化" size="13px" />
          <div class="col-auto" style="width: 8px" />
          <q-btn class="col" flat stack icon="img:imgs/my/language.svg" label="Lang" size="13px" />
        </div>
      </q-card>

      <div style="height: 8px" />

      <q-btn square class="fit" label="退出登录" color="negative" @click="signout" />

      <div style="height: 8px" />
    </div>

    <div v-else class="absolute-center" style="width: 90vw">
      <SignComp />
    </div>
  </q-page>
</template>

<script setup lang="ts">
import { useStore } from 'src/stores/store'
import { onMounted } from 'vue'
import SignComp from 'src/components/SignComp.vue'
import { useQuasar } from 'quasar'
import { useLogto } from '@logto/vue'

const $q = useQuasar()
const store = useStore()
const logto = useLogto()

const signout = async () => {
  if ($q.cookies.has('_logto')) {
    await logto.signOut('http://localhost:9000/callback')
  } else {
    store.signout()
  }
}

onMounted(() => {
  store.page = {
    title: '我的',
    current: '/my',
  }
})
</script>
