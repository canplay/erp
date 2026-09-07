<template>
  <q-page class="flex flex-center">
    <q-card style="min-width: 250px">
      <q-card-section>
        <q-form @submit="onSubmit" @reset="onReset" class="q-gutter-md">
          <q-input
            v-model="username"
            label="用户名"
            lazy-rules
            :rules="[(val) => (val && val.length > 0) || '此为必填项']"
          />

          <q-input
            v-model="password"
            type="password"
            label="密码"
            lazy-rules
            :rules="[(val) => (val && val.length > 0) || '此为必填项']"
          />

          <div class="row">
            <q-btn class="col" label="重置" type="reset" color="primary" flat />
            <div class="col-auto" style="width: 5px" />
            <q-btn class="col" label="登录" type="submit" color="primary" />
          </div>
        </q-form>
      </q-card-section>
    </q-card>
  </q-page>
</template>

<script>
import { useStore } from 'src/stores/store'

export default {
  name: 'LoginPage',

  data() {
    return {
      store: useStore(),
      username: '',
      password: '',
    }
  },

  methods: {
    onReset() {
      this.username = ''
      this.password = ''
    },

    onSubmit() {
      this.$q.cookies.remove('wstc')

      this.$axios
        .post(this.store.backend + '/login', {
          method: 'login',
          username: this.username,
          password: this.password,
        })
        .then((resp) => {
          if (resp.data.msg === 'username or password error') {
            this.$q.notify('用户名或密码错误')
            return
          }

          this.onReLogin(resp.data)
        })
        .catch(() => {
          this.$q.notify('网络错误，请稍后重试')
        })
    },

    onReLogin(user) {
      if (user) {
        this.login(user)
      } else if (this.$q.cookies.has('wstc') && this.$q.cookies.get('wstc') != 'undefined') {
        this.$axios
          .get(this.store.backend + '/info/' + this.$q.cookies.get('wstc'))
          .then((resp) => {
            this.login(resp.data)
          })
      }
    },

    login(user) {
      let time = setTimeout(() => {
        this.$q.loading.hide()
        clearTimeout(time)
      }, 120000)

      this.$q.loading.show()

      if (!this.store.test) {
        try {
          this.$axios
            .post(this.store.backend + '/options', {
              method: 'query',
            })
            .then(async (resp) => {
              for (let index = 0; index < resp.data.length; index++) {
                const element = resp.data[index]

                if (element.level === 0) {
                  this.store.options.system = element.options
                } else {
                  this.store.options.list = element
                }
              }

              if (this.store.schedule.system.list === null) {
                this.store.schedule.system.list = new Map()
                this.store.schedule.system.interval = setInterval(() => {
                  if (this.store.schedule.system.run) {
                    this.store.schedule.system.list.forEach((element) => {
                      if (element) element()
                    })
                  }
                }, this.store.options.system * 1000)
              }

              if (this.store.schedule.alert.list === null) {
                this.store.schedule.alert.list = new Map()
                this.store.schedule.alert.interval = setInterval(() => {
                  if (this.store.schedule.alert.run) {
                    this.store.schedule.alert.list.forEach((element) => {
                      if (element) element()
                    })
                  }
                }, this.store.options.alert * 1000)
              }

              this.$q.cookies.set('wstc', user.msg.id)

              this.store.user = {
                username: user.msg.username,
                password: user.msg.password_orgin,
                name: user.msg.username,
                location: '',
              }

              this.$axios
                .post(this.store.backend + '/options', {
                  method: 'car_class',
                })
                .then((resp) => {
                  if (resp.status === 200) {
                    let val = []

                    for (let index = 0; index < resp.data.length; index++) {
                      const element = resp.data[index]

                      val.push({
                        id: element.id,
                        name: element.cpt,
                        remark: element.remark,
                        free_time: element.free_time,
                        free_day: element.gratis_day,
                        hm10: element.hm10,
                        hm24: element.hm24,
                        overdue: element.cost_day,
                      })
                    }

                    this.store.options.car.class = val
                  } else {
                    this.$q.loading.hide()
                    clearTimeout(time)
                    this.$q.notify('网络错误，请稍后重试')
                  }
                })
                .catch(() => {
                  this.$q.loading.hide()
                  clearTimeout(time)
                  this.$q.notify('网络错误，请稍后重试')
                })

              this.$axios
                .post(this.store.backend + '/options', {
                  method: 'car_type',
                })
                .then((resp) => {
                  if (resp.status === 200) {
                    let val = []

                    for (let index = 0; index < resp.data.length; index++) {
                      const element = resp.data[index]

                      val.push({
                        id: element.id,
                        type: element.car_type,
                        class: element.ccid,
                      })
                    }

                    this.store.options.car.type = val
                  } else {
                    this.$q.loading.hide()
                    clearTimeout(time)
                    this.$q.notify('网络错误，请稍后重试')
                  }
                })
                .catch(() => {
                  this.$q.loading.hide()
                  clearTimeout(time)
                  this.$q.notify('网络错误，请稍后重试')
                })

              this.$axios
                .post(this.store.backend + '/options', {
                  method: 'car_color',
                })
                .then((resp) => {
                  if (resp.status === 200) {
                    let val = []

                    for (let index = 0; index < resp.data.length; index++) {
                      const element = resp.data[index]

                      val.push({
                        id: element.id,
                        cpt: element.cpt,
                      })
                    }

                    this.store.options.car.color = val
                  } else {
                    this.$q.loading.hide()
                    clearTimeout(time)
                    this.$q.notify('网络错误，请稍后重试')
                  }
                })
                .catch(() => {
                  this.$q.loading.hide()
                  clearTimeout(time)
                  this.$q.notify('网络错误，请稍后重试')
                })

              this.$axios
                .post(this.store.backend + '/options', {
                  method: 'causes_type',
                })
                .then((resp) => {
                  if (resp.status === 200) {
                    let val = []

                    for (let index = 0; index < resp.data.length; index++) {
                      const element = resp.data[index]

                      val.push({
                        id: element.id,
                        cpt: element.cpt,
                        unit: element.unit_id,
                      })
                    }

                    this.store.options.causes.type = val
                  } else {
                    this.$q.loading.hide()
                    clearTimeout(time)
                    this.$q.notify('网络错误，请稍后重试')
                  }
                })
                .catch(() => {
                  this.$q.loading.hide()
                  clearTimeout(time)
                  this.$q.notify('网络错误，请稍后重试')
                })

              this.$axios
                .post(this.store.backend + '/options', {
                  method: 'causes',
                })
                .then((resp) => {
                  if (resp.status === 200) {
                    let val = []

                    for (let index = 0; index < resp.data.length; index++) {
                      const element = resp.data[index]

                      val.push({
                        id: element.id,
                        cpt: element.cpt,
                        dct: element.dct_id,
                      })
                    }

                    this.store.options.causes.list = val
                  } else {
                    this.$q.loading.hide()
                    clearTimeout(time)
                    this.$q.notify('网络错误，请稍后重试')
                  }
                })
                .catch(() => {
                  this.$q.loading.hide()
                  clearTimeout(time)
                  this.$q.notify('网络错误，请稍后重试')
                })

              this.$q.loading.hide()
              clearTimeout(time)
              this.$router.push('/index')
            })
        } catch (e) {
          console.log(e)

          this.$q.loading.hide()
          clearTimeout(time)
          this.$q.notify('网络错误，请稍后重试')
        }
      } else {
        this.$q.loading.hide()
        clearTimeout(time)
        this.$router.push('/index')
      }
    },
  },

  mounted() {
    this.onReLogin()
  },
}
</script>
