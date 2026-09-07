<template>
  <q-page class="bg-white">
    <q-splitter v-model="splitterModel">
      <template v-slot:before>
        <q-tabs v-model="tab" vertical>
          <q-tab name="class" icon="filter_1" label="班报" />
          <q-tab name="day" icon="filter_2" label="日报" />
          <q-tab name="month" icon="filter_3" label="月报" />
          <q-tab name="year" icon="filter_4" label="年报" />
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
          <q-tab-panel name="class">
            <q-table
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
                      v-model="search.key.model"
                      :options="search.key.options"
                      label="扣车形式"
                    />

                    <div class="col-auto" style="width: 10px" />

                    <q-select
                      class="col-2"
                      v-model="search.unit.model"
                      :options="search.unit.options"
                      label="拖移指令单位"
                    />

                    <div class="col-auto" style="width: 10px" />

                    <q-input class="col" v-model="search.date.in" label="扣车日期">
                      <template v-slot:append>
                        <q-icon name="event" class="cursor-pointer">
                          <q-popup-proxy transition-show="scale" transition-hide="scale">
                            <q-date v-model="search.date.in" mask="YYYY-MM-DD">
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
                            <q-date v-model="search.date.out" mask="YYYY-MM-DD">
                              <div class="row items-center justify-end">
                                <q-btn v-close-popup label="关闭" color="primary" flat />
                              </div>
                            </q-date>
                          </q-popup-proxy>
                        </q-icon>
                      </template>
                    </q-input>

                    <div class="col-auto" style="width: 10px" />

                    <q-btn class="col-2" color="green" label="导出" @click="onExport" />

                    <div class="col-auto" style="width: 10px" />

                    <q-btn class="col-2" color="primary" label="查询" @click="onQuery" />
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

                    <div class="col-auto">
                      {{ scope.pagination.page }} - {{ scope.pagesNumber }}
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
              </template>
            </q-table>
          </q-tab-panel>

          <q-tab-panel name="day">
            <q-table
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
                      v-model="search.key.model"
                      :options="search.key.options"
                      label="扣车形式"
                    />

                    <div class="col-auto" style="width: 10px" />

                    <q-select
                      class="col-2"
                      v-model="search.unit.model"
                      :options="search.unit.options"
                      label="拖移指令单位"
                    />

                    <div class="col-auto" style="width: 10px" />

                    <q-input class="col" v-model="search.date.in" label="扣车日期">
                      <template v-slot:append>
                        <q-icon name="event" class="cursor-pointer">
                          <q-popup-proxy transition-show="scale" transition-hide="scale">
                            <q-date v-model="search.date.in" mask="YYYY-MM-DD">
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
                            <q-date v-model="search.date.out" mask="YYYY-MM-DD">
                              <div class="row items-center justify-end">
                                <q-btn v-close-popup label="关闭" color="primary" flat />
                              </div>
                            </q-date>
                          </q-popup-proxy>
                        </q-icon>
                      </template>
                    </q-input>

                    <div class="col-auto" style="width: 10px" />

                    <q-btn class="col-2" color="green" label="导出" @click="onExport" />

                    <div class="col-auto" style="width: 10px" />

                    <q-btn class="col-2" color="primary" label="查询" @click="onQuery" />
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

                    <div class="col-auto">
                      {{ scope.pagination.page }} - {{ scope.pagesNumber }}
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
              </template>
            </q-table>
          </q-tab-panel>

          <q-tab-panel name="month">
            <q-table
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
                      v-model="search.key.model"
                      :options="search.key.options"
                      label="扣车形式"
                    />

                    <div class="col-auto" style="width: 10px" />

                    <q-select
                      class="col-2"
                      v-model="search.unit.model"
                      :options="search.unit.options"
                      label="拖移指令单位"
                    />

                    <div class="col-auto" style="width: 10px" />

                    <q-input class="col" v-model="search.date.in" label="扣车日期">
                      <template v-slot:append>
                        <q-icon name="event" class="cursor-pointer">
                          <q-popup-proxy transition-show="scale" transition-hide="scale">
                            <q-date
                              v-model="search.date.in"
                              mask="YYYY-MM-DD"
                              @update:model-value="onMonthDateIn"
                              range
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
                              v-model="search.date.out"
                              mask="YYYY-MM-DD"
                              @update:model-value="onMonthDateOut"
                              range
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

                    <q-btn class="col-2" color="green" label="导出" @click="onExport" />

                    <div class="col-auto" style="width: 10px" />

                    <q-btn class="col-2" color="primary" label="查询" @click="onQuery" />
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

                    <div class="col-auto">
                      {{ scope.pagination.page }} - {{ scope.pagesNumber }}
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
              </template>
            </q-table>
          </q-tab-panel>

          <q-tab-panel name="year">
            <q-table
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
                      v-model="search.key.model"
                      :options="search.key.options"
                      label="扣车形式"
                    />

                    <div class="col-auto" style="width: 10px" />

                    <q-select
                      class="col-2"
                      v-model="search.unit.model"
                      :options="search.unit.options"
                      label="拖移指令单位"
                    />

                    <div class="col-auto" style="width: 10px" />

                    <q-input class="col" v-model="search.date.in" label="扣车日期">
                      <template v-slot:append>
                        <q-icon name="event" class="cursor-pointer">
                          <q-popup-proxy transition-show="scale" transition-hide="scale">
                            <q-date
                              v-model="search.date.in"
                              mask="YYYY-MM-DD"
                              @update:model-value="onYearDateIn"
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
                              v-model="search.date.out"
                              mask="YYYY-MM-DD"
                              @update:model-value="onYearDateOut"
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

                    <q-btn class="col-2" color="green" label="导出" @click="onExport" />

                    <div class="col-auto" style="width: 10px" />

                    <q-btn class="col-2" color="primary" label="查询" @click="onQuery" />
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

                    <div class="col-auto">
                      {{ scope.pagination.page }} - {{ scope.pagesNumber }}
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
              </template>
            </q-table>
          </q-tab-panel>
        </q-tab-panels>
      </template>
    </q-splitter>
  </q-page>
