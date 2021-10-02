/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENTIFIER = 258,
     I_CONSTANT = 259,
     F_CONSTANT = 260,
     STRING_LITERAL = 261,
     FUNC_NAME = 262,
     SIZEOF = 263,
     PTR_OP = 264,
     INC_OP = 265,
     DEC_OP = 266,
     LEFT_OP = 267,
     RIGHT_OP = 268,
     LE_OP = 269,
     GE_OP = 270,
     EQ_OP = 271,
     NE_OP = 272,
     AND_OP = 273,
     OR_OP = 274,
     MUL_ASSIGN = 275,
     DIV_ASSIGN = 276,
     MOD_ASSIGN = 277,
     ADD_ASSIGN = 278,
     SUB_ASSIGN = 279,
     LEFT_ASSIGN = 280,
     RIGHT_ASSIGN = 281,
     AND_ASSIGN = 282,
     XOR_ASSIGN = 283,
     OR_ASSIGN = 284,
     TYPEDEF_NAME = 285,
     ENUMERATION_CONSTANT = 286,
     TYPEDEF = 287,
     EXTERN = 288,
     STATIC = 289,
     AUTO = 290,
     REGISTER = 291,
     INLINE = 292,
     CONST = 293,
     RESTRICT = 294,
     VOLATILE = 295,
     BOOL = 296,
     CHAR = 297,
     SHORT = 298,
     INT = 299,
     LONG = 300,
     SIGNED = 301,
     UNSIGNED = 302,
     FLOAT = 303,
     DOUBLE = 304,
     VOID = 305,
     COMPLEX = 306,
     IMAGINARY = 307,
     STRUCT = 308,
     UNION = 309,
     ENUM = 310,
     ELLIPSIS = 311,
     CASE = 312,
     DEFAULT = 313,
     IF = 314,
     ELSE = 315,
     SWITCH = 316,
     WHILE = 317,
     DO = 318,
     FOR = 319,
     GOTO = 320,
     CONTINUE = 321,
     BREAK = 322,
     RETURN = 323,
     ALIGNAS = 324,
     ALIGNOF = 325,
     ATOMIC = 326,
     GENERIC = 327,
     NORETURN = 328,
     STATIC_ASSERT = 329,
     THREAD_LOCAL = 330,
     SEMI_COLON = 331,
     COMMA = 332,
     COLON = 333,
     LEFT_BRACE = 334,
     RIGHT_BRACE = 335,
     LEFT_BRACKET = 336,
     RIGHT_BRACKET = 337,
     LEFT_PARENTHESIS = 338,
     RIGHT_PARENTHESIS = 339,
     OPERATOR_NOT = 340,
     BITNOT = 341,
     ADD = 342,
     SUB = 343,
     MUL = 344,
     DIV = 345,
     MOD = 346,
     ASSIGN = 347,
     GREATER_THAN = 348,
     LESS_THAN = 349,
     BITAND = 350,
     BITXOR = 351,
     BITOR = 352,
     QUESTION = 353,
     DOT = 354
   };
#endif
/* Tokens.  */
#define IDENTIFIER 258
#define I_CONSTANT 259
#define F_CONSTANT 260
#define STRING_LITERAL 261
#define FUNC_NAME 262
#define SIZEOF 263
#define PTR_OP 264
#define INC_OP 265
#define DEC_OP 266
#define LEFT_OP 267
#define RIGHT_OP 268
#define LE_OP 269
#define GE_OP 270
#define EQ_OP 271
#define NE_OP 272
#define AND_OP 273
#define OR_OP 274
#define MUL_ASSIGN 275
#define DIV_ASSIGN 276
#define MOD_ASSIGN 277
#define ADD_ASSIGN 278
#define SUB_ASSIGN 279
#define LEFT_ASSIGN 280
#define RIGHT_ASSIGN 281
#define AND_ASSIGN 282
#define XOR_ASSIGN 283
#define OR_ASSIGN 284
#define TYPEDEF_NAME 285
#define ENUMERATION_CONSTANT 286
#define TYPEDEF 287
#define EXTERN 288
#define STATIC 289
#define AUTO 290
#define REGISTER 291
#define INLINE 292
#define CONST 293
#define RESTRICT 294
#define VOLATILE 295
#define BOOL 296
#define CHAR 297
#define SHORT 298
#define INT 299
#define LONG 300
#define SIGNED 301
#define UNSIGNED 302
#define FLOAT 303
#define DOUBLE 304
#define VOID 305
#define COMPLEX 306
#define IMAGINARY 307
#define STRUCT 308
#define UNION 309
#define ENUM 310
#define ELLIPSIS 311
#define CASE 312
#define DEFAULT 313
#define IF 314
#define ELSE 315
#define SWITCH 316
#define WHILE 317
#define DO 318
#define FOR 319
#define GOTO 320
#define CONTINUE 321
#define BREAK 322
#define RETURN 323
#define ALIGNAS 324
#define ALIGNOF 325
#define ATOMIC 326
#define GENERIC 327
#define NORETURN 328
#define STATIC_ASSERT 329
#define THREAD_LOCAL 330
#define SEMI_COLON 331
#define COMMA 332
#define COLON 333
#define LEFT_BRACE 334
#define RIGHT_BRACE 335
#define LEFT_BRACKET 336
#define RIGHT_BRACKET 337
#define LEFT_PARENTHESIS 338
#define RIGHT_PARENTHESIS 339
#define OPERATOR_NOT 340
#define BITNOT 341
#define ADD 342
#define SUB 343
#define MUL 344
#define DIV 345
#define MOD 346
#define ASSIGN 347
#define GREATER_THAN 348
#define LESS_THAN 349
#define BITAND 350
#define BITXOR 351
#define BITOR 352
#define QUESTION 353
#define DOT 354




/* Copy the first part of user declarations.  */
#line 2 "tsc.y"

