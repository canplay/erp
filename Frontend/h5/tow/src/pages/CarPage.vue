<template>
  <q-page class="bg-white">
    <q-splitter v-model="splitterModel">
      <template v-slot:before>
        <q-tabs v-model="tab" vertical>
          <q-tab name="all" icon="filter_1" label="全部" />
          <q-tab name="jj" icon="filter_2" label="交警" />
          <q-tab name="cg" icon="filter_3" label="城管" />
          <q-tab name="xf" icon="filter_4" label="消防" />
        </q-tabs>
      </template>

      <template v-slot:after>
        <q-table
          dense
          :rows="rows"
          :columns="columns"
          row-key="id"
          separator="cell"
          v-model:pagination="pagination"
          :rows-per-page-options="[10, 20, 30, 40, 50, 0]"
          @request="onRequest"
        >
          <template v-slot:top>
            <div class="column fit">
              <div class="col row">
                <q-select
                  class="col-2"
                  v-model="search.status.model"
                  :options="search.status.options"
                  label="车辆状态"
                />

                <div class="col-auto" style="width: 10px" />

                <q-select
                  class="col-2"
                  v-model="search.key.model"
                  :options="search.key.options"
                  label="扣车形式"
                />

                <div class="col-auto" style="width: 10px" />

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

                <div class="col-auto" style="width: 10px" />

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
              </div>

              <div class="col-auto" style="height: 10px" />

              <div class="col row">
                <q-select
                  class="col-2"
                  v-model="search.type.model"
                  :options="search.type.options"
                  label="查询类型"
                />

                <div class="col-auto" style="width: 10px" />

                <q-input class="col" v-model="search.content" label="查询内容" />

                <div class="col-auto" style="width: 10px" />

                <q-btn class="col-1" color="green" label="导出" @click="onExport" />

                <div class="col-auto" style="width: 10px" />

                <q-btn class="col-1" color="red" label="重置" @click="onReset" />

                <div class="col-auto" style="width: 10px" />

                <q-btn class="col-1" color="primary" label="查询" @click="onQuery" />

                <div class="col-auto" style="width: 10px" />

                <q-btn class="col-1" color="red" label="新增" @click="onNew" />
              </div>
            </div>
          </template>

          <template v-slot:header-cell="props">
            <q-th
              :props="props"
              style="
                font:
                  bold 15px arial,
                  sans-serif;
              "
            >
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
              <div class="col row" style="font-size: 16px">
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
              </div>

              <div class="col row" style="font-size: 16px">
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
              </div>

              <div class="row items-center" style="font-size: 16px">
                <div class="col row">
                  <div class="col-auto">总数量：{{ sum.sum }}</div>
                  <div class="col-auto" style="width: 10px" />
                  <div class="col-auto text-red">总在场：{{ sum.car_in }}</div>
                  <div class="col-auto" style="width: 10px" />
                  <div class="col-auto text-green">总放行：{{ sum.car_out }}</div>
                </div>

                <div class="col-auto">每页显示：</div>

                <div class="col-auto">
                  <q-select
                    dense
                    borderless
                    v-model="pagination.rowsPerPage"
                    :options="pagination.options"
                    @update:model-value="onRowsPerPage"
                  />
                </div>

                <div class="col-auto" style="width: 10px" />

                <div class="col-auto">{{ scope.pagination.page }} - {{ scope.pagesNumber }}</div>

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
          </template>
        </q-table>
      </template>
    </q-splitter>

    <q-dialog v-model="dialog.show" persistent>
      <q-card style="min-width: 70%">
        <q-card-section>
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

            <div class="col-auto" style="width: 10px" />

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

            <div class="col-auto" style="width: 10px" />

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

            <div class="col-auto" style="width: 10px" />

            <q-input class="col" v-model="dialog.dc.party.cardid" label="当事人身份证" />

            <div class="col-auto" style="width: 10px" />

            <q-input class="col" v-model="dialog.dc.party.tel" label="当事人联系电话" />
          </div>

          <div class="col-auto" style="height: 10px" />

          <div class="row">
            <q-input class="col" v-model="dialog.p.name" label="执勤人" />

            <div class="col-auto" style="width: 10px" />

            <q-input readonly class="col" v-model="dialog.dc.name" label="记录人" />

            <div class="col-auto" style="width: 10px" />

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

            <div class="col-auto" style="width: 10px" />

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

            <div class="col-auto" style="width: 10px" />

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

            <div class="col-auto" style="width: 10px" />

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

            <div class="col-auto" style="width: 10px" />

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

            <div class="col-auto" style="width: 10px" />

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

            <div class="col-auto" style="width: 10px" />

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

          <div class="col-auto" style="height: 10px" />

          <q-card class="col">
            <q-card-section class="row">
              <div class="col-auto">模板：</div>
              <div class="col-auto" style="width: 10px" />
              <a class="col-auto" href="/template/处罚决定.xlsx"> 处罚决定.xlsx </a>
              <div class="col-auto" style="width: 10px" />
              <a class="col-auto" href="/template/案卷记录.xlsx"> 案卷记录.xlsx </a>
              <div class="col-auto" style="width: 10px" />
              <a class="col-auto" href="/template/处罚决定.docx"> 处罚决定.docx </a>
              <div class="col-auto" style="width: 10px" />
              <a class="col-auto" href="/template/案卷记录.docx"> 案卷记录.docx </a>
            </q-card-section>
          </q-card>

          <div class="col-auto" style="height: 10px" />

          <q-card v-if="!dialog.create" class="col">
            <q-card-section class="row">
              附件：
              <br />
              <div v-for="(item, id) in this.dialog.attachment" :key="id">
                <a
                  v-if="item.type === 'attachment'"
                  :href="item.file"
                  target="_blank"
                  style="margin-right: 10px"
                >
                  {{ item.name }}
                </a>
              </div>
            </q-card-section>
          </q-card>

          <div class="col-auto" style="height: 10px" />

          <div>
            <input type="file" accept="image/*" hidden @change="onFileChange" ref="browserFile" />

            <q-uploader
              square
              float
              with-credentials
              :url="this.store.backend"
              label="附件"
              multiple
              class="fit"
              accept="*/*"
              @added="onAdded"
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

          <div class="col-auto" style="height: 10px" />

          <div class="row">
            <q-btn
              v-if="this.store.user.username === 'lmy'"
              class="col"
              color="warning"
              label="删除"
              style="font-size: 20px"
              @click="onDelete"
            />

            <div class="col-auto" style="width: 10px" />

            <q-btn
              class="col"
              color="negative"
              label="取消"
              style="font-size: 20px"
              v-close-popup
            />

            <div class="col-auto" style="width: 10px" />

            <q-btn
              class="col"
              color="positive"
              label="保存"
              style="font-size: 20px"
              @click="onSave('new')"
            />
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
</template>

