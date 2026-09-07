<template>
  <q-page padding class="relative-position">
    <div class="column desktop-only">
      <div class="col row">
        <q-card class="col" style="height: 300px">
          <q-card-section>
            <div id="echart1" ref="echart1" />
          </q-card-section>
        </q-card>

        <div class="col-auto" style="width: 10px" />

        <q-card class="col" style="height: 300px">
          <q-card-section>
            <div id="echart2" ref="echart2" />
          </q-card-section>
        </q-card>

        <div class="col-auto" style="width: 10px" />

        <q-card class="col" style="height: 300px">
          <q-card-section>
            <div id="echart3" ref="echart3" />
          </q-card-section>
        </q-card>
      </div>

      <div class="col-auto" style="height: 10px" />

      <div class="col">
        <q-card class="col" style="height: 300px">
          <q-card-section>
            <div id="echart4" ref="echart4" />
          </q-card-section>
        </q-card>
      </div>
    </div>

    <div class="mobile-only absolute-center" style="width: 90%">
      <div class="row">
        <q-btn color="primary" label="列表" class="col" size="48px" to="car" />
        <div class="col-auto" style="width: 8px" />
        <q-btn color="primary" label="录入" class="col" size="48px" to="input" />
      </div>

      <div style="height: 8px" />

      <div class="row">
        <q-btn color="primary" label="设置" class="col" size="48px" to="options" />
        <div class="col-auto" style="width: 8px" />
        <q-btn color="primary" label="报表" class="col" size="48px" to="reports" />
      </div>
    </div>
  </q-page>
</template>

<script>
import * as echarts from 'echarts'
import 'echarts/extension/bmap/bmap'
import { date } from 'quasar'
import { useStore } from 'src/stores/store'