#include <stdio.h>
#include "parser.h"
#include "tsc.tab.hpp"
#define YYERROR_VERBOSE 1  
int yylex ();
extern char yytext[];
void yyerror(const char *s)
{
	printf("%s\n", s);
}  


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 317 "tsc.tab.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  67
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2469

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  102
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  78
/* YYNRULES -- Number of rules.  */
#define YYNRULES  275
/* YYNRULES -- Number of states.  */
#define YYNSTATES  480

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   354

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     100,     2,   101,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    13,    15,    17,    19,
      21,    23,    25,    27,    34,    36,    40,    44,    48,    50,
      55,    59,    64,    68,    72,    75,    78,    85,    93,    95,
      99,   101,   104,   107,   110,   113,   118,   123,   125,   127,
     129,   131,   133,   135,   137,   142,   144,   148,   152,   156,
     158,   162,   166,   168,   172,   176,   178,   182,   186,   190,
     194,   196,   200,   204,   206,   210,   212,   216,   218,   222,
     224,   228,   230,   234,   236,   242,   244,   248,   250,   252,
     254,   256,   258,   260,   262,   264,   266,   268,   270,   272,
     276,   278,   281,   285,   287,   290,   292,   295,   297,   300,
     302,   305,   307,   310,   312,   314,   318,   322,   324,   326,
     328,   330,   332,   334,   336,   338,   340,   342,   344,   346,
     348,   350,   352,   354,   356,   358,   360,   362,   364,   366,
     368,   373,   379,   382,   384,   386,   388,   391,   394,   398,
     400,   403,   405,   408,   410,   412,   416,   419,   423,   425,
     430,   436,   442,   449,   452,   454,   458,   462,   464,   469,
     471,   473,   475,   477,   479,   481,   486,   491,   494,   496,
     498,   502,   506,   511,   518,   524,   530,   537,   543,   548,
     553,   558,   562,   567,   571,   574,   577,   579,   581,   584,
     588,   590,   592,   596,   599,   602,   604,   606,   610,   613,
     615,   618,   620,   622,   626,   629,   633,   639,   644,   650,
     655,   659,   663,   667,   672,   679,   685,   691,   698,   703,
     708,   711,   715,   719,   724,   728,   733,   735,   738,   740,
     745,   749,   752,   754,   757,   761,   764,   772,   774,   776,
     778,   780,   782,   784,   788,   793,   797,   800,   804,   806,
     809,   811,   813,   815,   818,   826,   832,   838,   844,   852,
     859,   867,   874,   882,   886,   889,   892,   895,   899,   901,
     904,   906,   908,   913,   917,   919
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     176,     0,    -1,     3,    -1,   104,    -1,   106,    -1,    83,
     128,    84,    -1,   107,    -1,     4,    -1,     5,    -1,    31,
      -1,     3,    -1,     6,    -1,     7,    -1,    72,    83,   126,
      77,   108,    84,    -1,   109,    -1,   108,    77,   109,    -1,
     158,    78,   126,    -1,    58,    78,   126,    -1,   103,    -1,
     110,    81,   128,    82,    -1,   110,    83,    84,    -1,   110,
      83,   111,    84,    -1,   110,    99,     3,    -1,   110,     9,
       3,    -1,   110,    10,    -1,   110,    11,    -1,    83,   158,
      84,    79,   162,    80,    -1,    83,   158,    84,    79,   162,
      77,    80,    -1,   126,    -1,   111,    77,   126,    -1,   110,
      -1,    10,   112,    -1,    11,   112,    -1,   113,   114,    -1,
       8,   112,    -1,     8,    83,   158,    84,    -1,    70,    83,
     158,    84,    -1,    95,    -1,    89,    -1,    87,    -1,    88,
      -1,    86,    -1,    85,    -1,   112,    -1,    83,   158,    84,
     114,    -1,   114,    -1,   115,    89,   114,    -1,   115,    90,
     114,    -1,   115,    91,   114,    -1,   115,    -1,   116,    87,
     115,    -1,   116,    88,   115,    -1,   116,    -1,   117,    12,
     116,    -1,   117,    13,   116,    -1,   117,    -1,   118,   100,
     117,    -1,   118,   101,   117,    -1,   118,    14,   117,    -1,
     118,    15,   117,    -1,   118,    -1,   119,    16,   118,    -1,
     119,    17,   118,    -1,   119,    -1,   120,    95,   119,    -1,
     120,    -1,   121,    96,   120,    -1,   121,    -1,   122,    97,
     121,    -1,   122,    -1,   123,    18,   122,    -1,   123,    -1,
     124,    19,   123,    -1,   124,    -1,   124,    98,   128,    78,
     125,    -1,   125,    -1,   112,   127,   126,    -1,    92,    -1,
      20,    -1,    21,    -1,    22,    -1,    23,    -1,    24,    -1,
      25,    -1,    26,    -1,    27,    -1,    28,    -1,    29,    -1,
     126,    -1,   128,    77,   126,    -1,   125,    -1,   131,    76,
      -1,   131,   132,    76,    -1,   166,    -1,   134,   131,    -1,
     134,    -1,   135,   131,    -1,   135,    -1,   147,   131,    -1,
     147,    -1,   148,   131,    -1,   148,    -1,   149,   131,    -1,
     149,    -1,   133,    -1,   132,    77,   133,    -1,   150,    92,
     161,    -1,   150,    -1,    32,    -1,    33,    -1,    34,    -1,
      75,    -1,    35,    -1,    36,    -1,    50,    -1,    42,    -1,
      43,    -1,    44,    -1,    45,    -1,    48,    -1,    49,    -1,
      46,    -1,    47,    -1,    41,    -1,    51,    -1,    52,    -1,
     146,    -1,   136,    -1,   143,    -1,    30,    -1,   137,    79,
     138,    80,    -1,   137,     3,    79,   138,    80,    -1,   137,
       3,    -1,    53,    -1,    54,    -1,   139,    -1,   138,   139,
      -1,   140,    76,    -1,   140,   141,    76,    -1,   166,    -1,
     135,   140,    -1,   135,    -1,   147,   140,    -1,   147,    -1,
     142,    -1,   141,    77,   142,    -1,    78,   129,    -1,   150,
      78,   129,    -1,   150,    -1,    55,    79,   144,    80,    -1,
      55,    79,   144,    77,    80,    -1,    55,     3,    79,   144,
      80,    -1,    55,     3,    79,   144,    77,    80,    -1,    55,
       3,    -1,   145,    -1,   144,    77,   145,    -1,   105,    92,
     129,    -1,   105,    -1,    71,    83,   158,    84,    -1,    38,
      -1,    39,    -1,    40,    -1,    71,    -1,    37,    -1,    73,
      -1,    69,    83,   158,    84,    -1,    69,    83,   129,    84,
      -1,   152,   151,    -1,   151,    -1,     3,    -1,    83,   150,
      84,    -1,   151,    81,    82,    -1,   151,    81,    89,    82,
      -1,   151,    81,    34,   153,   126,    82,    -1,   151,    81,
      34,   126,    82,    -1,   151,    81,   153,    89,    82,    -1,
     151,    81,   153,    34,   126,    82,    -1,   151,    81,   153,
     126,    82,    -1,   151,    81,   153,    82,    -1,   151,    81,
     126,    82,    -1,   151,    83,   154,    84,    -1,   151,    83,
      84,    -1,   151,    83,   157,    84,    -1,    89,   153,   152,
      -1,    89,   153,    -1,    89,   152,    -1,    89,    -1,   147,
      -1,   153,   147,    -1,   155,    77,    56,    -1,   155,    -1,
     156,    -1,   155,    77,   156,    -1,   131,   150,    -1,   131,
     159,    -1,   131,    -1,     3,    -1,   157,    77,     3,    -1,
     140,   159,    -1,   140,    -1,   152,   160,    -1,   152,    -1,
     160,    -1,    83,   159,    84,    -1,    81,    82,    -1,    81,
      89,    82,    -1,    81,    34,   153,   126,    82,    -1,    81,
      34,   126,    82,    -1,    81,   153,    34,   126,    82,    -1,
      81,   153,   126,    82,    -1,    81,   153,    82,    -1,    81,
     126,    82,    -1,   160,    81,    82,    -1,   160,    81,    89,
      82,    -1,   160,    81,    34,   153,   126,    82,    -1,   160,
      81,    34,   126,    82,    -1,   160,    81,   153,   126,    82,
      -1,   160,    81,   153,    34,   126,    82,    -1,   160,    81,
     153,    82,    -1,   160,    81,   126,    82,    -1,    83,    84,
      -1,    83,   154,    84,    -1,   160,    83,    84,    -1,   160,
      83,   154,    84,    -1,    79,   162,    80,    -1,    79,   162,
      77,    80,    -1,   126,    -1,   163,   161,    -1,   161,    -1,
     162,    77,   163,   161,    -1,   162,    77,   161,    -1,   164,
      92,    -1,   165,    -1,   164,   165,    -1,    81,   129,    82,
      -1,    99,     3,    -1,    74,    83,   129,    77,     6,    84,
      76,    -1,   168,    -1,   169,    -1,   172,    -1,   173,    -1,
     174,    -1,   175,    -1,     3,    78,   167,    -1,    57,   129,
      78,   167,    -1,    58,    78,   167,    -1,    79,    80,    -1,
      79,   170,    80,    -1,   171,    -1,   170,   171,    -1,   130,
      -1,   167,    -1,    76,    -1,   128,    76,    -1,    59,    83,
     128,    84,   167,    60,   167,    -1,    59,    83,   128,    84,
     167,    -1,    61,    83,   128,    84,   167,    -1,    62,    83,
     128,    84,   167,    -1,    63,   167,    62,    83,   128,    84,
      76,    -1,    64,    83,   172,   172,    84,   167,    -1,    64,
      83,   172,   172,   128,    84,   167,    -1,    64,    83,   130,
     172,    84,   167,    -1,    64,    83,   130,   172,   128,    84,
     167,    -1,    65,     3,    76,    -1,    66,    76,    -1,    67,
      76,    -1,    68,    76,    -1,    68,   128,    76,    -1,   177,
      -1,   176,   177,    -1,   178,    -1,   130,    -1,   131,   150,
     179,   169,    -1,   131,   150,   169,    -1,   130,    -1,   179,
     130,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    42,    42,    43,    44,    45,    46,    50,    51,    52,
      56,    60,    61,    65,    69,    70,    74,    75,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    92,    93,
      97,    98,    99,   100,   101,   102,   103,   107,   108,   109,
     110,   111,   112,   116,   117,   121,   122,   123,   124,   128,
     129,   130,   134,   135,   136,   140,   141,   142,   143,   144,
     148,   149,   150,   154,   155,   159,   160,   164,   165,   169,
     170,   174,   175,   179,   180,   184,   185,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   203,   204,
     208,   212,   213,   214,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   231,   232,   236,   237,   241,   242,
     243,   244,   245,   246,   250,   251,   252,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     269,   270,   271,   275,   276,   280,   281,   285,   286,   287,
     291,   292,   293,   294,   298,   299,   303,   304,   305,   309,
     310,   311,   312,   313,   317,   318,   322,   323,   327,   331,
     332,   333,   334,   338,   339,   343,   344,   348,   349,   353,
     354,   355,   356,   357,   358,   359,   360,   361,   362,   363,
     364,   365,   366,   370,   371,   372,   373,   377,   378,   383,
     384,   388,   389,   393,   394,   395,   399,   400,   404,   405,
     409,   410,   411,   415,   416,   417,   418,   419,   420,   421,
     422,   423,   424,   425,   426,   427,   428,   429,   430,   431,
     432,   433,   434,   435,   439,   440,   441,   445,   446,   447,
     448,   452,   456,   457,   461,   462,   466,   470,   471,   472,
     473,   474,   475,   479,   480,   481,   485,   486,   490,   491,
     495,   496,   500,   501,   505,   506,   507,   511,   512,   513,
     514,   515,   516,   520,   521,   522,   523,   524,   528,   529,
     533,   534,   538,   539,   543,   544
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENTIFIER", "I_CONSTANT", "F_CONSTANT",
  "STRING_LITERAL", "FUNC_NAME", "SIZEOF", "PTR_OP", "INC_OP", "DEC_OP",
  "LEFT_OP", "RIGHT_OP", "LE_OP", "GE_OP", "EQ_OP", "NE_OP", "AND_OP",
  "OR_OP", "MUL_ASSIGN", "DIV_ASSIGN", "MOD_ASSIGN", "ADD_ASSIGN",
  "SUB_ASSIGN", "LEFT_ASSIGN", "RIGHT_ASSIGN", "AND_ASSIGN", "XOR_ASSIGN",
  "OR_ASSIGN", "TYPEDEF_NAME", "ENUMERATION_CONSTANT", "TYPEDEF", "EXTERN",
  "STATIC", "AUTO", "REGISTER", "INLINE", "CONST", "RESTRICT", "VOLATILE",
  "BOOL", "CHAR", "SHORT", "INT", "LONG", "SIGNED", "UNSIGNED", "FLOAT",
  "DOUBLE", "VOID", "COMPLEX", "IMAGINARY", "STRUCT", "UNION", "ENUM",
  "ELLIPSIS", "CASE", "DEFAULT", "IF", "ELSE", "SWITCH", "WHILE", "DO",
  "FOR", "GOTO", "CONTINUE", "BREAK", "RETURN", "ALIGNAS", "ALIGNOF",
  "ATOMIC", "GENERIC", "NORETURN", "STATIC_ASSERT", "THREAD_LOCAL",
  "SEMI_COLON", "COMMA", "COLON", "LEFT_BRACE", "RIGHT_BRACE",
  "LEFT_BRACKET", "RIGHT_BRACKET", "LEFT_PARENTHESIS", "RIGHT_PARENTHESIS",
  "OPERATOR_NOT", "BITNOT", "ADD", "SUB", "MUL", "DIV", "MOD", "ASSIGN",
  "GREATER_THAN", "LESS_THAN", "BITAND", "BITXOR", "BITOR", "QUESTION",
  "DOT", "'<'", "'>'", "$accept", "primary_expression", "constant",
  "enumeration_constant", "string", "generic_selection",
  "generic_assoc_list", "generic_association", "postfix_expression",
  "argument_expression_list", "unary_expression", "unary_operator",
  "cast_expression", "multiplicative_expression", "additive_expression",
  "shift_expression", "relational_expression", "equality_expression",
  "and_expression", "exclusive_or_expression", "inclusive_or_expression",
  "logical_and_expression", "logical_or_expression",
  "conditional_expression", "assignment_expression", "assignment_operator",
  "expression", "constant_expression", "declaration",
  "declaration_specifiers", "init_declarator_list", "init_declarator",
  "storage_class_specifier", "type_specifier", "struct_or_union_specifier",
  "struct_or_union", "struct_declaration_list", "struct_declaration",
  "specifier_qualifier_list", "struct_declarator_list",
  "struct_declarator", "enum_specifier", "enumerator_list", "enumerator",
  "atomic_type_specifier", "type_qualifier", "function_specifier",
  "alignment_specifier", "declarator", "direct_declarator", "pointer",
  "type_qualifier_list", "parameter_type_list", "parameter_list",
  "parameter_declaration", "identifier_list", "type_name",
  "abstract_declarator", "direct_abstract_declarator", "initializer",
  "initializer_list", "designation", "designator_list", "designator",
  "static_assert_declaration", "statement", "labeled_statement",
  "compound_statement", "block_item_list", "block_item",
  "expression_statement", "selection_statement", "iteration_statement",
  "jump_statement", "translation_unit", "external_declaration",
  "function_definition", "declaration_list", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
      60,    62
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   102,   103,   103,   103,   103,   103,   104,   104,   104,
     105,   106,   106,   107,   108,   108,   109,   109,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   111,   111,
     112,   112,   112,   112,   112,   112,   112,   113,   113,   113,
     113,   113,   113,   114,   114,   115,   115,   115,   115,   116,
     116,   116,   117,   117,   117,   118,   118,   118,   118,   118,
     119,   119,   119,   120,   120,   121,   121,   122,   122,   123,
     123,   124,   124,   125,   125,   126,   126,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   128,   128,
     129,   130,   130,   130,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   132,   132,   133,   133,   134,   134,
     134,   134,   134,   134,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     136,   136,   136,   137,   137,   138,   138,   139,   139,   139,
     140,   140,   140,   140,   141,   141,   142,   142,   142,   143,
     143,   143,   143,   143,   144,   144,   145,   145,   146,   147,
     147,   147,   147,   148,   148,   149,   149,   150,   150,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   151,   151,
     151,   151,   151,   152,   152,   152,   152,   153,   153,   154,
     154,   155,   155,   156,   156,   156,   157,   157,   158,   158,
     159,   159,   159,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   161,   161,   161,   162,   162,   162,
     162,   163,   164,   164,   165,   165,   166,   167,   167,   167,
     167,   167,   167,   168,   168,   168,   169,   169,   170,   170,
     171,   171,   172,   172,   173,   173,   173,   174,   174,   174,
     174,   174,   174,   175,   175,   175,   175,   175,   176,   176,
     177,   177,   178,   178,   179,   179
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     6,     1,     3,     3,     3,     1,     4,
       3,     4,     3,     3,     2,     2,     6,     7,     1,     3,
       1,     2,     2,     2,     2,     4,     4,     1,     1,     1,
       1,     1,     1,     1,     4,     1,     3,     3,     3,     1,
       3,     3,     1,     3,     3,     1,     3,     3,     3,     3,
       1,     3,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     5,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     2,     3,     1,     2,     1,     2,     1,     2,     1,
       2,     1,     2,     1,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     5,     2,     1,     1,     1,     2,     2,     3,     1,
       2,     1,     2,     1,     1,     3,     2,     3,     1,     4,
       5,     5,     6,     2,     1,     3,     3,     1,     4,     1,
       1,     1,     1,     1,     1,     4,     4,     2,     1,     1,
       3,     3,     4,     6,     5,     5,     6,     5,     4,     4,
       4,     3,     4,     3,     2,     2,     1,     1,     2,     3,
       1,     1,     3,     2,     2,     1,     1,     3,     2,     1,
       2,     1,     1,     3,     2,     3,     5,     4,     5,     4,
       3,     3,     3,     4,     6,     5,     5,     6,     4,     4,
       2,     3,     3,     4,     3,     4,     1,     2,     1,     4,
       3,     2,     1,     2,     3,     2,     7,     1,     1,     1,
       1,     1,     1,     3,     4,     3,     2,     3,     1,     2,
       1,     1,     1,     2,     7,     5,     5,     5,     7,     6,
       7,     6,     7,     3,     2,     2,     2,     3,     1,     2,
       1,     1,     4,     3,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,   129,   108,   109,   110,   112,   113,   163,   159,   160,
     161,   123,   115,   116,   117,   118,   121,   122,   119,   120,
     114,   124,   125,   133,   134,     0,     0,   162,   164,     0,
     111,   271,     0,    95,    97,   127,     0,   128,   126,    99,
     101,   103,    93,     0,   268,   270,   153,     0,     0,     0,
       0,   169,    91,     0,   186,     0,   104,   107,   168,     0,
      94,    96,   132,     0,    98,   100,   102,     1,   269,     0,
      10,   157,     0,   154,     2,     7,     8,    11,    12,     0,
       0,     0,     9,     0,     0,     0,    42,    41,    39,    40,
      38,    37,    18,     3,     4,     6,    30,    43,     0,    45,
      49,    52,    55,    60,    63,    65,    67,    69,    71,    73,
      90,     0,   141,   199,   143,     0,     0,     0,     0,   162,
     187,   185,   184,    92,     0,     0,     0,   274,     0,   273,
       0,     0,     0,   167,     0,     0,   135,     0,   139,     0,
       0,     0,   149,     0,    34,     0,    31,    32,     0,     0,
      43,    75,    88,     0,     0,     0,    24,    25,     0,     0,
       0,    33,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   166,   140,     0,     0,   201,   198,   202,   142,   165,
     158,     0,   170,   188,   183,   105,   107,     2,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   252,
     246,     0,   250,   251,   237,   238,     0,   248,   239,   240,
     241,   242,     0,   226,   106,   275,   272,     0,   171,    38,
       0,     0,   196,   181,   195,     0,   190,   191,     0,     0,
     130,   136,   137,     0,     0,   144,   148,     0,   151,   156,
     150,   155,     0,     0,     0,     0,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    77,     0,     0,     5,
       0,    23,     0,    20,     0,    28,    22,    46,    47,    48,
      50,    51,    53,    54,    58,    59,    56,    57,    61,    62,
      64,    66,    68,    70,    72,     0,     0,   204,    38,     0,
       0,   220,     0,     0,   200,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   264,   265,   266,
       0,   253,   247,   249,     0,     0,   228,     0,     0,     0,
     232,     0,     0,   172,   179,     0,   178,    38,     0,     0,
     193,   201,   194,   180,     0,     0,   182,   131,   146,   138,
       0,     0,   152,    35,     0,    36,     0,    76,    89,     0,
      44,    19,     0,    21,     0,     0,     0,   205,   211,     0,
     210,     0,   221,   203,     0,   212,    38,     0,     0,   222,
       0,     0,   243,     0,   245,     0,     0,     0,     0,     0,
       0,   263,   267,     0,   235,     0,   224,   227,   231,   233,
     174,     0,     0,   175,   177,   189,   192,   197,   145,   147,
       0,     0,    14,     0,     0,    29,    74,   207,     0,     0,
     209,     0,     0,   213,   219,     0,   218,     0,   223,   236,
     244,     0,     0,     0,     0,     0,     0,   234,   225,   230,
       0,   173,   176,     0,     0,    13,     0,     0,    26,   206,
     208,   215,     0,     0,   216,   255,   256,   257,     0,     0,
       0,     0,     0,   229,    17,    15,    16,    27,   214,   217,
       0,     0,   261,     0,   259,     0,   254,   258,   262,   260
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    92,    93,    71,    94,    95,   411,   412,    96,   274,
     150,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   151,   152,   267,   211,   111,    31,   128,
      55,    56,    33,    34,    35,    36,   135,   136,   113,   244,
     245,    37,    72,    73,    38,    39,    40,    41,   118,    58,
      59,   122,   302,   236,   237,   238,   413,   303,   187,   326,
     327,   328,   329,   330,    42,   213,   214,   215,   216,   217,
     218,   219,   220,   221,    43,    44,    45,   130
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -367
static const yytype_int16 yypact[] =
{
    2199,  -367,  -367,  -367,  -367,  -367,  -367,  -367,  -367,  -367,
    -367,  -367,  -367,  -367,  -367,  -367,  -367,  -367,  -367,  -367,
    -367,  -367,  -367,  -367,  -367,    35,   -56,   -39,  -367,     1,
    -367,  -367,    69,  2289,  2289,  -367,    45,  -367,  -367,  2289,
    2289,  2289,  -367,  1847,  -367,  -367,    14,    31,   902,  2398,
    1574,  -367,  -367,   118,   137,   146,  -367,  1893,   -30,    27,
    -367,  -367,    60,  2327,  -367,  -367,  -367,  -367,  -367,    31,
    -367,    67,    80,  -367,  -367,  -367,  -367,  -367,  -367,  1645,
    1667,  1667,  -367,    86,    90,   902,  -367,  -367,  -367,  -367,
    -367,  -367,  -367,  -367,  -367,  -367,    68,  -367,  1574,  -367,
     160,   174,   255,    32,    23,    91,    94,   130,   145,    39,
    -367,   152,  2398,   -29,  2398,   170,   175,   179,   185,  -367,
    -367,  -367,   137,  -367,   118,   623,  1415,  -367,    69,  -367,
    2151,  1048,  1791,   -30,  2327,  2059,  -367,    29,  -367,    88,
    1574,   133,  -367,   902,  -367,   902,  -367,  -367,  2398,  1574,
     218,  -367,  -367,   -41,   201,   260,  -367,  -367,  1574,  1092,
     283,  -367,  1574,  1574,  1574,  1574,  1574,  1574,  1574,  1574,
    1574,  1574,  1574,  1574,  1574,  1574,  1574,  1574,  1574,  1574,
    1574,  -367,  -367,  1070,  1954,   -12,  -367,   172,  -367,  -367,
    -367,   282,  -367,  -367,  -367,  -367,   203,   216,  1574,   220,
     213,   217,   221,   955,   222,   298,   227,   230,  1437,  -367,
    -367,   196,  -367,  -367,  -367,  -367,   716,  -367,  -367,  -367,
    -367,  -367,   509,  -367,  -367,  -367,  -367,  1393,  -367,   225,
     229,  1163,  -367,  -367,    89,   231,   232,  -367,    66,  2102,
    -367,  -367,  -367,  1574,   198,  -367,   235,   135,  -367,  -367,
    -367,  -367,   233,   236,   237,   247,  -367,  -367,  -367,  -367,
    -367,  -367,  -367,  -367,  -367,  -367,  -367,  1574,  1574,  -367,
    1530,  -367,   -49,  -367,    70,  -367,  -367,  -367,  -367,  -367,
     160,   160,   174,   174,   255,   255,   255,   255,    32,    32,
      23,    91,    94,   130,   145,   199,  1393,  -367,   249,   252,
    1185,  -367,   251,   253,   172,  1278,  2012,   256,   955,   261,
     955,  1574,  1574,  1574,   279,   809,   266,  -367,  -367,  -367,
     202,  -367,  -367,  -367,  1574,   340,  -367,   107,  1415,   -57,
    -367,   262,  1393,  -367,  -367,  1574,  -367,   263,   264,  1733,
    -367,    28,  -367,  -367,  2245,   344,  -367,  -367,  -367,  -367,
     131,  1574,  -367,   269,   269,  -367,  2364,  -367,  -367,   509,
    -367,  -367,  1574,  -367,  1574,   267,  1393,  -367,  -367,  1574,
    -367,   268,  -367,  -367,  1393,  -367,   275,   276,  1300,  -367,
     277,   286,  -367,   955,  -367,    78,    97,   105,   281,  1552,
    1552,  -367,  -367,   290,  -367,   465,  -367,  -367,  -367,  -367,
    -367,   291,   294,  -367,  -367,  -367,  -367,  -367,  -367,  -367,
     273,   108,  -367,   287,   122,  -367,  -367,  -367,   295,   296,
    -367,   297,  1393,  -367,  -367,  1574,  -367,   302,  -367,  -367,
    -367,   955,   955,   955,  1574,  1207,  1322,  -367,  -367,  -367,
    1415,  -367,  -367,  1574,  2364,  -367,  1574,   487,  -367,  -367,
    -367,  -367,   303,   305,  -367,   310,  -367,  -367,   121,   955,
     126,   955,   141,  -367,  -367,  -367,  -367,  -367,  -367,  -367,
     955,   304,  -367,   955,  -367,   955,  -367,  -367,  -367,  -367
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -367,  -367,  -367,  -367,  -367,  -367,  -367,   -55,  -367,  -367,
     288,  -367,   -33,   115,   123,    63,   109,   215,   212,   214,
     219,   223,  -367,   -37,    -3,  -367,   -82,   -32,   -45,    16,
    -367,   270,  -367,   -44,  -367,  -367,   259,  -126,   -18,  -367,
      48,  -367,   326,  -120,  -367,   -48,  -367,  -367,   -22,   -52,
     -40,  -108,  -110,  -367,    55,  -367,   -23,   -93,  -177,  -124,
      42,  -366,  -367,    74,   -46,   -50,  -367,    10,  -367,   188,
    -173,  -367,  -367,  -367,  -367,   362,  -367,  -367
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
     114,   114,   224,   153,   112,   112,   120,   133,   304,   241,
      57,   110,   127,   110,   121,   114,    32,   138,   117,   112,
     186,   251,   235,   231,   324,   115,   116,    48,   268,   440,
      51,    51,    51,   361,    70,   398,   268,   114,    46,   173,
     174,   112,   325,   269,    49,   137,   169,   170,    62,    60,
      61,   131,   183,   132,   184,    64,    65,    66,   179,    32,
      54,   153,   154,   153,   114,   161,   114,   129,   112,   183,
     112,   184,    51,   185,   193,   300,   272,   155,   156,   157,
     212,   440,   194,   120,    50,   225,   114,   114,   138,   138,
     112,   112,    51,    69,   182,   114,   188,   114,   295,   112,
     114,   112,   196,   110,   112,   242,   196,   243,   249,   183,
      53,   339,    53,   241,    47,   246,   137,   137,    54,   332,
     252,    51,   253,   223,    63,   254,   320,   251,   230,   277,
     278,   279,   171,   172,    51,   120,    70,   180,    70,   134,
     226,   342,   390,   345,   185,    52,   255,   362,   234,   158,
     346,   159,    53,   314,   363,   268,   275,   141,    54,   140,
     142,   110,   431,   178,   304,   247,   309,   160,   248,   148,
     183,   212,   339,   149,   268,     8,     9,    10,    54,   120,
     299,   432,   268,   193,   395,   444,   175,   396,   366,   433,
     176,   114,   445,   138,   341,   112,   380,   378,   268,   447,
     234,    53,   448,   268,   397,   471,   110,    54,   119,   243,
     473,   348,   340,   250,    53,   352,   435,   436,   268,   223,
      54,   137,   123,   124,   331,   475,    54,   177,   338,   385,
     386,   387,   284,   285,   286,   287,   181,   360,   256,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   120,   162,
     163,   164,   193,   305,   189,   306,   191,   120,   382,   190,
     384,   165,   166,   271,   357,   358,   422,   167,   168,   192,
     389,   439,   321,   268,   349,   350,   268,   364,   392,   268,
     280,   281,   288,   289,   193,   270,   276,   110,   307,   133,
     282,   283,   393,   365,   308,   126,   311,   371,   310,   341,
     312,   316,   377,   317,   313,   315,   318,   333,   114,   344,
     266,   334,   112,   351,   110,   343,   463,   353,   193,   409,
     354,   355,   234,   439,   356,   223,   120,   416,   246,   401,
     193,   367,   402,   430,   368,   372,    97,   373,    97,   383,
     381,   388,   391,   394,   400,   403,   404,   407,   359,   417,
     420,   443,   458,   460,   462,   234,   223,   423,   424,   415,
     234,   428,   429,   418,   434,   446,   419,   144,   146,   147,
     470,   421,   437,   441,   193,   427,   442,   449,   450,   451,
     477,   455,   456,   457,   454,   468,    97,   469,   291,   465,
     290,   292,   223,   239,   195,   139,   114,   293,   408,   406,
     112,   414,   294,   399,   323,    68,     0,     0,     0,   472,
       0,   474,     0,     0,     0,     0,     0,     0,     0,   452,
     476,     0,   453,   478,     0,   479,     0,     0,    97,     0,
       0,     0,     0,     0,     0,     0,     0,   223,     0,     0,
     464,     0,     0,   466,   223,     0,     0,     0,     0,     0,
      97,    97,    97,    97,    97,    97,    97,    97,    97,    97,
      97,    97,    97,    97,    97,    97,    97,    97,    74,    75,
      76,    77,    78,    79,     0,    80,    81,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    97,     0,     0,     0,
      74,    75,    76,    77,    78,    79,    82,    80,    81,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    74,    75,    76,    77,    78,    79,    82,    80,
      81,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    97,     0,     0,     0,    83,     0,    84,     0,     0,
      82,     0,     0,     0,   222,   438,   324,     0,    85,     0,
      86,    87,    88,    89,    90,     0,     0,    83,    97,    84,
      91,     0,     0,     0,   325,     0,   222,   467,   324,     0,
      85,     0,    86,    87,    88,    89,    90,     0,     0,    83,
       0,    84,    91,     0,     0,     0,   325,     0,   222,     0,
     324,     0,    85,     0,    86,    87,    88,    89,    90,     0,
       0,     0,     0,     0,    91,     0,     0,     0,   325,     0,
       0,     0,    97,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   197,    75,    76,    77,
      78,    79,     0,    80,    81,     0,     0,     0,     0,    97,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    97,     1,    82,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,     0,
     198,   199,   200,     0,   201,   202,   203,   204,   205,   206,
     207,   208,    26,    83,    27,    84,    28,    29,    30,   209,
       0,     0,   125,   210,     0,     0,    85,     0,    86,    87,
      88,    89,    90,     0,     0,     0,     0,     0,    91,   197,
      75,    76,    77,    78,    79,     0,    80,    81,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     1,    82,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,     0,   198,   199,   200,     0,   201,   202,   203,
     204,   205,   206,   207,   208,    26,    83,    27,    84,    28,
      29,    30,   209,     0,     0,   125,   322,     0,     0,    85,
       0,    86,    87,    88,    89,    90,     0,     0,     0,     0,
       0,    91,    74,    75,    76,    77,    78,    79,     0,    80,
      81,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     1,
      82,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    26,    83,
      27,    84,    28,    29,    30,   209,     0,     0,     0,     0,
       0,     0,    85,     0,    86,    87,    88,    89,    90,     0,
       0,     0,     0,     0,    91,    74,    75,    76,    77,    78,
      79,     0,    80,    81,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     1,    82,     0,     0,     0,     0,     0,     0,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,   197,    75,
      76,    77,    78,    79,     0,    80,    81,     0,     0,     0,
       0,     0,    83,    27,    84,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    85,    82,    86,    87,    88,
      89,    90,     0,     0,     0,     0,     0,    91,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   198,   199,   200,     0,   201,   202,   203,   204,
     205,   206,   207,   208,     0,    83,     0,    84,     0,     0,
       0,   209,     0,     0,   125,     0,     0,     0,    85,     0,
      86,    87,    88,    89,    90,     0,     0,     0,     0,     0,
      91,    74,    75,    76,    77,    78,    79,     0,    80,    81,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    74,    75,    76,    77,    78,    79,    82,
      80,    81,   227,     0,     0,     0,     8,     9,    10,     0,
       0,     0,     0,     0,     0,    74,    75,    76,    77,    78,
      79,    82,    80,    81,   296,     0,     0,     0,     8,     9,
      10,     0,     0,     0,     0,     0,     0,     0,    83,   119,
      84,     0,     0,    82,     0,     0,     0,     0,     0,     0,
     228,    85,     0,    86,    87,    88,    89,   229,     0,     0,
      83,   119,    84,    91,     0,     0,     0,     0,     0,     0,
       0,     0,   297,    85,     0,    86,    87,    88,    89,   298,
       0,     0,    83,     0,    84,    91,    74,    75,    76,    77,
      78,    79,     0,    80,    81,    85,   273,    86,    87,    88,
      89,    90,     0,     0,     0,     0,     0,    91,    74,    75,
      76,    77,    78,    79,    82,    80,    81,   335,     0,     0,
       0,     8,     9,    10,     0,     0,     0,     0,     0,     0,
      74,    75,    76,    77,    78,    79,    82,    80,    81,   369,
       0,     0,     0,     8,     9,    10,     0,     0,     0,     0,
       0,     0,     0,    83,   119,    84,     0,     0,    82,     0,
       0,     0,     0,     0,     0,   336,    85,     0,    86,    87,
      88,    89,   337,     0,     0,    83,   119,    84,    91,     0,
       0,     0,     0,     0,     0,     0,     0,   370,    85,     0,
      86,    87,    88,    89,    90,     0,     0,    83,     0,    84,
      91,    74,    75,    76,    77,    78,    79,     0,    80,    81,
      85,   459,    86,    87,    88,    89,    90,     0,     0,     0,
       0,     0,    91,    74,    75,    76,    77,    78,    79,    82,
      80,    81,   374,     0,     0,     0,     8,     9,    10,     0,
       0,     0,     0,     0,     0,    74,    75,    76,    77,    78,
      79,    82,    80,    81,   425,     0,     0,     0,     8,     9,
      10,     0,     0,     0,     0,     0,     0,     0,    83,   119,
      84,     0,     0,    82,     0,     0,     0,     0,     0,     0,
     375,    85,     0,    86,    87,    88,    89,   376,     0,     0,
      83,   119,    84,    91,     0,     0,     0,     0,     0,     0,
       0,     0,   426,    85,     0,    86,    87,    88,    89,    90,
       0,     0,    83,     0,    84,    91,    74,    75,    76,    77,
      78,    79,     0,    80,    81,    85,   461,    86,    87,    88,
      89,    90,     0,     0,     0,     0,     0,    91,    74,    75,
      76,    77,    78,    79,    82,    80,    81,     0,     0,     0,
       0,     8,     9,    10,     0,     0,     0,     0,     0,     0,
      74,    75,    76,    77,    78,    79,    82,    80,    81,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    83,   119,    84,     0,     0,    82,     0,
       0,     0,     0,     0,     0,     0,    85,     0,    86,    87,
      88,    89,    90,     0,     0,    83,     0,    84,    91,     0,
       0,     0,     0,     0,   222,     0,     0,     0,    85,     0,
      86,    87,    88,    89,    90,     0,     0,    83,     0,    84,
      91,     0,     0,   319,     0,     0,     0,     0,     0,     0,
      85,     0,    86,    87,    88,    89,    90,     0,     0,     0,
       0,     0,    91,    74,    75,    76,    77,    78,    79,     0,
      80,    81,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    74,    75,    76,    77,    78,
      79,    82,    80,    81,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    74,    75,    76,
      77,    78,    79,    82,    80,    81,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      83,     0,    84,     0,     0,    82,     0,     0,     0,   359,
       0,     0,     0,    85,     0,    86,    87,    88,    89,    90,
       0,     0,    83,     0,    84,    91,     0,     0,   209,     0,
       0,     0,     0,     0,     0,    85,     0,    86,    87,    88,
      89,    90,     0,     0,    83,     0,    84,    91,    74,    75,
      76,    77,    78,    79,     0,    80,    81,    85,     0,    86,
      87,    88,    89,    90,     0,     0,     0,     0,     0,    91,
      74,    75,    76,    77,    78,    79,    82,    80,    81,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    82,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    83,     0,    84,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   143,     0,
      86,    87,    88,    89,    90,     0,    51,    83,     0,    84,
      91,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     145,     0,    86,    87,    88,    89,    90,     0,     0,     0,
       0,     0,    91,     1,     0,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,     0,
       0,     0,     0,     0,   232,     0,     0,     0,     0,     0,
       0,     0,    26,     0,    27,     0,    28,     0,    30,     0,
       0,     0,     0,     0,   183,     0,   339,   301,     0,     0,
       0,     1,    54,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    67,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      26,     0,    27,     0,    28,     0,    30,     0,     0,     0,
       0,     0,     0,     0,     0,   233,     0,     1,     0,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    26,     0,    27,     0,
      28,    29,    30,     1,     0,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    26,     0,    27,     0,    28,    29,    30,     0,
       0,     0,   125,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     1,   126,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    26,     0,    27,     0,    28,     0,    30,
       0,     0,     0,     0,     0,   183,     0,   184,   301,     0,
       0,     0,     1,    54,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    26,     0,    27,     0,    28,     0,    30,     0,     1,
       0,     0,     0,     0,     0,     0,   379,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      27,     0,     1,    29,     0,     0,     0,     0,     0,   240,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    27,     0,     0,    29,     0,     0,     0,
       0,     1,   347,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      26,     0,    27,     0,    28,    29,    30,     0,     0,     1,
     125,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    26,     0,
      27,     0,    28,    29,    30,     1,     0,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,   405,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    26,     0,    27,     0,    28,     1,
      30,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     1,    26,     0,
      27,     0,    28,     0,    30,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     1,     0,     0,     0,    27,     0,
       0,    29,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
       0,     0,   410,     0,     0,     0,     0,     0,     1,     0,
       0,     0,     0,     0,     0,    27,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    27
};

