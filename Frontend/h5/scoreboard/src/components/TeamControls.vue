<template>
  <q-card class="q-pa-md dark-card">
    <q-input
      filled
      :model-value="teamName"
      :label="`${isHome ? '主队' : '客队'}名称`"
      @update:model-value="onNameChange"
      class="q-mb-md"
    />

    <div class="text-center">
      <div class="text-h3 q-my-md">{{ teamScore }}</div>
      <div class="q-gutter-sm">
        <q-btn color="positive" label="加分 (+1)" @click="() => onScoreAdjust(1)" />
        <q-btn color="negative" label="减分 (-1)" @click="() => onScoreAdjust(-1)" />
        <q-btn color="warning" label="重置" @click="onScoreReset" />
      </div>
    </div>

    <div class="q-mt-md">
      <q-btn outline label="选择Logo" @click="onLogoSelect" class="full-width" />
    </div>
  </q-card>
</template>

<script setup lang="ts">
interface Props {
  teamName: string;
  teamScore: number;
  isHome: boolean;
}

interface Emits {
  (e: 'name-change', value: string | number | null): void;
  (e: 'score-adjust', delta: number): void;
  (e: 'score-reset'): void;
  (e: 'logo-select'): void;
}

defineProps<Props>();
const emit = defineEmits<Emits>();

const onNameChange = (value: string | number | null) => {
  emit('name-change', value || '');
};

const onScoreAdjust = (delta: number) => {
  emit('score-adjust', delta);
};

const onScoreReset = () => {
  emit('score-reset');
};

const onLogoSelect = () => {
  emit('logo-select');
};
</script>

<style scoped>
.dark-card {
  background-color: rgba(255, 255, 255, 0.05);
}
</style>