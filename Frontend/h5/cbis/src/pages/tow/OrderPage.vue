<template>
  <q-page class="bg-white">
    <q-table
      dense
      :rows="rows"
      :columns="columns"
      row-key="id"
      separator="cell"
      v-model:pagination="pagination"
      :rows-per-page-options="[10, 20, 30, 40, 50, 0]"
      @request="onRequest"
      virtual-scroll
      :style="`${store.height};width: calc(100vw - 200px)`"
      class="my-table"
      selection="multiple"
    >
      <!-- 顶部 -->
      <template v-slot:top>
        <div class="column fit">
          <div class="col row">
            <q-select
              class="col-2"
              v-model="search.unit.model"
              :options="search.unit.options"
              label="执法单位"
            />

            <div class="col-auto" style="width: 8px" />

            <q-select
              class="col"
              v-model="search.status.model"
              :options="search.status.options"
              label="车辆状态"
            />

            <div class="col-auto" style="width: 8px" />

            <q-select
              class="col"
              v-model="search.key.model"
              :options="search.key.options"
              label="扣车形式"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="search.date.in" label="扣车日期">
              <template v-slot:append>
                <q-icon name="event" class="cursor-pointer">
                  <q-popup-proxy transition-show="scale" transition-hide="scale">
                    <q-date
                      v-model="search.date.in"
                      range
                      mask="YYYY-MM-DD"
                      @update:model-value="onInDate"
                    >
                      <div class="row items-center justify-end">
                        <q-btn v-close-popup label="关闭" color="primary" flat />
                      </div>
                    </q-date>
                  </q-popup-proxy>
                </q-icon>
              </template>
            </q-input>

            <div class="col-auto" style="width: 8px" />
          </div>

          <div class="col-auto" style="height: 8px" />

          <div class="col row">
            <!-- <q-select class="col-1" v-model="search.type.model" :options="search.type.options" label="查询类型" />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="search.content" label="查询内容" />
 -->
            <q-input class="col" v-model="search.date.out" label="放车日期">
              <template v-slot:append>
                <q-icon name="event" class="cursor-pointer">
                  <q-popup-proxy transition-show="scale" transition-hide="scale">
                    <q-date
                      range
                      v-model="search.date.out"
                      mask="YYYY-MM-DD"
                      @update:model-value="onOutDate"
                    >
                      <div class="row items-center justify-end">
                        <q-btn v-close-popup label="关闭" color="primary" flat />
                      </div>
                    </q-date>
                  </q-popup-proxy>
                </q-icon>
              </template>
            </q-input>
            <div class="col-auto" style="width: 8px" />

            <q-btn class="col-1" color="green" label="导出" @click="onExport" />

            <div class="col-auto" style="width: 8px" />

            <q-btn class="col-1" color="red" label="重置" @click="onReset" />

            <div class="col-auto" style="width: 8px" />

            <q-btn class="col-1" color="primary" label="查询" @click="onQuery" />

            <div class="col-auto" style="width: 8px" />

            <q-btn class="col-1" color="red" label="新增" @click="onNew" />
          </div>
        </div>
      </template>

      <template v-slot:header-cell="props">
        <q-th :props="props" style="font-size: 15px; font-weight: bold">
          {{ props.col.label }}
        </q-th>
      </template>

      <template v-slot:body-cell="props">
        <q-td v-if="props.col.name === 'license'" :props="props">
          <q-btn color="primary" :label="props.value" @click="onView(props.row)" />
        </q-td>

        <q-td v-else :props="props">
          {{ props.value }}
        </q-td>
      </template>

      <template v-slot:bottom="scope">
        <div class="fit column">
          <!--  <div class="col row" style="font-size: 16px">
            <div class="col-auto text-red">当前在场：</div>
            <div class="col-auto" v-for="(item, index) in car_type.in" :key="index">
              {{ item.type }}：{{ item.sum }}&nbsp;&nbsp;&nbsp;&nbsp;
            </div>
            <div v-if="car_type.in.length >= 4" class="col-auto text-red">
              当前总在场：{{
                car_type.in[0].sum +
                car_type.in[1].sum +
                car_type.in[2].sum +
                car_type.in[3].sum
              }}
            </div>
          </div> -->

          <!--  <div class="col row" style="font-size: 16px">
            <div class="col-auto text-green">当前放行：</div>

            <div class="col-auto" v-for="(item, index) in car_type.out" :key="index">
              {{ item.type }}：{{ item.sum }}&nbsp;&nbsp;&nbsp;&nbsp;
            </div>

            <div v-if="car_type.out.length >= 4" class="col-auto text-green">
              当前总放行：{{
                car_type.out[0].sum +
                car_type.out[1].sum +
                car_type.out[2].sum +
                car_type.out[3].sum
              }}
            </div>
          </div> -->

          <div class="row items-center" style="font-size: 16px">
            <div class="col row">
              <div class="col-auto">总数量：{{ sum.sum }}</div>
              <div class="col-auto" style="width: 8px" />
              <div class="col-auto text-red">总在场：{{ sum.car_in }}</div>
              <div class="col-auto" style="width: 8px" />
              <div class="col-auto text-green">总放行：{{ sum.car_out }}</div>
            </div>

            <div class="col-auto">每页显示：</div>

            <div class="col-auto">
              <q-select
                dense
                borderless
                v-model="pagination!.rowsPerPage"
                :options="pageoptions"
                @update:model-value="onRowsPerPage"
              />
            </div>

            <div class="col-auto" style="width: 8px" />

            <div class="col-auto">{{ scope.pagination.page }} - {{ scope.pagesNumber }}</div>
            <div class="col-auto" style="width: 15px" />

            <div class="col-auto">
              {{ pagination!.rowsNumber }}
            </div>
            <div class="col-auto" style="width: 20px" />

            <q-btn
              class="col-auto"
              icon="first_page"
              color="grey-8"
              round
              dense
              flat
              :disable="scope.isFirstPage"
              @click="scope.firstPage"
            />

            <q-btn
              class="col-auto"
              icon="chevron_left"
              color="grey-8"
              round
              dense
              flat
              :disable="scope.isFirstPage"
              @click="scope.prevPage"
            />

            <q-btn
              class="col-auto"
              icon="chevron_right"
              color="grey-8"
              round
              dense
              flat
              :disable="scope.isLastPage"
              @click="scope.nextPage"
            />

            <q-btn
              class="col-auto"
              icon="last_page"
              color="grey-8"
              round
              dense
              flat
              :disable="scope.isLastPage"
              @click="scope.lastPage"
            />
          </div>
        </div>

        <!-- <div style="height:500px">
          框
        </div> -->
      </template>
    </q-table>
    <!--   <q-card  style="background-color: rgba(255, 255, 255, 0.9);">
          <q-card-section>
                        <div class="col text-primary text-h4">车辆记录</div>

                        <q-separator />

                        <div id="carecharts" style="min-height: 300px" />
                      </q-card-section>
        </q-card> -->

    <div v-if="showcar" class="row z-top" style="margin: 8px">
      <q-card class="col" style="background-color: rgba(255, 255, 255, 0.9)">
        <q-card-section>
          <div class="col text-primary text-h6">
            当前总在场：<span>{{ total_num1 }}</span>
          </div>

          <q-separator />

          <div id="noreleased" style="min-height: 300px" />
        </q-card-section>
      </q-card>

      <div class="col-auto" style="width: 8px" />

      <q-card class="col" style="background-color: rgba(255, 255, 255, 0.9)">
        <q-card-section>
          <div class="col text-primary text-h6">
            当前总放行：<span>{{ total_num2 }}</span>
          </div>

          <q-separator />

          <div id="released" style="min-height: 300px" />
        </q-card-section>
      </q-card>
    </div>

    <!--  <div class="q-pa-md">
       <q-dialog v-model="dialog" persistent>
      <q-card>
        <q-card-actions align="right">

          <q-btn flat label="导出全部数据" color="primary"  @click="dialog = false" />

          <q-btn flat label="导出当前数据" color="primary" @click = reportCurrn v-close-popup />
        </q-card-actions>
      </q-card>
    </q-dialog>
  </div> -->

    <q-dialog v-model="dialog.show" persistent>
      <q-card style="min-width: 70%; padding-bottom: 10px">
        <q-card-section>
          <div class="row">
            <q-input
              class="col"
              v-model="dialog.car.license"
              label="车牌 *"
              :rules="[(val) => !!val || '此为必填项']"
              ref="license"
            />

            <div class="col-auto" style="width: 8px" />

            <q-select
              class="col"
              v-model="dialog.car.type.model"
              :options="dialog.car.type.options"
              label="车辆类型"
            />

            <div class="col-auto" style="width: 8px" />

            <q-select
              class="col"
              v-model="dialog.car.color.model"
              :options="dialog.car.color.options"
              label="车身颜色"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="dialog.car.vehicle" label="车架号" />
          </div>

          <div class="row">
            <q-input class="col" v-model="dialog.car.engine" label="发动机号" />

            <div class="col-auto" style="width: 8px" />

            <q-select
              class="col"
              v-model="dialog.dc.key.model"
              :options="dialog.dc.key.options"
              label="扣车形式"
            />

            <div class="col-auto" style="width: 8px" />

            <q-select
              class="col"
              v-model="dialog.dc.type.model"
              :options="dialog.dc.type.options"
              label="扣车类型"
            />

            <div class="col-auto" style="width: 8px" />

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

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" square v-model="dialog.dc.party.name" label="当事人姓名" />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="dialog.dc.party.cardid" label="当事人身份证" />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="dialog.dc.party.tel" label="当事人联系电话" />
          </div>

          <div class="col-auto" style="height: 8px" />

          <div class="row">
            <q-input class="col" v-model="dialog.p.name" label="执勤人" />

            <div class="col-auto" style="width: 8px" />

            <q-input readonly class="col" v-model="dialog.dc.name" label="记录人" />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="dialog.driver" label="拖车驾驶员" />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="dialog.operator" label="拖车操作员" />
          </div>

          <div class="row">
            <q-select
              class="col"
              v-model="dialog.cmd.unit.model"
              :options="dialog.cmd.unit.options"
              label="拖移指令单位"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="dialog.cmd.user" label="拖移指令人" />

            <div class="col-auto" style="width: 8px" />

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

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="dialog.drag.money" label="拖移金额" />
          </div>

          <div class="row">
            <q-select
              :disable="store.user.level === 3 || store.user.level === 1 ? false : true"
              class="col"
              v-model="dialog.cv.cv.model"
              :options="dialog.cv.cv.options"
              label="交警处罚状态"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input
              :disable="store.user.level === 3 || store.user.level === 1 ? false : true"
              square
              class="col"
              v-model="dialog.cv.date"
              label="交警处罚时间"
            >
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

            <div class="col-auto" style="width: 8px" />

            <q-input
              class="col"
              :disable="store.user.level === 3 || store.user.level === 1 ? false : true"
              v-model="dialog.cv.name"
              label="交警执行人"
            />

            <div class="col-auto" style="width: 8px" />

            <q-select
              :disable="store.user.level === 3 || store.user.level === 1 ? false : true"
              class="col"
              v-model="dialog.cv.opinion.model"
              :options="dialog.cv.opinion.options"
              label="交警执行情况"
            />
          </div>

          <div class="row">
            <q-select
              :disable="store.user.level === 4 || store.user.level === 1 ? false : true"
              class="col"
              v-model="dialog.tv.tv.model"
              :options="dialog.tv.tv.options"
              label="城管处罚状态"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input
              :disable="store.user.level === 4 || store.user.level === 1 ? false : true"
              square
              class="col"
              v-model="dialog.tv.date"
              label="城管处罚时间"
            >
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

            <div class="col-auto" style="width: 8px" />

            <q-input
              class="col"
              :disable="store.user.level === 4 || store.user.level === 1 ? false : true"
              v-model="dialog.tv.name"
              label="城管执行人"
            />

            <div class="col-auto" style="width: 8px" />

            <q-select
              :disable="store.user.level === 4 || store.user.level === 1 ? false : true"
              class="col"
              v-model="dialog.tv.opinion.model"
              :options="dialog.tv.opinion.options"
              label="城管执行情况"
            />
          </div>

          <div class="col-auto" style="height: 8px" />

          <div class="row">
            <q-input class="col" v-model="dialog.car.remark" label="车辆备注" />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="dialog.remark" label="拖移备注" />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="dialog.rs.name" label="放行人" />

            <div class="col-auto" style="width: 8px" />

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

          <div class="col-auto" style="height: 8px" />

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

            <div class="col-auto" style="width: 8px" />

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

            <div class="col-auto" style="width: 8px" />

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

            <div class="col-auto" style="width: 8px" />

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

          <div class="col-auto" style="height: 8px" />

          <div class="row">
            <q-btn
              class="col"
              size="20px"
              :label="dialog.img.back_front === '' ? '前（回场）' : ''"
              @click="onUpload('back_front')"
              style="min-height: 18px"
            >
              <img
                v-if="dialog.img.back_front != ''"
                :src="dialog.img.back_front"
                style="max-width: 194px; max-height: 160px"
              />
            </q-btn>

            <div class="col-auto" style="width: 8px" />

            <q-btn
              class="col"
              size="20px"
              :label="dialog.img.back_back === '' ? '后（回场）' : ''"
              @click="onUpload('back_back')"
              style="min-height: 18px"
            >
              <img
                v-if="dialog.img.back_back != ''"
                :src="dialog.img.back_back"
                style="max-width: 194px; max-height: 160px"
              />
            </q-btn>

            <div class="col-auto" style="width: 8px" />

            <q-btn
              class="col"
              size="20px"
              :label="dialog.img.back_left === '' ? '左（回场）' : ''"
              @click="onUpload('back_left')"
              style="min-height: 18px"
            >
              <img
                v-if="dialog.img.back_left != ''"
                :src="dialog.img.back_left"
                style="max-width: 194px; max-height: 160px"
              />
            </q-btn>

            <div class="col-auto" style="width: 8px" />

            <q-btn
              class="col"
              size="20px"
              :label="dialog.img.back_right === '' ? '右（回场）' : ''"
              @click="onUpload('back_right')"
              style="min-height: 18px"
            >
              <img
                v-if="dialog.img.back_right != ''"
                :src="dialog.img.back_right"
                style="max-width: 194px; max-height: 160px"
              />
            </q-btn>
          </div>

          <div class="col-auto" style="height: 8px" />

          <q-card class="col">
            <q-card-section class="row">
              <div class="col-auto">模板：</div>
              <div class="col-auto" style="width: 8px" />
              <a class="col-auto" href="/template/处罚决定.xlsx"> 处罚决定.xlsx </a>
              <div class="col-auto" style="width: 8px" />
              <a class="col-auto" href="/template/案卷记录.xlsx"> 案卷记录.xlsx </a>
              <div class="col-auto" style="width: 8px" />
              <a class="col-auto" href="/template/处罚决定.docx"> 处罚决定.docx </a>
              <div class="col-auto" style="width: 8px" />
              <a class="col-auto" href="/template/案卷记录.docx"> 案卷记录.docx </a>
            </q-card-section>
          </q-card>

          <div class="col-auto" style="height: 8px" />

          <q-card v-if="!dialog.create" class="col">
            <q-card-section class="row">
              附件：
              <br />
              <div v-for="(item, id) in dialog.attachment" :key="id">
                <a
                  v-if="item.type === 'attachment'"
                  :href="item.file"
                  target="_blank"
                  style="margin-right: 8px"
                >
                  {{ item.name }}
                </a>
              </div>
            </q-card-section>
          </q-card>

          <div class="col-auto" style="height: 8px" />

          <div style="padding-bottom: 10px">
            <input type="file" accept="image/*" hidden @change="onFileChange" ref="browserFile" />

            <q-uploader
              square
              float
              with-credentials
              label="附件"
              multiple
              class="fit"
              accept="*/*"
              :url="
                store.server + '/api/route/post?auth=jwt&service=resource&path=/api/upload/upload/'
              "
              @added="onAdded"
              :headers="[
                {
                  name: 'Authorization',
                  value: `Bearer ${{
                    headers: {
                      Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
                    },
                  }}`,
                },
              ]"
              @uploaded="onUploaded"
              ref="uploader"
            >
              <template v-slot:header="scope">
                <div class="row no-wrap items-center q-pa-sm q-gutter-xs">
                  <q-btn
                    v-if="scope.queuedFiles.length > 0"
                    icon="clear_all"
                    @click="scope.removeQueuedFiles"
                    round
                    dense
                    flat
                  >
                    <q-tooltip>清空</q-tooltip>
                  </q-btn>

                  <q-btn
                    v-if="scope.uploadedFiles.length > 0"
                    icon="done_all"
                    @click="scope.removeUploadedFiles"
                    round
                    dense
                    flat
                  >
                    <q-tooltip>移除</q-tooltip>
                  </q-btn>

                  <q-spinner v-if="scope.isUploading" class="q-uploader__spinner" />

                  <div class="col">
                    <div class="q-uploader__title">上传</div>
                    <div class="q-uploader__subtitle">
                      {{ scope.uploadSizeLabel }} /
                      {{ scope.uploadProgressLabel }}
                    </div>
                  </div>

                  <q-btn v-if="scope.canAddFiles" type="a" icon="add_box" round dense flat>
                    <q-uploader-add-trigger />
                    <q-tooltip>选择文件</q-tooltip>
                  </q-btn>

                  <q-btn
                    v-if="scope.canUpload"
                    icon="cloud_upload"
                    @click="scope.upload"
                    round
                    dense
                    flat
                  >
                    <q-tooltip>开始上传</q-tooltip>
                  </q-btn>

                  <q-btn
                    v-if="scope.isUploading"
                    icon="clear"
                    @click="scope.abort"
                    round
                    dense
                    flat
                  >
                    <q-tooltip>取消上传</q-tooltip>
                  </q-btn>
                </div>
              </template>
            </q-uploader>
          </div>

          <div class="col-auto" style="height: 8px" />

          <div class="row" style="float: right">
            <q-btn
              v-if="store.user.level === 1"
              class="col"
              color="warning"
              label="删除"
              style="font-size: 20px"
              @click="onDelete"
            />

            <div class="col-auto" style="width: 8px" />

            <q-btn
              color="grey-4"
              v-close-popup
              label="取消"
              style="font-size: 20px; color: black !important"
            />

            <div class="col-auto" style="width: 8px" />

            <q-btn
              color="primary"
              label="保存"
              style="font-size: 20px; height: 20px"
              @click="onSave"
            />
            <div class="col-auto" style="width: 8px" />
          </div>
        </q-card-section>
      </q-card>
    </q-dialog>

    <q-dialog v-model="sheet.show">
      <q-card>
        <div v-html="sheet.content" />
      </q-card>
    </q-dialog>
  </q-page>

  <q-inner-loading :showing="loading">
    <q-spinner size="50px" color="primary" />
  </q-inner-loading>
