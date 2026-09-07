<template>
  <q-page padding>
    <div class="column">
      <div class="col row" style="max-height: 50px">
        <div class="col-9">
          <q-input v-model="filePath" label="文件路径" />
        </div>

        <div class="col-auto" style="min-width: 10px" />

        <div class="col">
          <q-btn
            class="fit"
            color="primary"
            label="选择汇总"
            @click="openFile"
          />
        </div>

        <div class="col-auto" style="min-width: 10px" />

        <div class="col">
          <q-btn
            class="fit"
            color="primary"
            label="分类导出"
            :disable="disableExport"
            @click="doExport"
          />
        </div>
      </div>

      <div class="col-auto" style="min-height: 30px" />

      <div class="col row">
        <div class="col">
          <q-select
            v-model="customers_model"
            :options="customers"
            label="客户"
            @input="customers_input"
          />
        </div>

        <div class="col-auto" style="min-width: 10px" />

        <div class="col">
          <q-select
            v-model="types_model"
            :options="types"
            label="类别"
            @input="types_input"
          />
        </div>
      </div>

      <div class="col-auto" style="min-height: 10px" />

      <div class="col row">
        <div class="col">
          <q-input v-model="operator" label="开单员" />
        </div>

        <div class="col-auto" style="min-width: 10px" />

        <div class="col">
          <q-input v-model="storage" label="仓管员" />
        </div>
      </div>

      <div class="col-auto" style="min-height: 10px" />

      <div class="col">
        <q-scroll-area style="height: 480px">
          <Table v-for="(item, index) of diqu" :key="index" :data="item" />
        </q-scroll-area>
      </div>

      <!-- <q-dialog v-model="dialog" persistent :maximized="true">
      <q-card>
        <q-bar>
          <q-space />
          <q-btn dense flat icon="close" v-close-popup>
            <q-tooltip>关闭</q-tooltip>
          </q-btn>
        </q-bar>

        <q-card-section class="fit">
          <webview
            class="fit"
            :src="
              '../statics/pdfjs-2.2.228-dist/web/viewer.html?file=' + pdfURL
            "
          />
        </q-card-section>
      </q-card>
    </q-dialog>-->
    </div>
  </q-page>
</template>

<script>
import XLSX from "xlsx";
import exceljs from "exceljs";
import fs from "fs";
// import html2Canvas from "html2canvas";
// import jsPDF from "jspdf";

