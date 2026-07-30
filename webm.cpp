/*
## AI成分
- 几乎全部由Greenchannel编写,多端适配由qingchuanhub编写 但是spdlog这种体力活就交给cline处理了
- readme除了"AI成分"和"作者的话"板块 剩下的全是ai生成的
- Gemini和DeepSeek V4 Flash负责给我答疑
- DeepSeek V4 Pro 负责审查我的代码
- 最后用cline排了一个try-catch的bug

## 作者的话
- Github的小朋友和大朋友们 早上/中午/晚上好啊
- 我是一个菜b小萌新 写了个练手小程序 随缘拓展吧 我没啥实战经验 所以代码风格可能有些诡异
- 但是我不是 -qingchuanhub
- 我用了spdlog和CLI11这两个库
- 想使用这个程序特别简单
- 确实 -qingchuanhub
- 直接编译代码,丢进一个文件夹里 然后配置一下环境变量
- 比如编译生成了main.exe
- 你可以改成任何想要的名字 我个人喜欢的是webm.exe
- 添加环境变量后 直接在cmd窗口里webm就可以使用力!
- 作者不会英语，项目中的英文来自 Google 翻译
- 作者是菜鸡小萌新 这种两百多行的小程序足足写了两个早上 一个晚上 一个上午
- 现在我就在这个上午写这条readme 我的屁股和腰已经非常疲惫了
- 我的耳机里放着Yourushika的歌 en 现在播放的是《うめき》
- 东北的夏天也不凉快!文字不要再叮我力!
- 别笑,我只看到了一个被暑期作业和补课班折磨疯的学生
- 写这条readme时 作者很困啊!!! 只睡了5小时就继续爬起来趁着周日休息写代码了!
- 不好 补课班作业没写
*/
//为什么不用万能头？-qingchuanhub
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include "CLI11.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

// 平台区分宏定义
#ifdef _WIN32
#include <windows.h>
#define PATH_SEP "\\"
#else
#include <unistd.h>
#define PATH_SEP "/"
#endif

namespace fs = std::filesystem;
const std::string CONFIG_FILE = "listConfig.txt";
const std::string LIST_FOLDER = "list";

// 开启Windows VT100 彩色终端（PowerShell/WindowsTerminal必备）
#ifdef _WIN32
void enableVTColor()
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hStdOut, &dwMode)) return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hStdOut, dwMode);
}
#else
void enableVTColor() {}
#endif

// 拼接list目录下文件完整路径
std::string getListFilePath(const std::string& fileName)
{
    return LIST_FOLDER + PATH_SEP + fileName + ".txt";
}

// 获取当前激活列表名
std::string getActiveList()
{
    if (!fs::exists(CONFIG_FILE))
        return "";

    std::ifstream cfg(CONFIG_FILE);
    std::string name;
    std::getline(cfg, name);
    cfg.close();
    return name;
}

// 设置激活列表
void setActiveList(const std::string& listName)
{
    std::ofstream cfg(CONFIG_FILE, std::ios::trunc);
    cfg << listName;
    cfg.close();
}

// 初始化目录
void initWorkspace()
{
    if (!fs::exists(LIST_FOLDER))
    {
        fs::create_directory(LIST_FOLDER);
        spdlog::info("创建数据目录: {}", LIST_FOLDER);
    }
    spdlog::info("初始化完成");
}

// 判断列表是否存在
bool listExists(const std::string& name)
{
    return fs::exists(getListFilePath(name));
}

// 创建列表
void createList(const std::string& name)
{
    if (listExists(name))
    {
        spdlog::warn("列表 {} 已存在", name);
        return;
    }
    std::ofstream f(getListFilePath(name));
    f.close();
    spdlog::info("成功创建列表: {}", name);
}

// 删除列表
void deleteList(const std::string& name)
{
    if (!listExists(name))
    {
        spdlog::error("列表 {} 不存在", name);
        return;
    }
    fs::remove(getListFilePath(name));
    spdlog::info("已删除列表: {}", name);

    // 如果删除的是当前活跃列表，清空配置
    std::string active = getActiveList();
    if (active == name)
    {
        setActiveList("");
        spdlog::warn("当前使用列表已被删除，请切换其他列表");
    }
}

// 添加键值对
void addKV(const std::string& key, const std::string& value)
{
    std::string active = getActiveList();
    if (active.empty())
    {
        spdlog::error("未选择任何列表，请先使用 set 切换列表");
        return;
    }
    if (key.find(':') != std::string::npos)
    {
        spdlog::warn("Key 不能包含冒号 ':'");
        return;
    }
    std::ofstream f(getListFilePath(active), std::ios::app);
    f << key << ":" << value << "\n";
    f.close();
    spdlog::info("添加成功 [{}:{}]", key, value);
}

// 打印当前列表全部内容
void printList()
{
    std::string active = getActiveList();
    if (active.empty())
    {
        spdlog::error("未选择任何列表");
        return;
    }
    std::ifstream f(getListFilePath(active));
    if (!f.is_open())
    {
        spdlog::error("打开列表文件失败");
        return;
    }
    std::string line;
    int idx = 1;
    std::cout << "===== 当前列表[" << active << "] =====\n";
    while (std::getline(f, line))
    {
        if (!line.empty())
            std::cout << idx++ << ". " << line << "\n";
    }
    f.close();
}