</template>

<script setup lang="ts">
import { ref } from 'vue';
import type { QTableProps } from 'quasar';
import { date } from 'quasar';
import { exportFile } from 'quasar';
import * as echarts from 'echarts';
import { useStore, type TowCarOptionItem } from 'stores/store';
import { api } from 'src/boot/axios';

const store = useStore();
const loading = ref(false);

// 类型定义
interface TableRow {
  id?: number | string;
  license?: string;
  vehicle?: string;
  engine?: string;
  car_type?: string;
  car_color?: string;
  car_remark?: string;
  dc_type?: string;
  dc_causes?: string;
  dc_date?: string;
  dc_address?: string;
  dc_key?: string;
  dc_party_name?: string;
  dc_party_cardid?: string;
  dc_party_tel?: string;
  dc_name?: string;
  dc_acc?: string;
  dc_into_date?: string;
  p_name?: string;
  p_id?: string;
  driver?: string;
  operator?: string;
  drag_km?: string | number;
  drag_unit?: string;
  drag_money?: string | number;
  cmd_unit?: string;
  cmd_user?: string;
  cv?: string;
  cv_acc?: string;
  cv_name?: string;
  cv_date?: string;
  cv_opinion?: string;
  tv?: string;
  tv_acc?: string;
  tv_name?: string;
  tv_date?: string;
  tv_opinion?: string;
  rc_name?: string;
  rc_idcard?: string;
  rc_tel?: string;
  parking_date?: string;
  parking_unit?: string;
  parking_money?: string | number;
  parking_payable?: number;
  parking_paidin?: string | number;
  remark?: string;
  rs_acc?: string;
  rs_name?: string;
  rs_date?: string;
  attachment?: string;
  create_user?: string;
  create_date?: string;
  update_user?: string;
  update_date?: string;
  delete?: boolean;
  [key: string]: unknown;
}

