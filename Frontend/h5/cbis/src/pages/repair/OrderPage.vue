<template>
  <q-page>
    <q-table
      square
      :rows="rows"
      :columns="columns"
      row-key="id"
      separator="cell"
      v-model:pagination="pagination"
      :filter="filterInput"
      @request="onRequest"
      virtual-scroll
      :style="`${store.height};width: calc(100vw - 200px)`"
    >
      <template v-slot:top>
        <div class="column fit">
          <div class="col row">
            <q-select
              class="col"
              v-model="search.select.model"
              :options="search.select.options"
              label="查询类型"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="search.content" label="查询内容" />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="search.date.in" label="入场日期">
              <template v-slot:append>
                <q-icon name="event" class="cursor-pointer">
                  <q-popup-proxy transition-show="scale" transition-hide="scale">
                    <q-date
                      v-model="search.date.in"
                      range
                      mask="YYYY-MM-DD"
                      @update:model-value="onDateIn"
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

            <q-input class="col" v-model="search.date.settle" label="结算日期">
              <template v-slot:append>
                <q-icon name="event" class="cursor-pointer">
                  <q-popup-proxy transition-show="scale" transition-hide="scale">
                    <q-date
                      v-model="search.date.settle"
                      range
                      mask="YYYY-MM-DD"
                      @update:model-value="onDateSettle"
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

          <div class="col-auto" style="height: 8px" />

          <div class="col row">
            <q-btn class="col-2" color="green" label="导出" @click="onExport" />

            <div class="col-auto" style="width: 8px" />

            <q-btn class="col-2" color="red" label="重置" @click="onReset" />

            <div class="col-auto" style="width: 8px" />

            <q-btn class="col-2" color="primary" label="查询" @click="onQuery" />

            <div class="col-auto" style="width: 8px" />

            <q-btn class="col-2" color="red" label="新增" @click="onNew(undefined)" />
          </div>
        </div>
      </template>

      <template v-slot:header-cell="props">
        <q-th :props="props" style="font-size: 15px; font-weight: bold">
          {{ props.col.label }}
        </q-th>
      </template>

      <template v-slot:body-cell="props">
        <q-td v-if="props.col.name === 'id'" :props="props">
          <q-btn color="primary" :label="props.value" @click="onView(props.row)" />
        </q-td>

        <q-td v-else-if="props.col.name === 'project'" :props="props">
          <q-list bordered>
            <q-item v-for="item in props.value" :key="item.id">
              <q-item-section>
                <q-item-label>名称: {{ item.name }}</q-item-label>
                <q-item-label caption lines="2">
                  标志: {{ item.tag }} - 主修人: {{ item.principal }} - 工时数: {{ item.hour }} -
                  维修金额: {{ item.amount }}
                </q-item-label>
              </q-item-section>

              <q-item-section side top>
                <q-item-label caption>序号: {{ item.serial }}</q-item-label>
              </q-item-section>
            </q-item>
          </q-list>
        </q-td>

        <q-td v-else-if="props.col.name === 'parts'" :props="props">
          <q-list bordered>
            <q-item v-for="item in props.value" :key="item.id">
              <q-item-section>
                <q-item-label>编码: {{ item.id }}</q-item-label>
                <q-item-label caption lines="2">
                  名称: {{ item.name }} - 标志: {{ item.tag }} - 数量: {{ item.num }} - 单位:
                  {{ item.unit }} - 单价: {{ item.price }} - 金额: {{ item.amount }}
                </q-item-label>
              </q-item-section>

              <q-item-section side top>
                <q-item-label caption>序号: {{ item.serial }}</q-item-label>
              </q-item-section>
            </q-item>
          </q-list>
        </q-td>

        <q-td v-else-if="props.col.name === 'cost_manhour'" :props="props">
          <q-list bordered>
            <q-item v-for="item in props.value" :key="item.id">
              <q-item-section>
                <q-item-label caption lines="2">
                  工时费: {{ item.manhour }} - 三包工时: {{ item.insurance }} - 工时折扣:
                  {{ item.discount }} - 实收工时: {{ item.receipt }}
                </q-item-label>
              </q-item-section>
            </q-item>
          </q-list>
        </q-td>

        <q-td v-else-if="props.col.name === 'cost_materials'" :props="props">
          <q-list bordered>
            <q-item v-for="item in props.value" :key="item.id">
              <q-item-section>
                <q-item-label caption lines="2">
                  材料费: {{ item.materials }} - 三包材料: {{ item.insurance }} - 材料折扣:
                  {{ item.discount }} - 实收材料: {{ item.receipt }}
                </q-item-label>
              </q-item-section>
            </q-item>
          </q-list>
        </q-td>

        <q-td v-else-if="props.col.name === 'cost_other'" :props="props">
          <q-list bordered>
            <q-item v-for="item in props.value" :key="item.id">
              <q-item-section>
                <q-item-label caption lines="2">
                  管理费: {{ item.manager }} - 外加工费: {{ item.extra }} - 其他费用:
                  {{ item.other }} - 优惠金额: {{ item.discount }}
                </q-item-label>
              </q-item-section>
            </q-item>
          </q-list>
        </q-td>

        <q-td v-else :props="props">
          {{ props.value }}
        </q-td>
      </template>
    </q-table>

    <q-dialog v-model="dialog.show" persistent>
      <q-card style="min-width: 70%">
        <q-card-section>
          <div class="row">
            <q-input
              class="col"
              v-model="dialog.plate"
              label="车牌"
              :rules="[(val) => !!val || '此为必填项']"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="dialog.type" label="车型" />

            <div class="col-auto" style="width: 8px" />

            <q-input
              class="col"
              v-model="dialog.date_in"
              label="入厂日期"
              :rules="[(val) => !!val || '此为必填项']"
            >
              <template v-slot:prepend>
                <q-icon name="event" class="cursor-pointer">
                  <q-popup-proxy transition-show="scale" transition-hide="scale">
                    <q-date v-model="dialog.date_in" mask="YYYY-MM-DD HH:mm:ss">
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
                    <q-time v-model="dialog.date_in" mask="YYYY-MM-DD HH:mm:ss" format24h>
                      <div class="row items-center justify-end">
                        <q-btn v-close-popup label="关闭" color="primary" flat />
                      </div>
                    </q-time>
                  </q-popup-proxy>
                </q-icon>
              </template>
            </q-input>

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="dialog.date_settle" label="结算日期">
              <template v-slot:prepend>
                <q-icon name="event" class="cursor-pointer">
                  <q-popup-proxy transition-show="scale" transition-hide="scale">
                    <q-date v-model="dialog.date_settle" mask="YYYY-MM-DD HH:mm:ss">
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
                    <q-time v-model="dialog.date_settle" mask="YYYY-MM-DD HH:mm:ss" format24h>
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
            <q-input
              class="col"
              v-model="dialog.class"
              label="修理类别"
              :rules="[(val) => !!val || '此为必填项']"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="dialog.vin" label="VIN号" />

            <div class="col-auto" style="width: 8px" />

            <q-input class="col" v-model="dialog.mileage" label="里程数" />

            <div class="col-auto" style="width: 8px" />

            <q-input
              class="col"
              v-model="dialog.phone"
              label="联系方式"
              :rules="[(val) => !!val || '此为必填项']"
            />
          </div>

          <div class="col-auto" style="height: 8px" />

          <q-table
            square
            :rows="dialog.project.rows"
            :columns="dialog.project.columns"
            row-key="serial"
            separator="cell"
          >
            <template v-slot:top>
              <div class="fit row">
                <div class="col text-h6">维修项目</div>

                <div class="col-6" />

                <q-btn class="col" color="primary" label="添加" @click="onPartAddRow" />

                <div class="col-auto" style="width: 8px" />

                <q-btn class="col" color="primary" label="删除" @click="onPartDelRow" />
              </div>
            </template>

            <template v-slot:header-cell="props">
              <q-th :props="props" style="font-size: 15px; font-weight: bold">
                {{ props.col.label }}
              </q-th>
            </template>

            <template v-slot:body="props">
              <q-tr :props="props">
                <q-td key="serial" :props="props">
                  {{ props.row.serial }}
                  <q-popup-edit v-model="props.row.serial">
                    <q-input v-model="props.row.serial" dense autofocus />
                  </q-popup-edit>
                </q-td>

                <q-td key="name" :props="props">
                  {{ props.row.name }}
                  <q-popup-edit v-model="props.row.name">
                    <q-input v-model="props.row.name" dense autofocus />
                  </q-popup-edit>
                </q-td>

                <q-td key="tag" :props="props">
                  {{ props.row.tag }}
                  <q-popup-edit v-model="props.row.tag">
                    <q-input v-model="props.row.tag" dense autofocus />
                  </q-popup-edit>
                </q-td>

                <q-td key="principal" :props="props">
                  {{ props.row.principal }}
                  <q-popup-edit v-model="props.row.principal">
                    <q-input v-model="props.row.principal" dense autofocus />
                  </q-popup-edit>
                </q-td>

                <q-td key="hour" :props="props">
                  {{ props.row.hour }}
                  <q-popup-edit v-model="props.row.hour">
                    <q-input v-model="props.row.hour" dense autofocus />
                  </q-popup-edit>
                </q-td>

                <q-td key="amount" :props="props">
                  {{ props.row.amount }}
                  <q-popup-edit v-model="props.row.amount">
                    <q-input
                      v-model="props.row.amount"
                      dense
                      autofocus
                      debounce="1000"
                      @update:model-value="onSum('amount_project')"
                    />
                  </q-popup-edit>
                </q-td>
              </q-tr>
            </template>
          </q-table>

          <div class="col-auto" style="height: 8px" />

          <q-table
            square
            :rows="dialog.parts.rows"
            :columns="dialog.parts.columns"
            row-key="serial"
            separator="cell"
          >
            <template v-slot:top>
              <div class="fit row">
                <div class="col text-h6">配件列表</div>

                <div class="col-6" />

                <q-btn
                  class="col"
                  color="primary"
                  label="添加"
                  @click="dialog.parts.dialog.show = true"
                />

                <div class="col-auto" style="width: 8px" />

                <q-btn class="col" color="primary" label="删除" @click="onPartsDelRow" />
              </div>
            </template>

            <template v-slot:header-cell="props">
              <q-th :props="props" style="font-size: 15px; font-weight: bold">
                {{ props.col.label }}
              </q-th>
            </template>

            <template v-slot:body="props">
              <q-tr :props="props">
                <q-td key="serial" :props="props">
                  {{ props.row.serial }}
                </q-td>

                <q-td key="id" :props="props">
                  {{ props.row.code }}
                </q-td>

                <q-td key="name" :props="props">
                  {{ props.row.name }}
                </q-td>

                <q-td key="tag" :props="props">
                  {{ props.row.tag }}
                </q-td>

                <q-td key="num" :props="props">
                  {{ props.row.num }}
                </q-td>

                <q-td key="unit" :props="props">
                  {{ props.row.unit }}
                </q-td>

                <q-td key="price" :props="props">
                  {{ props.row.price }}
                </q-td>

                <q-td key="amount" :props="props">
                  {{ props.row.amount }}
                </q-td>
              </q-tr>
            </template>
          </q-table>

          <div class="col-auto" style="height: 8px" />

          <q-table
            square
            :rows="dialog.cost_manhour.rows"
            :columns="dialog.cost_manhour.columns"
            row-key="serial"
            separator="cell"
          >
            <template v-slot:top>
              <div class="fit row">
                <div class="col text-h6">工时费</div>

                <div class="col-6" />

                <q-btn class="col" color="primary" label="添加" @click="onManhourAddRow" />

                <div class="col-auto" style="width: 8px" />

                <q-btn class="col" color="primary" label="删除" @click="onManhourDelRow" />
              </div>
            </template>

            <template v-slot:header-cell="props">
              <q-th :props="props" style="font-size: 15px; font-weight: bold">
                {{ props.col.label }}
              </q-th>
            </template>

            <template v-slot:body="props">
              <q-tr :props="props">
                <q-td key="manhour" :props="props">
                  {{ props.row.manhour }}
                  <q-popup-edit v-model="props.row.manhour">
                    <q-input
                      v-model="props.row.manhour"
                      dense
                      autofocus
                      debounce="1000"
                      @update:model-value="onSum('cost_manhour', props.row)"
                    />
                  </q-popup-edit>
                </q-td>

                <q-td key="insurance" :props="props">
                  {{ props.row.insurance }}
                  <q-popup-edit v-model="props.row.insurance">
                    <q-input
                      v-model="props.row.insurance"
                      dense
                      autofocus
                      debounce="1000"
                      @update:model-value="onSum('cost_manhour', props.row)"
                    />
                  </q-popup-edit>
                </q-td>

                <q-td key="discount" :props="props">
                  {{ props.row.discount }}
                  <q-popup-edit v-model="props.row.discount">
                    <q-input
                      v-model="props.row.discount"
                      dense
                      autofocus
                      debounce="1000"
                      @update:model-value="onSum('cost_manhour', props.row)"
                    />
                  </q-popup-edit>
                </q-td>

                <q-td key="receipt" :props="props">
                  {{ props.row.receipt }}
                  <q-popup-edit v-model="props.row.receipt">
                    <q-input
                      v-model="props.row.receipt"
                      dense
                      autofocus
                      debounce="1000"
                      @update:model-value="onSum('cost_manhour', props.row)"
                    />
                  </q-popup-edit>
                </q-td>
              </q-tr>
            </template>
          </q-table>

          <div class="col-auto" style="height: 8px" />

          <q-table
            square
            :rows="dialog.cost_materials.rows"
            :columns="dialog.cost_materials.columns"
            row-key="serial"
            separator="cell"
          >
            <template v-slot:top>
              <div class="fit row">
                <div class="col text-h6">材料费</div>

                <div class="col-6" />

                <q-btn class="col" color="primary" label="添加" @click="onMaterialsAddRow" />

                <div class="col-auto" style="width: 8px" />

                <q-btn class="col" color="primary" label="删除" @click="onMaterialsDelRow" />
              </div>
            </template>

            <template v-slot:header-cell="props">
              <q-th :props="props" style="font-size: 15px; font-weight: bold">
                {{ props.col.label }}
              </q-th>
            </template>

            <template v-slot:body="props">
              <q-tr :props="props">
                <q-td key="materials" :props="props">
                  {{ props.row.materials }}
                  <q-popup-edit v-model="props.row.materials">
                    <q-input
                      v-model="props.row.materials"
                      dense
                      autofocus
                      debounce="1000"
                      @update:model-value="onSum('cost_materials', props.row)"
                    />
                  </q-popup-edit>
                </q-td>

                <q-td key="insurance" :props="props">
                  {{ props.row.insurance }}
                  <q-popup-edit v-model="props.row.insurance">
                    <q-input
                      v-model="props.row.insurance"
                      dense
                      autofocus
                      debounce="1000"
                      @update:model-value="onSum('cost_materials', props.row)"
                    />
                  </q-popup-edit>
                </q-td>

                <q-td key="discount" :props="props">
                  {{ props.row.discount }}
                  <q-popup-edit v-model="props.row.discount">
                    <q-input
                      v-model="props.row.discount"
                      dense
                      autofocus
                      debounce="1000"
                      @update:model-value="onSum('cost_materials', props.row)"
                    />
                  </q-popup-edit>
                </q-td>

                <q-td key="receipt" :props="props">
                  {{ props.row.receipt }}
                  <q-popup-edit v-model="props.row.receipt">
                    <q-input
                      v-model="props.row.receipt"
                      dense
                      autofocus
                      debounce="1000"
                      @update:model-value="onSum('cost_materials', props.row)"
                    />
                  </q-popup-edit>
                </q-td>
              </q-tr>
            </template>
          </q-table>

          <div class="col-auto" style="height: 8px" />

          <q-table
            square
            :rows="dialog.cost_other.rows"
            :columns="dialog.cost_other.columns"
            row-key="serial"
            separator="cell"
          >
            <template v-slot:top>
              <div class="fit row">
                <div class="col text-h6">其他费</div>

                <div class="col-6" />

                <q-btn class="col" color="primary" label="添加" @click="onOtherAddRow" />

                <div class="col-auto" style="width: 8px" />

                <q-btn class="col" color="primary" label="删除" @click="onOtherDelRow" />
              </div>
            </template>

            <template v-slot:header-cell="props">
              <q-th :props="props" style="font-size: 15px; font-weight: bold">
                {{ props.col.label }}
              </q-th>
            </template>

            <template v-slot:body="props">
              <q-tr :props="props">
                <q-td key="manager" :props="props">
                  {{ props.row.manager }}
                  <q-popup-edit v-model="props.row.manager">
                    <q-input
                      v-model="props.row.manager"
                      dense
                      autofocus
                      debounce="1000"
                      @update:model-value="onSum('cost_other')"
                    />
                  </q-popup-edit>
                </q-td>

                <q-td key="extra" :props="props">
                  {{ props.row.extra }}
                  <q-popup-edit v-model="props.row.extra">
                    <q-input
                      v-model="props.row.extra"
                      dense
                      autofocus
                      debounce="1000"
                      @update:model-value="onSum('cost_other')"
                    />
                  </q-popup-edit>
                </q-td>

                <q-td key="other" :props="props">
                  {{ props.row.other }}
                  <q-popup-edit v-model="props.row.other">
                    <q-input
                      v-model="props.row.other"
                      dense
                      autofocus
                      debounce="1000"
                      @update:model-value="onSum('cost_other')"
                    />
                  </q-popup-edit>
                </q-td>

                <q-td key="discount" :props="props">
                  {{ props.row.discount }}
                  <q-popup-edit v-model="props.row.discount">
                    <q-input
                      v-model="props.row.discount"
                      dense
                      autofocus
                      debounce="1000"
                      @update:model-value="onSum('cost_other')"
                    />
                  </q-popup-edit>
                </q-td>
              </q-tr>
            </template>
          </q-table>

          <div class="col-auto" style="height: 8px" />

          <div class="row">
            <q-input readonly class="col" v-model="dialog.amount_total" label="合计金额" />

            <div class="col-auto" style="width: 8px" />

            <q-input
              readonly
              class="col"
              v-model="dialog.amount_total_chinese"
              label="合计金额（大写）"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input
              class="col"
              v-model="dialog.amount_receipt"
              label="实收金额"
              :rules="[(val) => !!val || '此为必填项']"
            />

            <div class="col-auto" style="width: 8px" />

            <q-input
              readonly
              class="col"
              v-model="dialog.amount_receipt_chinese"
              label="实收金额（大写）"
            />
          </div>

          <div class="row">
            <q-input class="col" v-model="dialog.remark" label="备注" />

            <div class="col-auto" style="width: 8px" />

            <q-select
              class="col-2"
              v-model="dialog.status.model"
              :options="dialog.status.options"
              label="状态"
            />
          </div>

          <div class="col-auto" style="height: 8px" />

          <div class="row">
            <q-btn
              class="col"
              color="negative"
              label="取消"
              style="font-size: 20px"
              v-close-popup
            />

            <div class="col-auto" style="width: 8px" />

            <q-btn
              class="col"
              color="positive"
              label="保存"
              style="font-size: 20px"
              @click="onSave"
            />

            <div class="col-auto" style="width: 8px" />

            <q-btn
              class="col"
              color="secondary"
              label="打印"
              style="font-size: 20px"
              @click="onPrint"
            />
          </div>
        </q-card-section>
      </q-card>
    </q-dialog>

    <q-dialog v-model="dialog.parts.dialog.show" persistent>
      <q-card style="min-width: 70%">
        <q-card-section>
          <div class="row">
            <q-select class="col" v-model="storage.model" :options="storage.options" label="名称" />

            <div class="col-auto" style="width: 8px" />

            <div class="col row text-h4 flex items-center">
              <div class="col">编码：</div>
              <div class="col">{{ storage.model.value.code ?? '' }}</div>
            </div>
          </div>

          <div class="col-auto" style="height: 8px" />

          <div class="row text-h4">
            <div class="col row">
              <div class="col">标志：</div>
              <div class="col">{{ storage.model.value.tag ?? '' }}</div>
            </div>

            <div class="col-auto" style="width: 8px" />

            <div class="col row">
              <div class="col">库存：</div>
              <div class="col">{{ storage.model.value.storage ?? 0 }}</div>
            </div>
          </div>

          <div class="col-auto" style="height: 8px" />

          <div class="row text-h4">
            <div class="col row">
              <div class="col">单位：</div>
              <div class="col">{{ storage.model.value.unit }}</div>
            </div>

            <div class="col-auto" style="width: 8px" />

            <div class="col row">
              <div class="col">单价：</div>
              <div class="col">{{ storage.model.value.price }}</div>
            </div>
          </div>

          <div class="col-auto" style="height: 8px" />

          <div class="row text-h4">
            <q-input
              class="col"
              label="数量"
              v-model="dialog.parts.dialog.num"
              dense
              autofocus
              debounce="1000"
              @update:model-value="onSumParts"
            />

            <div class="col-auto" style="width: 8px" />

            <div class="col row">
              <div class="col">金额：</div>
              <div class="col">{{ dialog.parts.dialog.amount }}</div>
            </div>
          </div>

          <div class="col-auto" style="height: 8px" />

          <div class="row">
            <q-btn
              class="col"
              color="negative"
              label="取消"
              style="font-size: 20px"
              v-close-popup
            />

            <div class="col-auto" style="width: 8px" />

            <q-btn
              class="col"
              color="positive"
              label="确定"
              style="font-size: 20px"
              @click="onCreate"
            />
          </div>
        </q-card-section>
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
import { exportFile } from 'quasar';
import { useStore } from 'stores/store';
import { api } from 'src/boot/axios';
import { numberToChineseUppercaseCurrency } from 'pixiu-number-toolkit';

