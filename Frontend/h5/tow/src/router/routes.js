const routes = [
  {
    path: '/',
    component: () => import('layouts/MainLayout.vue'),
    children: [
      { path: '', component: () => import('pages/LoginPage.vue') },
      { path: 'index', component: () => import('pages/IndexPage.vue') },
      { path: 'car', component: () => import('src/pages/CarPage.vue') },
      { path: 'reports', component: () => import('pages/ReportsPage.vue') },
      { path: 'options', component: () => import('pages/OptionsPage.vue') },
      { path: 'input', component: () => import('pages/InputPage.vue') },
    ],
  },

  // Always leave this as last one,
  // but you can also remove it
  {
    path: '/:catchAll(.*)*',
    component: () => import('pages/ErrorNotFound.vue'),
  },
]

export default routes
