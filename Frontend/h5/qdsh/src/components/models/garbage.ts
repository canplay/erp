export interface GarbageQRCode {
  errMsg: string
  billQRCode: string
}

export interface GarbageItem {
  address: string
  name: string
  principal: string
  telephone: string
  money: string
  bill_year: string
  bill_month: string
  amount: string
  price: string
  id?: number
}
