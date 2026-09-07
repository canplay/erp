import type { CancelToken, AxiosError } from 'axios';
import { api } from 'src/boot/axios';
import { useStore } from 'src/stores/store';

// API 响应消息类型
export type ApiMessage = string | { message?: string };

// API 错误类型
export type ApiError = AxiosError | Error;

export interface FetchResult {
  status: number;
  msg: ApiMessage;
  id?: string;
  token?: string;
  error?: string;
  errMsg?: string;
  billQRCode?: string;
}

export default function useFetch() {
  const store = useStore();

  const get = async (url: string, auth?: string, cancel?: CancelToken) => {
    let data: FetchResult = {
      status: 0,
      msg: '',
    };
    let error: ApiError | undefined;

    try {
      const resp = await api.get(store.server + url, {
        headers: {
          Authorization:
            typeof auth !== 'undefined' && auth !== '' ? 'Bearer ' + auth : '',
        },
        cancelToken: cancel,
      });

      if (
        resp.data.message !== undefined &&
        (resp.data.message === 'jwt token is invalid' ||
          resp.data.message === 'no jwt token')
      ) {
        data = {
          msg: '登录过期, 请重新登录',
          status: 0,
        };
      } else {
        data = resp.data;
      }
    } catch (err) {
      error = err as ApiError;
    }

    return { data, error };
  };

  const put = async (
    url: string,
    params: Record<string, unknown>,
    auth?: string,
    contentType?: string,
    cancel?: CancelToken
  ) => {
    let data: FetchResult = {
      status: 0,
      msg: '',
    };
    let error: ApiError | undefined;

    try {
      const resp = await api.put(store.server + url, params, {
        headers: {
          Authorization:
            typeof auth !== 'undefined' && auth !== '' ? 'Bearer ' + auth : '',
          'content-type': contentType !== '' ? contentType : 'application/json',
        },
        cancelToken: cancel,
      });

      if (
        resp.data.message !== undefined &&
        (resp.data.message === 'jwt token is invalid' ||
          resp.data.message === 'no jwt token')
      ) {
        data = {
          msg: '登录过期, 请重新登录',
          status: 0,
        };
      } else {
        data = resp.data;
      }
    } catch (err) {
      error = err as ApiError;
    }

    return { data, error };
  };

  const post = async (
    url: string,
    params: Record<string, unknown>,
    auth?: string,
    contentType?: string,
    cancel?: CancelToken
  ) => {
    let data: FetchResult = {
      status: 0,
      msg: '',
    };
    let error: ApiError | undefined;

    try {
      const resp = await api.post(store.server + url, params, {
        headers: {
          Authorization:
            typeof auth !== 'undefined' && auth !== '' ? 'Bearer ' + auth : '',
          'content-type': contentType !== '' ? contentType : 'application/json',
        },
        cancelToken: cancel,
      });

      if (
        resp.data.message !== undefined &&
        (resp.data.message === 'jwt token is invalid' ||
          resp.data.message === 'no jwt token')
      ) {
        data = {
          msg: '登录过期, 请重新登录',
          status: 0,
        };
      } else {
        data = resp.data;
      }
    } catch (err) {
      error = err as ApiError;
    }

    return { data, error };
  };

  const del = async (url: string, auth?: string, cancel?: CancelToken) => {
    let data: FetchResult = {
      status: 0,
      msg: '',
    };
    let error: ApiError | undefined;

    try {
      const resp = await api.delete(store.server + url, {
        headers: {
          Authorization:
            typeof auth !== 'undefined' && auth !== '' ? 'Bearer ' + auth : '',
        },
        cancelToken: cancel,
      });

      if (
        resp.data.message !== undefined &&
        (resp.data.message === 'jwt token is invalid' ||
          resp.data.message === 'no jwt token')
      ) {
        data = {
          msg: '登录过期, 请重新登录',
          status: 0,
        };
      } else {
        data = resp.data;
      }
    } catch (err) {
      error = err as ApiError;
    }

    return { data, error };
  };

  return { get, put, post, del };
}
