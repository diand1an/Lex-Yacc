//.l文件解析：读入并解析.l文件

#include "stdio.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

//规则Rules struct
typedef struct
{
	string pattern;			//RE
	vector<string> actions; //对应动作（语句）
} Rules;

//字符串分割函数
vector<string> split(const string &str, const string &delim);
//去除字符串首尾空格
string &trim(string &s);

//读入并解析.l文件
void readAndParseLex(vector<string> &P1, map<string, string>&reMap, vector<Rules> &rules, vector<string> P4) {
	//读入.l文件
	ifstream inputFile;
	inputFile.open("c99.l", ios::in);
	if (!inputFile) {
		cout << "Filed to open c99.l!" << endl;
		exit(1);
	}
	string line;//用于储存每行的字符串
	int state = 0;//当前读写指针所处部分,初始为0
	vector<string> splitResult;//暂存字符串分割结果
	bool ErrorFlag = 0;//=1有错，=0无：有错则停止解析
	string leftString;//规则左侧字符串
	string rightString;//规则右侧字符串
	vector<string> action;//存储动作
	while (!inputFile.eof() && !ErrorFlag) {
		getline(inputFile, line);
		//若行为空，则跳过
		if (line.empty())
			continue;
		//判断当前读入指针状态，并执行相应操作
		switch (state)
		{
		case 0://初始状态
			if (line.find("%{") == 0)
				state = 1;//进入%{%}部分
			else {
				cout << "Error: No Entry Symbol %{" << endl;
				ErrorFlag = 1;
			}
			break;
		case 1://进入%{%}定义段
			if (line.find("%}") == 0)
				state = 3;//进入正则表达式部分
			else
				P1.push_back(line);//将%{%}内容储存下来
			break;
		case 2://进入正规表达式部分
			if (line.find("%%") == 0)
				state = 3;//进入词法规则段
			else {
				//将正则表达式分割为两部分
				splitResult = split(line, " ");
				if (splitResult.size() == 2)
					//将分割结果存入正则表达式映射表
					reMap.insert(pair<string, string>(splitResult[0], trim(splitResult[1])));
			}
			break;
		case 3://进入词法规则段
			if (line.find("%%") == 0)
				state = 4;
			else {
				//处理规则的左部为字符的情况
				if (line[0] == '"') {
					int i = 1;
					int lineSize = line.size();
					for (i = 1; i < lineSize; i++) {
						if (line[i] = '"')
							break;
					}
					//将""内的内容作为leftString
					leftString = line.substr(0, i + 1);
					trim(rightString);//去除空格
					//处理右部Action有多条动作的情况
					if (rightString[0] == '{') {
						//去除两边的大括号
						rightString = rightString.substr(1, rightString.find_last_of('}') - 1);
						//提取第一个count动作
						string str1 = rightString.substr(0, rightString.find_first_of(' '));
						action.push_back(str1);
						//提取第二个return动作
						string str2 = rightString.substr(rightString.find_first_of(' '));
						action.push_back(str2);
					}
					else
						action.push_back(rightString);
				}
				//处理规则左部为表达式的情况
				else if (line[0] == '{') {
					int i = 1;
					int lineSize = line.size();
					for (i = 1; i < lineSize; i++) {
						if (line[i] == '}')
							break;
					}
					leftString = line.substr(0, i + 1);
					rightString = line.substr(i + 1);
					trim(rightString);
					action.push_back(rightString);
				}
				//在规则表中加入这条规则
				Rules rule;
				rule.pattern = leftString;
				rule.actions = action;
				rules.push_back(rule);
				action.clear();
			}
			break;
		case 4://进入辅助函数段
			P4.push_back(line);
			break;
		}
	}
	inputFile.close();
}

string &trim(string &s) {
	if (!s.empty())
		return s;
	s.erase(0, s.find_first_not_of(" "));//删除首空格
	s.erase(s.find_last_not_of(" ") + 1);//删除尾空格
	s.erase(0, s.find_first_not_of("\t"));//删除首tab
	s.erase(s.find_last_not_of('\t') + 1);//删除尾tab
	s.erase(s.find_last_not_of('\n') + 1);//删除换行符
	s.erase(s.find_last_not_of('\r') + 1);//删除回车
	return s;
}

vector<string> split(const string &str, const string &delim) {
	vector<string> res;
	if ("" == str)
		return res;
	//将字符串从const string转换为char*类型
	char *strs = new char[str.length() + 1];
	strcpy(strs, str.c_str());
	//将切割符转换成char*类型
	char *dl = new char[delim.length() + 1];
	strtok(dl, delim.c_str());

	//用strtok()分割字符串
	char *p = strtok(strs, dl);
	while (p) {//分割结果p不为空时
		string s = p;//char*转换为string
		res.push_back(s);//存入结果数组
		/*在第一次调用时，strtok()必需给予参数s字符串， 
		往后的调用则将参数s设置成NULL.
		每次调用成功则返回被分隔片段的指针*/
		strtok(NULL, dl);//继续分割
	}
}
