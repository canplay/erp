import type { CommonSign } from './models/common'

/**
 * 生成符合支付平台规范的请求签名
 * @param secret - API密钥
 * @param params - 请求参数（需包含appid/timestamp/nonce）
 * @returns 十六进制小写的HMAC-SHA256签名
 */
export async function generateSignature(secret: string, params: CommonSign): Promise<string> {
  // 1. 参数过滤与排序
  const sortedEntries = Object.entries(params)
    .filter(([key]) => key.toLowerCase() !== 'sign') // 排除sign字段
    .sort(([a], [b]) => a.localeCompare(b)) // 自然排序

  // 2. 构造签名字符串
  const signStr = sortedEntries
    .map(([k, v]) => `${k.toLowerCase()}=${v}`) // 字段名转小写
    .join('&')

  console.log(signStr)
  // 3. 计算HMAC-SHA256
  const key = await crypto.subtle.importKey(
    'raw',
    new TextEncoder().encode(secret),
    { name: 'HMAC', hash: 'SHA-256' },
    false,
    ['sign'],
  )

  const signature = await crypto.subtle.sign('HMAC', key, new TextEncoder().encode(signStr))

  // 4. 转换为十六进制
  return bufferToHex(signature)
}

/**
 * 生成随机Nonce字符串
 * @param bytesLength - 随机字节长度（推荐16字节）
 */
export function generateNonce(bytesLength = 16): string {
  const buffer = new Uint8Array(bytesLength)
  crypto.getRandomValues(buffer)
  return Array.from(buffer)
    .map((b) => b.toString(16).padStart(2, '0'))
    .join('')
}

/**
 * 生成当前时间戳（秒级）
 */
export function generateTimestamp(): number {
  return Math.floor(Date.now() / 1000)
}

// 工具函数：ArrayBuffer转十六进制
function bufferToHex(buffer: ArrayBuffer): string {
  return Array.from(new Uint8Array(buffer))
    .map((b) => b.toString(16).padStart(2, '0'))
    .join('')
    .toLowerCase()
}
