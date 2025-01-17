%token NUMBER IDENTIFIER
%token VOID INT CHAR DOUBLE FLOAT LONG SHORT BOOL FALSE TRUE
%token STATIC SIZEOF ENUM TYPEDEF STRUCT CONST
%token SWITCH BREAK CASE DEFAULT DO WHILE FOR AUTO CONTINUE IF ELSE RETURN
%token ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN
%token PTR_OP INC_OP DEC_OP AND_OP OR_OP LE_OP GE_OP EQ_OP NE_OP NOT_OP

%right = ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN
%left EQ_OP NE_OP
%left OR_OP
%left AND_OP
%left ELSE
%left < > LE_OP GE_OP
%left + -
%left * /


%%
program
    : declarations
    ;
declarations
    : var_declaration declarations
    | fun_declaration declarations
    | var_declaration
    | fun_declaration
    ;
var_declaration
    : type identifiers ';'
    | ENUM IDENTIFIER '{' identifiers '}' ';'
    | TYPEDEF STRUCT IDENTIFIER '{' declarations '}' ';'
    ;
identifiers
    : identifier
    | identifiers ',' identifier
    | identifiers ',' assign_statement
    ;
identifier
    : assign_statement
    | IDENTIFIER '[' NUMBER ']' '[' NUMBER ']'
    | IDENTIFIER '[' IDENTIFIER ']'
    | IDENTIFIER '[' NUMBER ']'
    | IDENTIFIER '[' ']'
    | IDENTIFIER '.' IDENTIFIER
    | IDENTIFIER PTR_OP IDENTIFIER
    | IDENTIFIER
    ;
fun_declaration
    : type IDENTIFIER '(' parameters ')' block
    | type IDENTIFIER '(' ')' block
    ;
type
    : IDENTIFIER '*'
    | CHAR '*'
    | CONST CHAR '*'
    | INT '*'
	| IDENTIFIER
    | VOID
    | CHAR
    | SHORT
    | INT
    | LONG
    | FLOAT
    | DOUBLE
    | BOOL
    | ENUM
    ;
parameters
    : type IDENTIFIER ',' parameters
    | type IDENTIFIER
    ;
block
    : '{' local_declarations statements '}'
    | '{' local_declarations '}'
    | '{' statements '}'
    | '{' '}'
    | ';'
    ;
local_declarations
    : var_declaration
    | local_declarations var_declaration
    ;
statements
    : statement
    | statements statement
    ;
statement
    : for_statement
    | WHILE '(' exp ')' block
    | RETURN exp ';'
    | assign_statements
    | BREAK ';'
    | expression_statement
    | fun_statement
    | if_statement
    | SWITCH '(' identifier ')' block
    | case_statement
    | block
    ;
fun_statement
    : IDENTIFIER '(' args ')' ';'
    ;
for_statement
    : FOR '(' var_declaration expression_statement exp ')' block
    | FOR '(' expression_statement expression_statement exp ')' block
    ;
assign_statements
    : assign_statements ',' assign_statement
    | assign_statement ';'
    | assign_statement
    ;
assign_statement
    : identifier '=' exp
    | '*' identifier '=' exp
    | identifier ADD_ASSIGN exp
    | identifier SUB_ASSIGN exp
    | identifier MUL_ASSIGN exp
    | identifier DIV_ASSIGN exp
    ;
expression_statement
    : ';'
    | exp ';'
    ;
if_statement
    : IF '(' exp ')' block
    | IF '(' exp ')' block ELSE block
    ;
case_statement
    : CASE simple_expression ':' statements
    | DEFAULT ':' statements
    ;
exp
    : simple_expression
    | mul_expression
    | add_expression
    | eq_expression
    | and_expression
    ;
simple_expression
    : '{' args '}'
    | TRUE
    | FALSE
    | INC_OP IDENTIFIER
    | DEC_OP IDENTIFIER
    | IDENTIFIER INC_OP
    | IDENTIFIER DEC_OP
    | NOT_OP IDENTIFIER
    | ''' IDENTIFIER '''
    | '"' IDENTIFIER '"'
    | NUMBER
    | identifier
    ;
mul_expression
    : exp '*' exp
    | exp '/' exp
    ;
add_expression
    : exp '+' exp
    | exp '-' exp
    ;
eq_expression
    : exp EQ_OP exp
    | exp NE_OP exp
    | exp '>' exp
    | exp '<' exp
    | exp GE_OP exp
    | exp LE_OP exp
    ;
and_expression
    : exp AND_OP exp
    | exp OR_OP exp
    ;
args
    : exp
    | args ',' exp
    ;
%%
void yyerror()
{
}