</template>

<script>
import { date, exportFile } from 'quasar'
import { useStore } from 'src/stores/store'

export default {
  name: 'PageReports',

  data() {
    return {
      store: useStore(),
      tab: 'class',
      splitterModel: 10,
      search: {
        unit: {
          model: '全部',
          options: ['全部', '交警部门', '卧龙执法点', '开化执法点', '新平执法点', '消防部门'],
        },
        key: {
          model: '全部',
          options: ['全部', '拖车', '锁车'],
        },
        date: {
          in: '',
          out: '',
        },
        content: '',
      },
      sum: {
        sum: 0,
        car_in: 0,
        car_out: 0,
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
      car_type: {
        in: [],
        out: [],
      },
    }
  },

  watch: {
    tab(newVal) {
      switch (newVal) {
        case 'class':
          this.search.content = this.store.user.username
          this.search.key.model = '全部'
          this.search.date.in = ''
          this.search.date.out = date.formatDate(Date.now(), 'YYYY-MM-DD')
          this.onQuery()
          break
        case 'day':
          this.search.content = ''
          this.search.key.model = '全部'
          this.search.date.in = ''
          this.search.date.out = date.formatDate(Date.now(), 'YYYY-MM-DD')
          this.onQuery()
          break
        case 'month':
          this.search.content = ''
          this.search.key.model = '全部'
          this.search.date.in = ''
          this.search.date.out = date.formatDate(Date.now(), 'YYYY-MM-DD')
          this.onQuery()
          break
        case 'year':
          this.search.content = ''
          this.search.key.model = '全部'
          this.search.date.in = ''
          this.search.date.out = date.formatDate(Date.now(), 'YYYY-MM-DD')
          this.onQuery()
          break
        default:
          this.search.content = ''
          this.search.key.model = '全部'
          this.search.date.in = ''
          this.search.date.out = date.formatDate(Date.now(), 'YYYY-MM-DD')
          break
      }
    },
  },

  methods: {
    wrapCsvValue(val, formatFn) {
      let formatted = formatFn !== void 0 ? formatFn(val) : val

      formatted = formatted === void 0 || formatted === null ? '' : String(formatted)

      formatted = formatted.split('"').join('""')

      return `\uFEFF${formatted}`
    },

    onExport() {
      let time = setTimeout(() => {
        this.$q.loading.hide()
        clearTimeout(time)
      }, 120000)

      this.$q.loading.show()

      const content = [this.columns.map((col) => this.wrapCsvValue(col.label))]
        .concat(
          this.rows.map((row) =>
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

      let status = exportFile('车辆列表.csv', content, 'text/csv')

      if (status !== true) {
        this.$q.notify('即将开始下载，请稍后...')
      }

      this.$q.loading.hide()
      clearTimeout(time)
    },

    onRowsPerPage(val) {
      if (val === '全部') {
        this.pagination.rowsPerPage = this.sum.sum
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
          content: this.search.content,
          model: this.search.content === '' ? '' : '记录人',
          cur: (page - 1) * rowsPerPage,
          max: rowsPerPage,
          sortBy: sortBy === null ? '' : sortBy,
          descending: descending,
          status: '',
          key: this.search.key.model,
          unit: this.search.unit.model === '全部' ? '' : this.search.unit.model,
        })
        .then((resp) => {
          if (resp.status === 200) {
            let json = JSON.parse(resp.data)
            for (let i = 0; i < json.value.length; ++i) {
              if (json.value[i].id === '') break

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
                dc_date: date.formatDate(json.value[i].dc_date, 'YYYY-MM-DD HH:mm:ss'),
                dc_address: json.value[i].dc_address,
                dc_key: json.value[i].dc_key,
                dc_party_name: json.value[i].dc_party_name,
                dc_party_cardid: json.value[i].dc_party_cardid,
                dc_party_tel: json.value[i].dc_party_tel,
                dc_name: json.value[i].dc_name,
                dc_acc: json.value[i].dc_acc,
                dc_into_date: date.formatDate(json.value[i].dc_into_date, 'YYYY-MM-DD HH:mm:ss'),
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
              name: this.search.content,
              key: this.search.key.model,
              unit: this.search.unit.model === '全部' ? '' : this.search.unit.model,
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
                  name: this.search.content,
                  key: this.search.key.model,
                  unit: this.search.unit.model === '全部' ? '' : this.search.unit.model,
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
                      name: this.search.content,
                      key: this.search.key.model,
                      unit: this.search.unit.model === '全部' ? '' : this.search.unit.model,
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
                          unit: this.search.unit.model === '全部' ? '' : this.search.unit.model,
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
              name: this.search.content,
              key: this.search.key.model,
              unit: this.search.unit.model === '全部' ? '' : this.search.unit.model,
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
                  name: this.search.content,
                  key: this.search.key.model,
                  unit: this.search.unit.model === '全部' ? '' : this.search.unit.model,
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
                      name: this.search.content,
                      key: this.search.key.model,
                      unit: this.search.unit.model === '全部' ? '' : this.search.unit.model,
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
                          unit: this.search.unit.model === '全部' ? '' : this.search.unit.model,
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
          in_date: this.search.date.in,
          out_date: this.search.date.out,
          content: '',
          model: '',
          status: '',
          name: this.search.content,
          key: this.search.key.model,
          unit: this.search.unit.model === '全部' ? '' : this.search.unit.model,
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

    onMonthDateIn(val) {
      let a = val
      if (a === null) {
        this.search.date.in = ''
        return
      } else if (typeof a === 'object') {
        a = a.from
      }

      this.search.date.in =
        date.formatDate(date.startOfDate(a, 'month'), 'YYYY-MM-DD HH:mm:ss') +
        ' - ' +
        date.formatDate(date.endOfDate(a, 'month'), 'YYYY-MM-DD HH:mm:ss')
    },

    onMonthDateOut(val) {
      let a = val
      if (a === null) {
        this.search.date.out = ''
        return
      } else if (typeof a === 'object') {
        a = a.from
      }

      this.search.date.out =
        date.formatDate(date.startOfDate(a, 'month'), 'YYYY-MM-DD HH:mm:ss') +
        ' - ' +
        date.formatDate(date.endOfDate(a, 'month'), 'YYYY-MM-DD HH:mm:ss')
    },

    onYearDateIn(val) {
      let a = val
      if (a === null) {
        this.search.date.in = ''
        return
      } else if (typeof a === 'object') {
        a = a.from
      }

      this.search.date.in =
        date.formatDate(date.startOfDate(a, 'year'), 'YYYY-MM-DD HH:mm:ss') +
        ' - ' +
        date.formatDate(date.endOfDate(a, 'year'), 'YYYY-MM-DD HH:mm:ss')
    },

    onYearDateOut(val) {
      let a = val
      if (a === null) {
        this.search.date.out = ''
        return
      } else if (typeof a === 'object') {
        a = a.from
      }

      this.search.date.out =
        date.formatDate(date.startOfDate(a, 'year'), 'YYYY-MM-DD HH:mm:ss') +
        ' - ' +
        date.formatDate(date.endOfDate(a, 'year'), 'YYYY-MM-DD HH:mm:ss')
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
    this.search.content = this.store.user.username
    this.search.date.in = ''
    this.search.date.out = date.formatDate(Date.now(), 'YYYY-MM-DD')
    this.onQuery()
  },
}
</script>
