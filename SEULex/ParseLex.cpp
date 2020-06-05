//.l文件解析：读入并解析.l文件；并标准化正则表达式

#include "stdio.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <ctime>
using namespace std;

const string COLLECTION("0123456789abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#%'()*+,-./:;<=>\?[\\]^{|} \n\t\v\f~&");

//规则Rules struct
typedef struct
{
	string pattern;			//RE
	vector<string> actions; //对应动作（语句）
} Rules;

//字符串分割函数（用于分割多个action）
vector<string> split(const string &str, const string &delim);
//去除字符串首尾空格
string &trim(string &s);
//替换{},即{L}->[0-9]
void repBrace(string &exp, map<string, string> &reMap);
//替换[],即[0-9]->(0|1|...|9)
void repSquareBrackets(string &exp);
//替换?和+
void repQueAndAdd(string &exp);
//处理"""情况
void handleQuotes(string &exp);
//加终结符$(作为模式的最后一个字符匹配一行的结尾)
void addDot(string &exp);
////读入并解析.l文件
void readAndParseLex(vector<string> &P1, map<string, string>&reMap, vector<Rules> &rules, vector<string> P4);
//标准化
void REStandardlization(vector<Rules> &rules, map<string, string>reMap);

/*
%{
P1
&}
Map<leftString,rightString>
%%
vector<Rules>:pattern,actions
%%
P4
*/

void read_Parse_REStandardlize(vector<string> &P1, map<string, string>&reMap, vector<Rules> &rules, vector<string> P4) {
	clock_t startT = clock();
	readAndParseLex(P1, reMap, rules, P4);
	REStandardlization(rules, reMap);
	clock_t endT = clock();
	cout << "Read and parse the file successfully! Times overhead: = " << (double)(endT - startT) / CLOCKS_PER_SEC << endl;
}

