<template>
  <q-page class="bg-white">
    <q-splitter v-model="splitterModel" style="height: calc(100vh - 50px)">
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

                    <div class="col-auto" style="width: 8px" />

                    <q-select
                      class="col-2"
                      v-model="search.unit.model"
                      :options="search.unit.options"
                      label="拖移指令单位"
                    />

                    <div class="col-auto" style="width: 8px" />

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

                    <div class="col-auto" style="width: 8px" />

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

                    <div class="col-auto" style="width: 8px" />

                    <q-btn class="col-2" color="green" label="导出" @click="onExport" />

                    <div class="col-auto" style="width: 8px" />

                    <q-btn class="col-2" color="primary" label="查询" @click="onQuery" />
                  </div>
                </div>
              </template>

              <template v-slot:header-cell="props">
                <q-th :props="props" style="font-size: 15px; font-weight: bold">
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
                        (car_type.in[0]?.sum ?? 0) +
                        (car_type.in[1]?.sum ?? 0) +
                        (car_type.in[2]?.sum ?? 0) +
                        (car_type.in[3]?.sum ?? 0)
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
                        (car_type.out[0]?.sum ?? 0) +
                        (car_type.out[1]?.sum ?? 0) +
                        (car_type.out[2]?.sum ?? 0) +
                        (car_type.out[3]?.sum ?? 0)
                      }}
                    </div>
                  </div>

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
                        :options="rowsPerPageOptions"
                        @update:model-value="onRowsPerPage"
                      />
                    </div>

                    <div class="col-auto" style="width: 8px" />

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

                    <div class="col-auto" style="width: 8px" />

                    <q-select
                      class="col-2"
                      v-model="search.unit.model"
                      :options="search.unit.options"
                      label="拖移指令单位"
                    />

                    <div class="col-auto" style="width: 8px" />

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

                    <div class="col-auto" style="width: 8px" />

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

                    <div class="col-auto" style="width: 8px" />

                    <q-btn class="col-2" color="green" label="导出" @click="onExport" />

                    <div class="col-auto" style="width: 8px" />

                    <q-btn class="col-2" color="primary" label="查询" @click="onQuery" />
                  </div>
                </div>
              </template>

              <template v-slot:header-cell="props">
                <q-th :props="props" style="font-size: 15px; font-weight: bold">
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
                        (car_type.in[0]?.sum ?? 0) +
                        (car_type.in[1]?.sum ?? 0) +
                        (car_type.in[2]?.sum ?? 0) +
                        (car_type.in[3]?.sum ?? 0)
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
                        (car_type.out[0]?.sum ?? 0) +
                        (car_type.out[1]?.sum ?? 0) +
                        (car_type.out[2]?.sum ?? 0) +
                        (car_type.out[3]?.sum ?? 0)
                      }}
                    </div>
                  </div>

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
                        :options="rowsPerPageOptions"
                        @update:model-value="onRowsPerPage"
                      />
                    </div>

                    <div class="col-auto" style="width: 8px" />

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

                    <div class="col-auto" style="width: 8px" />

                    <q-select
                      class="col-2"
                      v-model="search.unit.model"
                      :options="search.unit.options"
                      label="拖移指令单位"
                    />

                    <div class="col-auto" style="width: 8px" />

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

                    <div class="col-auto" style="width: 8px" />

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

                    <div class="col-auto" style="width: 8px" />

                    <q-btn class="col-2" color="green" label="导出" @click="onExport" />

                    <div class="col-auto" style="width: 8px" />

                    <q-btn class="col-2" color="primary" label="查询" @click="onQuery" />
                  </div>
                </div>
              </template>

              <template v-slot:header-cell="props">
                <q-th :props="props" style="font-size: 15px; font-weight: bold">
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

                    <div v-if="car_type.out.length >= 4" class="col-auto text-green">
                      当前总放行：{{
                        (car_type.out[0]?.sum ?? 0) +
                        (car_type.out[1]?.sum ?? 0) +
                        (car_type.out[2]?.sum ?? 0) +
                        (car_type.out[3]?.sum ?? 0)
                      }}
                    </div>
                  </div>

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
                        :options="rowsPerPageOptions"
                        @update:model-value="onRowsPerPage"
                      />
                    </div>

                    <div class="col-auto" style="width: 8px" />

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

                    <div class="col-auto" style="width: 8px" />

                    <q-select
                      class="col-2"
                      v-model="search.unit.model"
                      :options="search.unit.options"
                      label="拖移指令单位"
                    />

                    <div class="col-auto" style="width: 8px" />

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

                    <div class="col-auto" style="width: 8px" />

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

                    <div class="col-auto" style="width: 8px" />

                    <q-btn class="col-2" color="green" label="导出" @click="onExport" />

                    <div class="col-auto" style="width: 8px" />

                    <q-btn class="col-2" color="primary" label="查询" @click="onQuery" />
                  </div>
                </div>
              </template>

              <template v-slot:header-cell="props">
                <q-th :props="props" style="font-size: 15px; font-weight: bold">
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
                        (car_type.in[0]?.sum ?? 0) +
                        (car_type.in[1]?.sum ?? 0) +
                        (car_type.in[2]?.sum ?? 0) +
                        (car_type.in[3]?.sum ?? 0)
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
                        (car_type.out[0]?.sum ?? 0) +
                        (car_type.out[1]?.sum ?? 0) +
                        (car_type.out[2]?.sum ?? 0) +
                        (car_type.out[3]?.sum ?? 0)
                      }}
                    </div>
                  </div>

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
                        :options="rowsPerPageOptions"
                        @update:model-value="onRowsPerPage"
                      />
                    </div>

                    <div class="col-auto" style="width: 8px" />

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

  <q-inner-loading :showing="loading">
    <q-spinner size="50px" color="primary" />
  </q-inner-loading>
