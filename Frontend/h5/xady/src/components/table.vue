<template>
  <div style="page-break-after:always">
    <q-table
      :data="data.data"
      :columns="columns"
      row-key="name"
      separator="cell"
      dense
      :pagination.sync="pagination"
      :rows-per-page-options="[0]"
    >
      <template v-slot:top>
        <div class="column fit text-no-wrap">
          <div class="col q-table__title text-center">
            示例市交通服务有限公司销售凭证
          </div>
          <div class="col row">
            <div class="col">客户名称：{{ data.customer }}</div>
            <div class="col">联系电话：{{ data.phone }}</div>
            <div class="col">配送日期：{{ data.send }}</div>
            <div class="col">商品类别：{{ data.type }}</div>
          </div>
        </div>
      </template>

      <template v-slot:bottom>
        <div class="column fit" style="margin-top: 10px">
          <div class="col row text-no-wrap">
            <div class="col-auto">合计金额（大写）：</div>
            <div class="col">{{ sumD }}</div>
            <div class="col-auto">合计金额（小写）：</div>
            <div class="col">{{ sum }} 元</div>
          </div>

          <div class="col row text-no-wrap">
            <div class="col-auto">销售单位名称：</div>
            <div class="col">示例市交通服务有限公司</div>

            <div class="col-auto">统一社会信用代码：</div>
            <div class="col">91380000000100000B</div>

            <div class="col-auto">联系电话：</div>
            <div class="col">13300000001</div>
          </div>

          <div class="col row text-no-wrap">
            <div class="col-auto">销售单位地址：</div>
            <div class="col">示例市示例街道示例路1号</div>

            <div class="col-auto">食品经营许可证号：</div>
            <div class="col">JY138000000000000</div>
          </div>

          <div class="col row text-no-wrap">
            <div class="col">开单员：[ 只在分类导出表中显示 ]</div>

            <div class="col-auto" style="min-width:100px" />

            <div class="col">配送员：{{ data.driver }}</div>

            <div class="col-auto" style="min-width:100px" />

            <div class="col">仓管员：[ 只在分类导出表中显示 ]</div>

            <div class="col-auto" style="min-width:100px" />

            <div class="col">学校领导签字：</div>

            <div class="col-auto" style="min-width:100px" />

            <div class="col">收货人：</div>

            <div class="col-auto" style="min-width:100px" />
          </div>

          <div class="col row text-no-wrap">
            <div class="col">第一联：销售单位留存</div>

            <div class="col-auto" style="min-width:20px" />

            <div class="col">第二联：购货单位留存</div>

            <div class="col-auto" style="min-width:20px" />

            <div class="col">第三联：购货单位做账</div>

            <div class="col-auto" style="min-width:20px" />

            <div class="col">第四联：销售单位做账</div>

            <div class="col-auto" style="min-width:20px" />

            <div class="col">第五联：销售单位仓库</div>

            <div class="col-auto" style="min-width:20px" />
          </div>
        </div>
      </template>
    </q-table>

    <div style="min-height: 20px" />
  </div>
</template>

<script>
export default {
  props: {
    data: Object
  },

  data() {
    return {
      columns: [
        {
          name: "index",
          align: "left",
          label: "序号",
          field: "index"
        },
        {
          name: "name",
          align: "left",
          label: "商品名称",
          field: "name"
        },
        {
          name: "count",
          align: "left",
          label: "数量",
          field: "count"
        },
        {
          name: "units",
          align: "left",
          label: "单位",
          field: "units"
        },
        {
          name: "price",
          align: "left",
          label: "单价",
          field: "price"
        },
        {
          name: "sum",
          align: "left",
          label: "金额",
          field: "sum"
        },
        {
          name: "rules",
          align: "left",
          label: "规格",
          field: "rules"
        },
        {
          name: "maker",
          align: "left",
          label: "生产厂家",
          field: "maker"
        },
        {
          name: "date",
          align: "left",
          label: "生产日期",
          field: "date"
        },
        {
          name: "life",
          align: "left",
          label: "保质期",
          field: "life"
        },
        {
          name: "remark",
          align: "left",
          label: "备注",
          field: "remark"
        }
      ],
      sum: 0,
      sumD: "零",
      pagination: {
        rowsPerPage: 0
      }
    };
  },

  methods: {
    convet(n) {
      let fraction = ["角", "分"];
      let digit = ["零", "壹", "贰", "叁", "肆", "伍", "陆", "柒", "捌", "玖"];
      let unit = [
        ["元", "万", "亿"],
        ["", "拾", "佰", "仟"]
      ];
      let head = n < 0 ? "欠" : "";
      n = Math.abs(n);
      let s = "";
      for (let i = 0; i < fraction.length; i++) {
        s += (
          digit[
            Math.floor(
              (Math.floor(n * 1000 * 10 * Math.pow(10, i)) % (10 * 1000)) / 1000
            )
          ] + fraction[i]
        ).replace(/零./, "");
      }
      s = s || "整";
      n = Math.floor(n);
      for (let i = 0; i < unit[0].length && n > 0; i++) {
        let p = "";
        for (let j = 0; j < unit[1].length && n > 0; j++) {
          p = digit[n % 10] + unit[1][j] + p;
          n = Math.floor(n / 10);
        }
        s = p.replace(/(零.)*零$/, "").replace(/^$/, "零") + unit[0][i] + s;
      }
      return (
        head +
        s
          .replace(/(零.)*零元/, "元")
          .replace(/(零.)+/g, "零")
          .replace(/^整$/, "零元整")
      );
    },

    toDecimal(x) {
      let f = Math.round(parseFloat(x) * 100) / 100;
      let s = f.toString();
      let rs = s.indexOf(".");
      if (rs < 0) {
        rs = s.length;
        s += ".";
      }
      while (s.length <= rs + 2) {
        s += "0";
      }
      return s;
    }
  },

  created() {
    for (let index = 0; index < this.data.data.length; index++) {
      this.sum += parseFloat(this.data.data[index].sum);
    }
    this.sum = this.toDecimal(this.sum);
    this.sumD = this.convet(this.sum);
  }
};
</script>

<style></style>
