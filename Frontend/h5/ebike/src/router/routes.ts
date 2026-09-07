import type { RouteRecordRaw } from 'vue-router';

const routes: RouteRecordRaw[] = [
  {
    path: '/',
    component: () => import('layouts/MainLayout.vue'),
    children: [
      { path: '', component: () => import('pages/IndexPage.vue') },
      { path: '', component: () => import('pages/LoginPage.vue') },
      { path: 'car', component: () => import('src/pages/CarPage.vue') },
      {
        path: 'storage',
        component: () => import('src/pages/StoragePage.vue'),
      },
      {
        path: 'options',
        component: () => import('src/pages/OptionsPage.vue'),
      },
      {
        path: 'order',
        component: () => import('src/pages/OrderPage.vue'),
      },
      {
        path: 'reports',
        component: () => import('src/pages/ReportsPage.vue'),
      },
    ],
  },

  // Always leave this as last one,
  // but you can also remove it
  {
    path: '/:catchAll(.*)*',
    component: () => import('pages/ErrorNotFound.vue'),
  },
];

export default routes;
