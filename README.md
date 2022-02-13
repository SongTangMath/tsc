# tsc
A toy C89 compiler


## lex/yacc与flex/bison的关系.

flex是lex的gnu版,bison是yacc的gnu版(yacc并不是free software)

mac上只要安装了XCode自然就带有flex和bison
在linux上需要额外安装它们
sudo apt-get install flex bison

bison的文档是
https://www.gnu.org/software/bison/manual/bison.html

在Linux 4.14.81.bm.15-amd64 #1 SMP Debian 4.14.81.bm.15 x86_64 GNU/Linux
上自动安装的版本是flex 2.6.1 bison (GNU Bison) 3.0.4
bison2.3与3.0.4输出头文件扩展名可能不同

## yylex函数

lex会生成一个yylex函数，yacc通过调用这个函数来得知拿到的token是什么类型的，但是token的类型是在yacc中定义的。

lex的输入文件一般会被命名成 .l文件，通过lex XX.l 我们得到输出的文件是lex.yy.c
yacc会生成一个yyparse函数，这个函数会不断调用上面的yylex函数来得到token的类型
lex就是lexer(tokenizer)yacc就是parser

yylex在读到EOF的时候会返回0.yyparse根据返回值判断是否读到了EOF,所以当我们写action的时候注意解析到token要返回一个正整数

/* Returned upon end-of-file. */
#define YY_NULL 0
#define yyterminate() return YY_NULL

case YY_STATE_EOF(INITIAL):
	yyterminate();


## lex和yacc的输入文件格式

Definition section
%%
Rules section

%%
C code section

.l和.y的文件格式都是分成三段，用%%来分割，三个section的含义是：

Definition Section
这块可以放C语言的各种各种include，define等声明语句，但是要用%{ %}括起来。

如果是.l文件，可以放预定义的正则表达式：minus "-" 还要放token的定义，方法是：代号 正则表达式。然后到了，Rules Section就可以通过{符号} 来引用正则表达式

如果是.y文件，可以放token的定义，如：%token INTEGER PLUS ，这里的定一个的每个token都可以在y.tab.h中看到

Rules section
.l文件在这里放置的rules就是每个正则表达式要对应的动作，一般是返回一个token

.y文件在这里放置的rules就是满足一个语法描述时要执行的动作

不论是.l文件还是.y文件这里的动作都是用{}扩起来的，用C语言来描述，这些代码可以做你任何想要做的事情

C code Section
main函数，yyerror函数等的定义



在flex中,匹配到token后的动作由花括号内的内容决定,所以通常会在这里放一些C code
假设我们要在花括号中修改一个预定义的变量nword的内容,让它+1
那就应该在第一段里面定义这个nword
%{ int nword;%}
则int nword;会被原样输出到生成的lexer中
lex.yy.h中有几个函数
(1)当所有输入都被处理了会调用yywrap()
(2)yylex()返回下一个token类型

由于yacc总是调用yylex()我们应该总是把terminal定义到l文件中
错误示例:

additive_expression:
    multiplicative_expression
    | additive_expression Multiply_divide_mod multiplicative_expression
    ;

Multiply_divide_mod:
    '*' 
    | '\'
    | '%'

应该将Multiply_divide_mod定义到l文件中
("*"|"/"|"%") { printf("Multiply_divide_mod\n"); return Multiply_divide_mod; }
这种访问到一个文法节点的时候调用后面的代码块的内容相当于listener模式

在语法树的构建过程中,每个节点的类型是YYSTYPE

## 关于lex匹配多个规则的情况

When the generated scanner is run, it analyzes its input looking for strings 
which match any of its patterns. If it finds more than one match, it takes the 
one matching the most text (for trailing context rules, this includes the length 
of the trailing part, even though it will then be returned to the input). If it 
finds two or more matches of the same length, the rule listed first in the flex 
input file is chosen.


## 预定义变量与YYSTYPE