static const yytype_int16 yycheck[] =
{
      48,    49,   126,    85,    48,    49,    54,    59,   185,   135,
      32,    48,    57,    50,    54,    63,     0,    63,    50,    63,
     113,   141,   132,   131,    81,    48,    49,    83,    77,   395,
       3,     3,     3,    82,     3,    92,    77,    85,     3,    16,
      17,    85,    99,    84,    83,    63,    14,    15,     3,    33,
      34,    81,    81,    83,    83,    39,    40,    41,    19,    43,
      89,   143,    85,   145,   112,    98,   114,    57,   112,    81,
     114,    83,     3,   113,   122,   183,   158,     9,    10,    11,
     125,   447,   122,   131,    83,   130,   134,   135,   134,   135,
     134,   135,     3,    79,   112,   143,   114,   145,   180,   143,
     148,   145,   124,   140,   148,    76,   128,    78,   140,    81,
      83,    83,    83,   239,    79,   137,   134,   135,    89,   227,
     143,     3,   145,   126,    79,   148,   208,   247,   131,   162,
     163,   164,   100,   101,     3,   183,     3,    98,     3,    79,
     130,   234,   315,    77,   184,    76,   149,    77,   132,    81,
      84,    83,    83,   203,    84,    77,   159,    77,    89,    92,
      80,   198,    84,    18,   341,    77,   198,    99,    80,    83,
      81,   216,    83,    83,    77,    38,    39,    40,    89,   227,
     183,    84,    77,   231,    77,    77,    95,    80,   296,    84,
      96,   239,    84,   239,   234,   239,   306,   305,    77,    77,
     184,    83,    80,    77,   328,    84,   243,    89,    71,    78,
      84,   243,   234,    80,    83,    80,   389,   390,    77,   222,
      89,   239,    76,    77,   227,    84,    89,    97,   231,   311,
     312,   313,   169,   170,   171,   172,    84,   270,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,   296,    89,
      90,    91,   300,    81,    84,    83,    77,   305,   308,    84,
     310,    87,    88,     3,   267,   268,   374,    12,    13,    84,
     315,   395,    76,    77,    76,    77,    77,    78,    76,    77,
     165,   166,   173,   174,   332,    84,     3,   324,     6,   341,
     167,   168,   324,   296,    78,    92,    83,   300,    78,   339,
      83,     3,   305,    76,    83,    83,    76,    82,   356,    77,
      92,    82,   356,    78,   351,    84,   440,    84,   366,   351,
      84,    84,   306,   447,    77,   328,   374,   364,   350,   332,
     378,    82,   335,   383,    82,    84,    48,    84,    50,    78,
      84,    62,    76,     3,    82,    82,    82,     3,    79,    82,
      82,    78,   434,   435,   436,   339,   359,    82,    82,   362,
     344,    84,    76,   366,    83,    78,   369,    79,    80,    81,
      60,   374,    82,    82,   422,   378,    82,    82,    82,    82,
      76,   431,   432,   433,    82,    82,    98,    82,   176,   444,
     175,   177,   395,   134,   124,    69,   444,   178,   350,   344,
     444,   359,   179,   329,   216,    43,    -1,    -1,    -1,   459,
      -1,   461,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   422,
     470,    -1,   425,   473,    -1,   475,    -1,    -1,   140,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   440,    -1,    -1,
     443,    -1,    -1,   446,   447,    -1,    -1,    -1,    -1,    -1,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,     3,     4,
       5,     6,     7,     8,    -1,    10,    11,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   198,    -1,    -1,    -1,
       3,     4,     5,     6,     7,     8,    31,    10,    11,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,     8,    31,    10,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   243,    -1,    -1,    -1,    70,    -1,    72,    -1,    -1,
      31,    -1,    -1,    -1,    79,    80,    81,    -1,    83,    -1,
      85,    86,    87,    88,    89,    -1,    -1,    70,   270,    72,
      95,    -1,    -1,    -1,    99,    -1,    79,    80,    81,    -1,
      83,    -1,    85,    86,    87,    88,    89,    -1,    -1,    70,
      -1,    72,    95,    -1,    -1,    -1,    99,    -1,    79,    -1,
      81,    -1,    83,    -1,    85,    86,    87,    88,    89,    -1,
      -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,    99,    -1,
      -1,    -1,   324,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,     8,    -1,    10,    11,    -1,    -1,    -1,    -1,   351,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   364,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    -1,
      57,    58,    59,    -1,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      -1,    -1,    79,    80,    -1,    -1,    83,    -1,    85,    86,
      87,    88,    89,    -1,    -1,    -1,    -1,    -1,    95,     3,
       4,     5,     6,     7,     8,    -1,    10,    11,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    -1,    57,    58,    59,    -1,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    -1,    -1,    79,    80,    -1,    -1,    83,
      -1,    85,    86,    87,    88,    89,    -1,    -1,    -1,    -1,
      -1,    95,     3,     4,     5,     6,     7,     8,    -1,    10,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    70,
      71,    72,    73,    74,    75,    76,    -1,    -1,    -1,    -1,
      -1,    -1,    83,    -1,    85,    86,    87,    88,    89,    -1,
      -1,    -1,    -1,    -1,    95,     3,     4,     5,     6,     7,
       8,    -1,    10,    11,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    30,    31,    -1,    -1,    -1,    -1,    -1,    -1,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,     3,     4,
       5,     6,     7,     8,    -1,    10,    11,    -1,    -1,    -1,
      -1,    -1,    70,    71,    72,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    31,    85,    86,    87,
      88,    89,    -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    57,    58,    59,    -1,    61,    62,    63,    64,
      65,    66,    67,    68,    -1,    70,    -1,    72,    -1,    -1,
      -1,    76,    -1,    -1,    79,    -1,    -1,    -1,    83,    -1,
      85,    86,    87,    88,    89,    -1,    -1,    -1,    -1,    -1,
      95,     3,     4,     5,     6,     7,     8,    -1,    10,    11,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,     6,     7,     8,    31,
      10,    11,    34,    -1,    -1,    -1,    38,    39,    40,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,    31,    10,    11,    34,    -1,    -1,    -1,    38,    39,
      40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,    71,
      72,    -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,    -1,
      82,    83,    -1,    85,    86,    87,    88,    89,    -1,    -1,
      70,    71,    72,    95,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    82,    83,    -1,    85,    86,    87,    88,    89,
      -1,    -1,    70,    -1,    72,    95,     3,     4,     5,     6,
       7,     8,    -1,    10,    11,    83,    84,    85,    86,    87,
      88,    89,    -1,    -1,    -1,    -1,    -1,    95,     3,     4,
       5,     6,     7,     8,    31,    10,    11,    34,    -1,    -1,
      -1,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,     8,    31,    10,    11,    34,
      -1,    -1,    -1,    38,    39,    40,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    70,    71,    72,    -1,    -1,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    82,    83,    -1,    85,    86,
      87,    88,    89,    -1,    -1,    70,    71,    72,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    83,    -1,
      85,    86,    87,    88,    89,    -1,    -1,    70,    -1,    72,
      95,     3,     4,     5,     6,     7,     8,    -1,    10,    11,
      83,    84,    85,    86,    87,    88,    89,    -1,    -1,    -1,
      -1,    -1,    95,     3,     4,     5,     6,     7,     8,    31,
      10,    11,    34,    -1,    -1,    -1,    38,    39,    40,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,    31,    10,    11,    34,    -1,    -1,    -1,    38,    39,
      40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,    71,
      72,    -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,    -1,
      82,    83,    -1,    85,    86,    87,    88,    89,    -1,    -1,
      70,    71,    72,    95,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    82,    83,    -1,    85,    86,    87,    88,    89,
      -1,    -1,    70,    -1,    72,    95,     3,     4,     5,     6,
       7,     8,    -1,    10,    11,    83,    84,    85,    86,    87,
      88,    89,    -1,    -1,    -1,    -1,    -1,    95,     3,     4,
       5,     6,     7,     8,    31,    10,    11,    -1,    -1,    -1,
      -1,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,     8,    31,    10,    11,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    70,    71,    72,    -1,    -1,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    83,    -1,    85,    86,
      87,    88,    89,    -1,    -1,    70,    -1,    72,    95,    -1,
      -1,    -1,    -1,    -1,    79,    -1,    -1,    -1,    83,    -1,
      85,    86,    87,    88,    89,    -1,    -1,    70,    -1,    72,
      95,    -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,
      83,    -1,    85,    86,    87,    88,    89,    -1,    -1,    -1,
      -1,    -1,    95,     3,     4,     5,     6,     7,     8,    -1,
      10,    11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,    31,    10,    11,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,     8,    31,    10,    11,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      70,    -1,    72,    -1,    -1,    31,    -1,    -1,    -1,    79,
      -1,    -1,    -1,    83,    -1,    85,    86,    87,    88,    89,
      -1,    -1,    70,    -1,    72,    95,    -1,    -1,    76,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    -1,    85,    86,    87,
      88,    89,    -1,    -1,    70,    -1,    72,    95,     3,     4,
       5,     6,     7,     8,    -1,    10,    11,    83,    -1,    85,
      86,    87,    88,    89,    -1,    -1,    -1,    -1,    -1,    95,
       3,     4,     5,     6,     7,     8,    31,    10,    11,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    70,    -1,    72,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    -1,
      85,    86,    87,    88,    89,    -1,     3,    70,    -1,    72,
      95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      83,    -1,    85,    86,    87,    88,    89,    -1,    -1,    -1,
      -1,    -1,    95,    30,    -1,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    -1,
      -1,    -1,    -1,    -1,     3,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    69,    -1,    71,    -1,    73,    -1,    75,    -1,
      -1,    -1,    -1,    -1,    81,    -1,    83,    84,    -1,    -1,
      -1,    30,    89,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,     0,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      69,    -1,    71,    -1,    73,    -1,    75,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    -1,    30,    -1,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    69,    -1,    71,    -1,
      73,    74,    75,    30,    -1,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    69,    -1,    71,    -1,    73,    74,    75,    -1,
      -1,    -1,    79,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    30,    92,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    69,    -1,    71,    -1,    73,    -1,    75,
      -1,    -1,    -1,    -1,    -1,    81,    -1,    83,    84,    -1,
      -1,    -1,    30,    89,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    69,    -1,    71,    -1,    73,    -1,    75,    -1,    30,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      71,    -1,    30,    74,    -1,    -1,    -1,    -1,    -1,    80,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    71,    -1,    -1,    74,    -1,    -1,    -1,
      -1,    30,    80,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      69,    -1,    71,    -1,    73,    74,    75,    -1,    -1,    30,
      79,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    -1,
      71,    -1,    73,    74,    75,    30,    -1,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    69,    -1,    71,    -1,    73,    30,
      75,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    30,    69,    -1,
      71,    -1,    73,    -1,    75,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    30,    -1,    -1,    -1,    71,    -1,
      -1,    74,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,    30,    -1,
      -1,    -1,    -1,    -1,    -1,    71,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    71
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    30,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    69,    71,    73,    74,
      75,   130,   131,   134,   135,   136,   137,   143,   146,   147,
     148,   149,   166,   176,   177,   178,     3,    79,    83,    83,
      83,     3,    76,    83,    89,   132,   133,   150,   151,   152,
     131,   131,     3,    79,   131,   131,   131,     0,   177,    79,
       3,   105,   144,   145,     3,     4,     5,     6,     7,     8,
      10,    11,    31,    70,    72,    83,    85,    86,    87,    88,
      89,    95,   103,   104,   106,   107,   110,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   129,   135,   140,   147,   158,   158,   129,   150,    71,
     147,   152,   153,    76,    77,    79,    92,   130,   131,   169,
     179,    81,    83,   151,    79,   138,   139,   140,   166,   144,
      92,    77,    80,    83,   112,    83,   112,   112,    83,    83,
     112,   125,   126,   128,   158,     9,    10,    11,    81,    83,
      99,   114,    89,    90,    91,    87,    88,    12,    13,    14,
      15,   100,   101,    16,    17,    95,    96,    97,    18,    19,
      98,    84,   140,    81,    83,   152,   159,   160,   140,    84,
      84,    77,    84,   147,   152,   133,   150,     3,    57,    58,
      59,    61,    62,    63,    64,    65,    66,    67,    68,    76,
      80,   128,   130,   167,   168,   169,   170,   171,   172,   173,
     174,   175,    79,   126,   161,   130,   169,    34,    82,    89,
     126,   153,     3,    84,   131,   154,   155,   156,   157,   138,
      80,   139,    76,    78,   141,   142,   150,    77,    80,   129,
      80,   145,   158,   158,   158,   126,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    92,   127,    77,    84,
      84,     3,   128,    84,   111,   126,     3,   114,   114,   114,
     115,   115,   116,   116,   117,   117,   117,   117,   118,   118,
     119,   120,   121,   122,   123,   128,    34,    82,    89,   126,
     153,    84,   154,   159,   160,    81,    83,     6,    78,   129,
      78,    83,    83,    83,   167,    83,     3,    76,    76,    76,
     128,    76,    80,   171,    81,    99,   161,   162,   163,   164,
     165,   126,   153,    82,    82,    34,    82,    89,   126,    83,
     150,   152,   159,    84,    77,    77,    84,    80,   129,    76,
      77,    78,    80,    84,    84,    84,    77,   126,   126,    79,
     114,    82,    77,    84,    78,   126,   153,    82,    82,    34,
      82,   126,    84,    84,    34,    82,    89,   126,   153,    84,
     154,    84,   167,    78,   167,   128,   128,   128,    62,   130,
     172,    76,    76,   129,     3,    77,    80,   161,    92,   165,
      82,   126,   126,    82,    82,    56,   156,     3,   142,   129,
      58,   108,   109,   158,   162,   126,   125,    82,   126,   126,
      82,   126,   153,    82,    82,    34,    82,   126,    84,    76,
     167,    84,    84,    84,    83,   172,   172,    82,    80,   161,
     163,    82,    82,    78,    77,    84,    78,    77,    80,    82,
      82,    82,   126,   126,    82,   167,   167,   167,   128,    84,
     128,    84,   128,   161,   126,   109,   126,    80,    82,    82,
      60,    84,   167,    84,   167,    84,   167,    76,   167,   167
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
      
/* Line 1267 of yacc.c.  */
#line 2403 "tsc.tab.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 547 "tsc.y"


