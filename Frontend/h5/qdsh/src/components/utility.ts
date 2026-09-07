export default function useUtility() {
  const isPhone = (phone: string) => {
    const reg_tel = /^[1][3,4,5,7,8][0-9]{9}$/

    if (!reg_tel.test(phone)) {
      return false
    }
    return true
  }

  const isPlatform = (
    val: 'AlipayClient' | 'mp-qq' | 'mp-weixin' | 'toutiao' | 'swan' | 'quickapp',
  ) => {
    const ua = navigator.userAgent
    if (ua.indexOf(val) > -1) {
      return true
    } else {
      return false
    }
  }

  return { isPhone, isPlatform }
}