interface AttachmentItem {
  name: string;
  type: string;
  file: string;
  data?: File;
}

interface CarTypeItem {
  type: string;
  sum: number;
}

// CalcPayParams 类型已在下面使用处定义

const total_num1 = ref(0);
const total_num2 = ref(0);
const showcar = ref(false);

//柱形图
type EChartsOption = echarts.EChartsOption;
let carChart: echarts.ECharts;
const carRecord = ref({
  txt: [] as string[],
  num: [] as number[],
  id: '',
  total_num: 0,
});

const initCar = (cartype: { value: { id: string; txt: string[]; num: number[] } }) => {
  const dom = document.getElementById(cartype.value.id) as HTMLElement;
  carChart = echarts.init(dom);
  const option: EChartsOption = {
    //标题
    // title:{
    //   text:carRecord.value.total_num,
    //   top:10,
    //   left:200
    // },
    xAxis: {
      type: 'category',
      data: cartype.value.txt,
    },
    yAxis: {
      type: 'value',
    },
    series: [
      {
        data: cartype.value.num,
        type: 'bar',
        label: {
          show: true,
          position: 'top',
          distance: 10,
          verticalAlign: 'middle',
        },
      },
    ],
  };
  if (option && typeof option === 'object') {
    carChart.setOption(option);
    carRecord.value.txt = [];
    carRecord.value.num = [];
    carRecord.value.id = '';
    carRecord.value.total_num = 0;
  }
};

// let name = ref([])

const sum = ref({
  sum: 0,
  car_in: 0,
  car_out: 0,
});
//需要查询的列表内容
const search = ref({
  type: {
    model: '',
    // 类型查询
    options: ['', '车牌', '车辆类型', '车身颜色', '车架号', '发动机号', '车辆备注', '拖移备注'],
  },
  unit: {
    model: '全部',
    //执法单位
    options: ['全部', '交警部门', '卧龙执法点', '开化执法点', '新平执法点', '消防部门'],
  },
  status: {
    //车辆状态
    model: '未放行',
    options: ['全部', '未放行', '已放行'],
  },
  key: {
    //扣车形式
    model: '拖车',
    options: ['全部', '拖车', '锁车'],
  },
  date: {
    in: '',
    out: '',
  },
  content: '',
});

const pagination = ref({
  page: 1,
  rowsNumber: 0,
  rowsPerPage: 20,
  sortBy: '',
  descending: true,
} as QTableProps['pagination']);
const pageoptions = ref([10, 20, 30, 40, 50, '全部']);

//查询显示的列表参数
const columns = ref([
  { name: 'license', label: '车牌', field: 'license', align: 'center' },
  {
    name: 'car_type',
    label: '车辆类型',
    field: 'car_type',
    align: 'center',
  },
  {
    name: 'dc_key',
    label: '扣车形式',
    field: 'dc_key',
    align: 'center',
  },
  {
    name: 'dc_type',
    label: '扣车类型',
    field: 'dc_type',
    align: 'center',
  },
  {
    name: 'dc_causes',
    label: '扣车原因',
    field: 'dc_causes',
    align: 'center',
  },
  {
    name: 'dc_date',
    label: '扣车时间',
    field: 'dc_date',
    align: 'center',
    sortable: true,
  },
  {
    name: 'cmd_unit',
    label: '拖移指令单位',
    field: 'cmd_unit',
    align: 'center',
  },
  {
    name: 'dc_party_name',
    label: '当事人',
    field: 'dc_party_name',
    align: 'center',
  },
  { name: 'dc_name', label: '记录人', field: 'dc_name', align: 'center' },
  { name: 'remark', label: '备注', field: 'remark', align: 'center' },
  {
    name: 'parking_date',
    label: '停车时长(天)',
    field: 'parking_date',
    align: 'center',
  },
  {
    name: 'parking_money',
    label: '停车费(元)',
    field: 'parking_money',
    align: 'center',
  },
  { name: 'cv', label: '交通违法处理情况', field: 'cv', align: 'center' },
  {
    name: 'cv_opinion',
    label: '交警审批意见',
    field: 'cv_opinion',
    align: 'center',
  },
  { name: 'tv', label: '城市违法处理情况', field: 'tv', align: 'center' },
  {
    name: 'tv_opinion',
    label: '城管审批意见',
    field: 'tv_opinion',
    align: 'center',
  },
  {
    name: 'rc_name',
    label: '接车人姓名',
    field: 'rc_name',
    align: 'center',
  },
  {
    name: 'rs_date',
    label: '放车时间',
    field: 'rs_date',
    align: 'center',
    sortable: true,
  },
  {
    name: 'create_user',
    label: '创建人',
    field: 'create_user',
    align: 'center',
    sortable: true,
  },
  {
    name: 'create_date',
    label: '创建时间',
    field: 'create_date',
    align: 'center',
    sortable: true,
  },
  {
    name: 'update_user',
    label: '修改人',
    field: 'update_user',
    align: 'center',
    sortable: true,
  },
  {
    name: 'update_date',
    label: '修改时间',
    field: 'update_date',
    align: 'center',
    sortable: true,
  },
] as QTableProps['columns']);
const rows = ref([] as TableRow[]);
const browserFile = ref<HTMLInputElement | null>(null);
const uploader = ref<{ reset: () => void } | null>(null);
const license = ref<{ validate: () => boolean } | null>(null);
const dialog = ref({
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
      options: ['未随车携带驾驶证', '违反禁停标志', '无牌照', '交通肇事', '肇事拖移', '城管执法'],
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
    payable: 0,
    paidin: '',
  },
  remark: '',
  rs: {
    acc: '',
    name: '',
    date: '',
  },
  attachment: [] as AttachmentItem[],
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
});
const sheet = ref({
  show: false,
  content: '',
});
interface CarTypeData {
  in: CarTypeItem[];
  out: CarTypeItem[];
}

const car_type = ref<CarTypeData>({
  in: [] as CarTypeItem[],
  out: [] as CarTypeItem[],
});

