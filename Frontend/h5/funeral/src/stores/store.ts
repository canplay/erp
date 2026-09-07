import { defineStore, acceptHMRUpdate } from 'pinia'
import { ref } from 'vue'

export const useStore = defineStore('global', () => {
  const page = ref({
    path: '/',
    title: '首页',
  })
  const order = ref({
    show: false,
    name: '',
    phone: '',
    address: '',
    src: '',
    remark: '',
    zone: {
      model: 1,
      options: [{ value: 1, label: '罗汉山陵园' }],
    },
    type: {
      model: 1,
      options: [
        { value: 1, label: '上门服务' },
        { value: 2, label: '线上祭扫' },
        { value: 3, label: '代客祭扫' },
      ],
    },
  })
  return { page, order }
})

if (import.meta.hot) {
  import.meta.hot.accept(acceptHMRUpdate(useStore, import.meta.hot))
}
