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
- 比如编译生成了webm.exe
- 添加环境变量后 直接在cmd窗口里webm就可以使用力!
- 作者不会英语，项目中的英文来自 Google 翻译
- 作者是菜鸡小萌新 这种两百多行的小程序足足写了两个早上 一个晚上 一个上午
- 现在我就在这个上午写这条readme 我的屁股和腰已经非常疲惫了
- 我的耳机里放着Yorushika的歌 en 现在播放的是《うめき》
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
#include <queue>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <utility>
#include "CLI11.hpp"
#include "spdlog/spdlog.h"
#ifdef _WIN32
#   define NOMINMAX
#   include <windows.h>
#   include <direct.h>
#else
#   include <unistd.h>
#   include <sys/stat.h>
#   include <sys/wait.h>
#endif
namespace fs=std::filesystem;
const std::string configFilePath="configs/listConfig";
const std::string gitServerPath="configs/gitServer";
const std::string gitignorePath=".gitignore";
//以后杂七杂八的功能可能会用的 所以就先搞函数提升下开发效率
std::string fixGetline(std::string line){
    while ((!line.empty())&&(line.back()=='\r'||line.back()==' '||line.back()=='\t')){
        line.pop_back();
    }
    return line;
}
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
    return fixGetline(listName);
}
int editDistance(const std::string & a,const std::string & b){
    std::vector<std::vector<int>> dp;
    dp.resize(a.size()+1);
    for (int i=0;i<=a.size();i++){
        dp[i].resize(b.size()+1);
    }
    for(int i=1;i<=a.size();i++){
        dp[i][0]=i;
    }
    for (int i=1;i<=b.size();i++){
        dp[0][i]=i;
    }
    for (int i=1;i<=a.size();i++){
        for (int j=1;j<=b.size();j++){
            if (a[i-1]==b[j-1]){
                dp[i][j]=dp[i-1][j-1];
            }else{
                dp[i][j]=std::min({dp[i-1][j-1],dp[i][j-1],dp[i-1][j]})+1;
            }
        }
    }
    return dp[a.size()][b.size()];
}
int systemExitCode(int raw){
#ifdef _WIN32
    return raw;    
#else
    if (raw==-1||!WIFEXITED(raw)){
        return -1;
    }
    return WEXITSTATUS(raw);
#endif
}
std::string getTime(){
    const std::time_t t=std::time(nullptr);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm,&t);
#else
    localtime_r(&t,&tm);
#endif
    char buf[32];
    std::strftime(buf,sizeof(buf),"%Y-%m-%d %H:%M:%S",&tm);
    return buf;
}
int main(int argc,char *argv[]){
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);      
    SetConsoleCP(CP_UTF8);
#endif
    //argc个元素 argv[i]代表参数
