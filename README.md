# DhQuantTradeSystem

混合语言量化交易系统（C++ Core + Python Runtime）。

## 初始阶段 (Phase 0)

当前进度：工程基座搭建中。

## GUI Prototype

当前已补充一个纯前端原型层，使用 `PySide6 + QML`，不连接后端接口，只用于先把交易工作台需要的交互和信息结构走通。

安装 GUI 依赖：

```bash
pip install .[gui]
```

启动原型：

```bash
PYTHONPATH=python python -m dhquant.gui.app
```

或：

```bash
dhquant-gui
```