const onReset = () => {
  search.value.status.model = '全部';
  search.value.key.model = '全部';
  search.value.type.model = '';
  search.value.date.in = '';
  search.value.date.out = '';
  search.value.content = '';
};

//导出当前页面数据
const onExport = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);
  loading.value = true;
  // let pagination = {
  //   page: 1,
  //   rowsNumber: 0,
  //   rowsPerPage: 20,
  //   sortBy: 'update_date',
  //   descending: true,
  //   options: [10, 20, 30, 40, 50, '全部'],
  // };

  //    const exportData = () => {
  //   // 获取当前页面中的表格数据
  //   const tableData = rows.value;

  //   // 如果没有数据，则提示用户
  //   if (tableData.length === 0) {
  //     this.store.quasar.notify({
  //       color: 'warning',
  //       message: '没有数据可以导出',
  //     });
  //     return;
  //   }else{
  //      this.store.quasar.notify({

  //       message: '正在导出数据',
  //     });
  //   }

  //   // 将数据转换为CSV格式
  //   const csvContent = 'data:text/csv;charset=utf-8,' + tableData.map(row => Object.values(row).join(',')).join('\n');

  //   // 创建一个链接，让用户下载CSV文件
  //   const encodedUri = encodeURI(csvContent);
  //   const link = document.createElement('a');
  //   link.setAttribute('href', encodedUri);
  //   link.setAttribute('download', 'data.csv');
  //   document.body.appendChild(link);
  //   link.click();
  //   document.body.removeChild(link);
  // }
  //  return {
  //   rows,
  //   exportData,
  // }

  //导出当前页面数据
  const content = [columns.value!.map((col) => wrapCsvValue(col.label))]
    .concat(
      rows.value.map((row: TableRow) =>
        columns
          .value!
          .map((col) =>
            wrapCsvValue(
              typeof col.field === 'function'
                ? col.field(row) as string
                : row[col.field === void 0 ? col.name : col.field] as string,
              col.format,
            ),
          )
          .join(','),
      ),
    )
    .join('\r\n');
  const status = exportFile('车辆列表.csv', content, 'text/csv');

  if (status !== true) {
    store.quasar.notify('即将开始下载，请稍后...');
  }

  loading.value = false;
  clearTimeout(time);

  // api.
  //   .post('auth=jwt&service=tow&path=/api/car/count', {
  //     in_date: search.value.date.in,
  //     out_date: search.value.date.out,
  //     content: search.value.content,
  //     model: search.value.type.model === '' ? '' : search.value.type.model,
  //     status: search.value.status.model,
  //     key: search.value.key.model,
  //     name: '',
  //     unit: search.value.unit.model,
  //   },
  //     {
  //   headers: {
  //     Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
  //   },
  // }
  //   )
  //   .then((resp) => {
  //     //所有数据条数
  //     pagination.rowsNumber = parseInt(resp.data.message);

  //     //导出当前页面数据

  //   api.
  //     .post('auth=jwt&service=tow&path=/api/car/query', {
  //       in_date: search.value.date.in,
  //       out_date: search.value.date.out,
  //       content: search.value.content,
  //       model: search.value.type.model === '' ? '' : search.value.type.model,
  //       curPage: (pagination.page - 1) * pagination.rowsPerPage,
  //       maxPage: pagination.rowsNumber,
  //       sortBy: pagination.sortBy,
  //       descending: pagination.descending,
  //       status: search.value.status.model,
  //       key: search.value.key.model,
  //       name: '',
  //       unit: search.value.unit.model,
  //     },
  //       {
  //   headers: {
  //     Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
  //   },
  // }
  //     )
  //     .then((resp) => {
  //       if (resp.data.status === 1) {
  //         for (let i = 0; i < resp.data.message.length; i++) {
  //           if (resp.data.message[i].id === '') break;

  //           let attachment = [];
  //           if (resp.data.message[i].attachment && resp.data.message[i].attachment != '') {
  //             let attachments = resp.data.message[i].attachment.split(';');
  //             for (let index = 0; index < attachments.length; index++) {
  //               const element = attachments[index];
  //               attachment.push(element);
  //             }
  //           }

  //           let parking_money = calcPay({
  //             type: resp.data.message[i].car_type,
  //             date_in: resp.data.message[i].dc_into_date === '' ? resp.data.message[i].dc_date : resp.data.message[i].dc_into_date,
  //             date_out: resp.data.message[i].rs_date,
  //           });
  //           //填充查询的数据
  //           rows.push({
  //             id: resp.data.message[i].id,
  //             license: resp.data.message[i].license,
  //             vehicle: resp.data.message[i].vehicle,
  //             engine: resp.data.message[i].engine,
  //             car_type: resp.data.message[i].car_type,
  //             car_color: resp.data.message[i].car_color,
  //             car_remark: resp.data.message[i].car_remark,
  //             dc_type: resp.data.message[i].dc_type,
  //             dc_causes: resp.data.message[i].dc_causes,
  //             dc_date: date.formatDate(
  //               resp.data.message[i].dc_date,
  //               'YYYY-MM-DD HH:mm:ss'
  //             ),
  //             dc_address: resp.data.message[i].dc_address,
  //             dc_key: resp.data.message[i].dc_key,
  //             dc_party_name: resp.data.message[i].dc_party_name,
  //             dc_party_cardid: resp.data.message[i].dc_party_cardid,
  //             dc_party_tel: resp.data.message[i].dc_party_tel,
  //             dc_name: resp.data.message[i].dc_name,
  //             dc_acc: resp.data.message[i].dc_acc,
  //             dc_into_date: date.formatDate(
  //               resp.data.message[i].dc_into_date,
  //               'YYYY-MM-DD HH:mm:ss'
  //             ),
  //             p_name: resp.data.message[i].p_name,
  //             p_id: resp.data.message[i].p_id,
  //             driver: resp.data.message[i].driver,
  //             operator: resp.data.message[i].operator,
  //             drag_km: resp.data.message[i].drag_km,
  //             drag_unit: resp.data.message[i].drag_unit,
  //             drag_money: resp.data.message[i].drag_money,
  //             cmd_unit: resp.data.message[i].cmd_unit,
  //             cmd_user: resp.data.message[i].cmd_user,
  //             cv: resp.data.message[i].cv,
  //             cv_acc: resp.data.message[i].cv_acc,
  //             cv_name: resp.data.message[i].cv_name,
  //             cv_date: date.formatDate(
  //               resp.data.message[i].cv_date,
  //               'YYYY-MM-DD HH:mm:ss'
  //             ),
  //             cv_opinion: resp.data.message[i].cv_opinion,
  //             tv: resp.data.message[i].tv,
  //             tv_acc: resp.data.message[i].tv_acc,
  //             tv_name: resp.data.message[i].tv_name,
  //             tv_date: date.formatDate(
  //               resp.data.message[i].tv_date,
  //               'YYYY-MM-DD HH:mm:ss'
  //             ),
  //             tv_opinion: resp.data.message[i].tv_opinion,
  //             rc_name: resp.data.message[i].rc_name,
  //             rc_idcard: resp.data.message[i].rc_idcard,
  //             rc_tel: resp.data.message[i].rc_tel,
  //             parking_date: parking_money.date,
  //             parking_unit: parking_money.unit,
  //             parking_money: parking_money.payable,
  //             parking_payable: parking_money.payable,
  //             parking_paidin: parking_money.paidin,
  //             remark: resp.data.message[i].remark,
  //             rs_acc: resp.data.message[i].rs_acc,
  //             rs_name: resp.data.message[i].rs_name,
  //             rs_date: date.formatDate(
  //               resp.data.message[i].rs_date,
  //               'YYYY-MM-DD HH:mm:ss'
  //             ),
  //             attachment: resp.data.message[i].attachment,
  //             create_user: resp.data.message[i].create_user,
  //             create_date: date.formatDate(
  //               resp.data.message[i].create_date,
  //               'YYYY-MM-DD HH:mm:ss'
  //             ),
  //             update_user: resp.data.message[i].update_user,
  //             update_date: date.formatDate(
  //               resp.data.message[i].update_date,
  //               'YYYY-MM-DD HH:mm:ss'
  //             ),
  //           });
  //         }
  //         const content = [columns.value!.map((col: any) => wrapCsvValue(col.label)),]
  //           .concat(
  //             rows.values.map((row: any) =>
  //               columns.value!
  //                 .map((col: any) =>
  //                   wrapCsvValue(
  //                     typeof col.field === 'function'
  //                       ? col.field(row)
  //                       : row[
  //                       col.field === void 0 ? col.name : col.field
  //                       ],
  //                     col.format
  //                   )
  //                 )
  //                 .join(',')
  //             )
  //           )
  //           .join('\r\n');

  //         loading.value = false;
  //         clearTimeout(time);

  //         let status = exportFile('车辆列表.csv', content, 'text/csv');

  //         if (status == true) {
  //           store.quasar.notify('即将开始下载，请稍后...');
  //         } else {
  //           store.quasar.notify('下载失败，请重试');
  //         }
  //       } else {
  //         store.quasar.notify('网络错误，请稍后重试');
  //       }
  //     });
  // })
  // .catch(() => {
  //   store.quasar.notify('网络错误，请稍后重试');
  //   loading.value = false;
  //   clearTimeout(time);
  // });
};

