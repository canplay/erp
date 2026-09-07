<template>
  <div class="bg-white" style="height: calc(100vh - 81px)">
    <q-select
      square
      standout
      v-model="place.model"
      :options="place.options"
      label="选择停车场"
    />

    <div style="height: 5px" />

    <compQihua v-if="place.model === '示例站点A停车场'" />

    <compShuangqiao v-if="place.model === '示例花园地下停车场'" />

    <compXihua v-if="place.model === '西华公园停车场'" />

    <compPipadao v-if="place.model === '示例岛停车场'" />

    <compZhonghang v-if="place.model === '示例路中行停车场'" />

    <compMinzucun v-if="place.model === '示例村停车场'" />

    <compWenhua v-if="place.model === '示例广场停车场'" />

    <compHebinlu v-if="place.model === '示例路停车场'" />

    <compSixiao v-if="place.model === '示例学校停车场'" />

    <compErxiao v-if="place.model === '示例学校停车场'" />

    <compPanlong v-if="place.model === '盘龙示例站点E停车场'" />

    <compByg v-if="place.model === '示例馆停车场'" />

    <compXfct v-if="place.model === '城北晟宇停车场'" />

    <compCxt v-if="place.model === '城乡投停车场'" />
  </div>
</template>

<script lang="ts">
import { defineComponent, ref } from 'vue';
import compQihua from 'components/public_assets/dz_qihua.vue';
import compShuangqiao from 'components/public_assets/dz_sqhy.vue';
import compXihua from 'components/public_assets/dz_xihua.vue';
import compPipadao from 'components/public_assets/dz_pipadao.vue';
import compMinzucun from 'components/public_assets/dz_mzc.vue';
import compWenhua from 'components/public_assets/dz_wenhua.vue';
import compHebinlu from 'components/public_assets/dz_hbl.vue';
import compSixiao from 'components/public_assets/dz_sixiao.vue';
import compZhonghang from 'components/public_assets/dz_zhonghang.vue';
import compErxiao from 'components/public_assets/dz_erxiao.vue';
import compPanlong from 'components/public_assets/dz_panlong.vue';
import compByg from 'src/components/public_assets/dz_byg.vue';
import compXfct from 'components/public_assets/dz_xfct.vue';
import compCxt from 'components/public_assets/dz_cxt.vue';

export default defineComponent({
  name: 'CompPublicDz',

  components: {
    compQihua,
    compShuangqiao,
    compXihua,
    compZhonghang,
    compPipadao,
    compMinzucun,
    compWenhua,
    compHebinlu,
    compSixiao,
    compErxiao,
    compPanlong,
    compByg,
    compXfct,
    compCxt,
  },

  setup() {
    return {
      place: ref({
        model: '城乡投停车场',
        options: [
          '示例花园地下停车场',
          '西华公园停车场',
          '示例岛停车场',
          '示例路中行停车场',
          '示例村停车场',
          '示例广场停车场',
          '示例路停车场',
          '示例学校停车场',
          '示例学校停车场',
          '盘龙示例站点E停车场',
          '星光夜市停车场',
          '城北晟宇停车场',
          '城乡投停车场',
        ],
      }),
    };
  },
});
</script>
