import { defineStore, acceptHMRUpdate } from 'pinia'
import type { User } from 'src/components/model'
import { ref } from 'vue'

export const useStore = defineStore('global', () => {
  const page = ref({
    path: '/',
    title: '首页',
  })
  const logto = ref({
    url: 'https://logto.cm.example.com/',
    clientId: 'unh6mzm2ce7ikbvh7afwb',
  })
  const server = ref({
    erp: 'https://erp.example.com/api/route',
    self: 'https://qdzy.cm.example.com/',
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
      options: [{ value: 1, label: '西华云锦' }],
    },
    type: {
      model: 1,
      options: [
        { value: 1, label: '送水到家' },
        { value: 2, label: '家政服务' },
        { value: 3, label: '预约看房' },
      ],
    },
  })
  const user = ref<User>({
    id: '',
  })

  return {
    page,
    logto,
    server,
    order,
    user,
  }
})

if (import.meta.hot) {
  import.meta.hot.accept(acceptHMRUpdate(useStore, import.meta.hot))
}
