export interface CommonResponse<T> {
  status: number
  data?: T
  message: string
}

export interface CommonNews {
  author: string
  class: number
  content: string
  create_date: string
  create_user: string
  date: string
  hot: boolean
  id: string
  preview: string
  title: string
  update_date: string
  update_user: string
}

export interface CommonSession {
  access_token: string
  expires_in?: number
  id_token?: string
  refresh_token?: string
  token_type?: string
}

export interface CommonUser {
  signed: boolean
  id: string
  source: string
  username: string
  password: string
  realName: string
  nickName: string
  sex: string
  province: string
  city: string
  country: string
  avatar: string
  phone: string
  wechat: string
  alipay: string
  weibo: string
  email: string
  tiktok: string
  idCard: string
  permissions: {
    name: string
    access: {
      all: boolean
    }
  }
  options: {
    a?: number
  }
  additive: {
    hik_id: number
    parking: {
      current: []
      arrears: {
        num: number
        money: number
      }
    }
  }
  address: string
  backImage: string
  cityDesc: string
  clientId: string
  county: string
  countyDesc: string
  frontImage: string
  mobile: string
  modifyId: string
  pkId: string
  remark: string
  status: string
  thirds: string
  vip: string
}

export interface CommonNotify {
  list: {
    title: string
    img: string
    content: string
    date: string
    status: string
  }[]
  unread: 0
}

export interface CommonGoto {
  type: string
  url: string
}

export interface CommonSign {
  [key: string]: string | number
}

export interface CommonMenu {
  name: string
  appimg: string
  url: number
}
