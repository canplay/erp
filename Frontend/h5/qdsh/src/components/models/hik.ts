export interface Results {
  areaName: string
  chargeLeftNum: number
  chargeTotalNum: number
  description: string
  freeTime: string
  leftFixedSpaceNum: number
  leftParkingSpaceNum: number
  parkCode: string
  parkLatitude: string
  parkLevel: number
  parkLongitude: string
  parkName: string
  parkType: number
  parkingAddress: string
  parkingEndTime: string
  parkingStartTime: string
  payRuleDesc: string
  pictures: string
  totalFixedSpaceNum: number
  totalParkingSpaceNum: number
}

export interface Hik {
  code: number
  data: {
    driverId?: number
    results?: Results[]
    pageNo?: number
    pageSize?: number
    totalPage?: number
    totalRecord?: number
  }
  msg: string
}
