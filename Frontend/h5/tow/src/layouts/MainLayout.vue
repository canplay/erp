<template>
  <q-layout view="hHh lpR fFf" class="bg-cyan-2">
    <q-header
      elevated
      style="background-color: rgba(25, 118, 210, 0.9); backdrop-filter: blur(2px)"
      ref="header"
    >
      <q-toolbar class="row">
        <div class="col row">
          <q-btn class="col-auto" flat no-caps to="index">
            <q-avatar>
              <q-img src="/icons/favicon-32x32.png" />
            </q-avatar>
          </q-btn>

          <q-toolbar-title class="col-auto desktop-only">
            示例城市A市城乡投违章拖运处理平台
          </q-toolbar-title>
        </div>

        <q-btn-group v-if="this.store.user.username != ''" spread flat class="col desktop-only">
          <q-btn label="概览" to="index" />
          <q-btn label="列表" to="car" />
          <q-btn label="统计" to="reports" />
          <q-btn label="设置" to="options" />
        </q-btn-group>

        <div class="col">
          <div v-if="store.user.username != ''" class="row float-right">
            <div class="col-auto flex items-center desktop-only">
              {{ store.schedule.now.time }}
            </div>

            <div class="col-auto" style="width: 10px" />

            <q-btn-dropdown class="col" flat dense icon="person" :label="store.user.username">
              <q-list>
                <q-item clickable v-close-popup @click="onLoginout">
                  <q-item-section>
                    <q-item-label>退出登录</q-item-label>
                  </q-item-section>
                </q-item>
              </q-list>
            </q-btn-dropdown>
          </div>
        </div>
      </q-toolbar>
    </q-header>

    <q-page-container>
      <router-view />
    </q-page-container>
  </q-layout>
</template>

<script>
import { defineComponent } from 'vue'
import { date } from 'quasar'
import { useStore } from 'src/stores/store'

export default defineComponent({
  name: 'MainLayout',

  data() {
    return {
      store: useStore(),
    }
  },

  methods: {
    onLoginout() {
      this.$axios
        .get(this.store.backend + '/loginout')
        .then((resp) => {
          if (resp.data === 'ok') {
            this.$q.cookies.remove('wstc')
            this.store.user = {
              username: '',
              password: '',
              name: '',
              location: '',
            }
            if (this.$route.path != '/') {
              this.$router.push('/')
            }
          } else {
            this.$q.notify('网络错误，请稍后重试')
          }
        })
        .catch(() => {
          this.$q.notify('网络错误，请稍后重试')
        })
    },

    onUpdate() {
      window.location.reload()
    },
  },

  mounted() {
    this.store.schedule.now.time = setInterval(() => {
      this.store.schedule.now.time = date.formatDate(Date.now(), 'dddd YYYY-MM-DD HH:mm:ss')
    }, 1000)
  },

  beforeUnmount() {
    clearInterval(this.store.schedule.now.interval)
    clearInterval(this.store.schedule.system.interval)
    clearInterval(this.store.schedule.alert.interval)
    this.store.schedule.system.list = null
    this.store.schedule.alert.list = null
  },
})
</script>
