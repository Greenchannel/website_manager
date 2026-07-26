# website_manager — 命令行键值对管理工具

一个基于 **C++ / CLI11 / spdlog** 的命令行键值对管理工具。**仅支持 CMD 命令行操作**。

---

## 项目结构

```
website_manager/
├── main.cpp           ← 主程序
├── CLI11.hpp          ← CLI 参数解析库（单头文件）
├── spdlog/            ← 日志库（header-only）
├── main.exe           ← 编译后的可执行文件
├── readme.md          ← 本文件
├── listConfig.txt     ← 配置文件，记录当前列表名
└── list/              ← 存放所有列表数据文件
    └── *.txt          ← 用户创建的列表文件
```

---

## 编译

```bash
g++ -std=c++11 -I. -o main.exe main.cpp
```

> `-I.` 让编译器在当前目录搜索 spdlog 头文件。首次编译较慢（spdlog 头文件量大），后续增量编译会快得多。

---

## 子命令一览

| 命令 | 参数 | 说明 |
|------|------|------|
| `create` | `<name>` — 列表名 | 创建新列表，已存在则拒绝 |
| `set` | `<name>` — 列表名 | 切换到指定列表 |
| `add` | `<key>` — 键，`<value>` — 值 | 向当前列表追加一条键值对（键不能含 `:`） |
| `print` | 无 | 编号输出当前列表全部记录 |
| `find` | `<key>` — 查找键 | 在当前列表中精确查找并输出匹配的键值对 |
| `del` | `<name>` — 列表名 | 删除指定列表文件；若删除的是当前列表，自动清空配置 |
| `show` | 无 | 打印当前列表名 |
| `init` | 无 | （待实现）初始化环境 |

---

## 典型用法

```bash
# 创建列表并切换
main.exe create mylist

# 添加键值对
main.exe add 百度 https://baidu.com
main.exe add GitHub https://github.com

# 查看全部
main.exe print

# 按键查找
main.exe find GitHub

# 查看当前列表名
main.exe show

# 切换列表
main.exe set another

# 删除列表
main.exe del mylist
```

---

## 数据格式

列表文件以 `key:value` 格式存储，每行一条记录：

```
百度:https://baidu.com
GitHub:https://github.com
```

---

## 日志

使用 spdlog 输出带时间戳的彩色日志：

| 颜色 | 级别 | 场景 |
|------|------|------|
| 🔴 红色 | `error` | 系统错误、操作失败 |
| 🟡 黄色 | `warn` | 输入非法、文件冲突 |
| 🟢 绿色 | `info` | 操作成功确认 |

日志输出示例：
```
[2026-07-26 10:03:29.537] [info] File created successfully
[2026-07-26 10:03:29.537] [info] Switched to list 'work'
[2026-07-26 10:03:29.537] [warn] Key cannot contain ':'
[2026-07-26 10:03:29.537] [error] No list selected
```

数据展示（`print` / `find` / `show` 的内容输出）使用普通 `cout`，不走日志。

---

## 设计说明

- **`listConfig.txt`** — 存储当前活跃列表名（纯文本一行）
- **`list/` 目录** — 所有列表文件统一存放，以 `.txt` 为后缀
- **自动切换目录** — 程序启动时自动 `_chdir("list")`，后续所有文件操作都在 `list/` 下进行
- **工具函数** — `checkFile()` / `createFile()` / `getListName()` 封装底层文件操作，避免重复代码
- **追加模式** — `add` 使用 `ios::app`，不会覆盖已有数据

---

## AI成分
- 几乎全部由这个人编写 但是spdlog这种体力活就交给cline处理了
- readme为ai生成
- Gemini和DeepSeek V4 Flash负责给我答疑
- DeepSeek V4 Pro 负责审查我的代码

## 作者的话

- 作者不会英语，项目中的英文来自 Google 翻译
- 作者是菜鸡小萌新 这种两百多行的小程序足足写了两个早上 一个晚上 一个上午
- 现在我就在这个上午写这条readme 我的屁股和腰已经非常疲惫了
- 我的耳机里放着Yourushika的歌 en 现在播放的是《うめき》
