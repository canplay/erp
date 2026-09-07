<template>
  <q-page class="bg-grey-9 text-white flex flex-center">
    <div class="about-content q-pa-xl text-center">
      <h1 class="text-h3 q-mb-xl">记分牌系统</h1>
      
      <div class="q-mb-lg">
        <q-img
          src="https://placehold.co/200x200/333333/FFFFFF?text=LOGO"
          class="q-mx-auto"
          style="max-width: 150px;"
        />
      </div>
      
      <div class="q-mb-lg">
        <p class="text-h6">Version 1.0.0</p>
        <p class="text-subtitle2 text-grey-5">基于 Quasar Framework 构建</p>
      </div>
      
      <div class="q-mb-xl">
        <p>这是一个现代化的记分牌应用，支持多记分牌管理、实时分数更新、计时器等功能。</p>
        <p>适用于各种比赛和活动的计分需求。</p>
      </div>
      
      <div class="features q-mb-xl">
        <h2 class="text-h5 q-mb-md">主要功能</h2>
        <div class="q-gutter-y-md">
          <div class="feature-item">
            <q-icon name="sports" size="lg" class="q-mr-md" />
            <span>多记分牌管理</span>
          </div>
          <div class="feature-item">
            <q-icon name="timer" size="lg" class="q-mr-md" />
            <span>计时器功能（正计时/倒计时）</span>
          </div>
          <div class="feature-item">
            <q-icon name="keyboard" size="lg" class="q-mr-md" />
            <span>快捷键操作</span>
          </div>
          <div class="feature-item">
            <q-icon name="photo" size="lg" class="q-mr-md" />
            <span>队徽上传</span>
          </div>
          <div class="feature-item">
            <q-icon name="settings" size="lg" class="q-mr-md" />
            <span>实时控制面板</span>
          </div>
        </div>
      </div>
      
      <div class="q-mb-lg">
        <p class="text-subtitle1">© {{ new Date().getFullYear() }} 记分牌系统</p>
        <p class="text-caption text-grey-6">本软件按"现状"提供，不提供任何明示或暗示的保证。</p>
      </div>
      
      <q-btn
        label="返回首页"
        color="primary"
        to="/"
        class="q-mt-lg"
      />
    </div>
  </q-page>
</template>

<script setup lang="ts">
// 关于页面不需要特殊逻辑
</script>

<style scoped>
.about-content {
  max-width: 800px;
  width: 100%;
}

.feature-item {
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 1.1em;
}
</style>