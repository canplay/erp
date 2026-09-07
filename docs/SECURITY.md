# 安全策略

## 如何报告漏洞

如发现安全问题，请**不要**在公开 Issue 中描述细节。请通过以下方式私下联系维护者：

- 提交 [GitHub Security Advisory](../../security/advisories/new)（推荐）
- 或发送邮件至维护者

报告请包含：漏洞描述、复现步骤、影响范围、建议修复方案。维护者将在 72 小时内响应。

## 密钥管理约定

本项目**禁止硬编码任何密钥**。所有敏感配置通过环境变量注入：

### 环境变量清单

| 变量名 | 用途 | 示例 |
|---|---|---|
| `DATABASE_PASSWORD` | PostgreSQL 密码 | `openssl rand -base64 24` |
| `REDIS_PASSWORD` | Redis 密码 | `openssl rand -base64 24` |
| `DB_HOST` | 数据库主机 | `localhost` |
| `REDIS_HOST` | Redis 主机 | `localhost` |
| `JWT_KEY` | JWT 签名密钥（rs256 私钥） | `openssl genrsa 2048` |
| `JWT_PUBLIC_KEY_PATH` | JWT 公钥路径 | `./keys/jwt.pub` |
| `JWT_ISSUER` | JWT 签发者 | `https://your-domain.com` |
| `APP_SIGN_KEY` | 应用签名密钥 | `openssl rand -hex 24` |
| `LOGTO_CLIENT_ID` | Logto OIDC 客户端 ID | — |
| `LOGTO_CLIENT_SECRET` | Logto OIDC 客户端密钥 | — |
| `LOGTO_URL` | Logto 服务地址 | `https://logto.example.com` |
| `LOGTO_ISSUER` | Logto OIDC 签发者 | `https://logto.example.com/oidc` |
| `S3_ACCESS_KEY` | S3 访问密钥 | — |
| `S3_SECRET_KEY` | S3 秘密密钥 | — |
| `S3_URL` | S3 端点 | `https://s3.example.com` |
| `HIKVISION_URL` | 海康摄像头地址 | — |
| `HIKVISION_ID` | 海康接入 ID | — |
| `HIKVISION_KEY` | 海康接入密钥 | — |
| `UMS_APPID` | UMS 平台 AppId | — |
| `UMS_APPKEY` | UMS 平台 AppKey | — |
| `UMS_TOKEN` | UMS 平台 Token | — |
| `UMS_MID` | UMS 平台 Mid | — |
| `UMS_TID` | UMS 平台 Tid | — |
| `UMS_YSJC` | UMS 平台 Ysjc | — |

### .env 文件策略

仓库中的 `.env` 文件包含**随机生成的临时值**（`openssl rand` 生成），仅用于本地开发。生产部署前**必须**：

1. 生成新的随机密钥替换 `.env` 中的所有值
2. 或通过环境变量覆盖（推荐：使用 Docker/K8s secrets）

## 禁止事项

- ❌ 提交任何真实密钥、密码、连接字符串到仓库
- ❌ 提交 `.pem` / `.key` / `.p12` / `.pfx` 等证书文件
- ❌ 在日志、错误信息、注释中输出敏感数据
- ❌ 绕过 eslint/安全扫描门禁

## 依赖安全

- 定期运行 `pnpm audit` / `cargo audit` 检查依赖漏洞
- 及时更新存在安全漏洞的依赖版本
