<template>
  <q-scroll-area class="fit">
    <q-list class="text-primary">
      <q-item
        class="text-white"
        clickable
        v-ripple
        :active="store.router.currentRoute.path === '/index'"
        to="/index"
        active-class="active-menu-link"
        v-if="
          store.permissions.access?.all || store.permissions.access?.menu?.index
        "
      >
        概览
      </q-item>

      <q-expansion-item
        class="text-white"
        label="子系统"
        v-if="
          store.permissions.access?.all ||
           (store.permissions.access?.menu as any)?.plugin?.index
        "
      >
        <q-list>
          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            href="https://chat.cm.example.com"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.plugin?.meeting
            "
          >
            <q-item-section>社交</q-item-section>
          </q-item>

          <q-expansion-item
            class="text-white"
            label="商城"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.plugin?.shop
            "
          >
            <q-item
              clickable
              v-ripple
              :inset-level="0.5"
              href="https://shop-sadmin.ct.example.com"
              v-if="
                store.permissions.access?.all ||
                (store.permissions.access?.menu as any)?.plugin?.shop_sadmin
              "
            >
              <q-item-section>总后台</q-item-section>
            </q-item>

            <q-item
              clickable
              v-ripple
              :inset-level="0.5"
              href="https://shop-padmin.ct.example.com"
              v-if="
                store.permissions.access?.all ||
                (store.permissions.access?.menu as any)?.plugin?.shop_padmin
              "
            >
              <q-item-section>供应商中心</q-item-section>
            </q-item>

            <q-item
              clickable
              v-ripple
              :inset-level="0.5"
              href="https://shop-admin.ct.example.com"
              v-if="
                store.permissions.access?.all ||
                (store.permissions.access?.menu as any)?.plugin?.shop_admin
              "
            >
              <q-item-section>商家中心</q-item-section>
            </q-item>
          </q-expansion-item>

          <q-expansion-item
            class="text-white"
            label="支付中心"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.plugin?.pay
            "
          >
            <q-item
              clickable
              v-ripple
              :inset-level="0.5"
              href="https://pay-manager.ct.example.com"
              v-if="
                store.permissions.access?.all ||
                (store.permissions.access?.menu as any)?.plugin?.pay_manager
              "
            >
              <q-item-section>运营中心</q-item-section>
            </q-item>

            <q-item
              clickable
              v-ripple
              :inset-level="0.5"
              href="https://pay-merchant.ct.example.com"
              v-if="
                store.permissions.access?.all ||
                (store.permissions.access?.menu as any)?.plugin?.pay_merchant
              "
            >
              <q-item-section>商户中心</q-item-section>
            </q-item>
          </q-expansion-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            href="https://property-admin.ct.example.com"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.plugin?.property
            "
          >
            <q-item-section>智慧物业</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            href="https://dataease.cm.example.com"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.plugin?.viusal
            "
          >
            <q-item-section>可视化</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            href="https://git.cm.example.com"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.plugin?.git
            "
          >
            <q-item-section>源码仓库</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            href="https://harbor.cm.example.com"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.plugin?.hub
            "
          >
            <q-item-section>镜像仓库</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            href="https://ks.cm.example.com"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.plugin?.k8s
            "
          >
            <q-item-section>移动K8S集群</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            href="https://fb.cm.example.com"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.plugin?.focalboard
            "
          >
            <q-item-section>项目管理</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            href="https://post.cm.example.com"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.plugin?.api
            "
          >
            <q-item-section>API测试</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            href="https://sentry.cm.example.com"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.plugin?.sentry
            "
          >
            <q-item-section>BUG反馈</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            href="https://nats.cm.example.com"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.plugin?.nats
            "
          >
            <q-item-section>Nats</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            href="https://tools.cm.example.com"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.plugin?.tools
            "
          >
            <q-item-section>IT工具</q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item
        class="text-white"
        label="公共资源"
        v-if="
          store.permissions.access?.all ||
          (store.permissions.access?.menu as any)?.public_assets?.index
        "
      >
        <q-list>
          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            @click="goto('parking')"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.public_assets?.parking
            "
          >
            <q-item-section>智慧停车</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            @click="goto('lamp')"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.public_assets?.lamp
            "
          >
            <q-item-section>智慧路灯</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/public_assets/assets'"
            to="/public_assets/assets"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.public_assets?.assets
            "
          >
            <q-item-section>设施设备</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/public_assets/order'"
            to="/public_assets/order"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.public_assets?.order
            "
          >
            <q-item-section>工单</q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item
        class="text-white"
        label="社区物业"
        v-if="
          store.permissions.access?.all ||
          (store.permissions.access?.menu as any)?.property?.index
        "
      >
        <q-list>
          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/property/order'"
            to="/property/order"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.property?.order
            "
          >
            <q-item-section>订单</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/property/staff'"
            to="/property/staff"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.property?.staff
            "
          >
            <q-item-section>人员</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/property/car'"
            to="/property/car"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.property?.car
            "
          >
            <q-item-section>车辆</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/property/estate'"
            to="/property/estate"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.property?.estate
            "
          >
            <q-item-section>房产</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/property/feedback'"
            to="/property/feedback"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.property?.feedback
            "
          >
            <q-item-section>反馈</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/property/workorder'"
            to="/property/workorder"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.property?.workorder
            "
          >
            <q-item-section>工单</q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item
        class="text-white"
        label="牲畜交易"
        v-if="
          store.permissions.access?.all ||
          (store.permissions.access?.menu as any)?.livestock?.index
        "
      >
        <q-list>
          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/livestock/receipt'"
            to="/livestock/receipt"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.livestock?.receipt
            "
          >
            <q-item-section>出场收据</q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item
        class="text-white"
        label="环卫清运"
        v-if="
          store.permissions.access?.all ||
          (store.permissions.access?.menu as any)?.clean?.index
        "
      >
        <q-list>
          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            @click="goto('clean')"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.clean?.old_system
            "
          >
            <q-item-section>收费系统</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/clean/receipt'"
            to="/clean/receipt"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.clean?.receipt
            "
          >
            <q-item-section>非税收据</q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item
        class="text-white"
        label="校安配送"
        v-if="
          store.permissions.access?.all ||
          (store.permissions.access?.menu as any)?.distribution?.index
        "
      >
        <q-list>
          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            @click="goto('distribution')"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.distribution?.browser
            "
          >
            <q-item-section>配送系统</q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item
        class="text-white"
        label="绿化养护"
        v-if="
          store.permissions.access?.all ||
          (store.permissions.access?.menu as any)?.afforest?.index
        "
      >
        <q-list>
          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/afforest/zone'"
            to="/afforest/zone"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.afforest?.zone
            "
          >
            <q-item-section>区域</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/afforest/plant'"
            to="/afforest/plant"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.afforest?.plant
            "
          >
            <q-item-section>绿植</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/afforest/assets'"
            to="/afforest/assets"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.afforest?.assets
            "
          >
            <q-item-section>资产</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/afforest/patrol'"
            to="/afforest/patrol"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.afforest?.patrol
            "
          >
            <q-item-section>巡查</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/afforest/workorder'"
            to="/afforest/workorder"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.afforest?.workorder
            "
          >
            <q-item-section>工单</q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item
        class="text-white"
        label="违章拖车"
        v-if="
          store.permissions.access?.all ||
          (store.permissions.access?.menu as any)?.tow?.index
        "
      >
        <q-list>
          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/tow/order'"
            to="/tow/order"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.tow?.order
            "
          >
            <q-item-section>车辆记录</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/tow/report'"
            to="/tow/report"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.tow?.report
            "
          >
            <q-item-section>统计报表</q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item
        class="text-white"
        label="殡葬业务"
        v-if="
          store.permissions.access?.all ||
          (store.permissions.access?.menu as any)?.funeral?.index
        "
      >
      </q-expansion-item>

      <q-expansion-item
        class="text-white"
        label="车辆修理"
        v-if="
          store.permissions.access?.all ||
          (store.permissions.access?.menu as any)?.repair?.index
        "
      >
        <q-list>
          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/repair/order'"
            to="/repair/order"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.repair?.order
            "
          >
            <q-item-section>订单</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/repair/storage'"
            to="/repair/storage"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.repair?.storage
            "
          >
            <q-item-section>库存</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/repair/report'"
            to="/repair/report"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.repair?.report
            "
          >
            <q-item-section>统计报表</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/repair/option'"
            to="/repair/option"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.repair?.option
            "
          >
            <q-item-section>设置</q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item
        class="text-white"
        label="票务管理"
        v-if="
          store.permissions.access?.all ||
          (store.permissions.access?.menu as any)?.ticket?.index
        "
      >
        <q-list>
          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/ticket/seatsOrder'"
            to="/ticket/seatsOrder"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.ticket?.seatsOrder
            "
          >
            <q-item-section>座次票据</q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item
        class="text-white"
        label="项目管理"
        v-if="
          store.permissions.access?.all ||
          (store.permissions.access?.menu as any)?.project?.index
        "
      >
        <q-list>
          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/project/assets'"
            to="/project/assets"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.project?.assets
            "
          >
            <q-item-section>资产列表</q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item
        class="text-white"
        label="供应链管理"
        v-if="
          store.permissions.access?.all ||
          (store.permissions.access?.menu as any)?.supply?.index
        "
      >
        <q-list>
          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/supply/healthex'"
            to="/supply/healthex"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.supply?.healthex
            "
          >
            <q-item-section>体检预约</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/supply/groupbuy'"
            to="/supply/groupbuy"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.supply?.groupbuy
            "
          >
            <q-item-section>集采订单</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/supply/sellhouse'"
            to="/supply/sellhouse"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.supply?.sellhouse
            "
          >
            <q-item-section>房产预售</q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item
        class="text-white"
        label="客户管理"
        v-if="
          store.permissions.access?.all ||
          (store.permissions.access?.menu as any)?.customer?.index
        "
      >
        <q-list>
          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/customer/supplier'"
            to="/customer/supplier"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.customer?.supplier
            "
          >
            <q-item-section>供应商</q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item
        class="text-white"
        label="终端管理"
        v-if="
          store.permissions.access?.all ||
          (store.permissions.access?.menu as any)?.app?.index
        "
      >
        <q-list>
          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/app/news'"
            to="/app/news"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.app?.news
            "
          >
            <q-item-section>新闻</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/app/publicInfo'"
            to="/app/publicInfo"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.app?.publicInfo
            "
          >
            <q-item-section>信息收集</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/app/notify'"
            to="/app/notify"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.app?.notify
            "
          >
            <q-item-section>消息通知</q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item
        class="text-white"
        label="支付管理"
        v-if="
          store.permissions.access?.all ||
          (store.permissions.access?.menu as any)?.pay?.index
        "
      >
        <q-list>
          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/pay/detail'"
            to="/pay/detail"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.app?.detail
            "
          >
            <q-item-section>支付记录</q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item
        class="text-white"
        label="个人设置"
        v-if="
          store.permissions.access?.all ||
          (store.permissions.access?.menu as any)?.personal?.index
        "
      >
        <q-list>
          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/personal/info'"
            to="/personal/info"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.personal?.info
            "
          >
            <q-item-section>个人信息</q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item
        class="text-white"
        label="系统设置"
        v-if="
          store.permissions.access?.all ||
          (store.permissions.access?.menu as any)?.system?.index
        "
      >
        <q-list>
          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/system/user'"
            to="/system/user"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.system?.user
            "
          >
            <q-item-section>用户</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/system/permission'"
            to="/system.permission"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.system?.permission
            "
          >
            <q-item-section>权限</q-item-section>
          </q-item>

          <q-item
            clickable
            v-ripple
            :inset-level="0.5"
            :active="store.router.currentRoute.path === '/system/option'"
            to="/system/option"
            active-class="active-menu-link"
            v-if="
              store.permissions.access?.all ||
              (store.permissions.access?.menu as any)?.system?.option
            "
          >
            <q-item-section>设置</q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>
    </q-list>
  </q-scroll-area>
