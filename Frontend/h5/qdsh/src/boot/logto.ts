import { defineBoot } from '#q-app/wrappers'
import type { LogtoConfig } from '@logto/vue'
import { createLogto, UserScope } from '@logto/vue'
import { useStore } from 'src/stores/store'

export default defineBoot(({ app }) => {
  const store = useStore()
  const config: LogtoConfig = {
    endpoint: store.server.logto,
    appId: 'q7qv4rjhkip35cqepv3cq',
    scopes: [
      UserScope.CustomData,
      UserScope.Email,
      UserScope.Identities,
      UserScope.OrganizationRoles,
      UserScope.Organizations,
      UserScope.Phone,
      UserScope.Profile,
      UserScope.Roles,
    ],
  }
  app.use(createLogto, config)
})