export default {
  name: 'PageIndex',

  data() {
    return {
      store: useStore(),
    }
  },

  methods: {
    adjustSize(val) {
      val.style.width = val.parentElement.clientWidth - 30 + 'px'
      val.style.height = val.parentElement.parentElement.clientHeight + 'px'
    },

    onResize() {
      this.adjustSize(this.$refs.echart1)
      this.adjustSize(this.$refs.echart2)
      this.adjustSize(this.$refs.echart3)

      this.$refs.echart4.style.width = this.$refs.echart4.parentElement.clientWidth - 30 + 'px'
      this.$refs.echart4.style.height =
        document.querySelector('#q-app > div > div > main').clientHeight - 380 + 'px'
      this.$refs.echart4.parentElement.parentElement.style.height =
        this.$refs.echart4.clientHeight + 32 + 'px'
    },

    async initechart1() {
      let num = {
        d1: {
          two: 0,
          three: 0,
          car: 0,
        },
        d2: {
          two: 0,
          three: 0,
          car: 0,
        },
        d3: {
          two: 0,
          three: 0,
          car: 0,
        },
        d4: {
          two: 0,
          three: 0,
          car: 0,
        },
        d5: {
          two: 0,
          three: 0,
          car: 0,
        },
        d6: {
          two: 0,
          three: 0,
          car: 0,
        },
        d7: {
          two: 0,
          three: 0,
          car: 0,
        },
      }

      let day_7 = date.formatDate(Date.now(), 'YYYY-MM-DD 00:00:00 - YYYY-MM-DD 23:59:59')

      let resp = await this.$axios.post(this.store.backend + '/car', {
        method: 'count',
        in_date: day_7,
        out_date: '',
        content: '小型汽车',
        model: '车辆类型',
        status: '全部',
        unit: '',
      })
      num.d7.car = parseInt(resp.data)

      resp = await this.$axios.post(this.store.backend + '/car', {
        method: 'count',
        in_date: day_7,
        out_date: '',
        content: '二轮摩托车',
        model: '车辆类型',
        status: '全部',
        unit: '',
      })
      num.d7.two = parseInt(resp.data)

      resp = await this.$axios.post(this.store.backend + '/car', {
        method: 'count',
        in_date: day_7,
        out_date: '',
        content: '三轮摩托车',
        model: '车辆类型',
        status: '全部',
        unit: '',
      })
      num.d7.three = parseInt(resp.data)

      let echart = echarts.init(document.getElementById('echart1'))
      echart.setOption({
        title: {
          text: '近一周违章数量',
          left: 'center',
        },
        tooltip: {
          trigger: 'axis',
          axisPointer: {
            type: 'shadow',
          },
        },
        toolbox: {
          feature: {
            saveAsImage: {
              title: '下载',
            },
          },
        },
        xAxis: [
          {
            type: 'category',
            data: ['周一', '周二', '周三', '周四', '周五', '周六', '周日'],
          },
        ],
        yAxis: [
          {
            type: 'value',
          },
        ],
        series: [
          {
            name: '二轮摩托车',
            type: 'bar',
            data: [10, 52, 200, 334, 390, 330, num.d7.two],
          },
          {
            name: '三轮摩托车',
            type: 'bar',
            data: [10, 52, 200, 334, 390, 330, num.d7.three],
          },
          {
            name: '小型汽车',
            type: 'bar',
            data: [10, 52, 200, 334, 390, 330, num.d7.car],
          },
        ],
      })
    },

    async initechart2() {
      let echart = echarts.init(document.getElementById('echart2'))
      echart.setOption({
        title: {
          text: '近一周违章趋势',
          left: 'center',
        },
        tooltip: {
          trigger: 'axis',
        },
        toolbox: {
          feature: {
            saveAsImage: {
              title: '下载',
            },
          },
        },
        xAxis: {
          type: 'category',
          boundaryGap: false,
          data: ['周一', '周二', '周三', '周四', '周五', '周六', '周日'],
        },
        yAxis: {
          type: 'value',
        },
        series: [
          {
            name: '二轮车',
            type: 'line',
            stack: '总量',
            data: [120, 132, 101, 134, 90, 230, 210],
          },
          {
            name: '三轮车',
            type: 'line',
            stack: '总量',
            data: [220, 182, 191, 234, 290, 330, 310],
          },
          {
            name: '汽车',
            type: 'line',
            stack: '总量',
            data: [150, 232, 201, 154, 190, 330, 410],
          },
        ],
      })
    },

    async initechart3() {
      let echart = echarts.init(document.getElementById('echart3'))
      echart.setOption({
        title: {
          text: '违章原因占比',
          left: 'center',
        },
        tooltip: {
          trigger: 'item',
        },
        toolbox: {
          feature: {
            saveAsImage: {
              title: '下载',
            },
          },
        },
        series: [
          {
            name: '类型',
            type: 'pie',
            radius: '50%',
            data: [
              { value: 1048, name: '违章停车' },
              { value: 735, name: '僵尸车' },
              { value: 580, name: '故障车' },
              { value: 484, name: '其他车' },
            ],
            emphasis: {
              itemStyle: {
                shadowBlur: 10,
                shadowOffsetX: 0,
                shadowColor: 'rgba(0, 0, 0, 0.5)',
              },
            },
          },
        ],
      })
    },

    async initechart4() {
      let convertData = function (data) {
        var res = []
        for (var i = 0; i < data.length; i++) {
          var geoCoord = geoCoordMap[data[i].name]
          if (geoCoord) {
            res.push({
              name: data[i].name,
              value: geoCoord.concat(data[i].value),
            })
          }
        }
        return res
      }

      let geoCoordMap = {
        示例城市A市: [104.0000, 23.0000],
      }

      let data = [{ name: '示例城市A市', value: 9 }]

      let echart = echarts.init(document.getElementById('echart4'))
      echart.setOption({
        backgroundColor: 'transparent',
        title: {
          text: '违章地点分布',
          left: 'center',
          textStyle: {
            color: '#fff',
          },
        },
        tooltip: {
          trigger: 'item',
        },
        bmap: {
          center: [104.0000, 23.0000],
          zoom: 14,
          roam: true,
          mapStyle: {
            styleJson: [
              {
                featureType: 'water',
                elementType: 'all',
                stylers: {
                  color: '#044161',
                },
              },
              {
                featureType: 'land',
                elementType: 'all',
                stylers: {
                  color: '#004981',
                },
              },
              {
                featureType: 'boundary',
                elementType: 'geometry',
                stylers: {
                  color: '#064f85',
                },
              },
              {
                featureType: 'railway',
                elementType: 'all',
                stylers: {
                  visibility: 'off',
                },
              },
              {
                featureType: 'highway',
                elementType: 'geometry',
                stylers: {
                  color: '#004981',
                },
              },
              {
                featureType: 'highway',
                elementType: 'geometry.fill',
                stylers: {
                  color: '#005b96',
                  lightness: 1,
                },
              },
              {
                featureType: 'highway',
                elementType: 'labels',
                stylers: {
                  visibility: 'off',
                },
              },
              {
                featureType: 'arterial',
                elementType: 'geometry',
                stylers: {
                  color: '#004981',
                },
              },
              {
                featureType: 'arterial',
                elementType: 'geometry.fill',
                stylers: {
                  color: '#00508b',
                },
              },
              {
                featureType: 'poi',
                elementType: 'all',
                stylers: {
                  visibility: 'off',
                },
              },
              {
                featureType: 'green',
                elementType: 'all',
                stylers: {
                  color: '#056197',
                  visibility: 'off',
                },
              },
              {
                featureType: 'subway',
                elementType: 'all',
                stylers: {
                  visibility: 'off',
                },
              },
              {
                featureType: 'manmade',
                elementType: 'all',
                stylers: {
                  visibility: 'off',
                },
              },
              {
                featureType: 'local',
                elementType: 'all',
                stylers: {
                  visibility: 'off',
                },
              },
              {
                featureType: 'arterial',
                elementType: 'labels',
                stylers: {
                  visibility: 'off',
                },
              },
              {
                featureType: 'boundary',
                elementType: 'geometry.fill',
                stylers: {
                  color: '#029fd4',
                },
              },
              {
                featureType: 'building',
                elementType: 'all',
                stylers: {
                  color: '#1a5787',
                },
              },
              {
                featureType: 'label',
                elementType: 'all',
                stylers: {
                  visibility: 'off',
                },
              },
            ],
          },
        },
        series: [
          {
            name: 'Top 5',
            type: 'effectScatter',
            coordinateSystem: 'bmap',
            data: convertData(
              data
                .sort(function (a, b) {
                  return b.value - a.value
                })
                .slice(0, 6),
            ),
            encode: {
              value: 2,
            },
            symbolSize: function (val) {
              return val[2] / 10
            },
            showEffectOn: 'emphasis',
            rippleEffect: {
              brushType: 'stroke',
            },
            hoverAnimation: true,
            label: {
              formatter: '{b}',
              position: 'right',
              show: true,
            },
            itemStyle: {
              color: '#f4e925',
              shadowBlur: 10,
              shadowColor: '#333',
            },
            zlevel: 1,
          },
        ],
      })
    },
  },

  mounted() {
    window.addEventListener('resize', this.onResize)

    this.$nextTick(() => {
      this.onResize()
      this.initechart1()
      this.initechart2()
      this.initechart3()
      this.initechart4()
    })
  },

  beforeUnmount() {
    window.removeEventListener('resize', this.onResize)
  },
}
</script>
