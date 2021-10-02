# tsc
A toy C89 compiler
支持部分C99规范如for语句内的变量声明
不支持VLA 暂不支持typedef

## 关于printf
%d %i 十进制打印signed int

%u 十进制打印unsigned int

%ld 十进制打印signed long

%lu 十进制打印unsigned long

下面2项为C99规定但是部分编译器可能不支持

%lld 十进制打印signed long long

%llu 十进制打印unsigned long long

%f 打印float和double 实际上对于可变参数函数float会被提升为double


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

yylex有一个while(1)循环.通常来说自定义的action是找到一个token后就返回这个token的类型.如果不想返回token(例如读到了空白字符)
可以不返回.会继续寻找下个token(yylex的调用方看来就是没读到这个空白而是读到了空白的下个token)

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

## C99/C11关键字
以下为C99/C11关键字本次不支持

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

Note: The following %-parameters are the minimum sizes needed for real Lex.
%e	number of parsed tree nodes
%p	number of positions
%n	number of states
%k	number of packed character classes
%a	number of transitions
%o	size of output array

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


## llvm

mac上可以通过 brew install llvm 安装

假设test.cpp为
```
#include <stdio.h>
int main() {
  printf("hello world\n");
  return 0;
}
```
clang -emit-llvm test.cpp -S -o test.ll
可以用lli命令直接编译运行ll文件
lli test.ll
或者用llc将ll编译为汇编test.s
llc test.ll

## x64汇编

### 关于浮点常量
```
#include <stdio.h>
int main()
{
    float a=1000.0;
    int b=*(int *)&a;
    printf("%d\n",b);
    printf("%f\n",a);
    return 0;
}
```
结果为打印
1148846080
1000.000000
由于不能直接用mov系列指令将立即数放入浮点数寄存器,需要从内存中转一下
汇编中存储常量的片段为

.LC0:
.long	1148846080

当然也可以用.float来定义单精度浮点数.上面片段改成

.float 1000.0

运行结果一样.对于double则可以用下面来验证.如果直接用long long会有高低32位的问题

```
#include <stdio.h>
int main()
{
double a=1000.0;
int* b=(int *)&a;
printf("%d %d\n",*b,*(b+1));
printf("%f\n",a);
return 0;
}
}
```
### 汇编指令

.file string（.app-file string）：用于告诉汇编其接下来的代码、指令，属于所跟的字符串表示的逻辑文件（也就是逻辑上它们是属于这个文件的），字符串可以用双引号包围，也可以没有，但是如果是空字符串，必须用双引号；

.text subsection：告诉汇编其将接下来的指令放入text段中subsection指定的子段中，如果subsection省略，则默认使用0号段；

.global symbol, .globl symbol：汇编器将symbol暴露给链接器；

.type int：用于指定符号表中符号的类型；

.section internal_name：将接下来的代码放入由name指定的段中；

.ascii "string"...：定义字符串数据；

.size：用于添加调试信息，并且这些个指令只在生成COFF格式的文件有效；

.ident：用于向目标文件中添加标签；

### 数据指令

.align abs-expr, abs-expr, abs-expr：将当前位置的存储区域填充到一个特定的大小，指令后面跟的三个表达式分别表示填充的字节数、填充的内容、需要跳过的最大字节数。后面两个表达式可以省略。

.asciz "string"...：指定字符串数据，与.ascii不同的是.asciz会在字符串末尾自动填充一个空字符；

.float flonums：定义浮点型数据；

.int expressions：定义整型数据；

.byte expressions：定义字节数据；

.hword expressions：定义存放与两个字节的数据；

.octa bignums：定义十六个字节的数据，octa的意思是8个字，因为定义一个字为两个字节，因此八个字是十六字节；

.quad bignums：定义八个字节的数据；

.string "str"：拷贝str到目标文件中，也就是定义字符串；

.word expressions：定义一个字长度的数据，具体的字节数以及大小端格式视具体架构而定；

.long expressions：与.int一样；

.def internal_name：为符号name定义调试信息，直到遇到.endef指令为止；

.desc symbol, abs-expression：为符号定义描述；

.double flonums：定义双精度浮点数；

### 行为指令


.abort：立即结束汇编操作；

.data subsection：将接下来的指令放入指定的数据子段，如果子段没有指定，则默认放入0号子段；

.eject：强制换页；

.if absolute expression：条件性的汇编接下来的指令，和一般的语言中的if分支类似，可以有.else, .elseif等指令与之匹配，并以.endif表示条件汇编结束；

.include "file"：将其他汇编文件添加进来，可以通过命令行-I选项指定搜索路径；

.line line-number：修改逻辑行号；

.linkonce [type]：指导链接器如何操作接下来的指令；

.macro [macname [macargs ...]]定义以段宏，并以.endm结束，也可以使用.exitm提前推出宏，定义后的宏可以与一般操作码一样使用；

.rept count：重复执行接下来的代码指定次，直到遇到.endr指令；

.set symbol, expression：为符号赋值；

.skip size , fill：条过size指定数量的字节后获取fill指定数量的字节空间;

.space size , fill与.skip一样；

.stabd, .stabn, .stabs：定义调试信息。

## 杂项

统计代码行数

git ls-files | xargs wc -l