lex.yy.h中会声明全局变量

extern int yylineno;

extern char *yytext;

tsc.tab.h中会声明

extern YYSTYPE yylval;

YYSTYPE默认宏定义为int
如果想改变它可以在y文件的第一部分include一个头文件然后定义YYSTYPE宏为想要的类型.另外还有一些修改方法
可以用%union命令来把 YYSTYPE 定义为一个union

yytext会保存当前解析到的token
通常来说我们让l文件包含y文件生成的头文件这样可以在lexer中给yylval赋值
我们解析到一个token就讲相关信息保存到yylval的字段中(考虑将yylval定义成一个struct)
另外根据文档lex/yacc可能不能正确处理右递归,文法规则都要写成左递归的.



yyvsp 类型为YYSTYPE*
yyvsp[0]是值栈（value stack）当前的顶部。

可以把token绑定到YYSTYPE的某个域。如在y文件的第一部分定义
%token <iValue> INTEGER 
%type <nPtr> expr
把expr绑定到nPtr，把INTEGER绑定到iValue。yacc处理时会做转换。如：
expr: INTEGER { $$ = con($1); }

转换结果为：
yyval.nPtr = con(yyvsp[0].iValue);
这里的yyval是yyparse中的局部变量.

$1 $2等分别表示解析出来的文法部分.例如匹配了规则
additive_expression:
    additive_expression Multiply_divide_mod multiplicative_expression
则$1表示additive_expression $2表示Multiply_divide_mod $3表示multiplicative_expression
$$则表示冒号左侧的部分.如果没有使用%type<type_name>进行绑定,它们的类型都是YYSTYPE否则是YYSTYPE的type_name成员
(type_name可以带有'.'做成struct嵌套.实际上只是做了简单的字符串拼接)

## yyparse的局部变量

我们来看一下yyparse中声明的一些局部变量,可以利用这些局部变量传递一些结果用于构造我们自己的语法树
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


tsc.tab.cpp中有全局变量
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;

yyval与yylval的关系?在LL(1)文法中,只需要1个look ahead symbol就可以确定下一步的规约
例如1+2这个表达式
首先解析出一个1
这个1规约为一个INT_CONST
INT_CONST规约为 multiplicative_expression
这个multiplicative_expression规约为additive_expression

I_CONSTANT2 1
3 1
2 1
Add_minus
1 1
I_CONSTANT2 2
3 1
2 1
eof
1 2

要在运行的时候正确读取token字符串,只要在解析到token的时候把值保存到yylval中

  The state stack中的整数用于保存parse中的状态,对我们基本没用
  但是The semantic value stack中的值却可以利用

## C11关键字
以下为C11关键字本次不支持

"_Alignas"                              
"_Alignof"                              
"_Atomic"                               
"_Bool"                                 
"_Complex"                              
"_Generic"                              
"_Imaginary"                            
"_Noreturn"                             
"_Static_assert"                        
"_Thread_local"   

_Noreturn告诉编译器函数之后的代码不可达(如调用了abort()等)


## tsc中的lex/yacc规则

ANSI C grammar, Lex specification来自
http://www.quut.com/c/ANSI-C-grammar-l-2011.html
ANSI C Yacc grammar来自
http://www.quut.com/c/ANSI-C-grammar-y.html

(此基础上进行一些修改)
要生成cpp代码可以在bison用的y文件加1行
%language "c++"

然后执行
flex --header-file=lex.yy.h --c++ tsc.l 
bison -d tsc.y

但是发现很多问题.还是生成C代码然后手动改一下文件

flex --header-file=lex.yy.h tsc.l 
bison -d tsc.y

这个bison命令会给出一个错误tsc.y: conflicts: 1 shift/reduce
原因是它不知道结合性,可以忽略
lex.yy.cpp中的input()要改成yyinput()
tsc.tab.cpp中的void yyerror(const char *s);需要前置声明否则找不到符号