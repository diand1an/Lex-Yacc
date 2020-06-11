//
//yacc.y文件解析，及翻译
//

#pragma once
#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
using namespace std;

typedef vector<pair<string, vector<string>>> ProductionStrVec; //字符版，<产生式左部，右部>
ProductionStrVec productionStrVec; //string版本产生式，用编号下标寻址

typedef vector<pair<int, vector<int>>> ProductionVec;          //int版，<产生式左部，右部>
ProductionVec productionVec;       //int版本产生式，用于编号下标寻址

//前面0表示左，1表示右，后面是规则，字符串版本
vector<pair<int, vector<string>>> precedenceRulesStrVec;
//前面0表示左，1表示右，后面是规则，int版本
vector<pair<int, vector<int>>> precedenceRulesVec;

//vector<string>tokensVec,存储所有的token
vector<string> tokensVec;
//vector<string>funcVec,存储每一行用户程序
vector<string> funcVec;
//vector<string>productionsVec,存储每一条产生式
vector<string> productionsVec;

//map<string, int>tokensMap 映射：tokens -> 标号，<tokens,count>
map<string, int> tokensMap;
//tokensMap中的下标：
//终结符 char字符 + tokens：0-boundT,非终结符:(boundT+1)-(boundN)
int boundT, boundN;

//映射：symbol->productionVec的下标
//key为symbol，value为<以该symbol为左部的产生式在productionVec中的起始index，数量>
map<int, pair<int, int>> indexMap;


//读并解析yacc.y
//定义段，%token存入tokensMap
//定义段，%left、%right存入precedenceRulesStrVec
//规则段，存入productionStrVec
//辅助函数段，存入funcVec
int readAndParseYacc() {
	string filePath = "C:\\Users\\10176\\source\\repos\\Yacc\\Yacc\\yacc.y";
	ifstream inputFile;

	inputFile.open(filePath, ios::in);
	if (!inputFile) {
		cout << "Error : Failed to open yacc.y !!" << endl;
		exit(1);
	}
	cout << "Successfully opened the file yacc.y !" << endl;
	cout << "Start parsing." << endl;
	string str = "";
	inputFile >> str;

	//读取定义段
	while (str != "%%") {
		//读取所有%token
		if (str == "%token") {
			//读到%token，则再往后读一个字符串(即tokensVec中不保存"%token"，只保存"TOKEN")
			inputFile >> str;
			while (str != "%left" && str != "%right"
				&&str != "%token" && str != "%%") {
				tokensVec.push_back(str);
				inputFile >> str;
			}
		}

		//newSet用来暂时保存当前读到的符号
		vector<string> newSet;
		//读取所有%left, 左结合
		if (str == "%left") {
			inputFile >> str;
			while (str != "%left" && str != "%right"
				&&str != "%token" && str != "%%") {
				newSet.push_back(str);
				inputFile >> str;
			}
			//一行规则读完，将该条规则加入precedenceRulesStrVec，0表示左结合
			precedenceRulesStrVec.push_back(make_pair(0, newSet));//make_pair(,)无需写出类型， 就可以生成一个pair对象
			newSet.clear();
		}
		//读取所有%right，右结合
		if (str == "%right") {
			inputFile >> str;
			while (str != "%left" && str != "%right"
				&&str != "%token" && str != "%%") {
				newSet.push_back(str);
				inputFile >> str;
			}
			//一行规则读完，将该条规则加入precedenceRulesStrVec，1表示右结合
			precedenceRulesStrVec.push_back(make_pair(1, newSet));
			newSet.clear();
		}
	}

	//读取规则段
	if (str == "%%") {
		//读取产生式
		inputFile >> str;
		while (str != "%%") {
			//p为一条产生式，左边为产生式左部的非终结符，右边为产生式右部各个部分组成的vector
			//产生式： E(非终结符) -> A|B|… 
			pair<string, vector<string>> p;
			p.first = str;
			inputFile >> str;
			//读到";"，则同一个非终结符的所有产生式结束
			while (str != ";") {
				if (str == ":")
					inputFile >> str;
				if (str == "|") {
					//读到|，一条产生式结束，存入productionStrVec
					productionStrVec.push_back(p);//此时p.first=str；p.second未初始化，所以要清空second
					//只清空存储一条产生式右部的vector，左部的非终结符不变
					p.second.clear();
					inputFile >> str;
				}
				while (str != "|" && str != ";") {
					//将一条产生式右部的各个部分存入vector
					p.second.push_back(str);
					inputFile >> str;
				}
			}
			//将最后一条产生式存入productionStrVec
			productionStrVec.push_back(p);
			inputFile >> str;
		}
	}

	//读取辅助函数段
	if (str == "%%") {
		//读取程序代码段
		while (!inputFile.eof()) {
			//读取一行字符串，默认分隔符为换行符号
			getline(inputFile, str);
			//除去每一行最后的换行符
			str.erase(str.find_last_not_of('\r') + 1);
			funcVec.push_back(str);
		}
	}
	cout << "Successfully parsed." << endl;
	inputFile.close();
	return 0;
}

