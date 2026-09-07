# DataFusion ERP · 多业务综合管理平台

> 面向多行业中小微企业的综合管理平台，涵盖气瓶追溯、共享电单车、殡葬、拖车、消防、问卷调查等业务模块。

## 项目简介

DataFusion ERP 是一套多业务综合管理平台，为气瓶追溯、共享电单车监管、殡葬服务、拖车救援、消防安全、问卷调查等多个垂直行业提供一站式 SaaS 管理解决方案。

## 目录结构

```
erp/
├── backend/                    # 后端服务
│   ├── cpp/                    # Drogon C++ 服务（18 个子服务）
│   │   ├── hik/                # 海康摄像头接入
│   │   ├── cbis/               # CBIS 业务
│   │   ├── gateway/            # API 网关
│   │   ├── notify/             # 通知服务
│   │   ├── chat/               # 即时通讯
│   │   ├── clean/              # 清洁服务
│   │   ├── funeral/            # 殡葬业务
│   │   ├── pay/                # 支付服务
│   │   ├── tow/                # 拖车业务
│   │   ├── repair/             # 维修服务
│   │   ├── dingtalk/           # 钉钉接入
│   │   ├── liveup/             # 直播服务
│   │   ├── qdzy/               # 气瓶作业
│   │   ├── common/             # 公共库
│   │   ├── daemon/             # 守护进程
│   │   ├── protos/             # gRPC Proto 定义
│   │   ├── config.json         # 配置模板（环境变量占位）
│   │   └── config_debug.json   # 调试配置
│   ├── cpp_next/               # 下一代 C++ 服务（OIDC 认证）
│   ├── rust/                   # Actix Rust 服务
│   │   ├── ebike/              # 电单车监管（private/public/utility）
│   │   └── tow/                # 拖车业务
│   ├── dedicated/              # 独立部署服务（Lua + Moon 框架）
│   └── datafusion_schema.sql   # 数据库结构脚本（纯 DDL）
├── frontend/                   # 前端应用
│   ├── h5/                     # 移动端 H5 应用（Quasar + Vue 3）
│   │   ├── qdsh/               # 气瓶追溯（七都商合）
│   │   ├── qdzy/               # 气瓶作业
│   │   ├── ebike/              # 共享电踏车监管
│   │   ├── tow/                # 拖车
│   │   ├── funeral/            # 殡葬
│   │   ├── mapz/               # 地图
│   │   ├── wj/                 # 问卷调查
│   │   ├── xady/               # 消防
│   │   ├── scoreboard/         # 看板
│   │   ├── cbis/               # CBIS
│   │   └── uniapp/             # UniApp 小程序
│   ├── game/                   # 游戏化培训
│   │   ├── tarkov_like/        # 塔科夫风格
│   │   └── xiuxian_text/       # 修仙文字
│   └── micro_frontend/         # 微前端
│       ├── admin/              # 管理端
│       └── user/               # 用户端
├── thirdparty/                 # 第三方部署工具
│   ├── harbor/                 # 容器镜像仓库
│   ├── sentry/                 # 错误监控
│   ├── nakama/                 # 游戏服务器
│   ├── dify/                   # LLM 应用开发平台
│   ├── hoppscotch/             # API 测试工具
│   ├── logto/                  # 身份认证
│   ├── jeepay/                 # 支付系统
│   └── helm_*.yaml             # Helm 部署模板
└── docs/                       # 文档
```

## 技术栈

| 层 | 技术 |
|---|---|
| 后端框架 | Drogon (C++17) / Actix (Rust) / Lua (Moon) |
| 前端框架 | Quasar 2 + Vue 3 + Pinia + Vite / Taro (UniApp) |
| 数据库 | PostgreSQL |
| 缓存 / 任务 | Redis |
| 容器化 | Docker / Podman / Kubernetes |
| 部署 | Helm / docker-compose |

## 快速开始

### 前置要求

- C++17 编译器 + xmake
- Rust 1.85+
- Node.js 20+ / pnpm 9+
- PostgreSQL 14+
- Redis 6+

### 后端（C++ 服务）

```bash
cd backend/cpp

# 构建单个服务（hik/cbis/gateway/notify/chat/clean/funeral/pay/tow/repair/dingtalk/liveup/qdzy）
xmake build hik
xmake build cbis
xmake build gateway

# 构建全部
xmake build
```

服务配置通过环境变量注入（见 `docs/SECURITY.md` 环境变量清单）。

### 后端（Rust 服务）

```bash
cd backend/rust/ebike
cargo build --release
cargo run --release
```

### 后端（Lua 独立部署）

```bash
cd backend/dedicated
moon main.lua
```

### 前端

```bash
# H5 应用（qdsh/qdzy/ebike/tow/funeral/mapz/wj/xady/scoreboard/cbis）
cd frontend/h5/qdsh
pnpm install
pnpm dev    # Quasar dev server

# UniApp 小程序
cd frontend/h5/uniapp
pnpm install
pnpm dev:h5 # H5 模式

# 游戏化培训
cd frontend/game/xiuxian_text
pnpm install
pnpm dev

# 微前端
cd frontend/micro_frontend/admin
pnpm install
pnpm dev
```

### 数据库初始化

```bash
# 结构脚本（纯 DDL，无数据）
psql -U postgres -d datafusion -f backend/datafusion_schema.sql
```

## 环境变量清单

本项目**不硬编码任何密钥**，所有敏感配置通过环境变量注入。详见 [`docs/SECURITY.md`](docs/SECURITY.md)。

## 安全策略

本项目严格遵循"配置外置"原则：

- ✅ 所有服务配置（`config.json` / `config_debug.json`）使用 `${VAR}` 占位
- ✅ `.env` 文件提交**随机临时值**（仅用于本地开发，生产必须覆盖）
- ✅ 无 `.pem` / `.key` / `.p12` 等证书文件入库
- ✅ 部署流水线使用 `${{ secrets.* }}` 注入

详见 [`docs/SECURITY.md`](docs/SECURITY.md)。

## 第三方工具

`ThirdParty/` 目录包含 Harbor、Sentry、Nakama、Dify、Hoppscotch、Logto、Jeepay 等第三方部署工具。这些工具已清理其中混入的密钥，保留各自的开源许可证。使用这些工具时请参考其官方文档。

## 漏洞报告

如发现安全问题，请**不要**在 Issue 中公开描述。请通过以下方式私下联系维护者：

- 邮箱：security@example.com（占位，请替换为实际邮箱）
- 或提交私有安全 advisory

## 许可证

本项目基于 [MIT 许可证](LICENSE) 开源。

---

> ⚠️ **声明**：本仓库中的 `.env` 文件包含**随机生成的临时密钥**，仅用于本地开发测试。生产环境部署前**必须**替换为真实密钥并通过环境变量注入。