interface TableRow {
  [key: string]: unknown;
  id?: string | number;
  name?: string;
  field?: string | ((row: Record<string, unknown>) => unknown);
  amount?: number | string;
  manhour?: number | string;
  insurance?: number | string;
  materials?: number | string;
  discount?: number | string;
  receipt?: number | string;
  manager?: number | string;
}

interface DateValue {
  from: string;
  to: string;
}

const store = useStore();

const loading = ref(false);

const search = ref({
  date: {
    in: '',
    settle: '',
  },
  content: '',
  select: {
    model: '',
    options: ['', '车牌', '车型', '修理类别', '联系方式', '入场时间', '结算时间'],
  },
});

const pagination = ref({
  page: 1,
  rowsNumber: 0,
  rowsPerPage: 20,
  sortBy: 'update_date',
  descending: true,
} as QTableProps['pagination']);

const filterInput = ref('');

const columns = ref([
  {
    name: 'id',
    label: 'ID',
    field: 'id',
    align: 'center',
    sortable: true,
  },
  {
    name: 'plate',
    label: '车牌',
    field: 'plate',
    align: 'center',
    sortable: true,
  },
  {
    name: 'type',
    label: '车型',
    field: 'type',
    align: 'center',
    sortable: true,
  },
  {
    name: 'date_in',
    label: '入厂日期',
    field: 'date_in',
    align: 'center',
    sortable: true,
  },
  {
    name: 'date_settle',
    label: '结算日期',
    field: 'date_settle',
    align: 'center',
    sortable: true,
  },
  {
    name: 'class',
    label: '修理类别',
    field: 'class',
    align: 'center',
    sortable: true,
  },
  {
    name: 'vin',
    label: 'VIN号',
    field: 'vin',
    align: 'center',
    sortable: true,
  },
  {
    name: 'mileage',
    label: '里程数',
    field: 'mileage',
    align: 'center',
    sortable: true,
  },
  {
    name: 'phone',
    label: '联系方式',
    field: 'phone',
    align: 'center',
    sortable: true,
  },
  {
    name: 'project',
    label: '维修项目',
    field: 'project',
    align: 'center',
    sortable: true,
  },
  {
    name: 'parts',
    label: '配件列表',
    field: 'parts',
    align: 'center',
    sortable: true,
  },
  {
    name: 'cost_manhour',
    label: '工时费',
    field: 'cost_manhour',
    align: 'center',
    sortable: true,
  },
  {
    name: 'cost_materials',
    label: '材料费',
    field: 'cost_materials',
    align: 'center',
    sortable: true,
  },
  {
    name: 'cost_other',
    label: '其他费',
    field: 'cost_other',
    align: 'center',
    sortable: true,
  },
  {
    name: 'amount_total',
    label: '合计金额',
    field: 'amount_total',
    align: 'center',
    sortable: true,
  },
  {
    name: 'amount_receipt',
    label: '实收金额',
    field: 'amount_receipt',
    align: 'center',
    sortable: true,
  },
  {
    name: 'status',
    label: '状态',
    field: 'status',
    align: 'center',
    sortable: true,
  },
  {
    name: 'remark',
    label: '备注',
    field: 'remark',
    align: 'center',
    sortable: true,
  },
  {
    name: 'create_date',
    label: '创建日期',
    field: 'create_date',
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
    name: 'update_date',
    label: '更新日期',
    field: 'update_date',
    align: 'center',
    sortable: true,
  },
  {
    name: 'update_user',
    label: '更新人',
    field: 'update_user',
    align: 'center',
    sortable: true,
  },
] as QTableProps['columns']);

