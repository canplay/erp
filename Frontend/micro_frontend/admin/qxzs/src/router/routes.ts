import type { RouteRecordRaw } from 'vue-router';

const routes: RouteRecordRaw[] = [
  {
    path: '/',
    component: () => import('layouts/MainLayout.vue'),
    children: [
      { path: '', component: () => import('pages/IndexPage.vue') },
      { path: '/project', component: () => import('pages/ProjectPage.vue') },
      { path: '/assets', component: () => import('pages/AssetsPage.vue') },
      { path: '/business', component: () => import('pages/BusinessPage.vue') },
      { path: '/customer', component: () => import('pages/CustomerPage.vue') },
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
