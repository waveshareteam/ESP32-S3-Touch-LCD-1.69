# 配置

[English](README.md)

此目录预留给由 CI 或多个示例共同使用的 ESP-IDF 配置覆盖。当前示例仍在各自工程中
维护 `sdkconfig.defaults` 或 `sdkconfig.ci`；只有确实被多个第一方工程使用时才添加共享覆盖。

`markdown-audit.json` 定义有限检查器使用的双语首页契约，包括必需的页头组件、链接、徽章和章节图标。
`ci-routing-audit.json` 为维护者路由审计补充本仓库的文档资源、非构建路径和全局构建输入。