//导出数据调用函数
const wrapCsvValue = (val: string, formatFn?: (...args: unknown[]) => unknown) => {
  let formatted: string = formatFn !== void 0 ? String(formatFn(val)) : val;

  formatted = formatted === void 0 || formatted === null ? '' : String(formatted);

  formatted = formatted.split('"').join('""');

  return formatted;
};

const onInDate = (val: { from: string; to: string } | null) => {
  if (val === null) {
    search.value.date.in = '';
    return;
  }

  if (typeof val === 'object') {
    search.value.date.in = val.from + ' - ' + val.to;
  }
};

const onOutDate = (val: { from: string; to: string } | null) => {
  if (val === null) {
    search.value.date.out = '';
    return;
  }

  if (typeof val === 'object') {
    search.value.date.out = val.from + ' - ' + val.to;
  }
};

const onRowsPerPage = (val: string) => {
  if (val === '全部') {
    pagination.value!.rowsPerPage =
      (car_type.value.in[0]?.sum ?? 0) +
      (car_type.value.in[1]?.sum ?? 0) +
      (car_type.value.in[2]?.sum ?? 0) +
      (car_type.value.in[3]?.sum ?? 0) +
      (car_type.value.out[0]?.sum ?? 0) +
      (car_type.value.out[1]?.sum ?? 0) +
      (car_type.value.out[2]?.sum ?? 0) +
      (car_type.value.out[3]?.sum ?? 0);
  }
  onQuery();
};
//查询
const onRequest = (props: { pagination: QTableProps['pagination'] }) => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);
  showcar.value = true;

  loading.value = true;

  //对应类型赋值
  const page = props.pagination?.page ?? 1;
  const rowsPerPage = props.pagination?.rowsPerPage ?? 20;
  const sortBy = props.pagination?.sortBy ?? null;
  const descending = props.pagination?.descending ?? true;

  rows.value = [];
  car_type.value.in = [];
  car_type.value.out = [];

  //根据选的内容进行查询
  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/query',
      {
        in_date: search.value.date.in,
        out_date: search.value.date.out,
        content: search.value.content,
        model: search.value.type.model === '' ? '' : search.value.type.model,
        curPage: (page - 1) * rowsPerPage,
        maxPage: rowsPerPage,
        sortBy: sortBy,
        name: '',
        descending: descending,
        status: search.value.status.model,
        key: search.value.key.model,
        unit: search.value.unit.model,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      if (resp.data.status === 1) {
        //显示20条数据
        for (let i = 0; i < resp.data.data.length; i++) {
          if (resp.data.data[i].id === '') break;
          //调用停车费，计算停车费
          const parking_money = calcPay({
            type: resp.data.data[i].car_type,
            date_in: resp.data.data[i].dc_into_date,
            date_out: resp.data.data[i].rs_date,
          });

          rows.value.push({
            id: resp.data.data[i].id,
            license: resp.data.data[i].license,
            vehicle: resp.data.data[i].vehicle,
            engine: resp.data.data[i].engine,
            car_type: resp.data.data[i].car_type,
            car_color: resp.data.data[i].car_color,
            car_remark: resp.data.data[i].car_remark,
            dc_type: resp.data.data[i].dc_type,
            dc_causes: resp.data.data[i].dc_causes,
            dc_date:
              date.formatDate(resp.data.data[i].dc_date, 'YYYY-MM-DD HH:mm:ss') ===
              '1000-01-01 00:00:00'
                ? ''
                : date.formatDate(resp.data.data[i].dc_date, 'YYYY-MM-DD HH:mm:ss'),
            dc_address: resp.data.data[i].dc_address,
            dc_key: resp.data.data[i].dc_key,
            dc_party_name: resp.data.data[i].dc_party_name,
            dc_party_cardid: resp.data.data[i].dc_party_cardid,
            dc_party_tel: resp.data.data[i].dc_party_tel,
            dc_name: resp.data.data[i].dc_name,
            dc_acc: resp.data.data[i].dc_acc,
            dc_into_date:
              date.formatDate(resp.data.data[i].dc_into_date, 'YYYY-MM-DD HH:mm:ss') ===
              '1000-01-01 00:00:00'
                ? ''
                : date.formatDate(resp.data.data[i].dc_into_date, 'YYYY-MM-DD HH:mm:ss'),
            p_name: resp.data.data[i].p_name,
            p_id: resp.data.data[i].p_id,
            driver: resp.data.data[i].driver,
            operator: resp.data.data[i].operator,
            drag_km: resp.data.data[i].drag_km,
            drag_unit: resp.data.data[i].drag_unit,
            drag_money: resp.data.data[i].drag_money,
            cmd_unit: resp.data.data[i].cmd_unit,
            cmd_user: resp.data.data[i].cmd_user,
            cv: resp.data.data[i].cv,
            cv_acc: resp.data.data[i].cv_acc,
            cv_name: resp.data.data[i].cv_name,
            cv_date:
              date.formatDate(resp.data.data[i].cv_date, 'YYYY-MM-DD HH:mm:ss') ===
              '1000-01-01 00:00:00'
                ? ''
                : date.formatDate(resp.data.data[i].cv_date, 'YYYY-MM-DD HH:mm:ss'),
            cv_opinion: resp.data.data[i].cv_opinion,
            tv: resp.data.data[i].tv,
            tv_acc: resp.data.data[i].tv_acc,
            tv_name: resp.data.data[i].tv_name,
            tv_date:
              date.formatDate(resp.data.data[i].tv_date, 'YYYY-MM-DD HH:mm:ss') ===
              '1000-01-01 00:00:00'
                ? ''
                : date.formatDate(resp.data.data[i].tv_date, 'YYYY-MM-DD HH:mm:ss'),
            tv_opinion: resp.data.data[i].tv_opinion,
            rc_name: resp.data.data[i].rc_name,
            rc_idcard: resp.data.data[i].rc_idcard,
            rc_tel: resp.data.data[i].rc_tel,
            parking_date: parking_money.date,
            parking_unit: parking_money.unit,
            parking_money: parking_money.payable,
            parking_payable: parking_money.payable,
            parking_paidin: parking_money.paidin,
            remark: resp.data.data[i].remark,
            rs_acc: resp.data.data[i].rs_acc,
            rs_name: resp.data.data[i].rs_name,
            rs_date:
              date.formatDate(resp.data.data[i].rs_date, 'YYYY-MM-DD HH:mm:ss') ===
              '1000-01-01 00:00:00'
                ? ''
                : date.formatDate(resp.data.data[i].rs_date, 'YYYY-MM-DD HH:mm:ss'),
            attachment:
              resp.data.data[i].attachment === '' ? '' : JSON.parse(resp.data.data[i].attachment),
            create_user: resp.data.data[i].create_user,
            create_date: date.formatDate(resp.data.data[i].create_date, 'YYYY-MM-DD HH:mm:ss'),
            update_user: resp.data.data[i].update_user,
            update_date: date.formatDate(resp.data.data[i].update_date, 'YYYY-MM-DD HH:mm:ss'),
          });
        }
      } else {
        store.quasar.notify('网络错误，请稍后重试');
      }

      //调用接口查询不同类型汽车的未放行数量（后面四个接口）
      api
        .post(
          store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/count',
          {
            in_date: search.value.date.in,
            out_date: search.value.date.out,
            content: '小型汽车',
            model: '车辆类型',
            status: '未放行',
            key: search.value.key.model,
            name: '',
            unit: search.value.unit.model,
          },
          {
            headers: {
              Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
            },
          },
        )
        .then((resp) => {
          car_type.value.in.push({
            type: '小型汽车',
            sum: parseInt(resp.data.data),
          });
          api
            .post(
              store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/count',
              {
                in_date: search.value.date.in,
                out_date: search.value.date.out,
                content: '二轮摩托车',
                model: '车辆类型',
                status: '未放行',
                key: search.value.key.model,
                name: '',
                unit: search.value.unit.model,
              },
              {
                headers: {
                  Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
                },
              },
            )
            .then((resp) => {
              car_type.value.in.push({
                type: '二轮摩托车',
                sum: parseInt(resp.data.data),
              });
              api
                .post(
                  store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/count',
                  {
                    in_date: search.value.date.in,
                    out_date: search.value.date.out,
                    content: '三轮摩托车',
                    model: '车辆类型',
                    status: '未放行',
                    key: search.value.key.model,
                    name: '',
                    unit: search.value.unit.model,
                  },
                  {
                    headers: {
                      Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
                    },
                  },
                )
                .then((resp) => {
                  car_type.value.in.push({
                    type: '三轮摩托车',
                    sum: parseInt(resp.data.data),
                  });
                  api
                    .post(
                      store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/count',
                      {
                        in_date: search.value.date.in,
                        out_date: search.value.date.out,
                        content: '',
                        model: '',
                        status: '未放行',
                        key: search.value.key.model,
                        name: '',
                        unit: search.value.unit.model,
                      },
                      {
                        headers: {
                          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
                        },
                      },
                    )
                    .then((resp) => {
                      const sum =
                        parseInt(resp.data.data) -
                        (car_type.value.in[0]?.sum ?? 0) -
                        (car_type.value.in[1]?.sum ?? 0) -
                        (car_type.value.in[2]?.sum ?? 0);
                      car_type.value.in.push({
                        type: '其他车辆',
                        sum: sum,
                      });

                      for (let i = 0; i < car_type.value.in.length; i++) {
                        const item = car_type.value.in[i];
                        if (item) {
                          carRecord.value.txt.push(item.type);
                          carRecord.value.num.push(item.sum);
                        }
                      }
                      carRecord.value.total_num = resp.data.data;
                      carRecord.value.id = 'noreleased';
                      total_num1.value = resp.data.data;
                      // showcar.value = true;
                      initCar(carRecord); //调用柱形图
                    })
                    .catch(() => {
                      store.quasar.notify('网络错误，请稍后重试');
                      loading.value = false;
                      clearTimeout(time);
                    });
                })
                .catch(() => {
                  store.quasar.notify('网络错误，请稍后重试');
                  loading.value = false;
                  clearTimeout(time);
                });
            })
            .catch(() => {
              store.quasar.notify('网络错误，请稍后重试');
              loading.value = false;
              clearTimeout(time);
            });
        })
        .catch(() => {
          store.quasar.notify('网络错误，请稍后重试');
          loading.value = false;
          clearTimeout(time);
        });

      //调用接口查询不同类型汽车的已放行数量（后面四个接口）
      api
        .post(
          store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/count',
          {
            in_date: search.value.date.in,
            out_date: search.value.date.out,
            content: '小型汽车',
            model: '车辆类型',
            status: '已放行',
            key: search.value.key.model,
            name: '',
            unit: search.value.unit.model,
          },
          {
            headers: {
              Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
            },
          },
        )
        .then((resp) => {
          car_type.value.out.push({
            type: '小型汽车',
            sum: parseInt(resp.data.data),
          });

          api
            .post(
              store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/count',
              {
                in_date: search.value.date.in,
                out_date: search.value.date.out,
                content: '二轮摩托车',
                model: '车辆类型',
                status: '已放行',
                key: search.value.key.model,
                name: '',
                unit: search.value.unit.model,
              },
              {
                headers: {
                  Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
                },
              },
            )
            .then((resp) => {
              car_type.value.out.push({
                type: '二轮摩托车',
                sum: parseInt(resp.data.data),
              });

              api
                .post(
                  store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/count',
                  {
                    in_date: search.value.date.in,
                    out_date: search.value.date.out,
                    content: '三轮摩托车',
                    model: '车辆类型',
                    status: '已放行',
                    key: search.value.key.model,
                    name: '',
                    unit: search.value.unit.model,
                  },
                  {
                    headers: {
                      Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
                    },
                  },
                )
                .then((resp) => {
                  car_type.value.out.push({
                    type: '三轮摩托车',
                    sum: parseInt(resp.data.data),
                  });

                  api
                    .post(
                      store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/count',
                      {
                        in_date: search.value.date.in,
                        out_date: search.value.date.out,
                        content: '',
                        model: '',
                        status: '已放行',
                        key: search.value.key.model,
                        name: '',
                        unit: search.value.unit.model,
                      },
                      {
                        headers: {
                          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
                        },
                      },
                    )
                    .then((resp) => {
                      const sum =
                        parseInt(resp.data.data) -
                        (car_type.value.out[0]?.sum ?? 0) -
                        (car_type.value.out[1]?.sum ?? 0) -
                        (car_type.value.out[2]?.sum ?? 0);
                      car_type.value.out.push({
                        type: '其他车辆',
                        sum: sum,
                      });

                      for (let i = 0; i < car_type.value.out.length; i++) {
                        const item = car_type.value.out[i];
                        if (item) {
                          carRecord.value.txt.push(item.type);
                          carRecord.value.num.push(item.sum);
                        }
                      }
                      carRecord.value.total_num = resp.data.data;

                      carRecord.value.id = 'released';
                      total_num2.value = resp.data.data;
                      // showcar1.value = true;
                      initCar(carRecord);
                    })
                    .catch(() => {
                      store.quasar.notify('网络错误，请稍后重试');
                      loading.value = false;
                      clearTimeout(time);
                    });
                })
                .catch(() => {
                  store.quasar.notify('网络错误，请稍后重试');
                  loading.value = false;
                  clearTimeout(time);
                });
            })
            .catch(() => {
              store.quasar.notify('网络错误，请稍后重试');
              loading.value = false;
              clearTimeout(time);
            });
        })
        .catch(() => {
          store.quasar.notify('网络错误，请稍后重试');
          loading.value = false;
          clearTimeout(time);
        });

      pagination.value!.page = page;
      pagination.value!.rowsPerPage = rowsPerPage;
      pagination.value!.sortBy = sortBy;
      pagination.value!.descending = descending;

      loading.value = false;

      clearTimeout(time);
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      loading.value = false;
      clearTimeout(time);
    });
};

