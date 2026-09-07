import type { RouteRecordRaw } from 'vue-router';

const routes: RouteRecordRaw[] = [
  {
    path: '/',
    component: () => import('layouts/MainLayout.vue'),
    children: [
      { path: '', component: () => import('pages/SigninPage.vue') },
      { path: 'index', component: () => import('pages/IndexPage.vue') },
      {
        path: 'visual',
        component: () => import('pages/VisualPage.vue'),
      },
      { path: 'chat', component: () => import('pages/ChatPage.vue') },
      {
        path: 'public_assets.assets',
        component: () => import('pages/public_assets/AssetsPage.vue'),
      },
      {
        path: 'livestock',
        children: [
          {
            path: 'receipt',
            component: () => import('pages/livestock/ReceiptPage.vue'),
          },
        ],
      },
      {
        path: 'clean',
        children: [
          {
            path: 'receipt',
            component: () => import('pages/clean/ReceiptPage.vue'),
          },
        ],
      },
      {
        path: 'tow',
        children: [
          {
            path: 'order',
            component: () => import('pages/tow/OrderPage.vue'),
          },
          {
            path: 'report',
            // component: () => import('pages/tow/ordertable_test.vue'),
            // component: () => import('pages/tow/Reporttest.vue'),

            component: () => import('pages/tow/ReportPage.vue'),
          },
        ],
      },
      {
        path: 'afforest',
        children: [
          {
            path: 'zone',
            component: () => import('pages/afforest/ZonePage.vue'),
          },
          {
            path: 'plant',
            component: () => import('pages/afforest/PlantPage.vue'),
          },
          {
            path: 'assets',
            component: () => import('pages/afforest/AssetsPage.vue'),
          },
          {
            path: 'patrol',
            component: () => import('pages/afforest/PatrolPage.vue'),
          },
          {
            path: 'workorder',
            component: () => import('pages/afforest/WorkOrderPage.vue'),
          },
        ],
      },
      {
        path: 'repair',
        children: [
          {
            path: 'order',
            component: () => import('pages/repair/OrderPage.vue'),
          },
          {
            path: 'storage',
            component: () => import('pages/repair/StoragePage.vue'),
          },
        ],
      },
      {
        path: 'system',
        children: [
          {
            path: 'user',
            component: () => import('pages/system/UserPage.vue'),
          },
          {
            path: 'permission',
            component: () => import('pages/system/PermissionPage.vue'),
          },
          {
            path: 'option',
            component: () => import('pages/system/OptionPage.vue'),
          },
        ],
      },
      {
        path: 'property',
        children: [
          {
            path: 'estate',
            component: () => import('pages/property/EstatePage.vue'),
          },
        ],
      },
      {
        path: 'ticket',
        children: [
          {
            path: 'seatsOrder',
            component: () => import('pages/ticket/SeatsOrderPage.vue'),
          },
        ],
      },
      {
        path: 'project',
        children: [
          {
            path: 'assets',
            component: () => import('pages/project/AssetsPage.vue'),
          },
        ],
      },
      {
        path: 'pay',
        children: [
          {
            path: 'detail',
            component: () => import('pages/pay/DetailPage.vue'),
            // component: () => import('pages/pay/DetailPagetest.vue'),
          },
        ],
      },
      {
        path: 'app',
        children: [
          {
            path: 'publicInfo',
            component: () => import('pages/app/PublicInfoPage.vue'),
          },
          {
            path: 'notify',
            component: () => import('pages/app/NotifyPage.vue'),
          },
        ],
      },
      {
        path: 'supply',
        children: [
          {
            path: 'healthex',
            component: () => import('pages/supply/HealthExPage.vue'),
          },
          {
            path: 'groupbuy',
            component: () => import('pages/supply/GroupbuyPage.vue'),
          },
          {
            path: 'sellhouse',
            component: () => import('pages/supply/SellhousePage.vue'),
          },
        ],
      },
      {
        path: 'asset',
        children: [
          {
            path: 'assets',
            component: () => import('pages/asset/AssetsPage.vue'),
          },
        ],
      },
      {
        path: 'customer',
        children: [
          {
            path: 'supplier',
            component: () => import('pages/customer/SupplierPage.vue'),
          },
        ],
      },
      {
        path: '/:catchAll(.*)*',
        component: () => import('pages/ErrorNotFound.vue'),
      },
    ],
  },
];

export default routes;
