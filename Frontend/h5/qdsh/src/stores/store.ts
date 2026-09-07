import { defineStore } from 'pinia'
import { Notify, Cookies, Loading } from 'quasar'
import { ref } from 'vue'
import { alova } from 'src/boot/axios'
import { Client, Session, type Socket } from '@heroiclabs/nakama-js'
import type {
  CommonNotify,
  CommonResponse,
  CommonSession,
  CommonUser,
} from '../components/models/common'
import type { ChatComment } from 'src/components/models/chat'
import { useRouter } from 'vue-router'
// import wx from 'weixin-js-sdk';

export const useStore = defineStore('store', () => {
  const is_app = ref(false)
  const page = ref({
    current: '',
    title: '',
  })
  const drawer = ref({
    right: false,
  })
  const comment = ref({
    list: <ChatComment[]>[],
    selected: <ChatComment>{},
  })
  const notify = ref<CommonNotify>({
    list: [],
    unread: 0,
  })
  const server = ref({
    erp: 'https://erp.example.com/api/route',
    shop: '/shop-api',
    usercenter: 'https://gateway-api.ct.example.com',
    logto: 'https://logto.cm.example.com',
    nakama: {
      url: 'nakama-api.cm.example.com',
      port: '443',
      key: 'ebab90b6f3fb28a593eefcdf8f47eff5',
      client: <Client>{},
      session: <Session>{},
      socket: <Socket>{},
    },
  })
  const user = ref<CommonUser>({
    signed: false,
    id: '',
    source: '',
    username: '',
    password: '',
    realName: '',
    nickName: '游客',
    sex: '',
    province: '',
    city: '',
    country: '',
    avatar: 'imgs/noimg.svg',
    phone: '',
    wechat: '',
    alipay: '',
    weibo: '',
    email: '',
    tiktok: '',
    idCard: '',
    permissions: {
      name: '游客',
      access: {
        all: false,
      },
    },
    options: {},
    additive: {
      hik_id: 0,
      parking: {
        current: [],
        arrears: {
          num: 0,
          money: 0,
        },
      },
    },
    address: '',
    backImage: '',
    cityDesc: '',
    clientId: '',
    county: '',
    countyDesc: '',
    frontImage: '',
    mobile: '',
    modifyId: '',
    pkId: '',
    remark: '',
    status: '',
    thirds: '',
    vip: '',
  })
  const userInfo = () => {
    if (
      !Cookies.has('qdsh_session') ||
      Cookies.get('qdsh_session') === '' ||
      Cookies.get('qdsh_session') === undefined
    ) {
      return
    }

    const time = setTimeout(() => {
      Loading.hide()
      clearTimeout(time)
    }, 30000)

    Loading.show()

    const token: CommonSession = Cookies.get('qdsh_session')

    alova
      .Get<CommonResponse<CommonUser>>(`${server.value.usercenter}/usercenter/userinfo`, {
        headers: {
          Authorization: token.access_token,
        },
      })
      .then((resp) => {
        Loading.hide()
        clearTimeout(time)

        user.value = {
          signed: true,
          id: resp.data!.pkId,
          source: '',
          username: resp.data!.username,
          password: '',
          realName: resp.data!.realName,
          nickName: resp.data!.nickName,
          sex: '',
          province: resp.data!.province,
          city: '',
          country: '',
          avatar:
            resp.data!.avatar === '' || resp.data!.avatar === null
              ? 'imgs/noimg.svg'
              : resp.data!.avatar,
          phone: '',
          wechat: '',
          alipay: '',
          weibo: '',
          email: resp.data!.email,
          tiktok: '',
          idCard: resp.data!.idCard,
          permissions: {
            name: '普通用户',
            access: {
              all: true,
            },
          },
          options: {},
          additive: {
            hik_id: 0,
            parking: {
              current: [],
              arrears: {
                num: 0,
                money: 0,
              },
            },
          },
          address: resp.data!.address,
          backImage: resp.data!.backImage,
          cityDesc: resp.data!.cityDesc,
          clientId: resp.data!.clientId,
          county: resp.data!.county,
          countyDesc: resp.data!.countyDesc,
          frontImage: resp.data!.frontImage,
          mobile: resp.data!.mobile,
          modifyId: resp.data!.modifyId,
          pkId: resp.data!.pkId,
          remark: resp.data!.remark,
          status: resp.data!.status,
          thirds: resp.data!.thirds,
          vip: '',
        }
      })
      .catch(() => {
        Loading.hide()
        clearTimeout(time)
        Notify.create({
          message: '获取用户信息失败, 请稍后重试',
          type: 'negative',
          position: 'top',
        })
        return
      })
  }
  const signout = () => {
    if (Cookies.has('qdsh_session')) {
      const token: CommonSession = Cookies.get('qdsh_session')

      alova
        .Get<CommonResponse<CommonSession>>(
          `${server.value.usercenter}/usercenter/token/login/logout?token=${token.access_token}`,
          {
            headers: {
              Authorization: token.access_token,
            },
          },
        )
        .then((resp) => {
          if (resp === undefined || resp.message != '请求成功') {
            Notify.create({
              message: '网络错误, 请稍后重试',
              type: 'negative',
              position: 'top',
            })
          }
        })
        .catch((e) => {
          console.log(e)
        })

      Cookies.remove('qdsh_session')
    }

    user.value = {
      signed: false,
      id: '',
      source: '',
      username: '',
      password: '',
      realName: '',
      nickName: '游客',
      sex: '',
      province: '',
      city: '',
      country: '',
      avatar: 'imgs/noimg.svg',
      phone: '',
      wechat: '',
      alipay: '',
      weibo: '',
      email: '',
      tiktok: '',
      idCard: '',
      permissions: {
        name: '普通用户',
        access: {
          all: true,
        },
      },
      options: {},
      additive: {
        hik_id: 0,
        parking: {
          current: [],
          arrears: {
            num: 0,
            money: 0,
          },
        },
      },
      address: '',
      backImage: '',
      cityDesc: '',
      clientId: '',
      county: '',
      countyDesc: '',
      frontImage: '',
      mobile: '',
      modifyId: '',
      pkId: '',
      remark: '',
      status: '',
      thirds: '',
      vip: '',
    }
  }
  const signin = async (type = 'wechat', phone?: string, verify?: string) => {
    const router = useRouter()

    switch (type) {
      case 'mp-wechat':
        window.uniWeb.redirectTo({
          url: 'signin',
        })
        break
      case 'mp-wechat-callback':
        Cookies.set('qdsh_session', JSON.stringify({ access_token: phone! }))
        userInfo()
        await router.push('/')
        break
      case 'phone':
        {
          const time = setTimeout(() => {
            Loading.hide()
          }, 30000)

          Loading.show({
            message: '正在处理，请稍后...',
          })

          alova
            .Post<CommonResponse<CommonSession>>(
              `${server.value.usercenter}/usercenter/token/mobile/login`,
              {
                mobile: phone,
                clientId: 'qidu_web',
                mobileCode: verify,
              },
            )
            .then(async (resp) => {
              if (resp === undefined || resp.message != '请求成功') {
                Loading.hide()
                clearTimeout(time)
                Notify.create({
                  message: '网络错误, 请稍后重试',
                  type: 'negative',
                  position: 'top',
                })
                return
              }

              Loading.hide()
              clearTimeout(time)

              Cookies.set('qdsh_session', JSON.stringify(resp.data!))
              userInfo()
              return await router.push('/')
            })
            .catch((e) => {
              Loading.hide()
              clearTimeout(time)
              Notify.create({
                message: e,
                type: 'negative',
                position: 'top',
              })
            })
        }
        break
    }
  }
  const nakama_start = async () => {
    try {
      server.value.nakama.client = new Client(
        server.value.nakama.key,
        server.value.nakama.url,
        server.value.nakama.port,
        true,
      )

      user.value.id = 'cd8998c1-c8b5-4893-bcbf-df5a61355860'

      if (!Cookies.has('nakama')) {
        server.value.nakama.session = await server.value.nakama.client.authenticateDevice(
          user.value.id,
          false,
          user.value.id.replace(/-/g, ''),
        )
        Cookies.set('nakama_token', server.value.nakama.session.token)
        Cookies.set('nakama_refrashtoken', server.value.nakama.session.refresh_token)
      } else {
        server.value.nakama.session = Session.restore(
          Cookies.get('nakama_token'),
          Cookies.get('nakama_refrashtoken'),
        )
        server.value.nakama.session = await server.value.nakama.client.sessionRefresh(
          server.value.nakama.session,
        )
        await server.value.nakama.client.linkDevice(server.value.nakama.session, {
          id: user.value.id,
        })
      }

      const account = await server.value.nakama.client.getAccount(server.value.nakama.session)
      user.value.id = account.user!.id!
    } catch (e) {
      if (e instanceof Error) {
        Notify.create({
          message: e.message,
          type: 'negative',
          position: 'top',
        })
      }
    }
  }
  const nakama_close = async () => {
    await server.value.nakama.client.sessionLogout(
      server.value.nakama.session,
      server.value.nakama.session.token,
      server.value.nakama.session.refresh_token,
    )
    Cookies.remove('nakama_token')
    Cookies.remove('nakama_refrashtoken')
  }
  return {
    fetch,
    is_app,
    page,
    drawer,
    comment,
    notify,
    server,
    user,
    userInfo,
    signout,
    signin,
    nakama_start,
    nakama_close,
  }
})
