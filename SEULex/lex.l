%{
	#include<stdio.h>
	void singleLineComment();
	void multiLineComment();
%}

D           [0-9]
L           [a-zA-Z]
id          {L}({L}|{D})*
number      {D}+(.{D}+)?(E[+-]?{digit}+)?

%%

"//"        { singleLineComment(); return "(SINGLE_LINE_COMMENT)" }
"/*"        { multiLineComment(); return "(MULTI_LINE_COMMENT)" }
"=="        return ("EQ_OP");
"!="        return ("NE_OP");
"!"         return ("NOT_OP");
"++"        return ("INC_OP");
"--"        return ("DEC_OP");
"+="        return ("ADD_ASSIGN");
"-="        return ("SUB_ASSIGN");
"*="        return ("MUL_ASSIGN");
"/="        return ("DIV_ASSIGN");
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
"auto"      return ("AUASSIGN");
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
.			printf<<"Error:Unmatched Characters!";

%%

void multiLineComment()
{
	char c, prev=0;
	while((c = input())!=0) /* (EOF maps to 0) */
	{
		if((c =='/' && prev=='*')
			return;
		prev = c;
	}
	printf ("ERROR: unterminated comment!");
}

void singleLineComment()
{
	char c,prev=0;
	while((c == input()!=0))/* (EOF maps to 0) */
	{
		if(c ='\'&& prev='n')
			return;
	}
}
