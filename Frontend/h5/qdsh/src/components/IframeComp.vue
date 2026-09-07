<template>
  <div style="overflow: hidden; -webkit-overflow-scrolling: touch; -webkit-overflow: auto">
    <iframe
      seamless
      frameborder="0"
      sandbox="allow-forms allow-modals allow-same-origin allow-scripts"
      :src="props.url"
      style="width: 100%; height: 100%"
    />
  </div>
</template>

<script setup lang="ts">
const props = defineProps(['url'])
</script>