</template>

<script setup lang="ts">
import { ref, watch } from 'vue';
import type { QTableProps } from 'quasar';
import { date, exportFile } from 'quasar';
import { useStore, type TowCarOptionItem } from 'stores/store';
import { api } from 'src/boot/axios';

interface TableRow {
  [key: string]: unknown;
  id?: string | number;
  license?: string;
  car_type?: string;
  dc_key?: string;
  dc_type?: string;
  dc_causes?: string;
  dc_date?: string;
  cmd_unit?: string;
  dc_party_name?: string;
  dc_name?: string;
  remark?: string;
  parking_date?: string | number;
  parking_money?: string | number;
  cv?: string;
  cv_opinion?: string;
  tv?: string;
  tv_opinion?: string;
  rc_name?: string;
  rs_date?: string;
  create_user?: string;
  create_date?: string;
  update_user?: string;
  update_date?: string;
}

interface CarTypeItem {
  type: string;
  sum: number;
}

const store = useStore();

const loading = ref(false);

const tab = ref('class');
const splitterModel = ref(10);
const search = ref({
  type: {
    model: '',
    options: ['', '车牌', '车辆类型', '车身颜色', '车架号', '发动机号', '车辆备注', '拖移备注'],
  },
  unit: {
    model: '全部',
    options: ['全部', '交警部门', '卧龙执法点', '开化执法点', '新平执法点', '消防部门'],
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
  name: '',
});
const sum = ref({
  sum: 0,
  car_in: 0,
  car_out: 0,
});
const pagination = ref({
  page: 1,
  rowsNumber: 0,
  rowsPerPage: 20,
  sortBy: 'dc_date',
  descending: true,
} as QTableProps['pagination']);
const rowsPerPageOptions = ref([10, 20, 30, 40, 50, '全部']);

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
const car_type = ref({
  in: [] as CarTypeItem[],
  out: [] as CarTypeItem[],
});

watch(tab, (newVal) => {
  switch (newVal) {
    case 'class':
      search.value.content = store.user.name;
      search.value.key.model = '全部';
      search.value.date.in = '';
      search.value.date.out = date.formatDate(Date.now(), 'YYYY-MM-DD');
      onQuery();
      break;
    case 'day':
      search.value.content = '';
      search.value.key.model = '全部';
      search.value.date.in = '';
      search.value.date.out = date.formatDate(Date.now(), 'YYYY-MM-DD');
      onQuery();
      break;
    case 'month':
      search.value.content = '';
      search.value.key.model = '全部';
      search.value.date.in = '';
      search.value.date.out = date.formatDate(Date.now(), 'YYYY-MM-DD');
      onQuery();
      break;
    case 'year':
      search.value.content = '';
      search.value.key.model = '全部';
      search.value.date.in = '';
      search.value.date.out = date.formatDate(Date.now(), 'YYYY-MM-DD');
      onQuery();
      break;
    default:
      search.value.content = '';
      search.value.key.model = '全部';
      search.value.date.in = '';
      search.value.date.out = date.formatDate(Date.now(), 'YYYY-MM-DD');
      break;
  }
});

const wrapCsvValue = (val: string, formatFn?: (...args: unknown[]) => unknown) => {
  let formatted: string = formatFn !== void 0 ? String(formatFn(val)) : val;

  formatted = formatted === void 0 || formatted === null ? '' : String(formatted);

  formatted = formatted.split('"').join('""');

  return `\uFEFF${formatted}`;
};

const onExport = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

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
};

