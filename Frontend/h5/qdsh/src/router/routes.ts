import type { RouteRecordRaw } from 'vue-router'

const routes: RouteRecordRaw[] = [
  {
    path: '/',
    component: () => import('layouts/MainLayout.vue'),
    children: [
      {
        path: '',
        component: () => import('pages/IndexPage.vue'),
      },
      { path: 'discover', component: () => import('pages/DiscoverPage.vue') },
      { path: 'chat', component: () => import('pages/ChatPage.vue') },
      { path: 'shop', component: () => import('pages/ShopPage.vue') },
      { path: 'iframe', component: () => import('pages/IframePage.vue') },
      { path: 'my', component: () => import('pages/MyPage.vue') },
      { path: 'view', component: () => import('pages/ViewPage.vue') },
      { path: 'notify', component: () => import('pages/NotifyPage.vue') },
      { path: 'licence', component: () => import('pages/LicencePage.vue') },
      { path: 'privacy', component: () => import('pages/PrivacyPage.vue') },
      { path: 'garbage', component: () => import('pages/GarbagePage.vue') },
      { path: 'options', component: () => import('pages/OptionsPage.vue') },
      { path: 'assets', component: () => import('pages/AssetsPage.vue') },
      { path: 'game', component: () => import('pages/GamePage.vue') },
      { path: 'trailer', component: () => import('pages/TrailerPage.vue') },
      { path: 'intro', component: () => import('pages/IntroPage.vue') },
      { path: 'callback', component: () => import('pages/CallbackPage.vue') },
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
