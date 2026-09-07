import { defineBoot } from '#q-app/wrappers'
import type { LogtoConfig } from '@logto/vue'
import { createLogto } from '@logto/vue'

export default defineBoot(({ app }) => {
  const config: LogtoConfig = {
    endpoint: 'https://logto.cm.example.com/',
    appId: 'unh6mzm2ce7ikbvh7afwb',
  }
  app.use(createLogto, config)
})
