import { alova } from 'src/boot/axios'
import { useStore } from 'src/stores/store'
import type { CommonResponse } from '../models/common'
import type { Hik } from '../models/hik'

export default function useHikParking() {
  const store = useStore()

  const driver = async (): Promise<Hik> => {
    if (store.user.phone === '') throw new Error('手机号为空')

    const resp = await alova.Post<CommonResponse<Hik>>(
      `${store.server.erp}/post?service=hik&auth=public&path=/api/hik/exec`,
      {
        method: 'driver',
        phone: parseInt(store.user.phone),
      },
    )

    if (resp.message != undefined && resp.message.toUpperCase() != 'SUCCESS') {
      throw new Error(resp.message)
    }

    if (!resp.data) {
      throw new Error('No data returned')
    }

    return resp.data
  }

  const plates = async (): Promise<Hik> => {
    if (store.user.additive.hik_id === null) throw new Error('车主ID为空')

    const resp = await alova.Post<CommonResponse<Hik>>(
      `${store.server.erp}service=hik&auth=public&path=/api/hik/exec`,
      {
        method: 'plates',
        driverId: store.user.additive.hik_id,
      },
    )

    if (resp.message != undefined && resp.message.toUpperCase() != 'SUCCESS') {
      throw new Error(resp.message)
    }

    if (!resp.data) {
      throw new Error('No data returned')
    }

    return resp.data
  }

  const register = async (): Promise<boolean> => {
    if (store.user.phone === '') return false

    const resp = await alova.Post<CommonResponse<Hik>>(
      `${store.server.erp}/post?service=hik&auth=public&path=/api/hik/exec`,
      {
        method: 'register',
        phone: parseInt(store.user.phone),
      },
    )

    let hik_id = -1

    if (resp.message != undefined && resp.message === '手机号码已被注册') {
      hik_id = resp.data?.data?.driverId ?? -1
    } else if (resp.message != undefined && resp.message.toUpperCase() != 'SUCCESS') {
      console.log(resp.message)
      return false
    } else {
      hik_id = resp.data?.data?.driverId ?? -1
    }

    const resp1 = await alova.Post<CommonResponse<Hik>>(
      `${store.server.erp}/post?service=cbis&auth=public&path=/api/user/update`,
      {
        id: store.user.id,
        phone: store.user.phone,
      },
    )

    if (resp1.status) {
      store.user.additive.hik_id = hik_id
    } else {
      return false
    }

    return true
  }

  const add_plate = async (plate: string): Promise<boolean> => {
    const resp = await alova.Post<CommonResponse<Hik>>(
      `${store.server.erp}/post?service=hik&auth=public&path=/api/hik/exec`,
      {
        method: 'addPlate',
        driverId: store.user.additive.hik_id,
        plateNo: plate,
        plateColor: plate.length != 6 ? 1 : 5,
      },
    )

    if (resp.message != undefined && resp.message.toUpperCase() != 'SUCCESS') {
      console.log(resp.message)
      return false
    }

    return true
  }

  const del_plate = async (plate: string): Promise<boolean> => {
    const resp = await alova.Post<CommonResponse<Hik>>(
      `${store.server.erp}/post?service=hik&auth=public&path=/api/hik/exec`,
      {
        method: 'delPlate',
        driverId: store.user.additive.hik_id,
        plateNo: plate,
        plateColor: plate.length != 6 ? 1 : 5,
      },
    )

    if (resp.message != undefined && resp.message.toUpperCase() != 'SUCCESS') {
      console.log(resp.message)
      return false
    }

    return true
  }

  const query_parking = async (plate: string): Promise<Hik> => {
    const resp = await alova.Post<CommonResponse<Hik>>(
      `${store.server.erp}/post?service=hik&auth=public&path=/api/hik/exec`,
      {
        method: 'parkParking',
        plateNo: plate,
      },
    )

    if (resp.message != undefined && resp.message.toUpperCase() != 'SUCCESS') {
      throw new Error(resp.message)
    }

    if (!resp.data) {
      throw new Error('No data returned')
    }

    return resp.data
  }

  const query_arrears = async (plate: string): Promise<Hik> => {
    const resp = await alova.Post<CommonResponse<Hik>>(
      `${store.server.erp}/post?service=hik&auth=public&path=/api/hik/exec`,
      {
        method: 'parkArrears',
        plateNo: plate,
      },
    )

    if (resp.message != undefined && resp.message.toUpperCase() != 'SUCCESS') {
      throw new Error(resp.message)
    }

    if (!resp.data) {
      throw new Error('No data returned')
    }

    return resp.data
  }

  const query_detail = async (id: number, street: string): Promise<Hik> => {
    const resp = await alova.Post<CommonResponse<Hik>>(
      `${store.server.erp}/post?service=hik&auth=public&path=/api/hik/exec`,
      {
        method: 'parkOrdersDetail',
        phone: parseInt(store.user.phone),
        uniqueId: id,
        parkCode: street,
        isUseCoupon: 0,
      },
    )

    if (resp.message != undefined && resp.message.toUpperCase() != 'SUCCESS') {
      throw new Error(resp.message)
    }

    if (!resp.data) {
      throw new Error('No data returned')
    }

    return resp.data
  }

  const report = async (
    id: number,
    street: string,
    type: string,
    inTime: string,
    outTime: string,
    remark: string,
  ): Promise<boolean> => {
    const resp = await alova.Post<CommonResponse<Hik>>(
      `${store.server.erp}/post?service=hik&auth=public&path=/api/hik/exec`,
      {
        method: 'report',
        phone: parseInt(store.user.phone),
        uniqueId: id,
        parkCode: street,
        appealType: type,
        appealInTime: inTime,
        appealOutTime: outTime,
        appealRemark: remark,
        appealSource: 2,
      },
    )

    if (resp.message != undefined && resp.message.toUpperCase() != 'SUCCESS') {
      console.log(resp.message)
      return false
    }

    return true
  }

  const pay_batch = async (id: number): Promise<Hik> => {
    const resp = await alova.Post<CommonResponse<Hik>>(
      `${store.server.erp}/post?service=hik&auth=public&path=/api/hik/exec`,
      {
        method: 'payBatch',
        id: id,
        phone: parseInt(store.user.phone),
      },
    )

    if (resp.message != undefined && resp.message.toUpperCase() != 'SUCCESS') {
      throw new Error(resp.message)
    }

    if (!resp.data) {
      throw new Error('No data returned')
    }

    return resp.data
  }

  const park_infos = async (pageNo: number, pageSize: number): Promise<Hik> => {
    const resp = await alova.Post<CommonResponse<Hik>>(
      `${store.server.erp}/post?service=hik&auth=public&path=/api/hik/exec`,
      {
        method: 'parkinfos',
        pageNo: pageNo,
        pageSize: pageSize,
        parkName: '',
      },
    )

    if (resp.message != undefined && resp.message.toUpperCase() != 'SUCCESS') {
      throw new Error(resp.message)
    }

    if (!resp.data) {
      throw new Error('No data returned')
    }

    return resp.data
  }

  const park_info = async (parkCode: number): Promise<Hik> => {
    const resp = await alova.Post<CommonResponse<Hik>>(
      `${store.server.erp}/post?service=hik&auth=public&path=/api/hik/exec`,
      {
        method: 'parkinfo',
        parkCode: parkCode,
      },
    )

    if (resp.message != undefined && resp.message.toUpperCase() != 'SUCCESS') {
      throw new Error(resp.message)
    }

    if (!resp.data) {
      throw new Error('No data returned')
    }

    return resp.data
  }

  return {
    driver,
    plates,
    register,
    add_plate,
    del_plate,
    query_parking,
    query_arrears,
    query_detail,
    report,
    pay_batch,
    park_infos,
    park_info,
  }
}
