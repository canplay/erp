export interface CommonResponse<T> {
  status: number;
  data?: T;
  message: string;
}

export interface CommonNews {
  author: string;
  class: number;
  content: string;
  create_date: string;
  create_user: string;
  date: string;
  hot: boolean;
  id: string;
  preview: string;
  title: string;
  update_date: string;
  update_user: string;
}