#ifdef _WIN32
    if (_chdir("list")==-1){//当list不存在或者无法移动到list
#else
    if (chdir("list")==-1){
#endif
        int errnoCode=errno;
        spdlog::error("在切换目录到list时出现错误");
        spdlog::error("errnoCode: {}", errnoCode);
#ifdef _WIN32
        if (_mkdir("list")==-1){//如果创建list失败
#else
        if (mkdir("list",0755)==-1){
#endif
            errnoCode=errno;
            spdlog::error("无法创建list目录");
            spdlog::error("errnoCode: {}", errnoCode);
            return -1;
        }else{
            spdlog::info("成功创建目录");
#ifdef _WIN32            
            if (_chdir("list")==-1){//创建目录后却仍然无法进入
#else
            if (chdir("list")==-1){
#endif
                errnoCode=errno;
                spdlog::error("成功创建list目录,但无法进入");
                spdlog::error("errnoCode: {}", errnoCode);
                return -1;
            }
        }
    }//Google翻译立大功
    std::error_code ec;
    fs::create_directories(fs::path(configFilePath).parent_path(),ec);
    if (ec){
        spdlog::error("无法创建配置目录:{}",ec.message());
        return -1;
    }
    CLI::App app{"一个简单的键值对存储器"};
    std::string createListName;
    auto* create=app.add_subcommand("create","创建一个list");
    create->add_option("name",createListName,"设置创建list的名字")->required();
    create->callback([&](){
        if (checkFile(createListName)){
            spdlog::warn("list已存在");
            return;
        }
        if (!createFile(createListName)){
            spdlog::error("无法创建list");
            return;
        }
        //那么剩下的情况便是正常创建打开的文件
        spdlog::info("list成功创建");
    });
    std::string setListName;
    auto* set=app.add_subcommand("set","设置当前查询的list");//设置当前可查询的list
    set->add_option("name",setListName,"设置list名")->required();
    set->callback([&](){
        //首先要判断文件是否存在/能打开
        if (!checkFile(setListName)){//如果文件无法打开
            spdlog::warn("无法打开list");
            return;
        }
        //setListName是不带有文件后缀的
        //这个操作通过修改配置文件listConfig.txt实现
        std::ofstream file(configFilePath);
        if (!file.is_open()){//如果无法打开配置文件
            int errnoCode=errno;
            spdlog::error("无法打开配置文件");
            spdlog::error("errnoCode: {}", errnoCode);
            return;
        }
        file<<setListName;//直接输入即可(无后缀)
        spdlog::info("切换到: '{}'", setListName);
    });
    std::string addPairKey;
    std::string addPairValue;
    auto* add=app.add_subcommand("add","添加网页");
    add->add_option("key",addPairKey,"设置网页名")->required();
    add->add_option("value",addPairValue,"设置网页url")->required();
    add->callback([&](){
        if (addPairKey==""){
            spdlog::warn("键不能为空");
            return;            
        }
        if (addPairValue==""){
            spdlog::warn("值不能为空");
            return;
        }
        if (addPairKey.find(":")!=std::string::npos){//如果键中含有:
            spdlog::warn("键中不能包含 ':'");
            return;
        }
        std::string listName=getListName();
        if (listName==""){
            spdlog::error("输入不能为空");
            return;
        }
        std::ofstream file(listName+".txt",std::ios::app);//加上ios::app开启追加模式
        if (!file.is_open()){
            int errnoCode=errno;
            spdlog::error("无法打开list");
            spdlog::error("errnoCode: {}", errnoCode);
            return;
        }
        file<<addPairKey+":"+addPairValue+"\n";
        spdlog::info("Added: {} -> {}", addPairKey, addPairValue);
    });
    //默认输出listConfig.txt
    //当printListName非空时输出指定的文件
    auto* print=app.add_subcommand("print","输出当前list中的所有内容");
    std::string printListName="";
    print->add_option("name",printListName,"设置要输出的list名");
    print->callback([&](){
        if (printListName!=""){//如果printListName有内容
            if (!checkFile(printListName)){
                spdlog::error("无法打开list");
                return;
            }
            std::ifstream file(printListName+".txt");
            if (!file.is_open()){
                int errnoCode=errno;
                spdlog::error("无法打开list");
                spdlog::error("errnoCode: {}", errnoCode);
                return;
            }
            std::string line;
            int counter=0;
            while (getline(file,line)){
                line=fixGetline(line);
                if (line.empty()){
                    continue;
                }
                counter++;
                std::cout<<counter<<"."<<line<<"\n";
            }
        }else{//如果printListName没内容
            std::string listName=getListName();
            if (listName==""){//如果获取失败
                spdlog::error("输出不能为空");
                return;
            }
            std::ifstream file(listName+".txt");
            if (!file.is_open()){
                int errnoCode=errno;
                spdlog::error("无法打开list");
                spdlog::error("errnoCode: {}", errnoCode);
                return;
            }
            std::string listLine;
            int counter=0;
            while (getline(file,listLine)){
                listLine=fixGetline(listLine);
                if (listLine.empty()){
                    continue;
                }
                counter++;
                std::cout<<counter<<"."<<listLine<<"\n";
            }
        }
    });
    auto* find=app.add_subcommand("find","根据键查询当前list中的值");
    std::string findKey;
    find->add_option("key",findKey,"用于查询list中的值的键")->required();
    find->callback([&](){
        std::string listName=getListName();
        if (listName==""){//如果获取失败
            spdlog::error("输入不能为空");
            return;
        }
        std::ifstream file(listName+".txt");
        if (!file.is_open()){//如果list打不开
            int errnoCode=errno;
            spdlog::error("无法打开list");
            spdlog::error("errnoCode: {}", errnoCode);
            return;
        }
        std::string listLine;
        //0~listLine-1
        std::priority_queue<std::pair<int,std::pair<std::string,std::string>>,std::vector<std::pair<int, std::pair<std::string, std::string>>>,std::greater<>> findQueue;
        while (getline(file,listLine)){
            listLine=fixGetline(listLine);
            //读取到的listLine应该形如xxx=xxx
            if (listLine.empty()){
                continue;//如果这行空 直接跳过
            }
            std::size_t index=listLine.find(":");
            if (index==std::string::npos){//表示没有找到
                continue;
            }
            //std::string findPairKey=listLine.substr(0,index);//index-1-0+1 实际读取0~index-1
            //std::string findPairValue=listLine.substr(index+1);           
            //listLine.length()-(index+1)+1=listLine.length()-index-1+1=listLine.length()-index
            //可以推导出这个式子 可以作为substr的参数 但是完全没必要 substr会自己处理
            std::pair<int,std::pair<std::string,std::string>> findpair;
            findpair.second.first=listLine.substr(0,index);
            findpair.second.second=listLine.substr(index+1);
            findpair.first=editDistance(findpair.second.first,findKey);
            findQueue.push(findpair);
        }
        for(int i=1;i<=8;i++){
            if (findQueue.empty()){
                break;
            }
            std::cout<<i<<"."<<findQueue.top().second.first<<":"<<(findQueue.top().second.second)<<"\n";
            findQueue.pop();
        }
    });
    auto* del=app.add_subcommand("delete","删除list");
    std::string delListName;
    del->add_option("name",delListName,"需要删除的list名");
    del->callback([&](){
        if (delListName.empty()){//如果输入值为空
            spdlog::warn("输出不能为空");
            return;
        }
        if (!checkFile(delListName)){//如果文件不存在或无法打开
            spdlog::warn("无法打开文件");
            return;
        }
        //remove函数接收一个c风格字符串 所以拼接后要加上.c_str()转换成c风格字符串 且remove函数非零返回值表示错误 所以可以写if (remove(fileName)!=0)
        if (remove((delListName+".txt").c_str())!=0){
            spdlog::error("在删除过程中出现问题 ");
            return;
        }
        spdlog::info("删除成功");
        if (delListName==getListName()){
             std::ofstream file(configFilePath);
            if (!file.is_open()){
                spdlog::error("无法打开配置文件");
                return;
            }
            file<<"";
            spdlog::warn("list删除成功");
        }
    });
    auto* show=app.add_subcommand("show","显示当前list");
    show->callback([&](){
        std::ifstream file(configFilePath);
        if (!file.is_open()){
            spdlog::error("无法打开配置文件");
            return;
        }
        std::string line;
        getline(file,line);
        line=fixGetline(line);
        if (line.empty()){
            spdlog::error("当前未设置list");
            return;
        }
        std::cout<<line<<"\n";
    });
    auto* init=app.add_subcommand("init","初始化程序");
    init->callback([&](){
        std::ofstream configFile(configFilePath);
        if (!configFile.is_open()){
            int errnoCode=errno;
            spdlog::error("无法创建配置文件");
            spdlog::error("errnoCode: {}", errnoCode);
            return;
        }
        configFile.close();
        std::ofstream file(gitignorePath);
        if (!file.is_open()){
            int errnoCode=errno;
            spdlog::error("无法打开gitignore");
            spdlog::error("errnoCode: {}", errnoCode);
            return;
        }
        file<<"/configs/";
        system("git init");
    });
    std::string unionListA;
    std::string unionListB;
    auto* unionn=app.add_subcommand("union","将list A合并到list B");
    unionn->add_option("listA",unionListA,"list A的名字")->required();
    unionn ->add_option("listB",unionListB,"list B的名字")->required();
    unionn->callback([&](){
        if (!checkFile(unionListA)){
            spdlog::error("未找到list A");
            return;
        }
        std::ifstream listA(unionListA+".txt");
        if (!listA.is_open()){
            spdlog::error("无法打开list A");
            return;
        }
        if (!checkFile(unionListB)){
            spdlog::error("未找到list B");
            return;
        }
        std::ofstream listB(unionListB+".txt",std::ios::app);
        if (!listB.is_open()){
            spdlog::error("无法打开list B");
            return;
        }
        listB<<listA.rdbuf();
        if (listB.fail()){
            spdlog::error("在写入中出现错误");
        }
        spdlog::info("合并成功");
    });
    auto* dir=app.add_subcommand("dir","显示所有list");
    dir->callback([&](){
        int counter=0;
        fs::path currentPath=fs::current_path();
        for (const auto& entry : fs::directory_iterator(currentPath)){
            if (fs::is_regular_file(entry.status())){
                counter++;
                std::cout<<counter<<"."<<entry.path().string()<<"\n";
            }
        }
    });
    std::string bindServerIp=""; 
    auto* bind=app.add_subcommand("bind","设置git服务器");
    bind->add_option("ip",bindServerIp,"git服务器的IP地址")->required();
    bind->callback([&](){
        int returnValue=systemExitCode(std::system(("git remote set-url origin "+bindServerIp).c_str()));
        if (returnValue==0){
            spdlog::info("成功设置git服务器");
        }else if(returnValue==2){
            returnValue=systemExitCode(std::system(("git remote add origin "+bindServerIp).c_str()));
            if (returnValue==0){
                spdlog::info("成功设置git服务器");
            }else{
                spdlog::error("在设置过程中出现问题 git返回值:"+std::to_string(returnValue));
            }
        }else{
            spdlog::error("在设置过程中出现问题 git返回值:"+std::to_string(returnValue));
        }
        return;
    });
    auto* save=app.add_subcommand("save","上传本地内容到git服务器");
    save->callback([&](){
        int returnValue=systemExitCode(std::system("git add ."));
        if (returnValue!=0){
            spdlog::error("在git add时出现错误 git返回值:"+std::to_string(returnValue));
        }else{
            returnValue=systemExitCode(std::system(("git commit -m\""+getTime()+"\"").c_str()));
            if (returnValue!=0&&returnValue!=1){
                spdlog::error("在提交git commit时出现错误 git返回值:"+std::to_string(returnValue));
            }else{
                returnValue=systemExitCode(std::system("git push -u origin HEAD"));
                if (returnValue!=0){
                    spdlog::error("在上传数据到git服务器时出现错误 git返回值:"+std::to_string(returnValue));
                }
            }
        }
        return;
    });
    auto* get=app.add_subcommand("get","从git服务器拉取数据");
    get->callback([&](){
        int returnValue=systemExitCode(std::system("git fetch -f origin HEAD"));
        if (returnValue!=0){
            spdlog::error("在从git服务器拉取数据时出现错误 git返回值:"+std::to_string(returnValue));
            return;
        }
        returnValue=systemExitCode(std::system("git reset --hard FETCH_HEAD"));
        if (returnValue!=0){
            spdlog::error("在更改本地git时出现错误 git返回值:"+std::to_string(returnValue));
            return;    
        }
        return;
    });
    try {
        app.parse(argc,argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }
    return 0;
}
