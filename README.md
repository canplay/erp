# ERP 系统 · 特种设备安全综合管理平台

> 面向《特种设备使用管理规则》（TSG 08-2026）合规要求，覆盖"日管控、周排查、月调度"闭环管理的综合 ERP 平台。

## 项目简介

本项目为特种设备使用单位、监管机构、第三方服务商提供全生命周期安全管理能力，涵盖设备档案、巡检任务、隐患闭环、周排查、月调度、监管对接、应急联动、AI 智能识别（OCR）等核心业务模块。

## 目录结构

```
erp/
├── backend/              # 后端服务
│   ├── cpp/              # Drogon C++ 服务（主业务网关、各业务子服务）
│   ├── cpp_next/         # 下一代 C++ 服务（OIDC 认证、扩展模块）
│   ├── rust/             # Rust 服务（电单车、拖车业务）
│   ├── dedicated/        # 独立部署服务
│   └── datafusion.sql    # 数据库初始化脚本（生产数据需自行准备）
├── frontend/             # 前端应用
│   ├── h5/               # 移动端 H5 应用（Quasar + Vue 3）
│   │   ├── qdsh/         # 气瓶追溯
│   │   ├── qdzy/         # 气瓶作业
│   │   ├── ebike/        # 电单车
│   │   ├── tow/          # 拖车
│   │   ├── funeral/      # 殡葬
│   │   ├── mapz/         # 地图
│   │   ├── wj/           # 问卷调查
│   │   ├── xady/         # 消防
│   │   ├── scoreboard/   # 看板
│   │   ├── cbis/         # CBIS
│   │   └── uniapp/       # UniApp 小程序
│   ├── game/             # 游戏化培训
│   │   ├── tarkov_like/  # 塔科夫风格
│   │   └── xiuxian_text/ # 修仙文字
│   └── micro_frontend/   # 微前端（admin + user）
├── thirdparty/           # 第三方部署工具（已清理密钥，保留各自许可证）
│   ├── harbor/           # 容器镜像仓库
│   ├── sentry/           # 错误监控
│   ├── nakama/           # 游戏服务器
│   ├── dify/             # LLM 应用开发平台
│   ├── hoppscotch/       # API 测试工具
│   ├── logto/            # 身份认证
│   ├── jeepay/           # 支付系统
│   └── helm_*.yaml       # Kubernetes Helm 部署模板
└── docs/                 # 文档
```

## 技术栈

| 层 | 技术 |
|---|---|
| 后端框架 | Drogon (C++17) / Actix (Rust) |
| 前端框架 | Quasar 2 + Vue 3 + Pinia + Vite |
| 数据库 | PostgreSQL / MySQL |
| 缓存 / 任务 | Redis / Hangfire |
| 全文搜索 | Meilisearch |
| AI / OCR | PaddleOCR (PaddleX) |
| 容器化 | Docker / Podman / Kubernetes |
| 部署 | Helm / docker-compose |

## 快速开始

### 前置要求

- C++17 编译器 + CMake + xmake
- Rust 1.70+
- Node.js 18+ / pnpm
- PostgreSQL 14+
- Redis 6+

### 后端（C++ 服务）

```bash
cd backend/cpp
xmake build
xmake run core
```

服务配置通过环境变量注入（见 `docs/SECURITY.md` 环境变量清单）。

### 前端

```bash
cd frontend/h5/qdsh
pnpm install
pnpm dev
```

### 数据库初始化

```bash
psql -U postgres -d datafusion -f backend/datafusion.sql
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

## 贡献指南

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/xxx`)
3. 提交变更 (`git commit -m 'feat: 添加 xxx'`)
4. 推送分支 (`git push origin feature/xxx`)
5. 提交 Pull Request

提交前请确保：
- 不提交任何真实密钥、密码、连接字符串
- 代码通过 `pnpm lint` 与 `dotnet build`（如适用）
- 更新相关文档

## 漏洞报告

如发现安全问题，请**不要**在 Issue 中公开描述。请通过以下方式私下联系维护者：

- 邮箱：[维护者邮箱]
- 或提交私有安全 advisory

## 许可证

本项目基于 [MIT 许可证](LICENSE) 开源。

---

> ⚠️ **声明**：本仓库中的 `.env` 文件包含**随机生成的临时密钥**，仅用于本地开发测试。生产环境部署前**必须**替换为真实密钥并通过环境变量注入。
