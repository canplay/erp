export interface ChatComment {
  id: string
  content: string
  like: number
  dislike: number
  lng: string
  lat: string
  end_date: string
  create_date: string
  create_user: string
}

export interface ChatCommentDetail {
  id: string
  content: string
  parent: string
  like: number
  dislike: number
  create_date: string
  create_user: string
}
