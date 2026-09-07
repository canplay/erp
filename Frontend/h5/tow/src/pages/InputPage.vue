<template>
  <q-page class="bg-white" padding>
    <div class="row">
      <q-input
        class="col"
        v-model="dialog.car.license"
        label="车牌 *"
        :rules="[(val) => !!val || '此为必填项']"
        ref="license"
      />

      <div class="col-auto" style="width: 10px" />

      <q-select
        class="col"
        v-model="dialog.car.type.model"
        :options="dialog.car.type.options"
        label="车辆类型"
      />
    </div>

    <div class="row">
      <q-select
        class="col"
        v-model="dialog.car.color.model"
        :options="dialog.car.color.options"
        label="车身颜色"
      />

      <div class="col-auto" style="width: 10px" />

      <q-input class="col" v-model="dialog.car.vehicle" label="车架号" />
    </div>

    <div class="row">
      <q-input class="col" v-model="dialog.car.engine" label="发动机号" />

      <div class="col-auto" style="width: 10px" />

      <q-select
        class="col"
        v-model="dialog.dc.key.model"
        :options="dialog.dc.key.options"
        label="扣车形式"
      />
    </div>

    <div class="row">
      <q-select
        class="col"
        v-model="dialog.dc.type.model"
        :options="dialog.dc.type.options"
        label="扣车类型"
      />

      <div class="col-auto" style="width: 10px" />

      <q-select
        class="col"
        v-model="dialog.dc.causes.model"
        :options="dialog.dc.causes.options"
        label="扣车原因"
      />
    </div>

    <q-input square v-model="dialog.dc.address" label="扣车地址" />

    <div class="row">
      <q-input square class="col" v-model="dialog.dc.date" label="扣车时间">
        <template v-slot:prepend>
          <q-icon name="event" class="cursor-pointer">
            <q-popup-proxy transition-show="scale" transition-hide="scale">
              <q-date v-model="dialog.dc.date" mask="YYYY-MM-DD HH:mm:ss">
                <div class="row items-center justify-end">
                  <q-btn v-close-popup label="关闭" color="primary" flat />
                </div>
              </q-date>
            </q-popup-proxy>
          </q-icon>
        </template>

        <template v-slot:append>
          <q-icon name="access_time" class="cursor-pointer">
            <q-popup-proxy transition-show="scale" transition-hide="scale">
              <q-time v-model="dialog.dc.date" mask="YYYY-MM-DD HH:mm:ss" format24h>
                <div class="row items-center justify-end">
                  <q-btn v-close-popup label="关闭" color="primary" flat />
                </div>
              </q-time>
            </q-popup-proxy>
          </q-icon>
        </template>
      </q-input>

      <div class="col-auto" style="width: 10px" />

      <q-input class="col" v-model="dialog.dc.party.name" label="当事人姓名" />
    </div>

    <div class="row">
      <q-input class="col" v-model="dialog.dc.party.cardid" label="当事人身份证" />

      <div class="col-auto" style="width: 10px" />

      <q-input class="col" v-model="dialog.dc.party.tel" label="当事人联系电话" />
    </div>

    <div class="col-auto" style="height: 10px" />

    <div class="row">
      <q-input class="col" v-model="dialog.p.name" label="执勤人" />

      <div class="col-auto" style="width: 10px" />

      <q-input readonly class="col" v-model="dialog.dc.name" label="记录人" />
    </div>

    <div class="row">
      <q-input class="col" v-model="dialog.driver" label="拖车驾驶员" />

      <div class="col-auto" style="width: 10px" />

      <q-input class="col" v-model="dialog.operator" label="拖车操作员" />
    </div>

    <div class="row">
      <q-select
        class="col"
        v-model="dialog.cmd.unit.model"
        :options="dialog.cmd.unit.options"
        label="拖移指令单位"
      />

      <div class="col-auto" style="width: 10px" />

      <q-input class="col" v-model="dialog.cmd.user" label="拖移指令人" />
    </div>

    <div class="row">
      <q-input square class="col" v-model="dialog.dc.into_date" label="拖移入场时间">
        <template v-slot:prepend>
          <q-icon name="event" class="cursor-pointer">
            <q-popup-proxy transition-show="scale" transition-hide="scale">
              <q-date v-model="dialog.dc.into_date" mask="YYYY-MM-DD HH:mm:ss">
                <div class="row items-center justify-end">
                  <q-btn v-close-popup label="关闭" color="primary" flat />
                </div>
              </q-date>
            </q-popup-proxy>
          </q-icon>
        </template>

        <template v-slot:append>
          <q-icon name="access_time" class="cursor-pointer">
            <q-popup-proxy transition-show="scale" transition-hide="scale">
              <q-time v-model="dialog.dc.into_date" mask="YYYY-MM-DD HH:mm:ss" format24h>
                <div class="row items-center justify-end">
                  <q-btn v-close-popup label="关闭" color="primary" flat />
                </div>
              </q-time>
            </q-popup-proxy>
          </q-icon>
        </template>
      </q-input>

      <div class="col-auto" style="width: 10px" />

      <q-input class="col" v-model="dialog.drag.money" label="拖移金额" />
    </div>

    <div class="row">
      <q-select
        class="col"
        v-model="dialog.cv.cv.model"
        :options="dialog.cv.cv.options"
        label="交警处罚状态"
      />

      <div class="col-auto" style="width: 10px" />

      <q-input square class="col" v-model="dialog.cv.date" label="交警处罚时间">
        <template v-slot:prepend>
          <q-icon name="event" class="cursor-pointer">
            <q-popup-proxy transition-show="scale" transition-hide="scale">
              <q-date v-model="dialog.cv.date" mask="YYYY-MM-DD HH:mm:ss">
                <div class="row items-center justify-end">
                  <q-btn v-close-popup label="关闭" color="primary" flat />
                </div>
              </q-date>
            </q-popup-proxy>
          </q-icon>
        </template>

        <template v-slot:append>
          <q-icon name="access_time" class="cursor-pointer">
            <q-popup-proxy transition-show="scale" transition-hide="scale">
              <q-time v-model="dialog.cv.date" mask="YYYY-MM-DD HH:mm:ss" format24h>
                <div class="row items-center justify-end">
                  <q-btn v-close-popup label="关闭" color="primary" flat />
                </div>
              </q-time>
            </q-popup-proxy>
          </q-icon>
        </template>
      </q-input>
    </div>

    <div class="row">
      <q-input class="col" v-model="dialog.cv.name" label="交警执行人" />

      <div class="col-auto" style="width: 10px" />

      <q-select
        class="col"
        v-model="dialog.cv.opinion.model"
        :options="dialog.cv.opinion.options"
        label="交警执行情况"
      />
    </div>

    <div class="row">
      <q-select
        class="col"
        v-model="dialog.tv.tv.model"
        :options="dialog.tv.tv.options"
        label="城管处罚状态"
      />

      <div class="col-auto" style="width: 10px" />

      <q-input square class="col" v-model="dialog.tv.date" label="城管处罚时间">
        <template v-slot:prepend>
          <q-icon name="event" class="cursor-pointer">
            <q-popup-proxy transition-show="scale" transition-hide="scale">
              <q-date v-model="dialog.tv.date" mask="YYYY-MM-DD HH:mm:ss">
                <div class="row items-center justify-end">
                  <q-btn v-close-popup label="关闭" color="primary" flat />
                </div>
              </q-date>
            </q-popup-proxy>
          </q-icon>
        </template>

        <template v-slot:append>
          <q-icon name="access_time" class="cursor-pointer">
            <q-popup-proxy transition-show="scale" transition-hide="scale">
              <q-time v-model="dialog.tv.date" mask="YYYY-MM-DD HH:mm:ss" format24h>
                <div class="row items-center justify-end">
                  <q-btn v-close-popup label="关闭" color="primary" flat />
                </div>
              </q-time>
            </q-popup-proxy>
          </q-icon>
        </template>
      </q-input>
    </div>

    <div class="row">
      <q-input class="col" v-model="dialog.tv.name" label="城管执行人" />

      <div class="col-auto" style="width: 10px" />

      <q-select
        class="col"
        v-model="dialog.tv.opinion.model"
        :options="dialog.tv.opinion.options"
        label="城管执行情况"
      />
    </div>

    <div class="col-auto" style="height: 10px" />

    <div class="row">
      <q-input class="col" v-model="dialog.car.remark" bg-color="red" label="车辆备注" />

      <div class="col-auto" style="width: 10px" />

      <q-input class="col" v-model="dialog.remark" bg-color="red" label="拖移备注" />
    </div>

    <div class="row">
      <q-input class="col" v-model="dialog.rs.name" label="放行人" />

      <div class="col-auto" style="width: 10px" />

      <q-input square class="col" v-model="dialog.rs.date" label="放车时间">
        <template v-slot:prepend>
          <q-icon name="event" class="cursor-pointer">
            <q-popup-proxy transition-show="scale" transition-hide="scale">
              <q-date v-model="dialog.rs.date" mask="YYYY-MM-DD HH:mm:ss">
                <div class="row items-center justify-end">
                  <q-btn v-close-popup label="关闭" color="primary" flat />
                </div>
              </q-date>
            </q-popup-proxy>
          </q-icon>
        </template>

        <template v-slot:append>
          <q-icon name="access_time" class="cursor-pointer">
            <q-popup-proxy transition-show="scale" transition-hide="scale">
              <q-time v-model="dialog.rs.date" mask="YYYY-MM-DD HH:mm:ss" format24h>
                <div class="row items-center justify-end">
                  <q-btn v-close-popup label="关闭" color="primary" flat />
                </div>
              </q-time>
            </q-popup-proxy>
          </q-icon>
        </template>
      </q-input>
    </div>

    <div class="col-auto" style="height: 10px" />

    <div class="row">
      <q-btn
        class="col"
        size="40px"
        :label="dialog.img.front === '' ? '前' : ''"
        @click="onUpload('front')"
      >
        <img
          v-if="dialog.img.front != ''"
          :src="dialog.img.front"
          style="max-width: 194px; max-height: 160px"
        />
      </q-btn>

      <div class="col-auto" style="width: 10px" />

      <q-btn
        class="col"
        size="40px"
        :label="dialog.img.back === '' ? '后' : ''"
        @click="onUpload('back')"
      >
        <img
          v-if="dialog.img.back != ''"
          :src="dialog.img.back"
          style="max-width: 194px; max-height: 160px"
        />
      </q-btn>
    </div>

    <div class="col-auto" style="height: 10px" />

    <div class="row">
      <q-btn
        class="col"
        size="40px"
        :label="dialog.img.left === '' ? '左' : ''"
        @click="onUpload('left')"
      >
        <img
          v-if="dialog.img.left != ''"
          :src="dialog.img.left"
          style="max-width: 194px; max-height: 160px"
        />
      </q-btn>

      <div class="col-auto" style="width: 10px" />

      <q-btn
        class="col"
        size="40px"
        :label="dialog.img.right === '' ? '右' : ''"
        @click="onUpload('right')"
      >
        <img
          v-if="dialog.img.right != ''"
          :src="dialog.img.right"
          style="max-width: 194px; max-height: 160px"
        />
      </q-btn>
    </div>

    <div class="col-auto" style="height: 10px" />

    <div class="row">
      <q-btn
        class="col"
        size="30px"
        :label="dialog.img.back_front === '' ? '前（回场）' : ''"
        @click="onUpload('back_front')"
        style="min-height: 110px"
      >
        <img
          v-if="dialog.img.back_front != ''"
          :src="dialog.img.back_front"
          style="max-width: 194px; max-height: 160px"
        />
      </q-btn>

      <div class="col-auto" style="width: 10px" />

      <q-btn
        class="col"
        size="30px"
        :label="dialog.img.back_back === '' ? '后（回场）' : ''"
        @click="onUpload('back_back')"
        style="min-height: 110px"
      >
        <img
          v-if="dialog.img.back_back != ''"
          :src="dialog.img.back_back"
          style="max-width: 194px; max-height: 160px"
        />
      </q-btn>
    </div>

    <div class="col-auto" style="height: 10px" />

    <div class="row">
      <q-btn
        class="col"
        size="30px"
        :label="dialog.img.back_left === '' ? '左（回场）' : ''"
        @click="onUpload('back_left')"
        style="min-height: 110px"
      >
        <img
          v-if="dialog.img.back_left != ''"
          :src="dialog.img.back_left"
          style="max-width: 194px; max-height: 160px"
        />
      </q-btn>

      <div class="col-auto" style="width: 10px" />

      <q-btn
        class="col"
        size="30px"
        :label="dialog.img.back_right === '' ? '右（回场）' : ''"
        @click="onUpload('back_right')"
        style="min-height: 110px"
      >
        <img
          v-if="dialog.img.back_right != ''"
          :src="dialog.img.back_right"
          style="max-width: 194px; max-height: 160px"
        />
      </q-btn>
    </div>

    <input type="file" accept="image/*" hidden @change="onFileChange" ref="browserFile" />

    <div class="col-auto" style="height: 10px" />

    <div class="row">
      <q-btn class="col" color="negative" label="取消" style="font-size: 20px" to="index" />

      <div class="col-auto" style="width: 10px" />

      <q-btn
        class="col"
        color="positive"
        label="保存"
        style="font-size: 20px"
        @click="onSave('new')"
      />
    </div>
  </q-page>