const rows = ref([] as TableRow[]);

interface StorageModelValue {
  id?: string | number;
  code?: string | number;
  name?: string;
  tag?: string;
  storage?: string | number;
  unit?: string;
  price?: string | number;
  date_in?: string;
  date_out?: string;
  img?: string;
  status?: string;
  remark?: string;
  create_date?: string;
  create_user?: string;
  create_id?: string | number;
  update_date?: string;
  update_user?: string;
  update_id?: string | number;
}

interface StorageOption {
  label: string;
  value: StorageModelValue;
}

const storage = ref<{
  model: { label: string; value: StorageModelValue };
  options: StorageOption[];
}>({
  model: { label: '', value: {} },
  options: [],
});

const dialog = ref({
  mode: 'create',
  show: false,
  id: '',
  plate: '',
  type: '',
  date_in: '',
  date_settle: '',
  class: '',
  vin: '',
  mileage: '',
  phone: '',
  project: {
    columns: [
      {
        name: 'serial',
        label: '序号',
        field: 'serial',
        align: 'center',
      },
      {
        name: 'name',
        label: '名称',
        field: 'name',
        align: 'center',
      },
      {
        name: 'tag',
        label: '标志',
        field: 'tag',
        align: 'center',
      },
      {
        name: 'principal',
        label: '主修人',
        field: 'principal',
        align: 'center',
      },
      {
        name: 'hour',
        label: '工时数',
        field: 'hour',
        align: 'center',
      },
      {
        name: 'amount',
        label: '维修金额',
        field: 'amount',
        align: 'center',
      },
    ] as QTableProps['columns'],
    rows: [] as TableRow[],
  },
  parts: {
    dialog: {
      show: false,
      num: 0,
      amount: 0,
    },
    columns: [
      {
        name: 'serial',
        label: '序号',
        field: 'serial',
        align: 'center',
      },
      {
        name: 'id',
        label: '编码',
        field: 'id',
        align: 'center',
      },
      {
        name: 'name',
        label: '名称',
        field: 'name',
        align: 'center',
      },
      {
        name: 'tag',
        label: '标志',
        field: 'tag',
        align: 'center',
      },
      {
        name: 'num',
        label: '数量',
        field: 'num',
        align: 'center',
      },
      {
        name: 'unit',
        label: '单位',
        field: 'unit',
        align: 'center',
      },
      {
        name: 'price',
        label: '单价',
        field: 'price',
        align: 'center',
      },
      {
        name: 'amount',
        label: '金额',
        field: 'amount',
        align: 'center',
      },
    ] as QTableProps['columns'],
    rows: [] as TableRow[],
  },
  cost_manhour: {
    columns: [
      {
        name: 'manhour',
        label: '工时费',
        field: 'manhour',
        align: 'center',
      },
      {
        name: 'insurance',
        label: '三包工时',
        field: 'insurance',
        align: 'center',
      },
      {
        name: 'discount',
        label: '工时折扣',
        field: 'discount',
        align: 'center',
      },
      {
        name: 'receipt',
        label: '实收工时',
        field: 'receipt',
        align: 'center',
      },
    ] as QTableProps['columns'],
    rows: [] as TableRow[],
  },
  cost_materials: {
    columns: [
      {
        name: 'materials',
        label: '材料费',
        field: 'materials',
        align: 'center',
      },
      {
        name: 'insurance',
        label: '三包材料',
        field: 'insurance',
        align: 'center',
      },
      {
        name: 'discount',
        label: '材料折扣',
        field: 'discount',
        align: 'center',
      },
      {
        name: 'receipt',
        label: '实收材料',
        field: 'receipt',
        align: 'center',
      },
    ] as QTableProps['columns'],
    rows: [] as TableRow[],
  },
  cost_other: {
    columns: [
      {
        name: 'manager',
        label: '管理费',
        field: 'manager',
        align: 'center',
      },
      {
        name: 'extra',
        label: '外加工费',
        field: 'extra',
        align: 'center',
      },
      {
        name: 'other',
        label: '其他费用',
        field: 'other',
        align: 'center',
      },
      {
        name: 'discount',
        label: '优惠金额',
        field: 'discount',
        align: 'center',
      },
    ] as QTableProps['columns'],
    rows: [] as TableRow[],
  },
  create_date: '',
  create_user: '',
  create_id: '',
  update_date: '',
  update_user: '',
  update_id: '',
  amount_total: 0,
  amount_receipt: 0,
  amount_total_chinese: '零',
  amount_receipt_chinese: '零',
  status: {
    model: '待处理',
    options: ['待处理', '待支付', '已完成'],
  },
  remark: '',
  selected: [],
});

