%{
	#include<stdio.h>
	#include<iostream>
	#include<string>
%}

D           [0-9]
L           [a-zA-Z]
id          {L}({L}|{D})*
number      {D}+(.{D}+)?(E[+-]?{digit}+)?
blanks      ([ \t\n]+)

%s          SLCOMMENT
%s          MLCOMMENT
%%

"//"        {
                cout<<"(SINGLE_LINE_COMMENT)"<<endl;
                BEGIN SLCOMMENT;
            }
"/*"        {
                cout<<"(MULTI_LINE_COMMENT)"<<endl;
                BEGIN MLCOMMENT;
            }
"=="        return ("EQ_OP");
"!="        return ("NE_OP");
"!"         return ("NOT_OP");
"++"        return ("INC_OP");
"--"        return ("DEC_OP");
"+="        return ("ADD_TO");
"-="        return ("SUB_TO");
"*="        return ("MUL_TO");
"/="        return ("DIV_TO");
"="         return ("=");
"+"         return ("+");
"-"         return ("-");
"*"         return ("*");
"/"         return ("/");
">="        return ("GE_OP");
"<="        return ("LE_OP");
"<<"        return ("<<");
">>"        return (">>");
"<"         return ("<");
">"         return (">");
"%"         return ("%");
"||"        return ("OR_OP");
"&"         return ("AND_OP")
"{" 		return ("{");
"}" 		return ("}");
"[" 		return ("[");
"]" 		return ("]");
"(" 		return ("(");
")" 		return (")");
","         return (",");
";"         return (";");
":"         return (":");
"."         return (".");
"?"         return ("?");
"'"         return ("'");
"""         return (""");
{id}        return ("INDENTIFIER");
{number}    return ("NUMBER");
{blanks}    {;}
"public"    return ("PUBLIC");
"privalite" return ("PRIVALITE");
"protected" return ("PROTECTED");
"void"      return ("VOID");
"int"       return ("INT");
"char"      return ("CHAR");
"float"     return ("FLOAT");
"double"    return ("DOUBLE");
"bool"      return ("BOOL");
"true"      return ("TRUE");
"false"     return ("FALSE");
"long"      return ("LONG");
"short"     return ("SHORT");
"const"     return ("CONST");
"static"    return ("STATIC");
"auto"      return ("AUTO");
"enum"      return ("ENUM");
"typedef"   return ("TYPEDEF");
"struct"    return ("STRUCT");
"sizeof"	return ("SIZEOF");
"if"        return ("IF");
"else"      return ("ELSE");
"do"        return ("DO");
"while"     return ("WHILE");
"return"    return ("RETURN");
"continue"  return ("CONTINUE");
"switch"    return ("SWITCH");
"case"      return ("CASE");
"break"     return ("BREAK");
"default"   return ("DEFUALT");
.     

<SLCOMMENT>.    {;}
<SLCOMMENT>\n   {BEGIN INITIAL;}
<MLCOMMENT>.|\n {;}
<MLCOMMENT>"*/" {BEGIN INITIAL;} 
%%

void main(){
	string s;
	while(true){
		cin>>s;
		cout<<analysis(s)<<endl;
	}
}