</template>

<script>
import { date } from 'quasar'
import { useStore } from 'src/stores/store'

export default {
  name: 'PageInput',

  data() {
    return {
      store: useStore(),
      dialog: {
        show: false,
        create: true,
        id: 0,
        car: {
          license: '',
          vehicle: '',
          engine: '',
          type: {
            model: '小型汽车',
            options: ['二轮摩托车', '三轮摩托车', '小型汽车'],
          },
          color: {
            model: '白',
            options: ['白'],
          },
          remark: '',
          delete: false,
        },
        dc: {
          type: {
            model: '城市违法',
            options: ['交通违法', '交通肇事', '城市违法'],
          },
          causes: {
            model: '违反禁停标志',
            options: [
              '未随车携带驾驶证',
              '违反禁停标志',
              '无牌照',
              '交通肇事',
              '肇事拖移',
              '城管执法',
            ],
          },
          date: '',
          address: '',
          key: {
            model: '拖车',
            options: ['拖车', '锁车'],
          },
          party: {
            name: '',
            cardid: '',
            tel: '',
          },
          name: '',
          acc: '',
          into_date: '',
        },
        p: {
          name: '',
          id: '',
        },
        driver: '',
        operator: '',
        drag: {
          km: '',
          unit: '',
          money: '',
        },
        cmd: {
          unit: {
            model: '交警部门',
            options: ['交警部门', '卧龙执法点', '开化执法点', '新平执法点', '消防部门'],
          },
          user: '',
        },
        cv: {
          cv: {
            model: '未处罚',
            options: ['未处罚', '已处罚'],
          },
          acc: '',
          name: '',
          date: '',
          opinion: {
            model: '待定',
            options: ['待定', '同意放行', '不同意放行'],
          },
        },
        tv: {
          tv: {
            model: '未处罚',
            options: ['未处罚', '已处罚'],
          },
          acc: '',
          name: '',
          date: '',
          opinion: {
            model: '待定',
            options: ['待定', '同意放行', '不同意放行'],
          },
        },
        rc: {
          name: '',
          idcard: '',
          tel: '',
        },
        parking: {
          date: '',
          unit: '',
          money: '',
          payable: '',
          paidin: '',
        },
        remark: '',
        rs: {
          acc: '',
          name: '',
          date: '',
        },
        attachment: [],
        attachment_link: '',
        selected: '',
        img: {
          front: '',
          back: '',
          left: '',
          right: '',
          back_front: '',
          back_back: '',
          back_left: '',
          back_right: '',
        },
      },
    }
  },

  methods: {
    onFileChange(ev) {
      let file = ev.target.files[0]

      if (file === undefined) return

      let reader = new FileReader()
      let fileData = reader.readAsDataURL(file)

      reader.onload = () => {
        fileData = reader.result

        switch (this.dialog.selected) {
          case 'front':
            this.dialog.img.front = fileData
            this.dialog.attachment.push({
              name: file.name,
              type: 'front',
              file: '',
              data: file,
            })
            break
          case 'back':
            this.dialog.img.back = fileData
            this.dialog.attachment.push({
              name: file.name,
              type: 'back',
              file: '',
              data: file,
            })
            break
          case 'left':
            this.dialog.img.left = fileData
            this.dialog.attachment.push({
              name: file.name,
              type: 'left',
              file: '',
              data: file,
            })
            break
          case 'right':
            this.dialog.img.right = fileData
            this.dialog.attachment.push({
              name: file.name,
              type: 'right',
              file: '',
              data: file,
            })
            break
          case 'back_front':
            this.dialog.img.back_front = fileData
            this.dialog.attachment.push({
              name: file.name,
              type: 'back_front',
              file: '',
              data: file,
            })
            break
          case 'back_back':
            this.dialog.img.back_back = fileData
            this.dialog.attachment.push({
              name: file.name,
              type: 'back_back',
              file: '',
              data: file,
            })
            break
          case 'back_left':
            this.dialog.img.back_left = fileData
            this.dialog.attachment.push({
              name: file.name,
              type: 'back_left',
              file: '',
              data: file,
            })
            break
          case 'back_right':
            this.dialog.img.back_right = fileData
            this.dialog.attachment.push({
              name: file.name,
              type: 'back_right',
              file: '',
              data: file,
            })
            break
        }
      }

      reader.onerror = () => {
        this.$q.notify('文件读取失败，请重试')
      }
    },

    onUpload(val) {
      switch (val) {
        case 'front':
          if (this.dialog.img.front != '') {
            window.open(this.dialog.img.front)
          } else {
            this.dialog.selected = val
            this.$refs.browserFile.click()
          }
          break
        case 'back':
          if (this.dialog.img.back != '') {
            window.open(this.dialog.img.back)
          } else {
            this.dialog.selected = val
            this.$refs.browserFile.click()
          }
          break
        case 'left':
          if (this.dialog.img.left != '') {
            window.open(this.dialog.img.left)
          } else {
            this.dialog.selected = val
            this.$refs.browserFile.click()
          }
          break
        case 'right':
          if (this.dialog.img.right != '') {
            window.open(this.dialog.img.right)
          } else {
            this.dialog.selected = val
            this.$refs.browserFile.click()
          }
          break
        case 'back_front':
          if (this.dialog.img.back_front != '') {
            window.open(this.dialog.img.back_front)
          } else {
            this.dialog.selected = val
            this.$refs.browserFile.click()
          }
          break
        case 'back_back':
          if (this.dialog.img.back_back != '') {
            window.open(this.dialog.img.back_back)
          } else {
            this.dialog.selected = val
            this.$refs.browserFile.click()
          }
          break
        case 'back_left':
          if (this.dialog.img.back_left != '') {
            window.open(this.dialog.img.back_left)
          } else {
            this.dialog.selected = val
            this.$refs.browserFile.click()
          }
          break
        case 'back_right':
          if (this.dialog.img.back_right != '') {
            window.open(this.dialog.img.back_right)
          } else {
            this.dialog.selected = val
            this.$refs.browserFile.click()
          }
          break
      }
    },

    async onSave() {
      if (!this.$refs.license.validate()) return

      let time = setTimeout(() => {
        this.$q.loading.hide()
        clearTimeout(time)
      }, 120000)

      this.$q.loading.show()

      let upload = async () => {
        let secucess = true

        for (let index = 0; index < this.dialog.attachment.length; index++) {
          const element = this.dialog.attachment[index]

          if (element.type != 'attachment') {
            if (element.data.name === undefined) continue

            let data = new FormData()
            data.append('undefined', element.data)

            try {
              let resp = await this.$axios.post(this.store.backend + '/send', data)
              this.dialog.attachment[index].file = resp.data
            } catch (e) {
              console.log(e)
              secucess = false
            }
          }
        }

        return secucess
      }

      if (!(await upload())) {
        this.$q.notify('网络错误，请稍后重试')
        this.$q.loading.hide()
        clearTimeout(time)
        return
      }

      if (this.dialog.rs.date != '' && this.dialog.rs.date != undefined) {
        this.dialog.cv.cv.model = '已处罚'
        this.dialog.cv.date = date.formatDate(Date.now(), 'YYYY-MM-DD HH:mm:ss')
        this.dialog.cv.opinion.model = '同意放行'
      }

      // this.writeTemplater();

      let parking_money = this.calcPay({
        type: this.dialog.car.type.model,
        date_in: this.dialog.dc.into_date,
        date_out: this.dialog.rs.date,
      })

      this.$axios
        .post(this.store.backend + '/car', {
          method: 'add',
          id: this.dialog.car.id,
          license: this.dialog.car.license,
          vehicle: this.dialog.car.vehicle,
          engine: this.dialog.car.engine,
          car_remark: this.dialog.car.remark,
          car_type: this.dialog.car.type.model,
          dc_type: this.dialog.dc.type.model,
          dc_causes: this.dialog.dc.causes.model,
          car_color: this.dialog.car.color.model,
          dc_address: this.dialog.dc.address,
          dc_key: this.dialog.dc.key.model,
          dc_party_name: this.dialog.dc.party.name,
          dc_party_cardid: this.dialog.dc.party.cardid,
          dc_party_tel: this.dialog.dc.party.tel,
          p_name: this.dialog.p.name,
          p_id: this.dialog.p.id,
          dc_acc: this.dialog.dc.acc,
          dc_name: this.dialog.dc.name,
          dc_date: this.dialog.dc.date,
          dc_into_date: this.dialog.dc.into_date,
          remark: this.dialog.remark,
          driver: this.dialog.driver,
          operator: this.dialog.operator,
          drag_km: this.dialog.drag.km,
          drag_unit: this.dialog.drag.unit,
          drag_money: this.dialog.drag.money,
          cmd_unit: this.dialog.cmd.unit.model,
          cmd_user: this.dialog.cmd.user,
          cv: this.dialog.cv.cv.model,
          cv_acc: this.dialog.cv.acc,
          cv_name: this.dialog.cv.name,
          cv_date: this.dialog.cv.date,
          cv_opinion: this.dialog.cv.opinion.model,
          tv: this.dialog.tv.tv.model,
          tv_acc: this.dialog.tv.acc,
          tv_name: this.dialog.tv.name,
          tv_date: this.dialog.tv.date,
          tv_opinion: this.dialog.tv.opinion.model,
          rs_name: this.dialog.rs.name,
          rs_date: this.dialog.rs.date,
          parking_date: this.dialog.dc.into_date,
          parking_unit: parking_money.unit,
          parking_money: parking_money.payable,
          parking_payable: parking_money.payable,
          parking_paidin: parking_money.paidin,
          attachment: JSON.stringify(this.dialog.attachment),
          delete: this.dialog.car.delete,
          create_user: this.dialog.create ? this.store.user.username : '',
          create_date: this.dialog.create ? date.formatDate(Date.now(), 'YYYY-MM-DD HH:mm:ss') : '',
          update_user: this.store.user.username,
          update_date: date.formatDate(Date.now(), 'YYYY-MM-DD HH:mm:ss'),
        })
        .then((resp) => {
          if (resp) {
            this.$q.notify('保存成功')
            this.$router.push('/index')
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
    },

    calcPay(val) {
      if (val.date_in === null || val.date_in === undefined) {
        return {
          date: '未知',
          unit: '',
          payable: 0,
          paidin: 0,
        }
      }

      try {
        let type = this.store.options.car.type.filter((v) => {
          return v.type == val.type
        })

        let classes = this.store.options.car.class.filter((v) => {
          return v.id == type[0].class
        })

        let now = date.formatDate(
          val.date_out === null || val.date_out === undefined ? Date.now() : val.date_out,
          'YYYY-MM-DD HH:mm:ss',
        )
        let time = date.getDateDiff(
          now,
          date.formatDate(val.date_in, 'YYYY-MM-DD HH:mm:ss'),
          'days',
        )

        time = Math.ceil(time)

        let payable = 0
        if (time > classes[0].free_day) {
          payable = parseInt(classes[0].overdue) * (time - parseInt(classes[0].free_day))
        }

        return {
          date: time,
          unit: classes[0].name + ': ' + classes[0].remark,
          payable: payable.toFixed(0),
          paidin: 0,
        }
      } catch (e) {
        console.log(e)
        return {
          date: '未知',
          unit: '',
          payable: 0,
          paidin: 0,
        }
      }
    },
  },

  mounted() {
    this.dialog.create = true
    this.dialog.car.id = 0
    this.dialog.car.license = '云H'
    this.dialog.car.vehicle = ''
    this.dialog.car.engine = ''
    this.dialog.car.remark = ''
    this.dialog.dc.date = date.formatDate(Date.now(), 'YYYY-MM-DD HH:mm:ss')
    this.dialog.dc.address = ''
    this.dialog.dc.key.model = '拖车'
    this.dialog.dc.party.name = ''
    this.dialog.dc.party.cardid = ''
    this.dialog.dc.party.tel = ''
    this.dialog.dc.name = this.store.user.username
    this.dialog.dc.acc = ''
    this.dialog.dc.into_date = date.formatDate(Date.now(), 'YYYY-MM-DD HH:mm:ss')
    this.dialog.p.name = ''
    this.dialog.p.id = ''
    this.dialog.driver = ''
    this.dialog.operator = ''
    this.dialog.drag.km = ''
    this.dialog.drag.unit = ''
    this.dialog.drag.money = ''
    this.dialog.cmd.unit.model = '交警部门'
    this.dialog.cmd.user = ''
    this.dialog.cv.cv.model = '未处罚'
    this.dialog.cv.acc = ''
    this.dialog.cv.name = ''
    this.dialog.cv.date = ''
    this.dialog.cv.opinion.model = '待定'
    this.dialog.tv.tv.model = '未处罚'
    this.dialog.tv.acc = ''
    this.dialog.tv.name = ''
    this.dialog.tv.date = ''
    this.dialog.tv.opinion.model = '待定'
    this.dialog.rc.name = ''
    this.dialog.rc.idcard = ''
    this.dialog.rc.tel = ''
    this.dialog.parking.date = ''
    this.dialog.parking.unit = ''
    this.dialog.parking.money = ''
    this.dialog.parking.payable = ''
    this.dialog.parking.paidin = ''
    this.dialog.remark = ''
    this.dialog.rs.acc = ''
    this.dialog.rs.name = ''
    this.dialog.rs.date = ''
    this.dialog.img.front = ''
    this.dialog.img.back = ''
    this.dialog.img.left = ''
    this.dialog.img.right = ''
    this.dialog.img.back_front = ''
    this.dialog.img.back_back = ''
    this.dialog.img.back_left = ''
    this.dialog.img.back_right = ''
    this.dialog.attachment = []
    this.dialog.car.delete = false

    this.dialog.car.type.options = []

    for (let index = 0; index < this.store.options.car.type.length; index++) {
      const element = this.store.options.car.type[index]
      this.dialog.car.type.options.push(element.type)
    }

    this.dialog.car.type.model = this.dialog.car.type.options[21]

    this.dialog.car.color.options = []

    for (let index = 0; index < this.store.options.car.color.length; index++) {
      const element = this.store.options.car.color[index]
      this.dialog.car.color.options.push(element.cpt)
    }

    this.dialog.car.color.model = this.dialog.car.color.options[0]

    this.dialog.dc.type.options = []

    for (let index = 0; index < this.store.options.causes.type.length; index++) {
      const element = this.store.options.causes.type[index]
      this.dialog.dc.type.options.push(element.cpt)
    }

    this.dialog.dc.type.model = this.dialog.dc.type.options[0]

    this.dialog.dc.causes.options = []

    for (let index = 0; index < this.store.options.causes.list.length; index++) {
      const element = this.store.options.causes.list[index]
      this.dialog.dc.causes.options.push(element.cpt)
    }

    this.dialog.dc.causes.model = this.dialog.dc.causes.options[1]
  },
}
</script>
