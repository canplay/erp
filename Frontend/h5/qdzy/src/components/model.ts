export interface Response<T> {
  data: T
  status: number
}

export interface Slide {
  img: string
}

export interface Slides {
  slide: Slide[]
}

export interface News {
  id: string
  title: string
  preview: string
  content: string
  top: number
  update_date: string
  slide?: Slides
}

export interface User {
  id: string
  nickname?: string | null | undefined
  username?: string | null | undefined
  email?: string | null | undefined
  avatar?: string | null | undefined
  qq?: string | null | undefined
  wechat?: string | null | undefined
  phone?: string | null | undefined
  orgs?: string[] | null | undefined
  roles?: string[] | null | undefined
  orgRoles?: string[] | null | undefined
  permission?: string[] | null | undefined
}