export default {
  name: "PageIndex",

  components: {
    Table: () => import("../components/table.vue")
  },

  data() {
    return {
      filePath: "",
      customers: [],
      customers_model: null,
      customers_map: null,
      types: [],
      types_model: null,
      types_map: null,
      diqu: [],
      paged: false,
      limit: 12,
      count: 12,
      operator: "",
      storage: "",
      disableExport: true
      // dialog: false,
      // pdfURL: ""
    };
  },

  methods: {
    openFile() {
      this.$q.loading.show();

      this.$q.electron.remote.dialog
        .showOpenDialog({
          properties: ["openFile"],
          filters: [{ name: "Excel表格", extensions: ["xlsx"] }]
        })
        .then(async result => {
          if (result.canceled) {
            this.$q.loading.hide();
            return;
          }

          try {
            this.diqu = [];

            this.filePath = result.filePaths[0];

            const workbook = XLSX.readFile(this.filePath);
            const worksheet = workbook.Sheets[workbook.SheetNames[0]];

            let end = true;
            let index = 2;
            this.customers_map = new Map();
            let datas = [];
            while (end) {
              if (!worksheet["A" + index]) {
                if (!worksheet["A" + (index + 1)]) {
                  end = false;
                }

                if (
                  worksheet["B" + (index + 1)] &&
                  worksheet["B" + (index - 1)].w !=
                    worksheet["B" + (index + 1)].w
                ) {
                  datas = [];
                } else if (
                  worksheet["B" + (index + 2)] &&
                  worksheet["B" + (index + 1)].w !=
                    worksheet["B" + (index + 2)].w
                ) {
                  datas = [];
                }

                index++;
                continue;
              }

              datas.push({
                customer: worksheet["B" + index].w,
                phone: worksheet["D" + index].w,
                send: worksheet["E" + index].w,
                driver: worksheet["F" + index].w,
                type: worksheet["H" + index].w,
                name: worksheet["I" + index].w,
                rules: worksheet["J" + index].w,
                units: worksheet["M" + index].w,
                price: this.toDecimal(parseFloat(worksheet["N" + index].w)),
                count: worksheet["L" + index].w,
                sum: this.toDecimal(parseFloat(worksheet["O" + index].w)),
                remark: worksheet["K" + index].w
              });

              this.customers_map.set(worksheet["B" + index].w, datas);

              index++;
            }

            this.customers_model = this.types_model = "";

            this.customers = [];
            this.customers_map.forEach((value, key) => {
              this.customers.push(key);
            });

            this.disableExport = false;
            this.$q.loading.hide();
          } catch (error) {
            this.disableExport = true;
            this.$q.loading.hide();

            this.$q.dialog({
              title: "错误",
              message: "Excel中不存在指定的表头，请重新选择正确的文件",
              ok: {
                label: "确定"
              }
            });
          }
        });
    },

    customers_input() {
      if (!this.customers_map) return;

      this.$q.loading.show();

      this.diqu = [];
      this.types = [];
      this.types_model = "";

      this.types_map = new Map();

      this.customers_map.get(this.customers_model).forEach(value => {
        if (this.types[0] != "全部") this.types = ["全部"];

        let datas = [];

        if (this.types_map.get(value.type) === undefined) {
          datas.push(value);
          this.types_map.set(value.type, datas);
          this.types.push(value.type);
        } else {
          datas = this.types_map.get(value.type);
          datas.push(value);
          this.types_map.set(value.type, datas);
        }
      });

      this.$q.loading.hide();
    },

    types_input() {
      if (!this.types_map) return;

      this.$q.loading.show();

      this.diqu = [];

      if (this.types_model != "全部") {
        let datas = [];
        let xu = 1;
        this.types_map.get(this.types_model).forEach(value => {
          value.index = xu++;
          datas.push(value);
        });

        this.diqu.push({
          customer: datas[0].customer,
          phone: datas[0].phone,
          send: datas[0].send,
          driver: datas[0].driver,
          type: datas[0].type,
          data: datas
        });
      } else {
        this.types_map.forEach(value => {
          let datas = [];
          let xu = 1;
          for (let index = 0; index < value.length; index++) {
            value[index].index = xu++;
            datas.push(value[index]);
          }

          this.diqu.push({
            customer: datas[0].customer,
            phone: datas[0].phone,
            send: datas[0].send,
            driver: datas[0].driver,
            type: datas[0].type,
            data: datas
          });
        });
      }

      this.$q.loading.hide();
    },

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
    },

    addSheet(index, workbook, value) {
      let add = false;

      let worksheet = workbook.addWorksheet(
        index === 0 ? value.type : value.type + index / this.limit,
        {
          pageSetup: { paperSize: 9 }
        }
      );

      let column = worksheet.getColumn("A");
      column.width = 5;
      column = worksheet.getColumn("B");
      column.width = 22;
      column = worksheet.getColumn("C");
      column.width = 5;
      column = worksheet.getColumn("D");
      column.width = 5;
      column = worksheet.getColumn("E");
      column.width = 5;
      column = worksheet.getColumn("F");
      column.width = 5;
      column = worksheet.getColumn("G");
      column.width = 15;
      column = worksheet.getColumn("H");
      column.width = 15;
      column = worksheet.getColumn("I");
      column.width = 8;
      column = worksheet.getColumn("J");
      column.width = 8;
      column = worksheet.getColumn("K");
      column.width = 15;

      worksheet.addRow(["示例市交通服务有限公司销售凭证"]);
      let row = worksheet.lastRow;
      row.font = {
        name: "黑体",
        size: 22,
        bold: true
      };
      worksheet.mergeCells("A" + row.number + ":K" + row.number);
      row.alignment = {
        vertical: "middle",
        horizontal: "center"
      };

      worksheet.addRow([
        "客户名称：" +
          value.customer +
          "     联系电话：" +
          value.phone +
          "     配送日期：" +
          value.send +
          "     商品类别：" +
          value.type
      ]);
      row = worksheet.lastRow;
      row.font = {
        name: "黑体",
        size: 11
      };
      row.height = 21.5;
      row.alignment = {
        vertical: "middle",
        horizontal: "center"
      };
      worksheet.mergeCells("A" + row.number + ":K" + row.number);

      worksheet.addRow([
        "序号",
        "商品名称",
        "数量",
        "单位",
        "单价",
        "金额",
        "规格",
        "生产厂家",
        "生产日期",
        "保质期",
        "备注"
      ]);
      row = worksheet.lastRow;
      row.font = {
        name: "黑体",
        size: 11
      };
      row.alignment = {
        vertical: "middle",
        horizontal: "center"
      };
      row.height = 21.5;
      row.eachCell(cell => {
        cell.border = {
          top: { style: "thin" },
          left: { style: "thin" },
          bottom: { style: "thin" },
          right: { style: "thin" }
        };
      });

      let xu = 1;
      let sum = 0;
      for (let idx = index; idx < value.data.length; idx++) {
        if (idx / this.count === 1) {
          add = true;
          break;
        }

        worksheet.addRow([
          xu++,
          value.data[idx].name,
          value.data[idx].count,
          value.data[idx].units,
          value.data[idx].price,
          value.data[idx].sum,
          value.data[idx].rules,
          "",
          "",
          "",
          value.data[idx].remark
        ]);
        row = worksheet.lastRow;
        row.font = {
          name: "黑体",
          size: 11
        };
        row.height = 21.5;
        row.alignment = {
          vertical: "middle",
          horizontal: "left"
        };
        row.eachCell(cell => {
          cell.border = {
            top: { style: "thin" },
            left: { style: "thin" },
            bottom: { style: "thin" },
            right: { style: "thin" }
          };
        });

        sum += parseFloat(value.data[idx].sum);
      }

      sum = this.toDecimal(sum);

      worksheet.addRow([
        "合计金额（大写）：" +
          this.convet(sum) +
          "                                 合计金额（小写）：" +
          sum +
          " 元"
      ]);
      row = worksheet.lastRow;
      row.font = {
        name: "黑体",
        size: 11
      };
      row.height = 21.5;
      row.alignment = {
        vertical: "middle",
        horizontal: "left"
      };
      worksheet.mergeCells("A" + row.number + ":" + "K" + row.number);
      row.getCell(1).border = {
        top: { style: "thin" },
        left: { style: "thin" },
        bottom: { style: "thin" },
        right: { style: "thin" }
      };

      worksheet.addRow([
        "销售单位名称：示例市交通服务有限公司  统一社会信用代码：91380000000100000B  联系电话：13300000001"
      ]);
      row = worksheet.lastRow;
      row.font = {
        name: "黑体",
        size: 11
      };
      row.height = 21.5;
      row.alignment = {
        vertical: "middle",
        horizontal: "left"
      };
      worksheet.mergeCells("A" + row.number + ":" + "K" + row.number);
      row.getCell(1).border = {
        top: { style: "thin" },
        left: { style: "thin" },
        bottom: { style: "thin" },
        right: { style: "thin" }
      };

      worksheet.addRow([
        "销售单位地址：示例市示例街道示例路1号  食品经营许可证号：JY138000000000000"
      ]);
      row = worksheet.lastRow;
      row.font = {
        name: "黑体",
        size: 11
      };
      row.height = 21.5;
      row.alignment = {
        vertical: "middle",
        horizontal: "left"
      };
      worksheet.mergeCells("A" + row.number + ":" + "K" + row.number);
      row.getCell(1).border = {
        top: { style: "thin" },
        left: { style: "thin" },
        bottom: { style: "thin" },
        right: { style: "thin" }
      };

      worksheet.addRow([
        "开单员：" +
          (this.operator === "" ? "      " : this.operator) +
          "      配送员：" +
          (value.driver === "" ? "      " : value.driver) +
          "      仓管员：" +
          (this.storage === "" ? "      " : this.storage) +
          "      学校领导签字：            收货人："
      ]);
      row = worksheet.lastRow;
      row.font = {
        name: "黑体",
        size: 11
      };
      row.height = 21.5;
      row.alignment = {
        vertical: "middle",
        horizontal: "left"
      };
      worksheet.mergeCells("A" + row.number + ":" + "K" + row.number);

      worksheet.addRow([
        "第一联：销售单位留存 第二联：购货单位留存 第三联：购货单位做账 第四联：销售单位做账 第五联：销售单位仓库"
      ]);
      row = worksheet.lastRow;
      row.font = {
        name: "黑体",
        size: 11
      };
      row.height = 21.5;
      row.alignment = {
        vertical: "middle",
        horizontal: "left"
      };
      worksheet.mergeCells("A" + row.number + ":" + "K" + row.number);

      worksheet.pageSetup.margins = {
        left: 0,
        right: 0,
        top: 0,
        bottom: 0,
        header: 0,
        footer: 0
      };

      if (add) {
        this.count += this.limit;
        index += this.limit;
        this.addSheet(index, workbook, value);
      }
    },

    async doExport() {
      // let printTables = document.querySelectorAll(
      //   "div.q-table__container.q-table--cell-separator.q-table__card.q-table--dense.q-table--no-wrap"
      // );

      this.$q.loading.show();

      const workbook = new exceljs.Workbook();

      this.diqu.forEach(async value => {
        this.addSheet(0, workbook, value);
      });

      workbook.xlsx.writeBuffer().then(async buffer => {
        let file = process.cwd() + "\\" + this.diqu[0].customer + ".xlsx";

        fs.writeFile(file, buffer, {}, () => {});

        this.$q.electron.shell.openItem(file);

        setInterval(() => {
          this.$q.loading.hide();
        }, 5000);
      });
    }

    // async doExport() {
    //   this.$q.loading.show();

    //   const readFileAsync = util.promisify(fs.readFile);
    //   const writeFileAsync = util.promisify(fs.writeFile);

    //   const exlBuf = await readFileAsync("./export.xlsx");

    //   const data = [
    //     {
    //       customer: this.diqu[0].customer,
    //       phone: this.diqu[0].phone,
    //       send: this.diqu[0].send,
    //       type: this.diqu[0].type
    //     }
    //   ];

    //   const exlBuf2 = await ejsExcel.renderExcel(exlBuf, data);
    //   await writeFileAsync(this.diqu[0].customer + ".xlsx", exlBuf2);

    //   this.$q.loading.hide();
    // }

    // printPDF() {
    //   this.$q.loading.show();

    //   let printTables = document.querySelectorAll(
    //     "div.q-table__container.q-table--cell-separator.q-table__card.q-table--dense.q-table--no-wrap"
    //   );

    //   let PDF = new jsPDF({
    //     orientation: "l",
    //     unit: "mm",
    //     format: "a4"
    //   });

    //   printTables.forEach((value, index) => {
    //     html2Canvas(value, {
    //       allowTaint: true,
    //       height: value.scrollHeight,
    //       width: value.scrollWidth,
    //       background: "#FFFFFF"
    //     }).then(canvas => {
    //       let imgWidth = 290;
    //       let imgHeight = (290 / canvas.width) * canvas.height;
    //       let pageData = canvas.toDataURL("image/jpeg", 1.0);

    //       PDF.addImage(pageData, "jpeg", 3, 3, imgWidth, imgHeight);

    //       if (printTables.length - index != 1) {
    //         PDF.addPage({
    //           orientation: "l",
    //           unit: "mm",
    //           format: "a4"
    //         });
    //       }

    //       if (printTables.length - index === 1) {
    //         this.pdfURL = PDF.output("bloburi");
    //         this.$q.loading.hide();
    //         this.dialog = true;
    //       }
    //     });
    //   });
    // }
  }
};
</script>
