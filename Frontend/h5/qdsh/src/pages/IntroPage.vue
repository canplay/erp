<template>
  <q-page>
    <div class="content">Welcome</div>
    <div class="overlay" ref="overlayRef">
      <div class="panel left"></div>
      <div class="panel right"></div>
      <div class="cut-lines">
        <div class="line line-top"></div>
        <div class="line line-bottom"></div>
      </div>
    </div>
  </q-page>
</template>

<style lang="scss" scoped>
.content {
  width: 100%;
  height: 100vh;
  background: #f0f0f0;
  display: flex;
  justify-content: center;
  align-items: center;
  font-size: 2em;
}

.overlay {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  overflow: hidden;
  z-index: 1000;
}

.panel {
  position: absolute;
  top: 0;
  width: 50%;
  height: 100%;
  background: #000;
  transition: transform 1s;
}

.left {
  left: 0;
  transform-origin: left center;
  animation: slide-left 1s ease-in-out forwards;
}

.right {
  right: 0;
  transform-origin: right center;
  animation: slide-right 1s ease-in-out forwards;
}

/* 新增双对角线 */
.cut-lines {
  position: absolute;
  width: 100%;
  height: 100%;
}

/* 右上到左下对角线 */
.line {
  position: absolute;
  width: 2px;
  height: 280%; /* 加长线段确保覆盖 */
  background: #fff;
  animation: diagonal-cut 1s ease-in-out;
}

/* 右上角起始线 */
.line-top {
  top: -100%;
  right: 0;
  transform-origin: 100% 0;
  transform: rotate(-45deg);
}

/* 左下角起始线 */
.line-bottom {
  bottom: -100%;
  left: 0;
  transform-origin: 0 100%;
  transform: rotate(-45deg);
}

@keyframes slide-left {
  to {
    transform: translateX(-100%);
  }
}

@keyframes slide-right {
  to {
    transform: translateX(100%);
  }
}

@keyframes diagonal-cut {
  0% {
    transform: rotate(-45deg) translateY(-100%);
  }
  100% {
    transform: rotate(-45deg) translateY(100%);
  }
}
</style>

<script setup lang="ts">
import { onMounted, ref } from 'vue'

const overlayRef = ref<HTMLElement>()

onMounted(() => {
  window.addEventListener('load', () => {
    // 严格的空值检查
    if (!overlayRef.value) {
      console.error('Overlay element not found')
      return
    }

    // 单个动画结束处理（所有子动画完成后移除）
    let animationCount = 0
    const handleSingleEnd = () => {
      if (++animationCount === 3) {
        if (!overlayRef.value) {
          console.error('Overlay element not found')
          return
        }

        overlayRef.value.remove()
      }
    }

    // 查询所有动画元素
    const animatedElements = [
      overlayRef.value.querySelector('.left'),
      overlayRef.value.querySelector('.right'),
      overlayRef.value.querySelector('.cut-lines'),
    ].filter(Boolean) as HTMLElement[]

    // 添加事件监听
    animatedElements.forEach((el) => {
      el.addEventListener('animationend', handleSingleEnd)
    })
  })
})
</script>