//将所有token转换为正整数，从string转换为int
//tokensMap<token,对应的count>，0-boundT为终结符，（boundT+1）-boundN为非终结符
//翻译左、右结合优先级列表，从precedenceRulesStrVec转存到precedenceRulesVec
//翻译产生式，从productionStrVec转换为productionVec
//构建映射indexMap<symbol,index>；key为symbol，value为<以该symbol为左部的产生式在productionVec中的起始index，数量>
void translateExp() {
	cout << "Start translating. " << endl;

	int count = 0;

	//翻译字符,''中的字符
	for (auto &production : productionStrVec) {
		//在所有产生式的右部中找字符(即为''中的字符)
		for (auto &item : production.second)
			if (item.size() == 3 && item[0] == '\'' && item[2] == '\'') {
				item = item[1];
				//如果在tokensMap中已经找到了这个字符，就不再将其插入tokensMap
				//.find():如果map中没有要查找的数据，它返回的迭代器等于end函数返回的迭代器
				if (tokensMap.find(item) != tokensMap.end())
					continue;
				//在tokensMap中加入映射关系
				/*map 容器的成员函数 emplace() 可以在适当的位置直接构造新元素，从而避免复制和移动操作。
				它的参数通常是构造元素，也就是 pair<const K,T> 对象所需要的。
				只有当容器中现有元素的键与这个元素的键不同时，才会构造这个元素*/
				//emplace使用参数自动构造tokensMap内部对象
				//如果有重复的key值则自动舍弃
				tokensMap.emplace(item, count);
				count++;
			}
	}
	//翻译token
	for (auto &token : tokensVec) {
		tokensMap.emplace(token, count);
		count++;
	}
	//终结符的结束位置
	boundT = count - 1;

	//翻译非终结符
	for (auto &production : productionStrVec) {
		if (tokensMap.find(production.first) != tokensMap.end())
			continue;
		tokensMap.emplace(production.first, count);
		count++;
	}
	//非终结符的结束位置
	boundN = count - 1;


	//构建左、右结合运算符优先级列表，存到precedenceRulesVec：vector<pair<int,vector<int>>>
	for (auto &s : precedenceRulesStrVec) {
		//precedenceRulesStrVec：vector<pair<int, vector<string>>>
		vector<int> newSet;
		//将该条规则中的每个token翻译成数字
		for (auto &e : s.second)
			newSet.push_back(tokensMap.find(e)->second);
		//precedenceRulesVec：vector<pair<int,vector<int>>>。<左/右结合,<count>>
		precedenceRulesVec.push_back(make_pair(s.first, newSet));
	}

	//为了产生式标号从1开始，先在产生式vector中加入一条空白产生式
	productionVec.push_back(pair<int, vector<int>>(-1, vector<int>()));
	//S'->S产生式，产生式右部为第一个非终结符
	productionVec.push_back(pair<int, vector<int>>(-1, vector<int>({ boundT + 1 })));

	//翻译产生式
	int preleftInt = boundT + 1, counter = 0;
	int tempLeft;             //记录当前翻译的产生式左部
	vector<int> tempRightVec; //记录当前翻译的产生式右部
	for (auto &production : productionStrVec) {
		//翻译产生式右部
		tempRightVec.clear();
		for (auto &item : production.second)
			tempRightVec.push_back(tokensMap[item]);//tokensMap[item]是对应的count

		//翻译产生式左部
		tempLeft = tokensMap[production.first];
		//如果是新的产生式左部
		if (tempLeft != preleftInt) {
			//记录上一个产生式左部，以及在productionVec中的起始位置和长度
			indexMap.emplace(preleftInt, pair<int, int>(productionVec.size() - counter, counter));
			//为记录下一个产生式而将counter置零
			counter = 0;
			//更新当前产生式左部
			preleftInt = tempLeft;
		}
		//将当前产生式加入productionVec
		productionVec.push_back(pair<int, vector<int>>(tempLeft, tempRightVec));
		//同一个非终结符的产生式数量++
		counter++;
	}
	//记录最后一条产生式左部，以及在productionVec中的起始位置和长度
	indexMap.emplace(preleftInt, pair<int, int>(productionVec.size() - counter, counter));
	cout << "Successfully translated! " << endl;
}

void ParseAndThranslate() {
	clock_t startT = clock();
	readAndParseYacc();
	translateExp();
	clock_t endT = clock();
	cout << "...Read and parse the file successfully! Times overhead: = " << (double)(endT - startT) / CLOCKS_PER_SEC << endl;

	/*
	//test，把结果输出到txt中
	ofstream fout1("tokensMap.txt");
	for (auto &token : tokensMap) {
		fout1 << token.first << "  " << token.second << endl;
	}
	fout1.close();

	ofstream fout2("indexMap.txt");
	for (auto &ind : indexMap) {
		fout2 << ind.first << "  (" << ind.second.first << "," << ind.second.second << ")" << endl;
	}
	fout2.close();

	ofstream fout3("ProductionStrVec.txt");
	for (auto &pStr : productionStrVec) {
		fout3 << pStr.first << "  :";

		for (auto &pStrVec:pStr.second) {
			fout3 << pStrVec<<"  ";
		}
		fout3 << endl;
	}
	fout3.close();

	ofstream fout4("productionVec.txt");
	for (auto &pInt : productionVec) {
		fout4 << pInt.first << "  :";

		for (auto &pVec : pInt.second) {
			fout4 << pVec << "  ";
		}
		fout4 << endl;
	}
	fout4.close();

	ofstream fout5("precedenceRulesStrVec.txt");
	for (auto &pRStr : precedenceRulesStrVec) {
		fout5 << pRStr.first << "  :";

		for (auto &pRStrVec : pRStr.second) {
			fout5 << pRStrVec << "  ";
		}
		fout5 << endl;
	}
	fout5.close();

	ofstream fout6("precedenceRulesVec.txt");
	for (auto &pRInt : precedenceRulesVec) {
		fout6 << pRInt.first << "  :";

		for (auto &pRIntVec : pRInt.second) {
			fout6 << pRIntVec << "  ";
		}
		fout6 << endl;
	}
	fout6.close();
	*/
}