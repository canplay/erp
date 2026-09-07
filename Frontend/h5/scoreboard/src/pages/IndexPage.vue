<template>
  <q-page class="flex flex-center">
    <div class="q-pa-md" style="max-width: 400px; width: 100%">
      <div class="row justify-center q-mb-lg">
        <h2 class="text-h4 text-weight-bold">记分牌系统</h2>
      </div>

      <q-card class="q-mb-md">
        <q-card-section>
          <div class="text-h6">记分牌展示</div>
          <div class="text-subtitle2">查看和展示比赛分数</div>
        </q-card-section>

        <q-card-actions align="right">
          <q-btn color="primary" label="打开记分牌" @click="$router.push('/control?tab=display')" />
        </q-card-actions>
      </q-card>

      <q-card>
        <q-card-section>
          <div class="text-h6">控制面板</div>
          <div class="text-subtitle2">控制比分、时间和队伍信息</div>
        </q-card-section>

        <q-card-actions align="right">
          <q-btn color="secondary" label="打开控制面板" @click="$router.push('/control')" />
        </q-card-actions>
      </q-card>

      <div class="q-mt-xl text-center text-grey-7">
        <p>使用键盘快捷键控制记分牌：</p>
        <p>H/I - 主队加分 | G/O - 客队加分</p>
        <p>K - 主队减分 | L - 客队减分</p>
        <p>T - 启动/停止时钟 | U - 增加时间 | J - 减少时间</p>
      </div>
    </div>
  </q-page>
</template>