</template>

<style lang="sass">
.active-menu-link
  color: white
  background-color:#5F86AE

.size
  font-size:1rem
</style>

<script setup lang="ts">
import { openURL } from 'quasar';
import { useStore } from 'stores/store';

const store = useStore();

const goto = (name: string) => {
  switch (name) {
    case 'remote':
      if (!store.quasar.platform.is.electron) {
        store.quasar.notify('请下载使用App打开远程桌面');
      } else {
        window.electron.runRemote();
      }
      break;
    case 'shop':
      if (!store.quasar.platform.is.electron) {
        openURL('https://shop-pc.ct.example.com');
      } else {
        window.electron.newWindow(
          'https://shop-pc.ct.example.com',
          (store.user as { username: string; password: string }).username,
          (store.user as { username: string; password: string }).password,
          true
        );
      }
      break;
    case 'parking':
      if (!store.quasar.platform.is.electron) {
        openURL('https://psp.wsscxt.com/#/login');
      } else {
        window.electron.openURL('https://psp.wsscxt.com/#/login');
      }
      break;
    case 'lamp':
      if (!store.quasar.platform.is.electron) {
        openURL('https://wenshan.iots.city:11198/#/login');
      } else {
        window.electron.openURL('https://wenshan.iots.city:11198/#/login');
      }
      break;
    case 'clean':
      if (!store.quasar.platform.is.electron) {
        openURL('https://jcgs.cm.example.com/loginController.do?login');
      } else {
        window.electron.openURL(
          'https://jcgs.cm.example.com/loginController.do?login'
        );
      }
      break;
    case 'distribution':
      if (!store.quasar.platform.is.electron) {
        openURL('https://scm.sdongpo.com/cc_wsgt/superAdmin/view#/login');
      } else {
        window.electron.openURL(
          'https://scm.sdongpo.com/cc_wsgt/superAdmin/view#/login'
        );
      }
      break;
    default:
      openURL(name);
      break;
  }
};
</script>
