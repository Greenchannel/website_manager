/*
Github的小朋友和大朋友们 早上/中午/晚上好啊
我是一个菜b小萌新 写了个练手小程序 随缘拓展吧 我没啥实战经验 所以代码风格可能有些诡异
我用了spdlog和CLI11这两个库
想使用这个程序特别简单
直接编译代码,丢进一个文件夹里 然后配置一下环境变量
比如编译生成了main.exe
你可以改成任何想要的名字 我个人喜欢的是webm.exe
添加环境变量后 直接在cmd窗口里webm就可以使用力!
*/
#include <iostream>
#include <string>
#include <fstream>
#include <cerrno>
#include <direct.h>
#include <windows.h>
#include <cstdio>
#include "CLI11.hpp"
#include "spdlog/spdlog.h"
using namespace std;
const string configFilePath="listConfig.txt";
//以后杂七杂八的功能可能会用的 所以就先搞函数提升下开发效率
bool checkFile(const string& fileName){//判断文件是否存在
    ifstream file(fileName+".txt");
        if (file.is_open()){//如果被打开说明文件存在
            //cout<<"Error! The file already exists\n";
            return true;
        }
    return false;
}
bool createFile(const string& fileName){//创建文件
    ofstream file(fileName+".txt");
    if (!file.is_open()){//如果没打开这个文件
        return false;
    }
    return true;
}
//true表示创建成功 false表示文件名存在或者创建出现问题
string getListName(){
    ifstream file(configFilePath);
    if (!file.is_open()){//如果无法打开
        return "";
    }
    //如果打开了
    string listName;
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
    string createListName;
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
    string setListName;
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
        ofstream file(configFilePath);
        if (!file.is_open()){//如果无法打开配置文件
            int errnoCode=errno;
            spdlog::error("Unable to open the config file");
            spdlog::error("errnoCode: {}", errnoCode);
            return;
        }
        file<<setListName;//直接输入即可(无后缀)
        spdlog::info("Switched to list '{}'", setListName);
    });
    string addPairKey;
    string addPairValue;
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
        if (addPairKey.find(":")!=string::npos){//如果键中含有:
            spdlog::warn("Key cannot contain ':'");
            return;
        }
        string listName=getListName();
        if (listName==""){
            spdlog::error("No list selected");
            return;
        }
        ofstream file(listName+".txt",ios::app);//加上ios::app开启追加模式
        if (!file.is_open()){
            int errnoCode=errno;
            spdlog::error("Unable to open the list file");
            spdlog::error("errnoCode: {}", errnoCode);
            return;
        }
        file<<addPairKey+":"+addPairValue+"\n";
        spdlog::info("Added: {} → {}", addPairKey, addPairValue);
    });
    auto* print=app.add_subcommand("print","print all websites in this list");
    print->callback([&](){
        string listName=getListName();
        if (listName==""){//如果获取失败
            spdlog::error("No list selected");
            return;
        }
        ifstream file(listName+".txt");
        if (!file.is_open()){
            int errnoCode=errno;
            spdlog::error("Unable to open the list file");
            spdlog::error("errnoCode: {}", errnoCode);
            return;
        }
        string listLine;
        int counter=0;
        while (getline(file,listLine)){
            if (listLine.empty()){
                continue;
            }
            counter++;
            cout<<counter<<"."<<listLine<<"\n";
        }
    });
    auto* find=app.add_subcommand("find","find value by key");
    string findKey;
    find->add_option("key",findKey,"ask for the key used for find")->required();
    find->callback([&](){
        string listName=getListName();
        if (listName==""){//如果获取失败
            spdlog::error("No list selected");
            return;
        }
        ifstream file(listName+".txt");
        if (!file.is_open()){//如果list打不开
            int errnoCode=errno;
            spdlog::error("Unable to open the current list");
            spdlog::error("errnoCode: {}", errnoCode);
            return;
        }
        string listLine;
        //0~listLine-1
        while (getline(file,listLine)){
            //读取到的listLine应该形如xxx=xxx
            if (listLine.empty()){
                continue;//如果这行空 直接跳过
            }
            size_t index=listLine.find(":");
            if (index==string::npos){//表示没有找到
                continue;
            }
            string findPairKey=listLine.substr(0,index);//index-1-0+1 实际读取0~index-1
            string findPairValue=listLine.substr(index+1);           
            //listLine.length()-(index+1)+1=listLine.length()-index-1+1=listLine.length()-index
            //可以推导出这个式子 可以作为substr的参数 但是完全没必要 substr会自己处理
            if (findKey==findPairKey){
                cout<<findPairKey+":"<<findPairValue<<"\n";
            }           
        }
    });
    auto* del=app.add_subcommand("del","delete list");
    string delListName;
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
             ofstream file(configFilePath);
            if (!file.is_open()){
                spdlog::error("Unable to open listConfig.txt");
                return;
            }
            file<<"";
            spdlog::warn("listConfig.txt cleared — please use `set` to reconfigure");
        }
    });
    auto* show=app.add_subcommand("show","show the listName from listConfig.txt");
    show->callback([&](){
        ifstream file(configFilePath);
        if (!file.is_open()){
            spdlog::error("Unable to open listConfig.txt");
            return;
        }
        string line;
        getline(file,line);
        if (line.empty()){
            spdlog::error("listConfig.txt was not set");
            return;
        }
        cout<<line<<"\n";
    });
    auto* init=app.add_subcommand("init","init program");
    init->callback([&](){
        if (!createFile("listConfig")){//如果创建失败
            spdlog::error("Unable to create listConfig.txt");
            return;
        }
        spdlog::info("listConfig.txt created successfully");
    });
    try {
        app.parse(argc,argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }
    return 0;
}//auto* init=app.add_subcommand("init","init program");//TODO