const onQuery = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/count',
      {
        in_date: '',
        out_date: '',
        content: '',
        model: '',
        status: '',
        key: '',
        name: '',
        unit: '',
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      sum.value.sum = parseInt(resp.data.data);
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      loading.value = false;
      clearTimeout(time);
    });

  //未放行数量
  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/count',
      {
        in_date: '',
        out_date: '',
        content: '',
        model: '',
        status: '未放行',
        key: '',
        name: '',
        unit: '',
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      sum.value.car_in = parseInt(resp.data.data);
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      loading.value = false;
      clearTimeout(time);
    });
  //已放行数量
  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/count',
      {
        in_date: '',
        out_date: '',
        content: '',
        model: '',
        status: '已放行',
        key: '',
        name: '',
        unit: '',
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      sum.value.car_out = parseInt(resp.data.data);
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      loading.value = false;
      clearTimeout(time);
    });
  //根据选的查询内容进行查询统计
  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/count',
      {
        in_date: search.value.date.in,
        out_date: search.value.date.out,
        content: search.value.content,
        model: search.value.type.model === '' ? '' : search.value.type.model,
        status: search.value.status.model,
        key: search.value.key.model,
        name: '',
        unit: search.value.unit.model,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      pagination.value!.rowsNumber = parseInt(resp.data.data);

      onRequest({ pagination: pagination.value });

      loading.value = false;
      clearTimeout(time);
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      loading.value = false;
      clearTimeout(time);
    });
};

