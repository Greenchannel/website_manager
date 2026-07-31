# website_manager — 命令行键值对管理工具

一个基于 **C++ / CLI11 / spdlog** 的命令行键值对管理工具。**仅支持 CMD 命令行操作**。

---

## 项目结构

```
website_manager/
├── webm.cpp           ← 主程序
├── CLI11.hpp          ← CLI 参数解析库（单头文件）
├── spdlog/            ← 日志库（header-only）
├── webm.exe           ← 编译后的可执行文件
├── readme.md          ← 本文件
├── listConfig.txt     ← 配置文件，记录当前列表名
└── list/              ← 存放所有列表数据文件
    └── *.txt          ← 用户创建的列表文件
```

---

## 编译

```bash
g++ -std=c++23 -I. -o webm.exe webm.cpp
```
PowerShell
```powershell
g++ -std=c++23 -I. webm.cpp -o webm.exe
```

> `-I.` 让编译器在当前目录搜索 spdlog 头文件。首次编译较慢（spdlog 头文件量大），后续增量编译会快得多。
---

## webm.ps1
配套 PowerShell 快捷别名脚本 webm.ps1
放在 exe 同目录，一键简化调用

---

## 子命令一览
# cmd
| 命令 | 参数 | 说明 |
|------|------|------|
| `init` | 无 | 初始化环境 |
| `create` | `<name>` — 列表名(单个单词 / 中文均可,不能含空格)| 新建空白列表文件 list / 工作网址.txt |
| `dir` | 无 | 输出本机所有已创建列表 |
| `set` | `<name>` — 已存在的列表名 | 设置当前操作列表，后续 add/print/find 均作用于此列表 |
| `show` | 无 | 打印当前正在使用的列表名称 |
| `add` | `<key>` — 键，`<value>` — 值 | 向当前列表追加一条键值对（键不能含 `:`） |
| `print` | 无 | 带序号输出当前列表所有键值对 |
| `find` | `<key>` — 要查找的键 | 在当前列表中精确查找并输出匹配的键值对，无匹配则提示警告 |
| `union` | `<nameA>` — 源列表 `<nameB>` — 目标列表 | 将源列表全部内容追加到目标列表（无自动去重） |
| `delete` | `<name>` — 待删除列表名称 | 删除指定列表文件；若删除的是当前列表，自动清空配置 |

# PowerShell
| 命令 | 参数 | 说明 |
|------|------|------|
| `init` | 无 | 初始化环境 |
| `create` | `<name>` — 列表名(单个单词 / 中文均可,不能含空格)| 新建空白列表文件 list / 工作网址.txt |
| `dir` | 无 | 输出本机所有已创建列表 |
| `set` | `<name>` — 已存在的列表名 | 设置当前操作列表，后续 add/print/find 均作用于此列表 |
| `show` | 无 | 打印当前正在使用的列表名称 |
| `add` | 1.`<key>` — 键 `<value>` — 值 | 向当前列表追加一条键值对（键不能含 `:`） |
| `print` | 无 | 带序号输出当前列表所有键值对 |
| `find` | `<key>` — 要查找的键 | 在当前列表中精确查找并输出匹配的键值对，无匹配则提示警告 |
| `union` | `<nameA>` — 源列表 `<nameB>` — 目标列表 | 将源列表全部内容追加到目标列表（无自动去重） |
| `delete`| `<name>` — 待删除列表名称 | 删除指定列表文件；若删除的是当前列表，自动清空配置 |

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
PowerShell
```powershell
# 初始化目录
.\webm.exe init
# 创建列表
webm create work
# 切换列表
webm set work
# 添加网址
webm add B站 https://bilibili.com
webm add Github https://github.com
# 查询
webm find Github
# 查看全部内容
webm print
# 查看所有列表
webm dir
# 删除列表
webm delete test
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

## 作者的话
- 我用了spdlog和CLI11这两个库
- 想使用这个程序特别简单
- 直接编译代码,丢进一个文件夹里 然后配置一下环境变量
- 比如编译生成了main.exe
- 你可以改成任何想要的名字 我个人喜欢的是webm.exe
- 添加环境变量后 直接在cmd窗口里webm就可以使用力!

## website_manager WiKi
[在线Wiki完整文档](https://github.com/Greenchannel/website_manager/wiki)