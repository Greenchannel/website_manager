/*
## AI成分
- 几乎全部由这个人编写 但是spdlog这种体力活就交给cline处理了
- readme除了"AI成分"和"作者的话"板块 剩下的全是ai生成的
- Gemini和DeepSeek V4 Flash负责给我答疑
- DeepSeek V4 Pro 负责审查我的代码
- 最后用cline排了一个try-catch的bug

## 作者的话
- Github的小朋友和大朋友们 早上/中午/晚上好啊
- 我是一个菜b小萌新 写了个练手小程序 随缘拓展吧 我没啥实战经验 所以代码风格可能有些诡异
- 我用了spdlog和CLI11这两个库
- 想使用这个程序特别简单
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
#include <iostream>
#include <string>
#include <fstream>
#include <cerrno>
#include <filesystem>
#include <direct.h>
#include <windows.h>
#include <cstdio>
#include "CLI11.hpp"
#include "spdlog/spdlog.h"

namespace fs=std::filesystem;
const std::string configFilePath="listConfig.txt";
//以后杂七杂八的功能可能会用的 所以就先搞函数提升下开发效率
bool checkFile(const std::string& fileName){//判断文件是否存在
    std::ifstream file(fileName+".txt");
        if (file.is_open()){//如果被打开说明文件存在
            //std::cout<<"Error! The file already exists\n";
            return true;
        }
    return false;
}
bool createFile(const std::string& fileName){//创建文件
    std::ofstream file(fileName+".txt");
    if (!file.is_open()){//如果没打开这个文件
        return false;
    }
    return true;
}
//true表示创建成功 false表示文件名存在或者创建出现问题
std::string getListName(){
    std::ifstream file(configFilePath);
    if (!file.is_open()){//如果无法打开
        return "";
    }
    //如果打开了
    std::string listName;
    getline(file,listName);
    return listName;
}
int main(int argc,char *argv[]){
    //argc个元素 argv[i]代表参数
    if (_chdir("list")==-1){//当list不存在或者无法移动到list
        int errnoCode=errno;
        spdlog::error("An error occurred when we tried to move to the dictionary named list");
        spdlog::error("errnoCode: {}", errnoCode);
        if (_mkdir("list")==-1){//如果创建list失败
            errnoCode=errno;
            spdlog::error("We are trying to create the list directory, but we have encountered some problems");
            spdlog::error("errnoCode: {}", errnoCode);
            return -1;
        }else{
            spdlog::info("Success! We have successfully created the list directory");
            //_chdir("list");
            if (_chdir("list")==-1){//创建目录后却仍然无法进入
                errnoCode=errno;
                spdlog::error("We successfully created the list directory, but we still can't access it");
                spdlog::error("errnoCode: {}", errnoCode);
                return -1;
            }
        }
    }//Google翻译立大功
    CLI::App app{"A simple website manager"};
    std::string createListName;
    auto* create=app.add_subcommand("create","create a new list");
    create->add_option("name",createListName,"set the create list name")->required();
    create->callback([&](){
        if (checkFile(createListName)){
            spdlog::warn("The file already exists");
            return;
        }
        if (!createFile(createListName)){
            spdlog::error("Cannot create file");
            return;
        }
        //那么剩下的情况便是正常创建打开的文件
        spdlog::info("File created successfully");
    });
    std::string setListName;
    auto* set=app.add_subcommand("set","Set the list to search");//设置当前可查询的list
    set->add_option("name",setListName,"set the list name")->required();
    set->callback([&](){
        //首先要判断文件是否存在/能打开
        if (!checkFile(setListName)){//如果文件无法打开
            spdlog::warn("Unable to open this list");
            return;
        }
        //setListName是不带有文件后缀的
        //这个操作通过修改配置文件listConfig.txt实现
        std::ofstream file(configFilePath);
        if (!file.is_open()){//如果无法打开配置文件
            int errnoCode=errno;
            spdlog::error("Unable to open the config file");
            spdlog::error("errnoCode: {}", errnoCode);
            return;
        }
        file<<setListName;//直接输入即可(无后缀)
        spdlog::info("Switched to list '{}'", setListName);
    });
    std::string addPairKey;
    std::string addPairValue;
    auto* add=app.add_subcommand("add","add a key-value pair");
    add->add_option("key",addPairKey,"add pair`s key")->required();
    add->add_option("value",addPairValue,"add pair`s value")->required();
    add->callback([&](){
        if (addPairKey==""){
            spdlog::warn("Key cannot be empty");
            return;            
        }
        if (addPairValue==""){
            spdlog::warn("Value cannot be empty");
            return;
        }
        if (addPairKey.find(":")!=std::string::npos){//如果键中含有:
            spdlog::warn("Key cannot contain ':'");
            return;
        }
        std::string listName=getListName();
        if (listName==""){
            spdlog::error("No list selected");
            return;
        }
        std::ofstream file(listName+".txt",std::ios::app);//加上ios::app开启追加模式
        if (!file.is_open()){
            int errnoCode=errno;
            spdlog::error("Unable to open the list file");
            spdlog::error("errnoCode: {}", errnoCode);
            return;
        }
        file<<addPairKey+":"+addPairValue+"\n";
        spdlog::info("Added: {} -> {}", addPairKey, addPairValue);
    });
    //默认输出listConfig.txt
    //当printListName非空时输出指定的文件
    auto* print=app.add_subcommand("print","print all websites in list");
    std::string printListName="";
    print->add_option("name",printListName,"Display the contents of the file");
    print->callback([&](){
        if (printListName!=""){//如果printListName有内容
            if (!checkFile(printListName)){
                spdlog::error("Unable to open the file");
                return;
            }
            std::ifstream file(printListName+".txt");
            if (!file.is_open()){
                int errnoCode=errno;
                spdlog::error("Unable to open the list file");
                spdlog::error("errnoCode: {}", errnoCode);
                return;
            }
            std::string line;
            int counter=0;
            while (getline(file,line)){
                if (line.empty()){
                    continue;
                }
                counter++;
                std::cout<<counter<<"."<<line<<"\n";
            }
        }else{//如果printListName没内容
            std::string listName=getListName();
            if (listName==""){//如果获取失败
                spdlog::error("No list selected");
                return;
            }
            std::ifstream file(listName+".txt");
            if (!file.is_open()){
                int errnoCode=errno;
                spdlog::error("Unable to open the list file");
                spdlog::error("errnoCode: {}", errnoCode);
                return;
            }
            std::string listLine;
            int counter=0;
            while (getline(file,listLine)){
                if (listLine.empty()){
                    continue;
                }
                counter++;
                std::cout<<counter<<"."<<listLine<<"\n";
            }
        }
    });
    auto* find=app.add_subcommand("find","find value by key");
    std::string findKey;
    find->add_option("key",findKey,"ask for the key used for find")->required();
    find->callback([&](){
        std::string listName=getListName();
        if (listName==""){//如果获取失败
            spdlog::error("No list selected");
            return;
        }
        std::ifstream file(listName+".txt");
        if (!file.is_open()){//如果list打不开
            int errnoCode=errno;
            spdlog::error("Unable to open the current list");
            spdlog::error("errnoCode: {}", errnoCode);
            return;
        }
        std::string listLine;
        //0~listLine-1
        while (getline(file,listLine)){
            //读取到的listLine应该形如xxx=xxx
            if (listLine.empty()){
                continue;//如果这行空 直接跳过
            }
            std::size_t index=listLine.find(":");
            if (index==std::string::npos){//表示没有找到
                continue;
            }
            std::string findPairKey=listLine.substr(0,index);//index-1-0+1 实际读取0~index-1
            std::string findPairValue=listLine.substr(index+1);           
            //listLine.length()-(index+1)+1=listLine.length()-index-1+1=listLine.length()-index
            //可以推导出这个式子 可以作为substr的参数 但是完全没必要 substr会自己处理
            if (findKey==findPairKey){
                std::cout<<findPairKey+":"<<findPairValue<<"\n";
            }           
        }
    });
    auto* del=app.add_subcommand("delete","delete list");
    std::string delListName;
    del->add_option("name",delListName,"the name of the list");
    del->callback([&](){
        if (delListName.empty()){//如果输入值为空
            spdlog::warn("List name cannot be empty");
            return;
        }
        if (!checkFile(delListName)){//如果文件不存在或无法打开
            spdlog::warn("The file does not exist or cannot be opened");
            return;
        }
        //remove函数接收一个c风格字符串 所以拼接后要加上.c_str()转换成c风格字符串 且remove函数非零返回值表示错误 所以可以写if (remove(fileName)!=0)
        if (remove((delListName+".txt").c_str())!=0){
            spdlog::error("An issue occurred while deleting the list");
            return;
        }
        spdlog::info("The list was successfully deleted");
        if (delListName==getListName()){
             std::ofstream file(configFilePath);
            if (!file.is_open()){
                spdlog::error("Unable to open listConfig.txt");
                return;
            }
            file<<"";
            spdlog::warn("listConfig.txt cleared - please use `set` to reconfigure");
        }
    });
    auto* show=app.add_subcommand("show","show the listName from listConfig.txt");
    show->callback([&](){
        std::ifstream file(configFilePath);
        if (!file.is_open()){
            spdlog::error("Unable to open listConfig.txt");
            return;
        }
        std::string line;
        getline(file,line);
        if (line.empty()){
            spdlog::error("listConfig.txt was not set");
            return;
        }
        std::cout<<line<<"\n";
    });
    auto* init=app.add_subcommand("init","init program");
    init->callback([&](){
        if (!createFile("listConfig")){//如果创建失败
            spdlog::error("Unable to create listConfig.txt");
            return;
        }
        spdlog::info("listConfig.txt created successfully");
    });
    std::string unionListA;
    std::string unionListB;
    auto* unionn=app.add_subcommand("union","Merge list A into list B");
    unionn->add_option("listA",unionListA,"the name of the list")->required();
    unionn ->add_option("listB",unionListB,"the name of the list")->required();
    unionn->callback([&](){
        if (!checkFile(unionListA)){
            spdlog::error("List A not found");
            return;
        }
        std::ifstream listA(unionListA+".txt");
        if (!listA.is_open()){
            spdlog::error("Unable to open list A");
            return;
        }
        if (!checkFile(unionListB)){
            spdlog::error("List B not found");
            return;
        }
        std::ofstream listB(unionListB+".txt",std::ios::app);
        if (!listB.is_open()){
            spdlog::error("Unable to open list B");
            return;
        }
        listB<<listA.rdbuf();
        if (listB.fail()){
            spdlog::error("An error occurred during the writing process");
        }
        spdlog::info("Operation completed successfully");
    });
    auto* dir=app.add_subcommand("dir","Display all lists in the list directory");
    dir->callback([&](){
        int counter=0;
        fs::path currentPath=fs::current_path();
        for (const auto& entry : fs::recursive_directory_iterator(currentPath)){
            if (fs::is_regular_file(entry.status())){
                counter++;
                if (entry.path().string()==configFilePath){
                    continue;
                }
                std::cout<<counter<<"."<<entry.path().string()<<"\n";
            }
        }
    });
    try {
        app.parse(argc,argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }
    return 0;
}