const onView = (val: TableRow) => {
  dialog.value.create = false;
  dialog.value.id = val.id as number;
  dialog.value.car.license = val.license as string;
  dialog.value.car.vehicle = val.vehicle as string;
  dialog.value.car.engine = val.engine as string;
  dialog.value.car.type.model = val.car_type as string;
  dialog.value.car.color.model = val.car_color as string;
  dialog.value.car.remark = val.car_remark as string;
  dialog.value.dc.type.model = val.dc_type as string;
  dialog.value.dc.causes.model = val.dc_causes as string;
  dialog.value.dc.date = val.dc_date as string;
  dialog.value.dc.address = val.dc_address as string;
  dialog.value.dc.key.model = val.dc_key as string;
  dialog.value.dc.party.name = val.dc_party_name as string;
  dialog.value.dc.party.cardid = val.dc_party_cardid as string;
  dialog.value.dc.party.tel = val.dc_party_tel as string;
  dialog.value.dc.name = val.dc_name as string;
  dialog.value.dc.acc = val.dc_acc as string;
  dialog.value.dc.into_date = val.dc_into_date as string;
  dialog.value.p.name = val.p_name as string;
  dialog.value.p.id = val.p_id as string;
  dialog.value.driver = val.driver as string;
  dialog.value.operator = val.operator as string;
  dialog.value.drag.km = String(val.drag_km);
  dialog.value.drag.unit = val.drag_unit as string;
  dialog.value.drag.money = String(val.drag_money);
  dialog.value.cmd.unit.model = val.cmd_unit as string;
  dialog.value.cmd.user = val.cmd_user as string;
  dialog.value.cv.cv.model = val.cv as string;
  dialog.value.cv.acc = val.cv_acc as string;
  dialog.value.cv.name = val.cv_name as string;
  dialog.value.cv.date = val.cv_date as string;
  dialog.value.cv.opinion.model = val.cv_opinion as string;
  dialog.value.tv.tv.model = val.tv as string;
  dialog.value.tv.acc = val.tv_acc as string;
  dialog.value.tv.name = val.tv_name as string;
  dialog.value.tv.date = val.tv_date as string;
  dialog.value.tv.opinion.model = val.tv_opinion as string;
  dialog.value.rc.name = val.rc_name as string;
  dialog.value.rc.idcard = val.rc_idcard as string;
  dialog.value.rc.tel = val.rc_tel as string;
  dialog.value.parking.date = val.parking_date as string;
  dialog.value.parking.unit = val.parking_unit as string;
  dialog.value.parking.money = String(val.parking_money);
  dialog.value.parking.payable = val.parking_payable as number;
  dialog.value.parking.paidin = String(val.parking_paidin);
  dialog.value.remark = val.remark as string;
  dialog.value.rs.acc = val.rs_acc as string;
  dialog.value.rs.name = val.rs_name as string;
  dialog.value.rs.date = val.rs_date as string;
  dialog.value.img.front = '';
  dialog.value.img.back = '';
  dialog.value.img.left = '';
  dialog.value.img.right = '';
  dialog.value.img.back_front = '';
  dialog.value.img.back_back = '';
  dialog.value.img.back_left = '';
  dialog.value.img.back_right = '';
  dialog.value.attachment = [];
  dialog.value.car.delete = val.delete ?? false;

  if (val.attachment) {
    const attachments = JSON.parse(val.attachment);

    for (let index = 0; index < attachments.length; index++) {
      const element = attachments[index];

      switch (element.type) {
        case 'front':
          dialog.value.img.front = 'https://tc.wsscxt.com/upload/' + element.file;
          break;
        case 'back':
          dialog.value.img.back = 'https://tc.wsscxt.com/upload/' + element.file;
          break;
        case 'left':
          dialog.value.img.left = 'https://tc.wsscxt.com/upload/' + element.file;
          break;
        case 'right':
          dialog.value.img.right = 'https://tc.wsscxt.com/upload/' + element.file;
          break;
        case 'back_front':
          dialog.value.img.back_front = 'https://tc.wsscxt.com/upload/' + element.file;
          break;
        case 'back_back':
          dialog.value.img.back_back = 'https://tc.wsscxt.com/upload/' + element.file;
          break;
        case 'back_left':
          dialog.value.img.back_left = 'https://tc.wsscxt.com/upload/' + element.file;
          break;
        case 'back_right':
          dialog.value.img.back_right = 'https://tc.wsscxt.com/upload/' + element.file;
          break;
        default:
          if (dialog.value.attachment[index]) {
            dialog.value.attachment[index]!.file = 'https://tc.wsscxt.com/upload/' + element.file;
          }
          break;
      }
    }
  }

  dialog.value.show = true;
};

const onNew = () => {
  dialog.value.create = true;
  dialog.value.id = 0;
  dialog.value.car.license = '云H';
  dialog.value.car.vehicle = '';
  dialog.value.car.engine = '';
  dialog.value.car.remark = '';
  dialog.value.dc.date = date.formatDate(Date.now(), 'YYYY-MM-DD HH:mm:ss');
  dialog.value.dc.address = '';
  dialog.value.dc.key.model = '拖车';
  dialog.value.dc.party.name = '';
  dialog.value.dc.party.cardid = '';
  dialog.value.dc.party.tel = '';
  dialog.value.dc.name = store.user.name;
  dialog.value.dc.acc = '';
  dialog.value.dc.into_date = date.formatDate(Date.now(), 'YYYY-MM-DD HH:mm:ss');
  dialog.value.p.name = '';
  dialog.value.p.id = '';
  dialog.value.driver = '';
  dialog.value.operator = '';
  dialog.value.drag.km = '';
  dialog.value.drag.unit = '';
  dialog.value.drag.money = '';
  dialog.value.cmd.unit.model = '交警部门';
  dialog.value.cmd.user = '';
  dialog.value.cv.cv.model = '未处罚';
  dialog.value.cv.acc = '';
  dialog.value.cv.name = '';
  dialog.value.cv.date = '';
  dialog.value.cv.opinion.model = '待定';
  dialog.value.tv.tv.model = '未处罚';
  dialog.value.tv.acc = '';
  dialog.value.tv.name = '';
  dialog.value.tv.date = '';
  dialog.value.tv.opinion.model = '待定';
  dialog.value.rc.name = '';
  dialog.value.rc.idcard = '';
  dialog.value.rc.tel = '';
  dialog.value.parking.date = '';
  dialog.value.parking.unit = '';
  dialog.value.parking.money = '';
  dialog.value.parking.payable = 0;
  dialog.value.parking.paidin = '';
  dialog.value.remark = '';
  dialog.value.rs.acc = '';
  dialog.value.rs.name = '';
  dialog.value.rs.date = '';
  dialog.value.img.front = '';
  dialog.value.img.back = '';
  dialog.value.img.left = '';
  dialog.value.img.right = '';
  dialog.value.img.back_front = '';
  dialog.value.img.back_back = '';
  dialog.value.img.back_left = '';
  dialog.value.img.back_right = '';
  dialog.value.attachment = [];
  dialog.value.car.delete = false;
  dialog.value.show = true;
};

const onUpload = (val: string) => {
  switch (val) {
    case 'front':
      if (dialog.value.img.front !== '') {
        window.open(dialog.value.img.front);
      } else {
        dialog.value.selected = val;
        browserFile.value?.click();
      }
      break;
    case 'back':
      if (dialog.value.img.back !== '') {
        window.open(dialog.value.img.back);
      } else {
        dialog.value.selected = val;
        browserFile.value?.click();
      }
      break;
    case 'left':
      if (dialog.value.img.left !== '') {
        window.open(dialog.value.img.left);
      } else {
        dialog.value.selected = val;
        browserFile.value?.click();
      }
      break;
    case 'right':
      if (dialog.value.img.right !== '') {
        window.open(dialog.value.img.right);
      } else {
        dialog.value.selected = val;
        browserFile.value?.click();
      }
      break;
    case 'back_front':
      if (dialog.value.img.back_front !== '') {
        window.open(dialog.value.img.back_front);
      } else {
        dialog.value.selected = val;
        browserFile.value?.click();
      }
      break;
    case 'back_back':
      if (dialog.value.img.back_back !== '') {
        window.open(dialog.value.img.back_back);
      } else {
        dialog.value.selected = val;
        browserFile.value?.click();
      }
      break;
    case 'back_left':
      if (dialog.value.img.back_left !== '') {
        window.open(dialog.value.img.back_left);
      } else {
        dialog.value.selected = val;
        browserFile.value?.click();
      }
      break;
    case 'back_right':
      if (dialog.value.img.back_right !== '') {
        window.open(dialog.value.img.back_right);
      } else {
        dialog.value.selected = val;
        browserFile.value?.click();
      }
      break;
  }
};

const onFileChange = (ev: Event) => {
  const file = (ev.target as HTMLInputElement).files?.[0];

  if (file === undefined) return;

  const reader = new FileReader();
  reader.readAsDataURL(file);

  reader.onload = () => {
    switch (dialog.value.selected) {
      case 'front':
        dialog.value.img.front = reader.result as string;
        dialog.value.attachment.push({
          name: file.name,
          type: 'front',
          file: '',
          data: file,
        });
        break;
      case 'back':
        dialog.value.img.back = reader.result as string;
        dialog.value.attachment.push({
          name: file.name,
          type: 'back',
          file: '',
          data: file,
        });
        break;
      case 'left':
        dialog.value.img.left = reader.result as string;
        dialog.value.attachment.push({
          name: file.name,
          type: 'left',
          file: '',
          data: file,
        });
        break;
      case 'right':
        dialog.value.img.right = reader.result as string;
        dialog.value.attachment.push({
          name: file.name,
          type: 'right',
          file: '',
          data: file,
        });
        break;
      case 'back_front':
        dialog.value.img.back_front = reader.result as string;
        dialog.value.attachment.push({
          name: file.name,
          type: 'back_front',
          file: '',
          data: file,
        });
        break;
      case 'back_back':
        dialog.value.img.back_back = reader.result as string;
        dialog.value.attachment.push({
          name: file.name,
          type: 'back_back',
          file: '',
          data: file,
        });
        break;
      case 'back_left':
        dialog.value.img.back_left = reader.result as string;
        dialog.value.attachment.push({
          name: file.name,
          type: 'back_left',
          file: '',
          data: file,
        });
        break;
      case 'back_right':
        dialog.value.img.back_right = reader.result as string;
        dialog.value.attachment.push({
          name: file.name,
          type: 'back_right',
          file: '',
          data: file,
        });
        break;
    }
  };

  reader.onerror = () => {
    store.quasar.notify('文件读取失败，请重试');
  };
};

interface UploadedFile {
  name: string;
  xhr: { response: string };
}

const onUploaded = (val: { files: readonly UploadedFile[]; xhr: { response: string } }) => {
  const resp = JSON.parse(val.xhr.response);
  if (resp.status === 0) {
    store.quasar.notify('上传失败');
    return;
  }

  for (let index = 0; index < val.files.length; index++) {
    const element = val.files[index];
    if (element) {
      dialog.value.attachment.push({
        name: element.name,
        type: 'attachment',
        file: element.xhr.response,
      });
    }
  }
};

