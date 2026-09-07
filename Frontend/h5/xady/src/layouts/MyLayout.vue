<template>
  <q-layout view="lHh Lpr lFf" style="background-color: white">
    <q-header elevated>
      <q-toolbar class="q-electron-drag">
        <q-toolbar-title>
          {{ $q.electron.remote.getCurrentWindow().getTitle() }}
        </q-toolbar-title>

        <q-btn dense flat icon="minimize" @click="minimize">
          <q-tooltip content-style="font-size: 12px">最小化</q-tooltip>
        </q-btn>

        <q-btn dense flat icon="close" @click="close">
          <q-tooltip content-style="font-size: 12px">关闭</q-tooltip>
        </q-btn>
      </q-toolbar>
    </q-header>

    <q-page-container>
      <router-view />
    </q-page-container>
  </q-layout>
</template>

<script>
export default {
  name: "MyLayout",

  methods: {
    minimize() {
      this.$q.electron.remote.getCurrentWindow().minimize();
    },

    close() {
      window.close();
    },

    onResize() {
      this.style = {
        width: window.innerWidth + "px",
        height: window.innerHeight - 32 + "px"
      };
    }
  },

  created() {
    window.addEventListener("resize", this.onResize);
  },

  destroyed() {
    window.removeEventListener("resize", this.onresize);
  },

  mounted() {
    this.onResize();
  }
};
</script>
