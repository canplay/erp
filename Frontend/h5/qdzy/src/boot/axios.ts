import { defineBoot } from '#q-app/wrappers'
import axios, { type AxiosInstance } from 'axios'
import { createAlova } from 'alova'
import { axiosRequestAdapter } from '@alova/adapter-axios'
import VueHook from 'alova/vue'
import { Notify } from 'quasar'

declare module 'vue' {
  interface ComponentCustomProperties {
    $axios: AxiosInstance
    $api: AxiosInstance
  }
}

// Be careful when using SSR for cross-request state pollution
// due to creating a Singleton instance here;
// If any client changes this (global) instance, it might be a
// good idea to move this instance creation inside of the
// "export default () => {}" function below (which runs individually
// for each client)
const api = axios.create({
  withCredentials: true,
  timeout: 30000,
})

const alova = createAlova({
  requestAdapter: axiosRequestAdapter({
    axios: api,
  }),
  statesHook: VueHook,
  responded: {
    onSuccess(response) {
      try {
        switch (response.data.data) {
          case 'Logto授权超时':
            Notify.create({
              message: '登录超时，请重新登陆',
            })
            break
          default:
            return response.data
        }
      } catch {
        return response.data
      }
    },
  },
})

export default defineBoot(({ app }) => {
  // for use inside Vue files (Options API) through this.$axios and this.$api

  app.config.globalProperties.$axios = axios
  // ^ ^ ^ this will allow you to use this.$axios (for Vue Options API form)
  //       so you won't necessarily have to import axios in each vue file

  app.config.globalProperties.$api = api
  // ^ ^ ^ this will allow you to use this.$api (for Vue Options API form)
  //       so you can easily perform requests against your app's API
})

export { api, alova }