// 精确查找key
void findKey(const std::string& key)
{
    std::string active = getActiveList();
    if (active.empty())
    {
        spdlog::error("未选择任何列表");
        return;
    }
    std::ifstream f(getListFilePath(active));
    std::string line;
    bool found = false;
    while (std::getline(f, line))
    {
        size_t splitPos = line.find(':');
        if (splitPos == std::string::npos) continue;
        std::string k = line.substr(0, splitPos);
        std::string v = line.substr(splitPos + 1);
        if (k == key)
        {
            std::cout << key << " => " << v << "\n";
            found = true;
            break;
        }
    }
    if (!found)
        spdlog::warn("未查询到 key: {}", key);
    f.close();
}

// 列出全部列表
void showAllLists()
{
    std::cout << "===== 所有列表 =====\n";
    for (auto& entry : fs::directory_iterator(LIST_FOLDER))
    {
        std::string filename = entry.path().filename().string();
        size_t suffix = filename.find(".txt");
        if (suffix != std::string::npos)
            std::cout << filename.substr(0, suffix) << "\n";
    }
}

// 合并列表（简易合并，去重逻辑自行扩展）
void unionList(const std::string& src, const std::string& dst)
{
    if (!listExists(src) || !listExists(dst))
    {
        spdlog::error("源列表或目标列表不存在");
        return;
    }
    std::ifstream srcFile(getListFilePath(src));
    std::ofstream dstFile(getListFilePath(dst), std::ios::app);
    std::string line;
    while (std::getline(srcFile, line))
    {
        if (!line.empty()) dstFile << line << "\n";
    }
    srcFile.close();
    dstFile.close();
    spdlog::info("合并完成 {} -> {}", src, dst);
}

int main(int argc, char** argv)
{
    // 初始化终端色彩适配PowerShell
    enableVTColor();
    // 日志全局UTF8配置
    spdlog::set_default_logger(spdlog::stdout_color_mt("console"));
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

    CLI11::App app("website_manager 网址管理工具 | 兼容 CMD / PowerShell");
    app.require_subcommand(1);

    // init 初始化
    CLI11::App* initCmd = app.add_subcommand("init", "初始化数据目录");
    initCmd->callback([](){ initWorkspace(); });

    // create 创建列表
    CLI11::App* createCmd = app.add_subcommand("create", "创建新列表");
    std::string createName;
    createCmd->add_arg("name", "列表名称")->required()->store(createName);
    createCmd->callback([&](){ createList(createName); });

    // delete 删除列表
    CLI11::App* delCmd = app.add_subcommand("delete", "删除指定列表");
    std::string delName;
    delCmd->add_arg("name", "列表名称")->required()->store(delName);
    delCmd->callback([&](){ deleteList(delName); });

    // set 切换当前列表
    CLI11::App* setCmd = app.add_subcommand("set", "切换激活列表");
    std::string setName;
    setCmd->add_arg("name", "列表名称")->required()->store(setName);
    setCmd->callback([&](){
        if (!listExists(setName))
            spdlog::error("列表 {} 不存在", setName);
        else
        {
            setActiveList(setName);
            spdlog::info("当前列表切换为: {}", setName);
        }
    });

    // show 查看当前列表
    CLI11::App* showCmd = app.add_subcommand("show", "查看当前激活列表");
    showCmd->callback([](){
        std::string act = getActiveList();
        if (act.empty()) std::cout << "无激活列表\n";
        else std::cout << "当前列表: " << act << "\n";
    });

    // dir 列出全部列表
    CLI11::App* dirCmd = app.add_subcommand("dir", "展示所有列表");
    dirCmd->callback([](){ showAllLists(); });

    // add 添加键值
    CLI11::App* addCmd = app.add_subcommand("add", "添加网址/键值对");
    std::string addKey, addVal;
    addCmd->add_arg("key")->required()->store(addKey);
    addCmd->add_arg("value")->required()->store(addVal);
    addCmd->callback([&](){ addKV(addKey, addVal); });

    // print 打印全部内容
    CLI11::App* printCmd = app.add_subcommand("print", "打印当前列表全部数据");
    printCmd->callback([](){ printList(); });

    // find 精确查询key
    CLI11::App* findCmd = app.add_subcommand("find", "根据key查询value");
    std::string findK;
    findCmd->add_arg("key")->required()->store(findK);
    findCmd->callback([&](){ findKey(findK); });

    // union 合并两个列表
    CLI11::App* unionCmd = app.add_subcommand("union", "合并列表 union 源列表 目标列表");
    std::string unSrc, unDst;
    unionCmd->add_arg("src")->required()->store(unSrc);
    unionCmd->add_arg("dst")->required()->store(unDst);
    unionCmd->callback([&](){ unionList(unSrc, unDst); });

    CLI11_PARSE(app, argc, argv);
    return 0;
}