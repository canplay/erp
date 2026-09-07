<template>
  <q-page class="bg-white">
    <q-splitter v-model="splitterModel">
      <template v-slot:before>
        <q-tabs v-model="tab" vertical>
          <q-tab name="personal" icon="filter_1" label="个人" />
          <q-tab v-if="store.user.username === 'lmy'" name="system" icon="filter_2" label="系统" />
        </q-tabs>
      </template>

      <template v-slot:after>
        <q-tab-panels
          v-model="tab"
          animated
          swipeable
          vertical
          transition-prev="jump-up"
          transition-next="jump-up"
        >
          <q-tab-panel name="personal">
            <q-card>
              <q-card-section>
                <div class="row">
                  <div class="col text-h6 flex items-center">个人设置</div>
                  <q-btn class="col-2" color="primary" label="保存" @click="onSave('personal')" />
                </div>
              </q-card-section>
              <q-card-section>
                <div class="row">
                  <q-input class="col" v-model="user.name" label="姓名" />
                  <div class="col-auto" style="width: 10px" />
                  <q-input class="col" v-model="user.password" type="password" label="密码" />
                </div>
              </q-card-section>
            </q-card>
          </q-tab-panel>

          <q-tab-panel name="system">
            <q-card>
              <q-card-section>
                <div class="row">
                  <div class="col text-h6 flex items-center">系统设置</div>
                  <q-btn class="col-2" color="primary" label="保存" @click="onSave('system')" />
                </div>
              </q-card-section>
              <q-card-section>
                <q-table
                  dense
                  title="收费类别"
                  :rows="car.class.rows"
                  :columns="car.class.columns"
                  row-key="id"
                  separator="cell"
                  v-model:pagination="car.class.pagination"
                >
                  <template v-slot:body="props">
                    <q-tr :props="props">
                      <q-td key="name" :props="props">
                        {{ props.row.name }}
                        <q-popup-edit v-model="props.row.name">
                          <q-input v-model="props.row.name" dense autofocus />
                        </q-popup-edit>
                      </q-td>

                      <q-td key="remark" :props="props">
                        {{ props.row.remark }}
                        <q-popup-edit v-model="props.row.remark">
                          <q-input v-model="props.row.remark" dense autofocus />
                        </q-popup-edit>
                      </q-td>

                      <q-td key="free_time" :props="props">
                        {{ props.row.free_time }}
                        <q-popup-edit v-model="props.row.free_time">
                          <q-input v-model="props.row.free_time" dense autofocus />
                        </q-popup-edit>
                      </q-td>

                      <q-td key="free_day" :props="props">
                        {{ props.row.free_day }}
                        <q-popup-edit v-model="props.row.free_day">
                          <q-input v-model="props.row.free_day" dense autofocus />
                        </q-popup-edit>
                      </q-td>

                      <q-td key="hm10" :props="props">
                        {{ props.row.hm10 }}
                        <q-popup-edit v-model="props.row.hm10">
                          <q-input v-model="props.row.hm10" dense autofocus />
                        </q-popup-edit>
                      </q-td>

                      <q-td key="hm24" :props="props">
                        {{ props.row.hm24 }}
                        <q-popup-edit v-model="props.row.hm24">
                          <q-input v-model="props.row.hm24" dense autofocus />
                        </q-popup-edit>
                      </q-td>

                      <q-td key="overdue" :props="props">
                        {{ props.row.overdue }}
                        <q-popup-edit v-model="props.row.overdue">
                          <q-input v-model="props.row.overdue" dense autofocus />
                        </q-popup-edit>
                      </q-td>
                    </q-tr>
                  </template>
                </q-table>
              </q-card-section>
            </q-card>
          </q-tab-panel>
        </q-tab-panels>
      </template>
    </q-splitter>
  </q-page>
</template>

<script>
import { useStore } from 'src/stores/store'

export default {
  name: 'PageOptions',

  data() {
    return {
      store: useStore(),
      tab: 'personal',
      splitterModel: 10,
      user: {
        name: '',
        password: '',
      },
      car: {
        class: {
          pagination: {
            rowsPerPage: 0,
            sortBy: 'id',
            descending: false,
          },
          rows: [],
          columns: [
            {
              name: 'name',
              label: '类别',
              align: 'center',
            },
            {
              name: 'remark',
              label: '备注',
              align: 'center',
            },
            {
              name: 'free_time',
              label: '免费时间',
              align: 'center',
            },
            {
              name: 'free_day',
              label: '免费天数',
              align: 'center',
            },
            {
              name: 'hm10',
              label: '10小时内收费',
              align: 'center',
            },
            {
              name: 'hm24',
              label: '10-24内收费',
              align: 'center',
            },
            {
              name: 'overdue',
              label: '超期后每日收费',
              align: 'center',
            },
          ],
        },
      },
    }
  },

  watch: {
    tab(newVal) {
      switch (newVal) {
        case 'personal':
          break
        case 'system':
          this.querySystem()
          break
        default:
          break
      }
    },
  },

  methods: {
    queryPersonal() {},

    querySystem() {},

    onSave(val) {
      switch (val) {
        case 'personal':
          {
            let time = setTimeout(() => {
              this.$q.loading.hide()
              clearTimeout(time)
            }, 120000)

            this.$q.loading.show()

            this.$axios
              .post(this.store.backend + '/user', {
                method: 'update',
                name: this.user.name,
                username: this.store.user.username,
                password: this.user.password,
                level: 0,
                update_user: this.store.user.username,
              })
              .then((resp) => {
                if (resp.status === 200) {
                  if (resp.data) {
                    this.$q.notify('保存成功，请重新登录')

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
                    this.$q.notify('保存失败，请稍后重试')
                  }
                } else {
                  this.$q.notify('网络错误，请稍后重试')
                }

                this.$q.loading.hide()
                clearTimeout(time)
              })
              .catch(() => {
                this.$q.notify('网络错误，请稍后重试')
                this.$q.loading.hide()
                clearTimeout(time)
              })
          }
          break
        case 'system':
          {
            let time = setTimeout(() => {
              this.$q.loading.hide()
              clearTimeout(time)
            }, 120000)

            this.$q.loading.show()

            let r = false

            let update = async () => {
              for (let index = 0; index < this.car.class.rows.length; index++) {
                const element = this.car.class.rows[index]

                let resp = await this.$axios.post(this.store.backend + '/options', {
                  method: 'update_car_class',
                  id: parseInt(element.id),
                  cpt: element.name,
                  remark: element.remark,
                  free_time: parseInt(element.free_time),
                  hm10: element.hm10,
                  hm24: element.hm24,
                  gratis_day: parseInt(element.free_day),
                  cost_day: element.overdue,
                })

                if (resp.status === 200) {
                  r = resp.data
                } else {
                  r = false
                }
              }
            }

            update()
              .then(() => {
                if (r) {
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

                        this.$q.notify('保存成功')
                      } else {
                        this.$q.notify('网络错误，请稍后重试')
                      }
                    })
                    .catch(() => {
                      this.$q.notify('网络错误，请稍后重试')
                    })
                } else {
                  this.$q.notify('保存失败，请稍后重试')
                }

                this.$q.loading.hide()
                clearTimeout(time)
              })
              .catch(() => {
                this.$q.notify('网络错误，请稍后重试')
                this.$q.loading.hide()
                clearTimeout(time)
              })
          }
          break
        default:
          break
      }
    },
  },

  mounted() {
    this.user.name = this.store.user.name
    this.user.password = this.store.user.password
    this.car.class.rows = this.store.options.car.class
  },
}
</script>
