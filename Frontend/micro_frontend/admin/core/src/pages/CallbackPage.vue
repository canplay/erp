<template>
  <q-page class="row items-center justify-evenly text-h1 text-bold">
    <p v-if="isLoading">重定向...</p>
  </q-page>
</template>

<script setup lang="ts">
import { useHandleSignInCallback } from '@logto/vue';
import { useRouter } from 'vue-router';

const router = useRouter();

const { isLoading } = useHandleSignInCallback(() => {
  // Do something when finished, e.g. redirect to home page
  router.push('/').catch(() => {
    window.location.reload();
  });
});
</script>