const onRowsPerPage = (val: string) => {
  if (val === '全部') {
    pagination.value!.rowsPerPage = sum.value.sum;
  }
  onQuery();
};

const onRequest = (props: { pagination: QTableProps['pagination'] }) => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  const page = props.pagination?.page ?? 1;
  const rowsPerPage = props.pagination?.rowsPerPage ?? 20;
  const sortBy = props.pagination?.sortBy ?? null;
  const descending = props.pagination?.descending ?? true;

  rows.value = [];
  car_type.value.in = [];
  car_type.value.out = [];

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/query',
      {
        in_date: search.value.date.in,
        out_date: search.value.date.out,
        content: search.value.content,
        model: search.value.type.model === '' ? '' : '记录人',
        cur: (page - 1) * rowsPerPage,
        max: rowsPerPage,
        sortBy: sortBy,
        descending: descending,
        status: '',
        name: search.value.name,
        key: search.value.key.model,
        unit: search.value.unit.model === '全部' ? '' : search.value.unit.model,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      if (resp.data.status === 1) {
        for (let i = 0; i < resp.data.data.length; i++) {
          if (resp.data.data[i].id === '') break;

          const parking_money = calcPay({
            type: resp.data.data[i].car_type,
            date_in:
              resp.data.data[i].dc_into_date === ''
                ? resp.data.data[i].dc_date
                : resp.data.data[i].dc_into_date,
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
      } else if (resp.data.message === 'no data') {
        store.quasar.notify('无数据');
        return;
      } else {
        store.quasar.notify('网络错误，请稍后重试');
      }

      api
        .post(
          store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/count',
          {
            in_date: search.value.date.in,
            out_date: search.value.date.out,
            content: '小型汽车',
            model: '车辆类型',
            status: '未放行',
            name: search.value.name,
            key: search.value.key.model,
            unit: search.value.unit.model === '全部' ? '' : search.value.unit.model,
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
                name: search.value.name,
                key: search.value.key.model,
                unit: search.value.unit.model === '全部' ? '' : search.value.unit.model,
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
                    method: 'count',
                    in_date: search.value.date.in,
                    out_date: search.value.date.out,
                    content: '三轮摩托车',
                    model: '车辆类型',
                    status: '未放行',
                    name: search.value.name,
                    key: search.value.key.model,
                    unit: search.value.unit.model === '全部' ? '' : search.value.unit.model,
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
                        name: search.value.name,
                        unit: search.value.unit.model === '全部' ? '' : search.value.unit.model,
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

      api
        .post(
          store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/count',
          {
            in_date: search.value.date.in,
            out_date: search.value.date.out,
            content: '小型汽车',
            model: '车辆类型',
            status: '已放行',
            name: search.value.name,
            key: search.value.key.model,
            unit: search.value.unit.model === '全部' ? '' : search.value.unit.model,
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
                name: search.value.name,
                key: search.value.key.model,
                unit: search.value.unit.model === '全部' ? '' : search.value.unit.model,
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
                    name: search.value.name,
                    key: search.value.key.model,
                    unit: search.value.unit.model === '全部' ? '' : search.value.unit.model,
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
                        name: search.value.name,
                        unit: search.value.unit.model === '全部' ? '' : search.value.unit.model,
                      },
                      {
                        headers: {
                          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
                        },
                      },
                    )
                    .then(() => {
                      const outSum = car_type.value.out.reduce((acc, item) => acc + item.sum, 0);
                      car_type.value.out.push({
                        type: '其他车辆',
                        sum: outSum,
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

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=tow&path=/api/car/count',
      {
        in_date: search.value.date.in,
        out_date: search.value.date.out,
        content: '',
        model: '',
        status: '',
        name: search.value.name,
        key: search.value.key.model,
        unit: search.value.unit.model === '全部' ? '' : search.value.unit.model,
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

const onMonthDateIn = (val: { from: string; to: string } | string | null) => {
  let a: string = typeof val === 'object' && val !== null ? val.from : val as string;
  if (a === null || a === undefined) {
    search.value.date.in = '';
    return;
  } else if (typeof a === 'object') {
    a = (a as { from: string; to: string }).from;
  }

  search.value.date.in =
    date.formatDate(date.startOfDate(a, 'month'), 'YYYY-MM-DD HH:mm:ss') +
    ' - ' +
    date.formatDate(date.endOfDate(a, 'month'), 'YYYY-MM-DD HH:mm:ss');
};

const onMonthDateOut = (val: { from: string; to: string } | string | null) => {
  let a: string = typeof val === 'object' && val !== null ? val.from : val as string;
  if (a === null || a === undefined) {
    search.value.date.out = '';
    return;
  } else if (typeof a === 'object') {
    a = (a as { from: string; to: string }).from;
  }

  search.value.date.out =
    date.formatDate(date.startOfDate(a, 'month'), 'YYYY-MM-DD HH:mm:ss') +
    ' - ' +
    date.formatDate(date.endOfDate(a, 'month'), 'YYYY-MM-DD HH:mm:ss');
};

const onYearDateIn = (val: { from: string; to: string } | string | null) => {
  let a: string = typeof val === 'object' && val !== null ? val.from : val as string;
  if (a === null || a === undefined) {
    search.value.date.in = '';
    return;
  } else if (typeof a === 'object') {
    a = (a as { from: string; to: string }).from;
  }

  search.value.date.in =
    date.formatDate(date.startOfDate(a, 'year'), 'YYYY-MM-DD HH:mm:ss') +
    ' - ' +
    date.formatDate(date.endOfDate(a, 'year'), 'YYYY-MM-DD HH:mm:ss');
};

const onYearDateOut = (val: { from: string; to: string } | string | null) => {
  let a: string = typeof val === 'object' && val !== null ? val.from : val as string;
  if (a === null || a === undefined) {
    search.value.date.out = '';
    return;
  } else if (typeof a === 'object') {
    a = (a as { from: string; to: string }).from;
  }

  search.value.date.out =
    date.formatDate(date.startOfDate(a, 'year'), 'YYYY-MM-DD HH:mm:ss') +
    ' - ' +
    date.formatDate(date.endOfDate(a, 'year'), 'YYYY-MM-DD HH:mm:ss');
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

    const now = date.formatDate(
      val.date_out === null || val.date_out === undefined ? Date.now() : val.date_out,
      'YYYY-MM-DD HH:mm:ss',
    );
    let time = date.getDateDiff(now, date.formatDate(val.date_in, 'YYYY-MM-DD HH:mm:ss'), 'days');

    time = Math.ceil(time);

    let payable = 0;
    if (time > parseInt(String(val_class[0]?.cost_day ?? '0'))) {
      payable =
        parseInt(String(val_class[0]?.gratis_day ?? '0')) *
        (time - parseInt(String(val_class[0]?.cost_day ?? '0')));
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

search.value.name = store.user.name;
search.value.date.in = '';
search.value.date.out = date.formatDate(Date.now(), 'YYYY-MM-DD');

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