const onAdded = () => {
  if (!dialog.value.create) {
    uploader.value?.reset();
    store.quasar.notify('没有保存权限');
    
  }
};

const onDelete = async () => {
  dialog.value.car.delete = true;
  await onSave();
};

const onSave = async () => {
  if (!dialog.value.create && store.user.level === 4) {
    store.quasar.notify('没有保存权限');
    return;
  }

  if (!license.value?.validate()) return;

  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  const upload = async () => {
    let secucess = true;

    for (let index = 0; index < dialog.value.attachment.length; index++) {
      const element = dialog.value.attachment[index];

      if (element && element.type !== 'attachment') {
        if (element.data?.name === undefined) continue;

        const data = new FormData();
        data.append('file', element.data);

        try {
          const resp = await api.post(
            store.server + '/api/route/post?auth=jwt&service=resource&path=/api/upload/upload',
            data,
            {
              headers: {
                Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
              },
            },
          );
          if (dialog.value.attachment[index]) {
            dialog.value.attachment[index]!.file = resp.data.data;
          }
        } catch {
          secucess = false;
        }
      }
    }

    return secucess;
  };

  if (!(await upload())) {
    store.quasar.notify('网络错误，请稍后重试');
    loading.value = false;
    clearTimeout(time);
    return;
  }

  if (dialog.value.rs.date !== '' && dialog.value.rs.date !== undefined) {
    dialog.value.cv.cv.model = '已处罚';
    dialog.value.cv.date = date.formatDate(Date.now(), 'YYYY-MM-DD HH:mm:ss');
    dialog.value.cv.opinion.model = '同意放行';
  }

  // this.writeTemplater();

  const parking_money = calcPay({
    type: dialog.value.car.type.model,
    date_in: dialog.value.dc.into_date,
    date_out: dialog.value.rs.date,
  });

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/update',
      {
        id: dialog.value.id,
        license: dialog.value.car.license,
        vehicle: dialog.value.car.vehicle,
        engine: dialog.value.car.engine,
        car_remark: dialog.value.car.remark,
        car_type: dialog.value.car.type.model,
        dc_type: dialog.value.dc.type.model,
        dc_causes: dialog.value.dc.causes.model,
        car_color: dialog.value.car.color.model,
        dc_address: dialog.value.dc.address,
        dc_key: dialog.value.dc.key.model,
        dc_party_name: dialog.value.dc.party.name,
        dc_party_cardid: dialog.value.dc.party.cardid,
        dc_party_tel: dialog.value.dc.party.tel,
        p_name: dialog.value.p.name,
        p_id: dialog.value.p.id,
        dc_acc: dialog.value.dc.acc,
        dc_name: dialog.value.dc.name,
        dc_date: dialog.value.dc.date,
        dc_into_date: dialog.value.dc.into_date,
        remark: dialog.value.remark,
        driver: dialog.value.driver,
        operator: dialog.value.operator,
        drag_km: dialog.value.drag.km,
        drag_unit: dialog.value.drag.unit,
        drag_money: dialog.value.drag.money,
        cmd_unit: dialog.value.cmd.unit.model,
        cmd_user: dialog.value.cmd.user,
        cv: dialog.value.cv.cv.model,
        cv_acc: dialog.value.cv.acc,
        cv_name: dialog.value.cv.name,
        cv_date: dialog.value.cv.date,
        cv_opinion: dialog.value.cv.opinion.model,
        tv: dialog.value.tv.tv.model,
        tv_acc: dialog.value.tv.acc,
        tv_name: dialog.value.tv.name,
        tv_date: dialog.value.tv.date,
        tv_opinion: dialog.value.tv.opinion.model,
        rs_name: dialog.value.rs.name,
        rs_date: dialog.value.rs.date,
        parking_date: dialog.value.dc.into_date,
        parking_unit: parking_money.unit,
        parking_money: parking_money.payable,
        parking_payable: parking_money.payable,
        parking_paidin: parking_money.paidin,
        attachment: JSON.stringify(dialog.value.attachment),
        delete: dialog.value.car.delete,
        create_user: dialog.value.create ? store.user.name : '',
        create_date: dialog.value.create ? date.formatDate(Date.now(), 'YYYY-MM-DD HH:mm:ss') : '',
        update_user: store.user.name,
        update_date: date.formatDate(Date.now(), 'YYYY-MM-DD HH:mm:ss'),
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      if (resp.data.status === 1) {
        dialog.value.show = false;
        store.quasar.notify('保存成功');
        onQuery();
      } else {
        store.quasar.notify('保存失败，请稍后重试');
      }

      loading.value = false;
      clearTimeout(time);
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      loading.value = false;
      clearTimeout(time);
    });
};

const calcPay = (val: { type: string; date_in: string; date_out: string }) => {
  if (
    val.type === '' ||
    val.date_in === null ||
    val.date_in === undefined ||
    val.date_in === '' ||
    val.date_in === '1000-01-01 00:00:00'
  ) {
    return {
      date: '未知',
      unit: '',
      payable: 0,
      paidin: 0,
    };
  }
  try {
    //filter()数组筛选当前类型
    const type = store.options.tow.car.type.filter(
      (v: TowCarOptionItem) => {
        return v.type === val.type;
      },
    );
    const val_class = store.options.tow.car.class.filter(
      (v: TowCarOptionItem) => {
        return v.id === type[0]?.ccid;
      },
    );
    
    if (!val_class || val_class.length === 0) {
       return {
      date: '未知',
      unit: '',
      payable: 0,
      paidin: 0,
    };
    }

    //格式化指定形式的日期
    const now = date.formatDate(
      val.date_out === null ||
        val.date_out === undefined ||
        val.date_out === '' ||
        val.date_out === '1000-01-01 00:00:00'
        ? Date.now()
        : val.date_out,
      'YYYY-MM-DD HH:mm:ss',
    );
    //将间隔时间转化为天数
    let time = date.getDateDiff(now, date.formatDate(val.date_in, 'YYYY-MM-DD HH:mm:ss'), 'days');

    time = Math.ceil(time);

    let payable = 0;
    const costDay = parseInt(String(val_class[0]?.cost_day ?? '0'));
    const gratisDay = parseInt(String(val_class[0]?.gratis_day ?? '0'));
    
    if (time > costDay) {
      //计算停车费
      payable = gratisDay * (time - costDay);
    }
    return {
      date: time.toString(),
      unit: String(val_class[0]?.cpt ?? '') + ': ' + String(val_class[0]?.remark ?? ''),
      payable: payable,
      paidin: 0,
    };
  } catch {
    return {
      date: '未知',
      unit: '',
      payable: 0,
      paidin: 0,
    };
  }
};

api
  .get(store.server + '/api/route/get?auth=jwt&service=tow&path=/api/options/car_class', {
    headers: {
      Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
    },
  })
  .then((resp) => {
    if (resp.data.status !== 0) {
      store.options.tow.car.class = resp.data.data;
    } else {
      store.quasar.notify('网络错误，请稍后重试');
    }
  })
  .catch(() => {
    store.quasar.notify('网络错误，请稍后重试');
  });

api
  .get(store.server + '/api/route/get?auth=jwt&service=tow&path=/api/options/car_type', {
    headers: {
      Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
    },
  })
  .then((resp) => {
    if (resp.data.status !== 0) {
      store.options.tow.car.type = resp.data.data;
    } else {
      store.quasar.notify('网络错误，请稍后重试');
    }
  })
  .catch(() => {
    store.quasar.notify('网络错误，请稍后重试');
  });

api
  .get(store.server + '/api/route/get?auth=jwt&service=tow&path=/api/options/car_color', {
    headers: {
      Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
    },
  })
  .then((resp) => {
    if (resp.data.status !== 0) {
      store.options.tow.car.color = resp.data.data;
    } else {
      store.quasar.notify('网络错误，请稍后重试');
    }
  })
  .catch(() => {
    store.quasar.notify('网络错误，请稍后重试');
  });

api
  .get(store.server + '/api/route/get?auth=jwt&service=tow&path=/api/options/causes_type', {
    headers: {
      Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
    },
  })
  .then((resp) => {
    if (resp.data.status !== 0) {
      store.options.tow.causes.type = resp.data.data;
    } else {
      store.quasar.notify('网络错误，请稍后重试');
    }
  })
  .catch(() => {
    store.quasar.notify('网络错误，请稍后重试');
  });

api
  .get(store.server + '/api/route/get?auth=jwt&service=tow&path=/api/options/causes', {
    headers: {
      Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
    },
  })
  .then((resp) => {
    if (resp.data.status !== 0) {
      store.options.tow.causes.list = resp.data.data;
    } else {
      store.quasar.notify('网络错误，请稍后重试');
    }
  })
  .catch(() => {
    store.quasar.notify('网络错误，请稍后重试');
  });
</script>