void readAndParseLex(vector<string> &P1, map<string, string>&reMap, vector<Rules> &rules, vector<string> P4) {
	//读入.l文件
	ifstream inputFile;
	inputFile.open("lex.l", ios::in);
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
		case 2:
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
				//处理规则的左部为""字符的情况
				if (line[0] == '"') {
					int i = 1;
					//特殊情况：当左部为"""时
					if (line[1] == '\"'&&line[2] == '\"') {
						leftString = "\"\"\"";
						i = 2;
					}
					else {
						for (i = 1; i < line.size(); i++) {
							if (line[i] = '"')
								break;
						}
						//将""内的内容作为leftString
						leftString = line.substr(0, i + 1);
					}
					rightString = line.substr(i + 1);
					trim(rightString);//去除空格
					//处理右部Action有多条动作的情况（即有{}括起来时）
					if (rightString[0] == '{') {
						//去除两边的大括号
						rightString = rightString.substr(1, rightString.find_last_of('}') - 1);
						//提取第一个动作
						string str1 = rightString.substr(0, rightString.find_first_of(' '));
						action.push_back(str1);
						//提取第二个动作
						string str2 = rightString.substr(rightString.find_first_of(' '));
						action.push_back(str2);
					}
					else
						action.push_back(rightString);
				}
				//处理规则左部为表达式{}的情况
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
				//左边字符为.时
				else if (line[0] == '.') {
					leftString = line[0];
					rightString = line.substr(1);
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

void REStandardlization(vector<Rules> &rules, map<string, string>reMap) {
	//用迭代器遍历并处理map
	for (map<string, string>::iterator i = reMap.begin(); i != reMap.end(); i++) {
		repBrace(i->second, reMap);
		repSquareBrackets(i->second);
		repQueAndAdd(i->second);
	}
	//遍历并处理vector的pattern
	for (auto &e : rules) {
		handleQuotes(e.pattern);
		repBrace(e.pattern, reMap);
		addDot(e.pattern);
	}
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

//RE Standardlization

void repBrace(string &exp, map<string, string> &reMap) {
	string reName;
	string reExp;
	if (exp.length() == 1)return;
	for (int i = 0; i < exp.length(); i++) {
		if (exp[i] == '{') {
			int lenthOfExp = (exp.find_first_of('}') - 1) - (i + 1) + 1;
			reName = exp.substr(i + 1, lenthOfExp);
			//在map中找到需要的表达式
			map<string, string>::iterator  find = reMap.find(reName);
			//构造规范化的表达式
			reExp += find->second;
			i = exp.find_first_of('}', i + 1);
		}
		else
			//当表达式没有{}时
			reExp += exp[i];
	}
	exp = reExp;
}

void repSquareBrackets(string &exp) {
	string reExp;
	if (exp.length() == 1)return;
	for (int i = 0; i < exp.length(); i++) {
		//[0-9]
		if (exp[i] == '['&&exp[i + 1] == '0') {
			reExp += '(';
			int startIndex = COLLECTION.find_first_of('0');
			int endIndex = COLLECTION.find_first_of('9');
			for (int j = startIndex; j < endIndex; j++) {
				reExp = reExp + COLLECTION[j] + '|';
			}
			reExp += '9)';
			i = exp.find_first_of(']', i + 1);
		}
		//[a-zA-Z]
		else if (exp[i] == '['&&exp[i + 1] == 'a') {
			reExp += '(';
			int startIndex = COLLECTION.find_first_of('a');
			int endIndex = COLLECTION.find_first_of('Z');
			for (int j = startIndex; j < endIndex; j++) {
				reExp = reExp + COLLECTION[j] + '|';
			}
			reExp += 'Z)';
			i = exp.find_first_of(']', i + 1);
		}
		//[+-]or[-+]
		else if (exp[i] == '[' && (exp[i + 1] == '+' || exp[i + 1] == '-')) {
			reExp += "(+|-)";
			i = exp.find_first_of(']', i + 1);
		}
		else
			reExp += exp[i];
	}
}

void repQueAndAdd(string &exp) {
	string reExp;
	vector<int>lParPos;//用于存储左括号的位置
	for (int i = 0; i < exp.length(); i++) {
		reExp += exp[i];
		if (exp[i] == '(') {
			//读到左括号，将其位置保存下来
			lParPos.push_back(reExp.length() - 1);
		}
		else if (i + 1 < exp.length() && exp[i] == '('&&exp[i + 1] == '?') {
			//x?->(@|x),ε记作@
			string temp;
			temp = temp + "(@|" + reExp.substr(lParPos.back()) + ")";
			//将原来的表达式内最后一个左括号开始的表达式全都删除
			reExp.erase(lParPos.back(), reExp.length());
			reExp += temp;
			//把最后一个左括号的位置弹出
			lParPos.pop_back();
			i++;
		}
		else if (exp[i] == ')' && i + 1 < exp.length() && exp[i + 1] == '+') {
			//x+ -> xx*
			string temp;
			temp += reExp.substr(lParPos.back());//x
			temp += reExp.substr(lParPos.back());//xx
			temp += '*';//xx*
			reExp.erase(lParPos.back(), reExp.length());
			reExp += temp;
			lParPos.pop_back();
			i++;
		}
	}
	exp = reExp;
}

void handleQuotes(string &exp) {
	if (exp == "\"\"\"")
		exp = "\"";
	else
		//两层迭代器
		//remove删除成员后需要立即调用erase()，确保真正的删除
		exp.erase(remove(exp.begin(), exp.end(), '"'), exp.end());
}

void addDot(string &exp) {
	string reExp;
	for (int i = 0; i < exp.length(); i++) {
		reExp += exp[i];
		//当前字符为最后一个字符，or当前字符为(，不加$
		if ((i == exp.length() - 1) || (exp[i] == '('))continue;
		//下一字符为|或)，不加$
		else if (i + 1 < exp.length() && (exp[i + 1] == '|' || exp[i + 1] == ')' || exp[i + 1] == '*'))continue;
		//当前字符为|，不属||情况，不加$
		else if (i + 1 < exp.length() && exp[i] == '|'&&exp[i] != '|')continue;
		//当前字符为|，属于||情况,加$
		else if (i + 1 < exp.length() && exp[i] == '|'&&exp[i] == '|')
			reExp += '$';
		//当前字符为\，属于\*情况，加$
		else if (i + 1 < exp.length() && exp[i] == '/'&&exp[i + 1] == '*')
			reExp += '$';
		//其余情况，加$
		else reExp += '$';
	}
	exp = reExp;
}