const onReset = () => {
  search.value.select.model = '';
  search.value.date.in = '';
  search.value.date.settle = '';
  search.value.content = '';
};

const onDateIn = (val: DateValue | null) => {
  if (val === null) {
    search.value.date.in = '';
    return;
  }

  if (typeof val === 'object') {
    search.value.date.in = val.from + ' - ' + val.to;
  }
};

const onDateSettle = (val: DateValue | null) => {
  if (val === null) {
    search.value.date.settle = '';
    return;
  }

  if (typeof val === 'object') {
    search.value.date.settle = val.from + ' - ' + val.to;
  }
};

const wrapCsvValue = (val: string, formatFn?: (...args: unknown[]) => unknown) => {
  let formatted: string = formatFn !== void 0 ? String(formatFn(val)) : val;

  formatted = formatted === void 0 || formatted === null ? '' : String(formatted);

  formatted = formatted.split('"').join('""');

  return formatted;
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
                 ? String(col.field(row))
                 : String(row[col.field === void 0 ? col.name : col.field]),
               col.format,
             ),
          )

          .join(','),
      ),
    )
    .join('\r\n');

  const status = exportFile('订单列表.csv', content, 'text/csv');

  if (status !== true) {
    store.quasar.notify('即将开始下载，请稍后...');
  }

  loading.value = false;
  clearTimeout(time);
};