<script>
import { date, uid, exportFile } from 'quasar'
import Docxtemplater from 'docxtemplater'
import PizZip from 'pizzip'
import PizZipUtils from 'pizzip/utils/index.js'
import { useStore } from 'src/stores/store'

export default {
  name: 'PageCar',

  data() {
    return {
      store: useStore(),
      tab: 'all',
      splitterModel: 10,
      sum: {
        sum: 0,
        car_in: 0,
        car_out: 0,
      },
      search: {
        type: {
          model: '',
          options: [
            '',
            '车牌',
            '车辆类型',
            '车身颜色',
            '车架号',
            '发动机号',
            '车辆备注',
            '拖移备注',
          ],
        },
        status: {
          model: '未放行',
          options: ['全部', '未放行', '已放行'],
        },
        key: {
          model: '拖车',
          options: ['全部', '拖车', '锁车'],
        },
        date: {
          in: '',
          out: '',
        },
        content: '',
      },
      pagination: {
        page: 1,
        rowsNumber: 0,
        rowsPerPage: 20,
        sortBy: 'dc_date',
        descending: false,
        options: [10, 20, 30, 40, 50, '全部'],
      },
      columns: [
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
      ],
      rows: [],
      unit: '',
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
      sheet: {
        show: false,
        content: '',
      },
      car_type: {
        in: [],
        out: [],
      },
    }
  },

  watch: {
    tab(newVal) {
      switch (newVal) {
        case 'jj':
          this.unit = '交警部门'
          this.onQuery()
          break
        case 'cg':
          this.unit = '城管部门'
          this.onQuery()
          break
        case 'xf':
          this.unit = '消防部门'
          this.onQuery()
          break
        default:
          this.unit = ''
          this.onQuery()
          break
      }
    },
  },

  methods: {
    onReset() {
      this.search.status.model = '全部'
      this.search.key.model = '全部'
      this.search.type.model = ''
      this.search.date.in = ''
      this.search.date.out = ''
      this.search.content = ''
    },

    wrapCsvValue(val, formatFn) {
      let formatted = formatFn !== void 0 ? formatFn(val) : val

      formatted = formatted === void 0 || formatted === null ? '' : String(formatted)

      formatted = formatted.split('"').join('""')

      return `${formatted}`
    },

    onExport() {
      let time = setTimeout(() => {
        this.$q.loading.hide()
        clearTimeout(time)
      }, 120000)

      this.$q.loading.show()

      let rows = []
      let pagination = {
        page: 1,
        rowsNumber: 0,
        rowsPerPage: 20,
        sortBy: 'dc_date',
        descending: false,
        options: [10, 20, 30, 40, 50, '全部'],
      }

      this.$axios
        .post(this.store.backend + '/car', {
          method: 'count',
          in_date: this.search.date.in,
          out_date: this.search.date.out,
          content: this.search.type.model === '' ? '' : this.search.content,
          model: this.search.content === '' ? '' : this.search.type.model,
          status: this.search.status.model,
          key: this.search.key.model,
          name: this.search.content,
          unit: this.unit,
        })
        .then((resp) => {
          pagination.rowsNumber = parseInt(resp.data)

          this.$axios
            .post(this.store.backend + '/car', {
              method: 'query',
              in_date: this.search.date.in,
              out_date: this.search.date.out,
              content: this.search.type.model === '' ? '' : this.search.content,
              model: this.search.content === '' ? '' : this.search.type.model,
              cur: (pagination.page - 1) * pagination.rowsPerPage,
              max: pagination.rowsNumber,
              sortBy: pagination.sortBy === null ? '' : pagination.sortBy,
              descending: pagination.descending,
              status: this.search.status.model,
              key: this.search.key.model,
              unit: this.unit,
            })
            .then((resp) => {
              if (resp.status === 200) {
                let json = JSON.parse(resp.data)

                for (let i = 0; i < json.value.length; ++i) {
                  if (json.value[i].id === '') break

                  let attachment = []
                  if (json.value[i].attachment && json.value[i].attachment != '') {
                    let attachments = json.value[i].attachment.split(';')
                    for (let index = 0; index < attachments.length; index++) {
                      const element = attachments[index]
                      attachment.push(element)
                    }
                  }

                  let parking_money = this.calcPay({
                    type: json.value[i].car_type,
                    date_in: json.value[i].dc_into_date,
                    date_out: json.value[i].rs_date,
                  })

                  rows.push({
                    id: json.value[i].id,
                    license: json.value[i].license,
                    vehicle: json.value[i].vehicle,
                    engine: json.value[i].engine,
                    car_type: json.value[i].car_type,
                    car_color: json.value[i].car_color,
                    car_remark: json.value[i].car_remark,
                    dc_type: json.value[i].dc_type,
                    dc_causes: json.value[i].dc_causes,
                    dc_date: date.formatDate(json.value[i].dc_date, 'YYYY-MM-DD HH:mm:ss'),
                    dc_address: json.value[i].dc_address,
                    dc_key: json.value[i].dc_key,
                    dc_party_name: json.value[i].dc_party_name,
                    dc_party_cardid: json.value[i].dc_party_cardid,
                    dc_party_tel: json.value[i].dc_party_tel,
                    dc_name: json.value[i].dc_name,
                    dc_acc: json.value[i].dc_acc,
                    dc_into_date: date.formatDate(
                      json.value[i].dc_into_date,
                      'YYYY-MM-DD HH:mm:ss',
                    ),
                    p_name: json.value[i].p_name,
                    p_id: json.value[i].p_id,
                    driver: json.value[i].driver,
                    operator: json.value[i].operator,
                    drag_km: json.value[i].drag_km,
                    drag_unit: json.value[i].drag_unit,
                    drag_money: json.value[i].drag_money,
                    cmd_unit: json.value[i].cmd_unit,
                    cmd_user: json.value[i].cmd_user,
                    cv: json.value[i].cv,
                    cv_acc: json.value[i].cv_acc,
                    cv_name: json.value[i].cv_name,
                    cv_date: date.formatDate(json.value[i].cv_date, 'YYYY-MM-DD HH:mm:ss'),
                    cv_opinion: json.value[i].cv_opinion,
                    tv: json.value[i].tv,
                    tv_acc: json.value[i].tv_acc,
                    tv_name: json.value[i].tv_name,
                    tv_date: date.formatDate(json.value[i].tv_date, 'YYYY-MM-DD HH:mm:ss'),
                    tv_opinion: json.value[i].tv_opinion,
                    rc_name: json.value[i].rc_name,
                    rc_idcard: json.value[i].rc_idcard,
                    rc_tel: json.value[i].rc_tel,
                    parking_date: parking_money.date,
                    parking_unit: parking_money.unit,
                    parking_money: parking_money.payable,
                    parking_payable: parking_money.payable,
                    parking_paidin: parking_money.paidin,
                    remark: json.value[i].remark,
                    rs_acc: json.value[i].rs_acc,
                    rs_name: json.value[i].rs_name,
                    rs_date: date.formatDate(json.value[i].rs_date, 'YYYY-MM-DD HH:mm:ss'),
                    attachment: json.value[i].attachment,
                    create_user: json.value[i].create_user,
                    create_date: date.formatDate(json.value[i].create_date, 'YYYY-MM-DD HH:mm:ss'),
                    update_user: json.value[i].update_user,
                    update_date: date.formatDate(json.value[i].update_date, 'YYYY-MM-DD HH:mm:ss'),
                  })
                }

                const content = [this.columns.map((col) => this.wrapCsvValue(col.label))]
                  .concat(
                    rows.map((row) =>
                      this.columns
                        .map((col) =>
                          this.wrapCsvValue(
                            typeof col.field === 'function'
                              ? col.field(row)
                              : row[col.field === void 0 ? col.name : col.field],
                            col.format,
                          ),
                        )
                        .join(','),
                    ),
                  )
                  .join('\r\n')

                this.$q.loading.hide()
                clearTimeout(time)

                let status = exportFile('车辆列表.csv', content, 'text/csv')

                if (status == true) {
                  this.$q.notify('即将开始下载，请稍后...')
                } else {
                  this.$q.notify('下载失败，请重试')
                }
              } else {
                this.$q.notify('网络错误，请稍后重试')
              }
            })
        })
        .catch(() => {
          this.$q.notify('网络错误，请稍后重试')
          this.$q.loading.hide()
          clearTimeout(time)
        })
    },

    onInDate(val) {
      if (val === null) {
        this.search.date.in = ''
        return
      }

      if (typeof val === 'object') {
        this.search.date.in = val.from + ' - ' + val.to
      }
    },

    onOutDate(val) {
      if (val === null) {
        this.search.date.out = ''
        return
      }

      if (typeof val === 'object') {
        this.search.date.out = val.from + ' - ' + val.to
      }
    },

    onRowsPerPage(val) {
      if (val === '全部') {
        this.pagination.rowsPerPage =
          this.car_type.in[0].sum +
          this.car_type.in[1].sum +
          this.car_type.in[2].sum +
          this.car_type.in[3].sum +
          this.car_type.out[0].sum +
          this.car_type.out[1].sum +
          this.car_type.out[2].sum +
          this.car_type.out[3].sum
      }
      this.onQuery()
    },

    onRequest(props) {
      let time = setTimeout(() => {
        this.$q.loading.hide()
        clearTimeout(time)
      }, 120000)

      this.$q.loading.show()

      let { page, rowsPerPage, sortBy, descending } = props.pagination

      this.rows = []
      this.car_type.in = []
      this.car_type.out = []

      this.$axios
        .post(this.store.backend + '/car', {
          method: 'query',
          in_date: this.search.date.in,
          out_date: this.search.date.out,
          content: this.search.type.model === '' ? '' : this.search.content,
          model: this.search.content === '' ? '' : this.search.type.model,
          cur: (page - 1) * rowsPerPage,
          max: rowsPerPage,
          sortBy: sortBy === null ? '' : sortBy,
          descending: descending,
          status: this.search.status.model,
          key: this.search.key.model,
          unit: this.unit,
        })
        .then((resp) => {
          if (resp.status === 200) {
            let json = JSON.parse(resp.data)

            for (let i = 0; i < json.value.length; ++i) {
              if (json.value[i].id === '') break

              let attachment = []
              if (json.value[i].attachment && json.value[i].attachment != '') {
                let attachments = json.value[i].attachment.split(';')
                for (let index = 0; index < attachments.length; index++) {
                  const element = attachments[index]
                  attachment.push(element)
                }
              }

              let parking_money = this.calcPay({
                type: json.value[i].car_type,
                date_in: json.value[i].dc_into_date,
                date_out: json.value[i].rs_date,
              })

              this.rows.push({
                id: json.value[i].id,
                license: json.value[i].license,
                vehicle: json.value[i].vehicle,
                engine: json.value[i].engine,
                car_type: json.value[i].car_type,
                car_color: json.value[i].car_color,
                car_remark: json.value[i].car_remark,
                dc_type: json.value[i].dc_type,
                dc_causes: json.value[i].dc_causes,
                dc_date:
                  date.formatDate(json.value[i].dc_date, 'YYYY-MM-DD HH:mm:ss') ===
                  '1000-01-01 00:00:00'
                    ? ''
                    : date.formatDate(json.value[i].dc_date, 'YYYY-MM-DD HH:mm:ss'),
                dc_address: json.value[i].dc_address,
                dc_key: json.value[i].dc_key,
                dc_party_name: json.value[i].dc_party_name,
                dc_party_cardid: json.value[i].dc_party_cardid,
                dc_party_tel: json.value[i].dc_party_tel,
                dc_name: json.value[i].dc_name,
                dc_acc: json.value[i].dc_acc,
                dc_into_date:
                  date.formatDate(json.value[i].dc_into_date, 'YYYY-MM-DD HH:mm:ss') ===
                  '1000-01-01 00:00:00'
                    ? ''
                    : date.formatDate(json.value[i].dc_into_date, 'YYYY-MM-DD HH:mm:ss'),
                p_name: json.value[i].p_name,
                p_id: json.value[i].p_id,
                driver: json.value[i].driver,
                operator: json.value[i].operator,
                drag_km: json.value[i].drag_km,
                drag_unit: json.value[i].drag_unit,
                drag_money: json.value[i].drag_money,
                cmd_unit: json.value[i].cmd_unit,
                cmd_user: json.value[i].cmd_user,
                cv: json.value[i].cv,
                cv_acc: json.value[i].cv_acc,
                cv_name: json.value[i].cv_name,
                cv_date:
                  date.formatDate(json.value[i].cv_date, 'YYYY-MM-DD HH:mm:ss') ===
                  '1000-01-01 00:00:00'
                    ? ''
                    : date.formatDate(json.value[i].cv_date, 'YYYY-MM-DD HH:mm:ss'),
                cv_opinion: json.value[i].cv_opinion,
                tv: json.value[i].tv,
                tv_acc: json.value[i].tv_acc,
                tv_name: json.value[i].tv_name,
                tv_date:
                  date.formatDate(json.value[i].tv_date, 'YYYY-MM-DD HH:mm:ss') ===
                  '1000-01-01 00:00:00'
                    ? ''
                    : date.formatDate(json.value[i].tv_date, 'YYYY-MM-DD HH:mm:ss'),
                tv_opinion: json.value[i].tv_opinion,
                rc_name: json.value[i].rc_name,
                rc_idcard: json.value[i].rc_idcard,
                rc_tel: json.value[i].rc_tel,
                parking_date: parking_money.date,
                parking_unit: parking_money.unit,
                parking_money: parking_money.payable,
                parking_payable: parking_money.payable,
                parking_paidin: parking_money.paidin,
                remark: json.value[i].remark,
                rs_acc: json.value[i].rs_acc,
                rs_name: json.value[i].rs_name,
                rs_date:
                  date.formatDate(json.value[i].rs_date, 'YYYY-MM-DD HH:mm:ss') ===
                  '1000-01-01 00:00:00'
                    ? ''
                    : date.formatDate(json.value[i].rs_date, 'YYYY-MM-DD HH:mm:ss'),
                attachment: json.value[i].attachment,
                create_user: json.value[i].create_user,
                create_date: date.formatDate(json.value[i].create_date, 'YYYY-MM-DD HH:mm:ss'),
                update_user: json.value[i].update_user,
                update_date: date.formatDate(json.value[i].update_date, 'YYYY-MM-DD HH:mm:ss'),
              })
            }
          } else {
            this.$q.notify('网络错误，请稍后重试')
          }

          this.$axios
            .post(this.store.backend + '/car', {
              method: 'count',
              in_date: this.search.date.in,
              out_date: this.search.date.out,
              content: '小型汽车',
              model: '车辆类型',
              status: '未放行',
              key: this.search.key.model,
              name: this.search.content,
              unit: this.unit,
            })
            .then((resp) => {
              this.car_type.in.push({
                type: '小型汽车',
                sum: parseInt(resp.data),
              })

              this.$axios
                .post(this.store.backend + '/car', {
                  method: 'count',
                  in_date: this.search.date.in,
                  out_date: this.search.date.out,
                  content: '二轮摩托车',
                  model: '车辆类型',
                  status: '未放行',
                  key: this.search.key.model,
                  name: this.search.content,
                  unit: this.unit,
                })
                .then((resp) => {
                  this.car_type.in.push({
                    type: '二轮摩托车',
                    sum: parseInt(resp.data),
                  })

                  this.$axios
                    .post(this.store.backend + '/car', {
                      method: 'count',
                      in_date: this.search.date.in,
                      out_date: this.search.date.out,
                      content: '三轮摩托车',
                      model: '车辆类型',
                      status: '未放行',
                      key: this.search.key.model,
                      name: this.search.content,
                      unit: this.unit,
                    })
                    .then((resp) => {
                      this.car_type.in.push({
                        type: '三轮摩托车',
                        sum: parseInt(resp.data),
                      })

                      this.$axios
                        .post(this.store.backend + '/car', {
                          method: 'count',
                          in_date: this.search.date.in,
                          out_date: this.search.date.out,
                          content: '',
                          model: '',
                          status: '未放行',
                          key: this.search.key.model,
                          name: this.search.content,
                          unit: this.unit,
                        })
                        .then((resp) => {
                          let sum =
                            parseInt(resp.data) -
                            this.car_type.in[0].sum -
                            this.car_type.in[1].sum -
                            this.car_type.in[2].sum
                          this.car_type.in.push({
                            type: '其他车辆',
                            sum: sum,
                          })
                        })
                        .catch(() => {
                          this.$q.notify('网络错误，请稍后重试')
                          this.$q.loading.hide()
                          clearTimeout(time)
                        })
                    })
                    .catch(() => {
                      this.$q.notify('网络错误，请稍后重试')
                      this.$q.loading.hide()
                      clearTimeout(time)
                    })
                })
                .catch(() => {
                  this.$q.notify('网络错误，请稍后重试')
                  this.$q.loading.hide()
                  clearTimeout(time)
                })
            })
            .catch(() => {
              this.$q.notify('网络错误，请稍后重试')
              this.$q.loading.hide()
              clearTimeout(time)
            })

          this.$axios
            .post(this.store.backend + '/car', {
              method: 'count',
              in_date: this.search.date.in,
              out_date: this.search.date.out,
              content: '小型汽车',
              model: '车辆类型',
              status: '已放行',
              key: this.search.key.model,
              name: this.search.content,
              unit: this.unit,
            })
            .then((resp) => {
              this.car_type.out.push({
                type: '小型汽车',
                sum: parseInt(resp.data),
              })

              this.$axios
                .post(this.store.backend + '/car', {
                  method: 'count',
                  in_date: this.search.date.in,
                  out_date: this.search.date.out,
                  content: '二轮摩托车',
                  model: '车辆类型',
                  status: '已放行',
                  key: this.search.key.model,
                  name: this.search.content,
                  unit: this.unit,
                })
                .then((resp) => {
                  this.car_type.out.push({
                    type: '二轮摩托车',
                    sum: parseInt(resp.data),
                  })

                  this.$axios
                    .post(this.store.backend + '/car', {
                      method: 'count',
                      in_date: this.search.date.in,
                      out_date: this.search.date.out,
                      content: '三轮摩托车',
                      model: '车辆类型',
                      status: '已放行',
                      key: this.search.key.model,
                      name: this.search.content,
                      unit: this.unit,
                    })
                    .then((resp) => {
                      this.car_type.out.push({
                        type: '三轮摩托车',
                        sum: parseInt(resp.data),
                      })

                      this.$axios
                        .post(this.store.backend + '/car', {
                          method: 'count',
                          in_date: this.search.date.in,
                          out_date: this.search.date.out,
                          content: '',
                          model: '',
                          status: '已放行',
                          key: this.search.key.model,
                          name: this.search.content,
                          unit: this.unit,
                        })
                        .then((resp) => {
                          let sum =
                            parseInt(resp.data) -
                            this.car_type.out[0].sum -
                            this.car_type.out[1].sum -
                            this.car_type.out[2].sum
                          this.car_type.out.push({
                            type: '其他车辆',
                            sum: sum,
                          })
                        })
                        .catch(() => {
                          this.$q.notify('网络错误，请稍后重试')
                          this.$q.loading.hide()
                          clearTimeout(time)
                        })
                    })
                    .catch(() => {
                      this.$q.notify('网络错误，请稍后重试')
                      this.$q.loading.hide()
                      clearTimeout(time)
                    })
                })
                .catch(() => {
                  this.$q.notify('网络错误，请稍后重试')
                  this.$q.loading.hide()
                  clearTimeout(time)
                })
            })
            .catch(() => {
              this.$q.notify('网络错误，请稍后重试')
              this.$q.loading.hide()
              clearTimeout(time)
            })

          this.pagination.page = page
          this.pagination.rowsPerPage = rowsPerPage
          this.pagination.sortBy = sortBy
          this.pagination.descending = descending

          this.$q.loading.hide()
          clearTimeout(time)
        })
        .catch(() => {
          this.$q.notify('网络错误，请稍后重试')
          this.$q.loading.hide()
          clearTimeout(time)
        })
    },

    onQuery() {
      let time = setTimeout(() => {
        this.$q.loading.hide()
        clearTimeout(time)
      }, 120000)

      this.$q.loading.show()

      this.$axios
        .post(this.store.backend + '/car', {
          method: 'count',
          in_date: '',
          out_date: '',
          content: '',
          model: '',
          status: '',
          key: '',
          unit: '',
        })
        .then((resp) => {
          this.sum.sum = parseInt(resp.data)
        })
        .catch(() => {
          this.$q.notify('网络错误，请稍后重试')
          this.$q.loading.hide()
          clearTimeout(time)
        })

      this.$axios
        .post(this.store.backend + '/car', {
          method: 'count',
          in_date: '',
          out_date: '',
          content: '',
          model: '',
          status: '未放行',
          key: '',
          unit: '',
        })
        .then((resp) => {
          this.sum.car_in = parseInt(resp.data)
        })
        .catch(() => {
          this.$q.notify('网络错误，请稍后重试')
          this.$q.loading.hide()
          clearTimeout(time)
        })

      this.$axios
        .post(this.store.backend + '/car', {
          method: 'count',
          in_date: '',
          out_date: '',
          content: '',
          model: '',
          status: '已放行',
          key: '',
          unit: '',
        })
        .then((resp) => {
          this.sum.car_out = parseInt(resp.data)
        })
        .catch(() => {
          this.$q.notify('网络错误，请稍后重试')
          this.$q.loading.hide()
          clearTimeout(time)
        })

      this.$axios
        .post(this.store.backend + '/car', {
          method: 'count',
          in_date: this.search.date.in,
          out_date: this.search.date.out,
          content: this.search.type.model === '' ? '' : this.search.content,
          model: this.search.content === '' ? '' : this.search.type.model,
          status: this.search.status.model,
          key: this.search.key.model,
          name: this.search.content,
          unit: this.unit,
        })
        .then((resp) => {
          this.pagination.rowsNumber = parseInt(resp.data)
          this.onRequest({ pagination: this.pagination })

          this.$q.loading.hide()
          clearTimeout(time)
        })
        .catch(() => {
          this.$q.notify('网络错误，请稍后重试')
          this.$q.loading.hide()
          clearTimeout(time)
        })
    },

    onView(val) {
      this.dialog.create = false
      this.dialog.car.id = val.id
      this.dialog.car.license = val.license
      this.dialog.car.vehicle = val.vehicle
      this.dialog.car.engine = val.engine
      this.dialog.car.type.model = val.car_type
      this.dialog.car.color.model = val.car_color
      this.dialog.car.remark = val.car_remark
      this.dialog.dc.type.model = val.dc_type
      this.dialog.dc.causes.model = val.dc_causes
      this.dialog.dc.date = val.dc_date
      this.dialog.dc.address = val.dc_address
      this.dialog.dc.key.model = val.dc_key
      this.dialog.dc.party.name = val.dc_party_name
      this.dialog.dc.party.cardid = val.dc_party_cardid
      this.dialog.dc.party.tel = val.dc_party_tel
      this.dialog.dc.name = val.dc_name
      this.dialog.dc.acc = val.dc_acc
      this.dialog.dc.into_date = val.dc_into_date
      this.dialog.p.name = val.p_name
      this.dialog.p.id = val.p_id
      this.dialog.driver = val.driver
      this.dialog.operator = val.operator
      this.dialog.drag.km = val.drag_km
      this.dialog.drag.unit = val.drag_unit
      this.dialog.drag.money = val.drag_money
      this.dialog.cmd.unit.model = val.cmd_unit
      this.dialog.cmd.user = val.cmd_user
      this.dialog.cv.cv.model = val.cv
      this.dialog.cv.acc = val.cv_acc
      this.dialog.cv.name = val.cv_name
      this.dialog.cv.date = val.cv_date
      this.dialog.cv.opinion.model = val.cv_opinion
      this.dialog.tv.tv.model = val.tv
      this.dialog.tv.acc = val.tv_acc
      this.dialog.tv.name = val.tv_name
      this.dialog.tv.date = val.tv_date
      this.dialog.tv.opinion.model = val.tv_opinion
      this.dialog.rc.name = val.rc_name
      this.dialog.rc.idcard = val.rc_idcard
      this.dialog.rc.tel = val.rc_tel
      this.dialog.parking.date = val.parking_date
      this.dialog.parking.unit = val.parking_unit
      this.dialog.parking.money = val.parking_money
      this.dialog.parking.payable = val.parking_payable
      this.dialog.parking.paidin = val.parking_paidin
      this.dialog.remark = val.remark
      this.dialog.rs.acc = val.rs_acc
      this.dialog.rs.name = val.rs_name
      this.dialog.rs.date = val.rs_date
      this.dialog.img.front = ''
      this.dialog.img.back = ''
      this.dialog.img.left = ''
      this.dialog.img.right = ''
      this.dialog.img.back_front = ''
      this.dialog.img.back_back = ''
      this.dialog.img.back_left = ''
      this.dialog.img.back_right = ''
      this.dialog.attachment = []
      this.dialog.car.delete = val.delete

      if (val.attachment) {
        this.dialog.attachment = JSON.parse(val.attachment)

        for (let index = 0; index < this.dialog.attachment.length; index++) {
          const element = this.dialog.attachment[index]
          switch (element.type) {
            case 'front':
              this.dialog.img.front = '/upload/' + element.file
              break
            case 'back':
              this.dialog.img.back = '/upload/' + element.file
              break
            case 'left':
              this.dialog.img.left = '/upload/' + element.file
              break
            case 'right':
              this.dialog.img.right = '/upload/' + element.file
              break
            case 'back_front':
              this.dialog.img.back_front = '/upload/' + element.file
              break
            case 'back_back':
              this.dialog.img.back_back = '/upload/' + element.file
              break
            case 'back_left':
              this.dialog.img.back_left = '/upload/' + element.file
              break
            case 'back_right':
              this.dialog.img.back_right = '/upload/' + element.file
              break
            default:
              this.dialog.attachment[index].file = '/upload/' + element.file
              break
          }
        }
      }

      this.dialog.show = true
    },

    onNew() {
      this.dialog.create = true
      this.dialog.car.id = uid().replace('-', '')
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
      this.dialog.show = true
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

    onAdded() {},

    onUploaded(val) {
      for (let index = 0; index < val.files.length; index++) {
        const element = val.files[index]
        this.dialog.attachment.push({
          name: element.name,
          type: 'attachment',
          file: element.xhr.response,
        })
      }
    },

    onDelete() {
      this.dialog.car.delete = true
      this.onSave()
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
            this.dialog.show = false
            this.$q.notify('保存成功')
            this.onQuery()
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
          val.date_out === null ||
            val.date_out === undefined ||
            val.date_out === '1000-01-01T00:00:00'
            ? Date.now()
            : val.date_out,
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

    loadFile(url, callback) {
      PizZipUtils.getBinaryContent(url, callback)
    },

    writeTemplater() {
      this.loadFile('http://localhost:8080/处罚决定 - 副本.docx', (error, content) => {
        if (error) {
          throw error
        }
        const zip = new PizZip(content)
        const doc = new Docxtemplater(zip, {
          paragraphLoop: true,
          linebreaks: true,
        })
        doc.setData({
          title: {
            year: date.formatDate(Date.now(), 'YYYY'),
            mark1: date.formatDate(Date.now(), 'M'),
            mark2: date.formatDate(Date.now(), 'Dss'),
          },
          dc_party_name: this.dialog.dc.party.name,
          dc_party_tel: this.dialog.dc.party.tel,
          dc_address: this.dialog.dc.address,
          start: {
            year: date.formatDate(Date.now(), 'YYYY'),
            month: date.formatDate(Date.now(), 'M'),
            day: date.formatDate(Date.now(), 'D'),
          },
          end: {
            year: date.formatDate(Date.now(), 'YYYY'),
            month: date.formatDate(Date.now(), 'M'),
            day: date.formatDate(Date.now(), 'D'),
          },
          footer: {
            year: date.formatDate(Date.now(), 'YYYY'),
            month: date.formatDate(Date.now(), 'M'),
            day: date.formatDate(Date.now(), 'D'),
          },
          item: [
            {
              name: '物品1',
              spec: '个',
              num: 1,
              licence: this.dialog.car.license,
              remark: '',
            },
            {
              name: '物品2',
              spec: '个',
              num: 1,
              licence: this.dialog.car.license,
              remark: '',
            },
          ],
        })

        try {
          doc.render()
        } catch (e) {
          let replaceErrors = (key, value) => {
            if (value instanceof Error) {
              return Object.getOwnPropertyNames(value).reduce((err, key) => {
                err[key] = value[key]
                return err
              }, {})
            }
            return value
          }
          console.log(JSON.stringify({ error: e }, replaceErrors))

          if (error.properties && error.properties.errors instanceof Array) {
            const errorMessages = error.properties.errors
              .map((err) => {
                return err.properties.explanation
              })
              .join('\n')
            console.log('errorMessages', errorMessages)
          }
          throw error
        }

        const out = doc.getZip().generate({
          type: 'blob',
          mimeType: 'application/vnd.openxmlformats-officedocument.wordprocessingml.document',
        })

        exportFile('output.docx', out)
      })
    },
  },

  mounted() {
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

    this.onQuery()
  },
}
</script>
