import { defineStore, acceptHMRUpdate } from 'pinia'
import { ref } from 'vue'

export const useStore = defineStore('global', () => {
  const backend = process.env.DEV ? 'http://127.0.0.1:51530' : 'https://tow.cm.example.com'
  const test = ref(false)
  const options = ref({
    system: null,
    alert: null,
    list: [],
    car: {
      class: [],
      type: [],
      color: [],
    },
    causes: {
      type: [],
      list: [],
    },
  })
  const schedule = ref({
    now: {
      time: '0000-00-00 00:00:00',
      interval: null,
    },
    system: {
      run: true,
      list: null,
      interval: null,
    },
    alert: {
      run: true,
      list: null,
      interval: null,
    },
  })
  const user = ref({
    username: '',
    password: '',
    name: '',
    location: '',
  })
  const xlsx = ref({
    决定书: [
      ['示例城市A市城乡管理综合执法局'],
      ['查封（扣押）决定书'],
      ['文城乡执查扣决字〔2021〕A-001号'],
      ['                                 '],
      ['当事人：', '联系电话：'],
      ['法定代表人（负责人）：'],
      ['地 址：'],
      [
        '根据《示例城市A壮族苗族自治州贯彻<示例省份省城市建设管理条例>实施办法》第二十七条第七项的规定，你单位（人）        涉嫌（存在）在示例城市A市\n                            人行道上非指定地点停放的问题，现决定对你单位（人）的有关物品/场所予以查封（扣押）。在查封（扣押）期间，对查封扣押的场所、设施和财物，应当妥善保存，不得使用、销毁或者擅自转移。当事人不得擅自启封。\n查封（扣押）物品保存地点/场所地点：依法暂扣并对车辆进行锁定\n查封（扣押）物品期限：自      年   月   日至      年   月   日。查封扣押物品保存条件：留置保存。本决定书附《查封（扣押）物品清单》。\n你单位（人）可以对本决定进行陈述和申辩。如不服本决定，可根据《中华人民共和国行政复议法》，在收到本决定之日起60日内向示例城市A市人民政府申请行政复议，或者根据《中华人民共和国行政诉讼法》在6个月内向示例城市A市人民法院提起行政诉讼。',
      ],
      ['执法人员（签名）：', '执法证号：'],
      ['执法人员（签名）：', '执法证号：'],
      ['联系电话：', '办公地址：'],
      [' '],
      [null, null, '执法单位(印章)'],
      [null, null, '年   月   日'],
    ],
    物品清单: [
      ['查封（扣押）物品清单'],
      ['文城乡执查扣物字〔2021〕A-001号'],
      ['序号', '物品名称', '规格', '数量', '车辆号牌', '备注'],
      [null, null, null, '（质量）'],
      [],
      [],
      [],
      [],
      [],
      [],
      [' '],
      ['当事人（签章）：', null, null, null, null, '示例城市A市城乡管理综合执法局'],
      ['年   月   日', null, null, null, '年   月   日'],
    ],
    送达回执: [
      ['示例城市A市城乡管理综合执法局行政文书'],
      ['送达回执'],
      [null, null, null, '案件名称：'],
      ['受送达人\n名称', null, null, '电话'],
      ['受送达人\n地址'],
      [
        '送达文书\n名称及文号',
        '示例城市A市城乡管理综合执法局\n查封（扣押）决定书\n（文城乡执查扣决字〔2021〕A-001）',
        null,
        '示例城市A市城乡管理综合执法局           \n查封（扣押）物品清单\n（文城乡执查扣物字〔2021〕A-001号）',
      ],
      ['送达地点', '示例城市A市城乡管理综合执法局'],
      ['送达日期', '     年   月   日   时   分'],
      [
        '送达方式\n（在□处打“√”）',
        '□直接送达。\n□邮寄送达，回执号码          。\n□传真（电子邮件）送达。\n□委托送达，受托部门               。\n□转交送达，转交人与当事人关系\n□留置送达，受送达人拒绝接受上述文书，代收人或者见证人不愿意在回执上签名，执法人员将上述文书留置在\n               。',
      ],
      ['受送达人\n拒收理由'],
      ['受送达人或者\n代收人（签名）'],
      [null, '                               年   月   日'],
      ['送达人（签名）'],
      ['备 注'],
    ],
  })
  return {
    backend,
    test,
    options,
    schedule,
    user,
    xlsx,
  }
})

if (import.meta.hot) {
  import.meta.hot.accept(acceptHMRUpdate(useStore, import.meta.hot))
}