const onRequest = (props: { pagination: QTableProps['pagination'] }) => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  const pagination = props.pagination ?? { page: 1, rowsPerPage: 20, sortBy: 'update_date', descending: true, rowsNumber: 0 };
  const { page, rowsPerPage, sortBy, descending, rowsNumber } = pagination;

  const p = pagination;

  rows.value = [];

  let plate = '';
  let val_type = '';
  let val_class = '';
  let phone = '';
  let date_in = '';
  let date_settle = '';

  switch (search.value.select.model) {
    case '车牌':
      plate = search.value.content;
      break;
    case '车型':
      val_type = search.value.content;
      break;
    case '修理类别':
      val_class = search.value.content;
      break;
    case '联系方式':
      phone = search.value.content;
      break;
    case '入场时间':
      date_in = search.value.content;
      break;
    case '结算时间':
      date_settle = search.value.content;
      break;
  }

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=repair&path=/api/order/list',
      {
        plate: plate,
        type: val_type,
        class: val_class,
        phone: phone,
        date_in: date_in,
        date_settle: date_settle,
        curPage: ((page ?? 1) - 1) * (rowsPerPage ?? 20),
        maxPage: (rowsPerPage ?? 20) === 0 ? rowsNumber : (rowsPerPage ?? 20),
        sortBy: sortBy,
        descending: descending,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      if (resp.data.status !== 0) {
        for (let i = 0; i < resp.data.data.length; i++) {
          if (resp.data.data[i].id === '') break;

          rows.value.push({
            id: resp.data.data[i].id,
            plate: resp.data.data[i].plate,
            type: resp.data.data[i].type,
            date_in: resp.data.data[i].date_in,
            date_settle: resp.data.data[i].date_settle,
            class: resp.data.data[i].class,
            vin: resp.data.data[i].vin,
            mileage: resp.data.data[i].mileage,
            phone: resp.data.data[i].phone,
            project: resp.data.data[i].project,
            parts: resp.data.data[i].parts,
            cost_manhour: resp.data.data[i].cost_manhour,
            cost_materials: resp.data.data[i].cost_materials,
            cost_other: resp.data.data[i].cost_other,
            amount_total: resp.data.data[i].amount_total,
            amount_receipt: resp.data.data[i].amount_receipt,
            status: resp.data.data[i].status,
            remark: resp.data.data[i].remark,
            create_date: resp.data.data[i].create_date,
            create_user: resp.data.data[i].create_user,
            create_id: resp.data.data[i].create_id,
            update_date: resp.data.data[i].update_date,
            update_user: resp.data.data[i].update_user,
            update_id: resp.data.data[i].update_id,
          });
        }
      } else if (resp.data.message === 'no data') {
        store.quasar.notify('无数据');
      } else {
        store.quasar.notify('网络错误，请稍后重试');
      }

      p.page = page ?? 1;
      p.rowsPerPage = rowsPerPage ?? 20;
      p.sortBy = sortBy ?? 'update_date';
      p.descending = descending ?? true;

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

  let plate = '';
  let val_type = '';
  let val_class = '';
  let phone = '';
  let date_in = '';
  let date_settle = '';

  switch (search.value.select.model) {
    case '车牌':
      plate = search.value.content;
      break;
    case '车型':
      val_type = search.value.content;
      break;
    case '修理类别':
      val_class = search.value.content;
      break;
    case '联系方式':
      phone = search.value.content;
      break;
    case '入场时间':
      date_in = search.value.content;
      break;
    case '结算时间':
      date_settle = search.value.content;
      break;
  }

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=repair&path=/api/order/count',
      {
        plate: plate,
        type: val_type,
        class: val_class,
        phone: phone,
        date_in: date_in,
        date_settle: date_settle,
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

const onCreate = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  api
    .put(
      store.server + '/api/route/post?auth=jwt&service=repair&path=/api/storage/info',
      {
        code: '',
        name: '',
        curPage: 0,
        maxPage: 100,
        sortBy: 'code',
        descending: true,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      if (resp.data.status !== 0) {
        for (let i = 0; i < resp.data.data.length; i++) {
          if (resp.data.data[i].id === '') break;

          storage.value.model = { label: '', value: {} };
          storage.value.options = [];

          storage.value.options.push({
            label: resp.data.data[i].name,
            value: {
              id: resp.data.data[i].id,
              code: resp.data.data[i].code,
              name: resp.data.data[i].name,
              tag: resp.data.data[i].tag,
              storage: resp.data.data[i].num,
              unit: resp.data.data[i].unit,
              price: resp.data.data[i].price,
              date_in: resp.data.data[i].date_in,
              date_out: resp.data.data[i].date_out,
              img: resp.data.data[i].img,
              status: resp.data.data[i].status,
              remark: resp.data.data[i].remark,
              create_date: resp.data.data[i].create_date,
              create_user: resp.data.data[i].create_user,
              create_id: resp.data.data[i].create_id,
              update_date: resp.data.data[i].update_date,
              update_user: resp.data.data[i].update_user,
              update_id: resp.data.data[i].update_id,
            },
          });

          dialog.value.create_user = store.user.name;
          dialog.value.create_id = store.user.id;
          dialog.value.update_user = store.user.name;
          dialog.value.update_id = store.user.id;
          dialog.value.mode = 'create';
          dialog.value.show = true;
        }
      } else {
        store.quasar.notify('网络错误，请稍后重试');
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

const onView = (val: TableRow | undefined) => {
  if (val === undefined) return;
  
  dialog.value.id = (val.id ?? '') as string;
  dialog.value.plate = (val.plate ?? '') as string;
  dialog.value.type = (val.type ?? '') as string;
  dialog.value.date_in = (val.date_in ?? '') as string;
  dialog.value.date_settle = (val.date_settle ?? '') as string;
  dialog.value.class = (val.class ?? '') as string;
  dialog.value.vin = (val.vin ?? '') as string;
  dialog.value.mileage = (val.mileage ?? '') as string;
  dialog.value.phone = (val.phone ?? '') as string;
  dialog.value.project.rows = val.project as TableRow[] | undefined ?? [];
  dialog.value.parts.rows = val.parts as TableRow[] | undefined ?? [];
  dialog.value.cost_manhour.rows = val.cost_manhour as TableRow[] | undefined ?? [];
  dialog.value.cost_materials.rows = val.cost_materials as TableRow[] | undefined ?? [];
  dialog.value.cost_other.rows = val.cost_other as TableRow[] | undefined ?? [];
  dialog.value.create_user = (val.create_user ?? '') as string;
  dialog.value.create_id = (val.create_id ?? '') as string;
  dialog.value.update_user = store.user.name;
  dialog.value.update_id = store.user.id;
  dialog.value.amount_total = Number(val.amount_total ?? 0);
  dialog.value.amount_receipt = Number(val.amount_receipt ?? 0);
  dialog.value.amount_total_chinese = numberToChineseUppercaseCurrency(Number(val.amount_total ?? 0));
  dialog.value.amount_receipt_chinese = numberToChineseUppercaseCurrency(Number(val.amount_receipt ?? 0));
  dialog.value.status.model = (val.status ?? '待处理') as string;
  dialog.value.remark = (val.remark ?? '') as string;
  dialog.value.mode = 'view';
  dialog.value.show = true;
};

const onNew = (val: TableRow | undefined) => {
  if (val === undefined) {
    dialog.value.id = '';
    dialog.value.plate = '';
    dialog.value.type = '';
    dialog.value.date_in = '';
    dialog.value.date_settle = '';
    dialog.value.class = '';
    dialog.value.vin = '';
    dialog.value.mileage = '';
    dialog.value.phone = '';
    dialog.value.project.rows = [];
    dialog.value.parts.rows = [];
    dialog.value.cost_manhour.rows = [];
    dialog.value.cost_materials.rows = [];
    dialog.value.cost_other.rows = [];
    dialog.value.create_user = store.user.name;
    dialog.value.create_id = store.user.id;
    dialog.value.update_user = store.user.name;
    dialog.value.update_id = store.user.id;
    dialog.value.amount_total = 0;
    dialog.value.amount_receipt = 0;
    dialog.value.amount_total_chinese = numberToChineseUppercaseCurrency(0);
    dialog.value.amount_receipt_chinese = numberToChineseUppercaseCurrency(0);
    dialog.value.status.model = '待处理';
    dialog.value.remark = '';
    dialog.value.mode = 'create';
    dialog.value.show = true;
    return;
  }
  
  dialog.value.id = (val.id ?? '') as string;
  dialog.value.plate = (val.plate ?? '') as string;
  dialog.value.type = (val.type ?? '') as string;
  dialog.value.date_in = (val.date_in ?? '') as string;
  dialog.value.date_settle = (val.date_settle ?? '') as string;
  dialog.value.class = (val.class ?? '') as string;
  dialog.value.vin = (val.vin ?? '') as string;
  dialog.value.mileage = (val.mileage ?? '') as string;
  dialog.value.phone = (val.phone ?? '') as string;
  dialog.value.project.rows = val.project as TableRow[] | undefined ?? [];
  dialog.value.parts.rows = val.parts as TableRow[] | undefined ?? [];
  dialog.value.cost_manhour.rows = val.cost_manhour as TableRow[] | undefined ?? [];
  dialog.value.cost_materials.rows = val.cost_materials as TableRow[] | undefined ?? [];
  dialog.value.cost_other.rows = val.cost_other as TableRow[] | undefined ?? [];
  dialog.value.create_user = (val.create_user ?? '') as string;
  dialog.value.create_id = (val.create_id ?? '') as string;
  dialog.value.update_user = store.user.name;
  dialog.value.update_id = store.user.id;
  dialog.value.amount_total = Number(val.amount_total ?? 0);
  dialog.value.amount_receipt = Number(val.amount_receipt ?? 0);
  dialog.value.amount_total_chinese = numberToChineseUppercaseCurrency(Number(val.amount_total ?? 0));
  dialog.value.amount_receipt_chinese = numberToChineseUppercaseCurrency(Number(val.amount_receipt ?? 0));
  dialog.value.status.model = (val.status ?? '待处理') as string;
  dialog.value.remark = (val.remark ?? '') as string;
  dialog.value.mode = 'view';
  dialog.value.show = true;
};

const onSumParts = () => {
  const totalAmount = 
    parseFloat(storage.value.model.value.price as string) * dialog.value.parts.dialog.num;

  dialog.value.parts.dialog.amount = totalAmount;
};

const onSum = (type: string, val?: TableRow) => {
  dialog.value.amount_total = 0;
  dialog.value.amount_receipt = 0;

  switch (type) {
    case 'cost_manhour':
      if (val) {
        val.receipt = parseFloat(String(val.manhour)) + parseFloat(String(val.insurance)) - parseFloat(String(val.discount));
      }
      break;
    case 'cost_materials':
      if (val) {
        val.receipt =
          parseFloat(String(val.materials)) + parseFloat(String(val.insurance)) - parseFloat(String(val.discount));
      }
      break;
  }

  dialog.value.project.rows.forEach((val: TableRow) => {
    dialog.value.amount_total += parseFloat(String(val.amount ?? 0));
    dialog.value.amount_receipt += parseFloat(String(val.amount ?? 0));
  });
  dialog.value.parts.rows.forEach((val: TableRow) => {
    dialog.value.amount_total += parseFloat(String(val.amount ?? 0));
    dialog.value.amount_receipt += parseFloat(String(val.amount ?? 0));
  });
  dialog.value.cost_manhour.rows.forEach((val: TableRow) => {
    dialog.value.amount_total += parseFloat(String(val.manhour ?? 0)) + parseFloat(String(val.insurance ?? 0));
    dialog.value.amount_receipt +=
      parseFloat(String(val.manhour ?? 0)) + parseFloat(String(val.insurance ?? 0)) - parseFloat(String(val.discount ?? 0));
  });
  dialog.value.cost_materials.rows.forEach((val: TableRow) => {
    dialog.value.amount_total += parseFloat(String(val.materials ?? 0)) + parseFloat(String(val.insurance ?? 0));
    dialog.value.amount_receipt +=
      parseFloat(String(val.materials ?? 0)) + parseFloat(String(val.insurance ?? 0)) - parseFloat(String(val.discount ?? 0));
  });
  dialog.value.cost_other.rows.forEach((val: TableRow) => {
    dialog.value.amount_total +=
      parseFloat(String(val.manager ?? 0)) + parseFloat(String(Number(val.extra ?? 0))) + parseFloat(String(Number(val.other ?? 0)));
    dialog.value.amount_receipt +=
      parseFloat(String(val.manager ?? 0)) +
      parseFloat(String(Number(val.extra ?? 0))) +
      parseFloat(String(Number(val.other ?? 0))) -
      parseFloat(String(val.discount ?? 0));
  });

  dialog.value.amount_total_chinese = numberToChineseUppercaseCurrency(dialog.value.amount_total);
  dialog.value.amount_receipt_chinese = numberToChineseUppercaseCurrency(
    dialog.value.amount_receipt,
  );
};

const onPartAddRow = () => {
  dialog.value.project.rows.push({
    serial: dialog.value.project.rows.length + 1,
    name: '',
    tag: '',
    principal: '',
    hour: '',
    amount: '',
  });
};

const onPartDelRow = () => {
  dialog.value.project.rows.splice(dialog.value.project.rows.length - 1);
  onSum('amount_project');
};

const onPartsDelRow = () => {
  dialog.value.parts.rows.splice(dialog.value.parts.rows.length - 1);
  onSum('amount_parts');
};

const onManhourAddRow = () => {
  dialog.value.cost_manhour.rows.push({
    manhour: 0,
    insurance: 0,
    discount: 0,
    receipt: 0,
  });
};

const onManhourDelRow = () => {
  dialog.value.cost_manhour.rows.splice(dialog.value.cost_manhour.rows.length - 1);
  onSum('cost_manhour');
};

const onMaterialsAddRow = () => {
  dialog.value.cost_materials.rows.push({
    materials: 0,
    insurance: 0,
    discount: 0,
    receipt: 0,
  });
};

const onMaterialsDelRow = () => {
  dialog.value.cost_materials.rows.splice(dialog.value.cost_materials.rows.length - 1);
  onSum('cost_materials');
};

const onOtherAddRow = () => {
  dialog.value.cost_other.rows.push({
    manager: 0,
    extra: 0,
    other: 0,
    discount: 0,
  });
};

const onOtherDelRow = () => {
  dialog.value.cost_other.rows.splice(dialog.value.cost_other.rows.length - 1);
  onSum('cost_other');
};

const onSave = () => {
  const time = setTimeout(() => {
    loading.value = false;
    clearTimeout(time);
  }, 120000);

  loading.value = true;

  api
    .post(
      store.server + '/api/route/post?auth=jwt&service=repair&path=/api/order/info',
      {
        id: dialog.value.mode === 'view' ? dialog.value.id : '',
        plate: dialog.value.plate,
        type: dialog.value.type,
        date_in: dialog.value.date_in,
        date_settle: dialog.value.date_settle,
        class: dialog.value.class,
        vin: dialog.value.vin,
        mileage: dialog.value.mileage,
        phone: dialog.value.phone,
        project: dialog.value.project.rows,
        parts: dialog.value.parts.rows,
        cost_manhour: dialog.value.cost_manhour.rows,
        cost_materials: dialog.value.cost_materials.rows,
        cost_other: dialog.value.cost_other.rows,
        amount_total: dialog.value.amount_total,
        amount_receipt: dialog.value.amount_receipt,
        status: dialog.value.status.model,
        remark: dialog.value.remark,
        create_user: dialog.value.create_user,
        create_id: dialog.value.create_id,
        update_user: dialog.value.update_user,
        update_id: dialog.value.update_id,
      },
      {
        headers: {
          Authorization: 'Bearer ' + store.quasar.cookies.get('qdsh'),
        },
      },
    )
    .then((resp) => {
      loading.value = false;
      clearTimeout(time);

      if (resp.data.status !== 1) {
        store.quasar.notify('保存失败，请稍后重试');
        
      } else {
        store.quasar.notify('保存成功');
        dialog.value.show = false;
        onQuery();
      }
    })
    .catch(() => {
      store.quasar.notify('网络错误，请稍后重试');
      loading.value = false;
      clearTimeout(time);
    });
};

const onPrint = () => {
  window.open(
    'print/repair_settle_print.html?id=' +
      encodeURI(dialog.value.id) +
      '&user=' +
      encodeURI(store.user.name) +
      '&plate=' +
      encodeURI(dialog.value.plate) +
      '&type=' +
      encodeURI(dialog.value.type) +
      '&date_in=' +
      encodeURI(dialog.value.date_in) +
      '&date_settle=' +
      encodeURI(dialog.value.date_settle) +
      '&class=' +
      encodeURI(dialog.value.class) +
      '&vin=' +
      encodeURI(dialog.value.vin) +
      '&mileage=' +
      encodeURI(dialog.value.mileage) +
      '&phone=' +
      encodeURI(dialog.value.phone) +
      '&project=' +
      encodeURI(JSON.stringify(dialog.value.project.rows)) +
      '&parts=' +
      encodeURI(JSON.stringify(dialog.value.parts.rows)) +
      '&cost_manhour=' +
      encodeURI(JSON.stringify(dialog.value.cost_manhour.rows)) +
      '&cost_materials=' +
      encodeURI(JSON.stringify(dialog.value.cost_materials.rows)) +
      '&cost_other=' +
      encodeURI(JSON.stringify(dialog.value.cost_other.rows)) +
      '&amount_total=' +
      encodeURI(dialog.value.amount_total.toString()) +
      '&amount_total_chinese=' +
      encodeURI(dialog.value.amount_total_chinese.toString()) +
      '&amount_receipt=' +
      encodeURI(dialog.value.amount_receipt.toString()) +
      '&amount_receipt_chinese=' +
      encodeURI(dialog.value.amount_receipt_chinese.toString()) +
      '&status=' +
      encodeURI(dialog.value.status.model) +
      '&remark=' +
      encodeURI(dialog.value.remark) +
      '&create_date=' +
      encodeURI(dialog.value.create_date) +
      '&create_user=' +
      encodeURI(dialog.value.create_user) +
      '&create_id=' +
      encodeURI(dialog.value.create_id) +
      '&update_date=' +
      encodeURI(dialog.value.update_date) +
      '&update_user=' +
      encodeURI(dialog.value.update_user) +
      '&update_id=' +
      encodeURI(dialog.value.update_id),
  );
};
</script>
