
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         MOF_parse
#define yylex           MOF_lex
#define yyerror         MOF_error
#define yylval          MOF_lval
#define yychar          MOF_char
#define yydebug         MOF_debug
#define yynerrs         MOF_nerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "cimple/tools/libmof/MOF.y"


#include <cassert>
#include "MOF_Types.h"
#include "REF_Parser.h"

extern int MOF_lex();

static void MOF_trace(const char* str);



/* Line 189 of yacc.c  */
#line 94 "cimple/tools/libmof/MOF_Yacc.cpp"

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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOK_ALIAS_IDENT = 258,
     TOK_ANY = 259,
     TOK_AS = 260,
     TOK_ASSOCIATION = 261,
     TOK_BOOLEAN = 262,
     TOK_CHAR16 = 263,
     TOK_CHAR_VALUE = 264,
     TOK_CLASS = 265,
     TOK_CLOSE_BRACE = 266,
     TOK_CLOSE_BRACKET = 267,
     TOK_CLOSE_PAREN = 268,
     TOK_COLON = 269,
     TOK_COMMA = 270,
     TOK_DATETIME = 271,
     TOK_DISABLEOVERRIDE = 272,
     TOK_ENABLEOVERRIDE = 273,
     TOK_EQUAL = 274,
     TOK_FLAVOR = 275,
     TOK_IDENT = 276,
     TOK_INDICATION = 277,
     TOK_INSTANCE = 278,
     TOK_METHOD = 279,
     TOK_NULL_VALUE = 280,
     TOK_OF = 281,
     TOK_OPEN_BRACE = 282,
     TOK_OPEN_BRACKET = 283,
     TOK_OPEN_PAREN = 284,
     TOK_PARAMETER = 285,
     TOK_PRAGMA = 286,
     TOK_PROPERTY = 287,
     TOK_QUALIFIER = 288,
     TOK_REAL32 = 289,
     TOK_REAL64 = 290,
     TOK_REAL_VALUE = 291,
     TOK_REF = 292,
     TOK_REFERENCE = 293,
     TOK_RESTRICTED = 294,
     TOK_SCHEMA = 295,
     TOK_SCOPE = 296,
     TOK_SEMICOLON = 297,
     TOK_SINT16 = 298,
     TOK_SINT32 = 299,
     TOK_SINT64 = 300,
     TOK_SINT8 = 301,
     TOK_STRING = 302,
     TOK_STRING_VALUE = 303,
     TOK_TOSUBCLASS = 304,
     TOK_TRANSLATABLE = 305,
     TOK_BOOL_VALUE = 306,
     TOK_INT_VALUE = 307,
     TOK_UINT16 = 308,
     TOK_UINT32 = 309,
     TOK_UINT64 = 310,
     TOK_UINT8 = 311
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 14 "cimple/tools/libmof/MOF.y"

    MOF_char16 char_value;
    struct MOF_String_Literal
    {
        char* raw;
        char* escaped;
    }
    string_literal;
    char* string_value;
    MOF_sint64 int_value;
    int bool_value;
    double real_value;
    MOF_mask flavor;
    MOF_mask scope;

    class MOF_Class_Decl* class_decl;
    class MOF_Qualifier_Decl* qual_decl;
    class MOF_Class_Decl* struct_decl;
    class MOF_Feature* feature;
    class MOF_Literal* literal;
    class MOF_Instance_Decl* inst_decl;
    class MOF_Property* prop;
    class MOF_Property_Decl* prop_decl;
    class MOF_Qualifier* qual;
    class MOF_Reference_Decl* ref_decl;
    class MOF_Parameter* param;
    class MOF_Method_Decl* method_decl;
    class MOF_Object_Reference* obj_ref;

    struct _ref_init
    {
      char* alias;
      class MOF_Object_Reference* obj_ref;
    }
    ref_init;

    struct _decl_init
    {
        int array_index;
        class MOF_Literal* initializer;
    }
    decl_init;



/* Line 214 of yacc.c  */
#line 232 "cimple/tools/libmof/MOF_Yacc.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 244 "cimple/tools/libmof/MOF_Yacc.cpp"

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
# if YYENABLE_NLS
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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  31
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   242

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  57
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  66
/* YYNRULES -- Number of rules.  */
#define YYNRULES  144
/* YYNRULES -- Number of states.  */
#define YYNSTATES  222

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   311

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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     8,    10,    13,    15,    17,
      19,    21,    27,    29,    31,    33,    35,    37,    39,    41,
      45,    52,    54,    56,    60,    62,    65,    66,    72,    74,
      78,    80,    82,    84,    86,    88,    90,    92,    94,    96,
     102,   103,   105,   109,   111,   113,   115,   117,   119,   123,
     125,   129,   133,   137,   139,   140,   143,   144,   146,   149,
     153,   158,   163,   167,   170,   171,   174,   175,   176,   179,
     181,   183,   185,   189,   194,   197,   199,   201,   205,   207,
     210,   211,   215,   220,   223,   226,   227,   229,   231,   235,
     240,   243,   247,   250,   252,   256,   259,   263,   266,   268,
     269,   271,   273,   277,   282,   287,   291,   294,   295,   297,
     300,   302,   305,   310,   315,   318,   320,   323,   327,   329,
     331,   333,   335,   337,   339,   341,   343,   345,   347,   349,
     351,   353,   355,   357,   359,   363,   365,   369,   371,   373,
     375,   377,   379,   381,   383
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      58,     0,    -1,    59,    -1,    -1,    60,    -1,    61,    -1,
      60,    61,    -1,    62,    -1,    83,    -1,    66,    -1,   107,
      -1,    31,    63,    29,    64,    13,    -1,    21,    -1,   122,
      -1,    21,    -1,    40,    -1,     6,    -1,    22,    -1,    38,
      -1,    33,    67,    42,    -1,    65,    14,   117,    68,    71,
      74,    -1,    69,    -1,    70,    -1,   116,    19,   119,    -1,
     116,    -1,    19,   121,    -1,    -1,    15,    41,    29,    72,
      13,    -1,    73,    -1,    72,    15,    73,    -1,    40,    -1,
      10,    -1,     6,    -1,    22,    -1,    32,    -1,    38,    -1,
      24,    -1,    30,    -1,     4,    -1,    15,    20,    29,    75,
      13,    -1,    -1,    76,    -1,    75,    15,    76,    -1,    18,
      -1,    17,    -1,    39,    -1,    49,    -1,    50,    -1,    28,
      78,    12,    -1,    79,    -1,    78,    15,    79,    -1,    65,
      80,    81,    -1,    29,   121,    13,    -1,   119,    -1,    -1,
      14,    82,    -1,    -1,    76,    -1,    82,    76,    -1,    84,
      85,    42,    -1,    77,    84,    85,    42,    -1,    10,    21,
      86,    87,    -1,    27,    88,    11,    -1,     5,     3,    -1,
      -1,    14,    21,    -1,    -1,    -1,    88,    89,    -1,    90,
      -1,    95,    -1,    99,    -1,    91,    92,    42,    -1,    77,
      91,    92,    42,    -1,   117,    65,    -1,    93,    -1,    94,
      -1,   116,    19,   119,    -1,   116,    -1,    19,   121,    -1,
      -1,    96,    97,    42,    -1,    77,    96,    97,    42,    -1,
     114,    65,    -1,    19,    98,    -1,    -1,     3,    -1,   115,
      -1,   100,   101,    42,    -1,    77,   100,   101,    42,    -1,
     117,    65,    -1,    29,   102,    13,    -1,    29,    13,    -1,
     103,    -1,   102,    15,   103,    -1,   104,   105,    -1,    77,
     104,   105,    -1,   106,    65,    -1,   116,    -1,    -1,   117,
      -1,   114,    -1,   108,   109,    42,    -1,    77,   108,   109,
      42,    -1,    23,    26,    21,   110,    -1,    27,   111,    11,
      -1,     5,     3,    -1,    -1,   112,    -1,   111,   112,    -1,
     113,    -1,    77,   113,    -1,    21,    19,   118,    42,    -1,
      21,    19,     3,    42,    -1,    21,    37,    -1,   122,    -1,
      28,    12,    -1,    28,    52,    12,    -1,    56,    -1,    46,
      -1,    53,    -1,    43,    -1,    54,    -1,    44,    -1,    55,
      -1,    45,    -1,    34,    -1,    35,    -1,     8,    -1,    47,
      -1,     7,    -1,    16,    -1,   121,    -1,   119,    -1,    27,
     120,    11,    -1,   121,    -1,   120,    15,   121,    -1,    52,
      -1,    36,    -1,     9,    -1,    51,    -1,   122,    -1,    25,
      -1,    48,    -1,   122,    48,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   214,   214,   221,   224,   231,   235,   242,   247,   252,
     257,   273,   280,   287,   307,   313,   319,   325,   331,   348,
     356,   371,   376,   384,   390,   399,   406,   414,   422,   427,
     435,   440,   445,   450,   455,   460,   465,   470,   475,   483,
     489,   496,   501,   510,   515,   520,   525,   530,   546,   554,
     560,   570,   582,   587,   593,   600,   606,   613,   618,   639,
     646,   656,   668,   676,   682,   689,   695,   703,   707,   719,
     724,   729,   745,   752,   764,   776,   781,   789,   795,   804,
     811,   827,   836,   848,   859,   865,   873,   884,   901,   908,
     919,   931,   936,   944,   949,   966,   972,   983,   992,   998,
    1006,  1013,  1031,  1038,  1049,  1060,  1068,  1074,  1089,  1094,
    1103,  1109,  1119,  1128,  1149,  1157,  1190,  1196,  1218,  1223,
    1228,  1233,  1238,  1243,  1248,  1253,  1258,  1263,  1268,  1273,
    1278,  1283,  1299,  1304,  1312,  1320,  1325,  1342,  1350,  1358,
    1366,  1374,  1383,  1401,  1406
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TOK_ALIAS_IDENT", "TOK_ANY", "TOK_AS",
  "TOK_ASSOCIATION", "TOK_BOOLEAN", "TOK_CHAR16", "TOK_CHAR_VALUE",
  "TOK_CLASS", "TOK_CLOSE_BRACE", "TOK_CLOSE_BRACKET", "TOK_CLOSE_PAREN",
  "TOK_COLON", "TOK_COMMA", "TOK_DATETIME", "TOK_DISABLEOVERRIDE",
  "TOK_ENABLEOVERRIDE", "TOK_EQUAL", "TOK_FLAVOR", "TOK_IDENT",
  "TOK_INDICATION", "TOK_INSTANCE", "TOK_METHOD", "TOK_NULL_VALUE",
  "TOK_OF", "TOK_OPEN_BRACE", "TOK_OPEN_BRACKET", "TOK_OPEN_PAREN",
  "TOK_PARAMETER", "TOK_PRAGMA", "TOK_PROPERTY", "TOK_QUALIFIER",
  "TOK_REAL32", "TOK_REAL64", "TOK_REAL_VALUE", "TOK_REF", "TOK_REFERENCE",
  "TOK_RESTRICTED", "TOK_SCHEMA", "TOK_SCOPE", "TOK_SEMICOLON",
  "TOK_SINT16", "TOK_SINT32", "TOK_SINT64", "TOK_SINT8", "TOK_STRING",
  "TOK_STRING_VALUE", "TOK_TOSUBCLASS", "TOK_TRANSLATABLE",
  "TOK_BOOL_VALUE", "TOK_INT_VALUE", "TOK_UINT16", "TOK_UINT32",
  "TOK_UINT64", "TOK_UINT8", "$accept", "start", "MOF_specification",
  "MOF_productions", "MOF_production", "compiler_directive", "pragma_name",
  "pragma_param", "name", "qual_decl", "qual_decl_body", "qual_decl_init",
  "qual_decl_array_init", "qual_decl_scalar_init", "qual_decl_scope",
  "scope_list", "scope", "qual_decl_flavor", "flavor_list", "flavor",
  "qual_list", "qual_list_body", "qual", "qual_param", "qual_flavor",
  "qual_flavor_list", "class_decl", "class_head", "class_body",
  "class_alias", "super_class_name", "feature_list", "feature",
  "prop_decl", "prop_decl_head", "prop_decl_body", "prop_decl_array_init",
  "prop_decl_scalar_init", "ref_decl", "ref_head", "ref_body", "ref_init",
  "method_decl", "method_head", "method_body", "param_list", "param",
  "param_head", "param_body", "param_data_type", "inst_decl", "inst_head",
  "inst_body", "inst_alias", "prop_list", "prop", "prop_body",
  "object_ref", "obj_ref", "array_index", "data_type", "init",
  "array_init", "scalar_init_list", "literal", "string_literal", 0
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
     305,   306,   307,   308,   309,   310,   311
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    57,    58,    59,    59,    60,    60,    61,    61,    61,
      61,    62,    63,    64,    65,    65,    65,    65,    65,    66,
      67,    68,    68,    69,    69,    70,    70,    71,    72,    72,
      73,    73,    73,    73,    73,    73,    73,    73,    73,    74,
      74,    75,    75,    76,    76,    76,    76,    76,    77,    78,
      78,    79,    80,    80,    80,    81,    81,    82,    82,    83,
      83,    84,    85,    86,    86,    87,    87,    88,    88,    89,
      89,    89,    90,    90,    91,    92,    92,    93,    93,    94,
      94,    95,    95,    96,    97,    97,    98,    98,    99,    99,
     100,   101,   101,   102,   102,   103,   103,   104,   105,   105,
     106,   106,   107,   107,   108,   109,   110,   110,   111,   111,
     112,   112,   113,   113,   114,   115,   116,   116,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   118,   118,   119,   120,   120,   121,   121,   121,
     121,   121,   121,   122,   122
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     1,     1,     2,     1,     1,     1,
       1,     5,     1,     1,     1,     1,     1,     1,     1,     3,
       6,     1,     1,     3,     1,     2,     0,     5,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     5,
       0,     1,     3,     1,     1,     1,     1,     1,     3,     1,
       3,     3,     3,     1,     0,     2,     0,     1,     2,     3,
       4,     4,     3,     2,     0,     2,     0,     0,     2,     1,
       1,     1,     3,     4,     2,     1,     1,     3,     1,     2,
       0,     3,     4,     2,     2,     0,     1,     1,     3,     4,
       2,     3,     2,     1,     3,     2,     3,     2,     1,     0,
       1,     1,     3,     4,     4,     3,     2,     0,     1,     2,
       1,     2,     4,     4,     2,     1,     2,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     0,     0,     0,     0,     0,     2,     4,     5,
       7,     9,     0,     8,     0,    10,     0,    64,     0,    16,
      14,    17,    18,    15,    54,     0,    49,    12,     0,     0,
       0,     1,     6,     0,     0,    67,     0,     0,     0,     0,
      66,   107,     0,     0,    56,    53,    48,     0,     0,     0,
      19,     0,     0,     0,    59,     0,     0,     0,   108,   110,
     102,    63,     0,    61,     0,   104,   139,   142,   138,   143,
     140,   137,     0,   135,   141,     0,     0,    51,    50,     0,
      13,   130,   128,   131,   126,   127,   121,   123,   125,   119,
     129,   120,   122,   124,   118,    26,    60,   103,    62,     0,
       0,    68,    69,    80,    70,    85,    71,     0,     0,     0,
       0,   111,   105,   109,    65,   106,   134,     0,   144,    52,
      44,    43,    45,    46,    47,    57,    55,    11,     0,     0,
       0,    21,    22,    24,   114,    80,    85,     0,     0,     0,
      75,    76,    78,     0,     0,     0,     0,    83,    74,     0,
       0,   133,   132,   136,    58,    25,   116,     0,     0,    40,
       0,     0,     0,     0,    79,    72,     0,    86,    84,    87,
     115,    81,    92,     0,     0,    93,    99,     0,   101,   100,
      88,   113,   112,   117,     0,     0,    20,    23,    73,    82,
      89,    77,    99,    91,     0,    95,    98,    97,     0,     0,
      96,    94,    38,    32,    31,    33,    36,    37,    34,    35,
      30,     0,    28,     0,    27,     0,     0,    41,    29,    39,
       0,    42
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     6,     7,     8,     9,    10,    28,    79,    24,    11,
      30,   130,   131,   132,   159,   211,   212,   186,   216,   125,
      12,    25,    26,    44,    77,   126,    13,    14,    36,    40,
      63,    53,   101,   102,   103,   139,   140,   141,   104,   105,
     144,   168,   106,   107,   146,   174,   175,   176,   195,   177,
      15,    16,    38,    65,    57,    58,    59,   178,   169,   142,
     179,   150,    45,    72,    73,    74
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -120
static const yytype_int16 yypact[] =
{
      10,     8,    24,   108,    21,   108,    75,  -120,    10,  -120,
    -120,  -120,     1,  -120,    42,  -120,    49,    81,    72,  -120,
    -120,  -120,  -120,  -120,   -13,    69,  -120,  -120,    62,    93,
      66,  -120,  -120,    42,    49,  -120,    71,    -6,    73,   109,
      97,   111,    70,    70,   105,  -120,  -120,   108,    78,   146,
    -120,    83,    85,     2,  -120,   101,   107,    57,  -120,  -120,
    -120,  -120,   110,  -120,   134,  -120,  -120,  -120,  -120,  -120,
    -120,  -120,    17,  -120,    92,   128,   165,  -120,  -120,   136,
      92,  -120,  -120,  -120,  -120,  -120,  -120,  -120,  -120,  -120,
    -120,  -120,  -120,  -120,  -120,    12,  -120,  -120,  -120,   113,
      18,  -120,  -120,    16,  -120,   132,  -120,   126,   108,   108,
     170,  -120,  -120,  -120,  -120,  -120,  -120,    70,  -120,  -120,
    -120,  -120,  -120,  -120,  -120,  -120,   165,  -120,    70,    15,
     141,  -120,  -120,   138,  -120,    16,   132,   126,    70,   118,
    -120,  -120,   142,     3,   121,    89,   122,  -120,   139,   125,
     127,  -120,  -120,  -120,  -120,  -120,  -120,   158,   130,   173,
     167,   154,   156,   161,  -120,  -120,   167,  -120,  -120,  -120,
      92,  -120,  -120,    18,    67,  -120,   180,   108,  -120,  -120,
    -120,  -120,  -120,  -120,   181,   189,  -120,  -120,  -120,  -120,
    -120,  -120,   180,  -120,   131,  -120,  -120,  -120,   201,   183,
    -120,  -120,  -120,  -120,  -120,  -120,  -120,  -120,  -120,  -120,
    -120,    74,  -120,   165,  -120,   201,    77,  -120,  -120,  -120,
     165,  -120
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -120,  -120,  -120,  -120,   205,  -120,  -120,  -120,    -5,  -120,
    -120,  -120,  -120,  -120,  -120,  -120,     4,  -120,  -120,  -119,
     -36,  -120,   169,  -120,  -120,  -120,  -120,   208,   184,  -120,
    -120,  -120,  -120,  -120,   124,    91,  -120,  -120,  -120,   129,
      94,  -120,  -120,   135,    90,  -120,    34,    59,    44,  -120,
    -120,   222,   203,  -120,  -120,   185,   182,   -34,  -120,   -93,
     -41,  -120,  -106,  -120,   -40,   -43
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -91
static const yytype_int16 yytable[] =
{
      29,    56,   133,    75,   151,    80,   167,   154,    95,    81,
      82,     1,   109,    98,    42,    55,    43,   100,    83,   108,
       1,    56,     3,    99,     2,    81,    82,   156,   116,    17,
       3,   128,   117,     2,    83,   138,    84,    85,     3,    99,
     129,     4,    27,     5,   129,    86,    87,    88,    89,    90,
      18,    69,    84,    85,   187,    91,    92,    93,    94,   109,
     191,    86,    87,    88,    89,    90,   108,   157,   112,    35,
     152,    91,    92,    93,    94,    31,    37,   153,    55,    66,
     193,    46,   194,   196,    47,     3,    39,   214,   155,   215,
     219,    48,   220,    41,   217,    67,    81,    82,   164,   196,
     170,   221,   172,   147,   148,    83,    68,    49,    50,   173,
      99,    62,    61,    54,    19,    60,    64,     3,    69,    76,
     110,    70,    71,    84,    85,    96,    69,    97,    55,    20,
      21,   114,    86,    87,    88,    89,    90,   115,    81,    82,
     118,   119,    91,    92,    93,    94,    22,    83,    23,   127,
     134,   143,    99,    81,    82,   145,   158,   160,   173,     3,
     165,   166,    83,   171,   180,    84,    85,   181,   -90,   182,
     183,   184,   197,   149,    86,    87,    88,    89,    90,    66,
      84,    85,   120,   121,    91,    92,    93,    94,   185,    86,
      87,    88,    89,    90,    42,    67,   188,    42,   189,    91,
      92,    93,    94,   190,   122,   202,    68,   203,   129,   199,
     198,   204,   213,    32,   123,   124,    78,    51,    69,   218,
      33,    70,    71,   205,   135,   206,   161,   163,   201,   136,
     162,   207,   192,   208,    34,   137,   200,    52,   111,   209,
       0,   210,   113
};

static const yytype_int16 yycheck[] =
{
       5,    37,    95,    43,   110,    48,     3,   126,    49,     7,
       8,    10,    53,    11,    27,    21,    29,    53,    16,    53,
      10,    57,    28,    21,    23,     7,     8,    12,    11,    21,
      28,    19,    15,    23,    16,    19,    34,    35,    28,    21,
      28,    31,    21,    33,    28,    43,    44,    45,    46,    47,
      26,    48,    34,    35,   160,    53,    54,    55,    56,   100,
     166,    43,    44,    45,    46,    47,   100,    52,    11,    27,
     110,    53,    54,    55,    56,     0,    27,   117,    21,     9,
      13,    12,    15,   176,    15,    28,     5,    13,   128,    15,
      13,    29,    15,    21,   213,    25,     7,     8,   138,   192,
     143,   220,    13,   108,   109,    16,    36,    14,    42,   145,
      21,    14,     3,    42,     6,    42,     5,    28,    48,    14,
      19,    51,    52,    34,    35,    42,    48,    42,    21,    21,
      22,    21,    43,    44,    45,    46,    47,     3,     7,     8,
      48,    13,    53,    54,    55,    56,    38,    16,    40,    13,
      37,    19,    21,     7,     8,    29,    15,    19,   194,    28,
      42,    19,    16,    42,    42,    34,    35,    42,    29,    42,
      12,    41,   177,     3,    43,    44,    45,    46,    47,     9,
      34,    35,    17,    18,    53,    54,    55,    56,    15,    43,
      44,    45,    46,    47,    27,    25,    42,    27,    42,    53,
      54,    55,    56,    42,    39,     4,    36,     6,    28,    20,
      29,    10,    29,     8,    49,    50,    47,    33,    48,   215,
      12,    51,    52,    22,   100,    24,   135,   137,   194,   100,
     136,    30,   173,    32,    12,   100,   192,    34,    56,    38,
      -1,    40,    57
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    10,    23,    28,    31,    33,    58,    59,    60,    61,
      62,    66,    77,    83,    84,   107,   108,    21,    26,     6,
      21,    22,    38,    40,    65,    78,    79,    21,    63,    65,
      67,     0,    61,    84,   108,    27,    85,    27,   109,     5,
      86,    21,    27,    29,    80,   119,    12,    15,    29,    14,
      42,    85,   109,    88,    42,    21,    77,   111,   112,   113,
      42,     3,    14,    87,     5,   110,     9,    25,    36,    48,
      51,    52,   120,   121,   122,   121,    14,    81,    79,    64,
     122,     7,     8,    16,    34,    35,    43,    44,    45,    46,
      47,    53,    54,    55,    56,   117,    42,    42,    11,    21,
      77,    89,    90,    91,    95,    96,    99,   100,   114,   117,
      19,   113,    11,   112,    21,     3,    11,    15,    48,    13,
      17,    18,    39,    49,    50,    76,    82,    13,    19,    28,
      68,    69,    70,   116,    37,    91,    96,   100,    19,    92,
      93,    94,   116,    19,    97,    29,   101,    65,    65,     3,
     118,   119,   121,   121,    76,   121,    12,    52,    15,    71,
      19,    92,    97,   101,   121,    42,    19,     3,    98,   115,
     122,    42,    13,    77,   102,   103,   104,   106,   114,   117,
      42,    42,    42,    12,    41,    15,    74,   119,    42,    42,
      42,   119,   104,    13,    15,   105,   116,    65,    29,    20,
     105,   103,     4,     6,    10,    22,    24,    30,    32,    38,
      40,    72,    73,    29,    13,    15,    75,    76,    73,    13,
      15,    76
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
# if YYLTYPE_IS_TRIVIAL
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
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
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
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
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
        case 2:

/* Line 1455 of yacc.c  */
#line 215 "cimple/tools/libmof/MOF.y"
    {
        /* printf("start rule reached\n"); */
    ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 221 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("MOF_specification:1");
    ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 225 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("MOF_specification:2");
    ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 232 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("MOF_productions:1");
    ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 236 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("MOF_productions:2");
    ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 243 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("MOF_production:1");
        // MOF_warning_printf("pragma ignored");
    ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 248 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("MOF_production:3");
        MOF_Class_Decl::handle((yyvsp[(1) - (1)].class_decl));
    ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 253 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("MOF_production:4");
        MOF_Qualifier_Decl::handle((yyvsp[(1) - (1)].qual_decl));
    ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 258 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("MOF_production:5");
        MOF_Instance_Decl::handle((yyvsp[(1) - (1)].inst_decl));
    ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 274 "cimple/tools/libmof/MOF.y"
    {
        MOF_Pragma::handle((yyvsp[(2) - (5)].string_value), (yyvsp[(4) - (5)].string_literal).raw);
    ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 281 "cimple/tools/libmof/MOF.y"
    {
        (yyval.string_value) = (yyvsp[(1) - (1)].string_value);
    ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 288 "cimple/tools/libmof/MOF.y"
    {
        (yyval.string_literal) = (yyvsp[(1) - (1)].string_literal);
    ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 308 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("name:1");
        /* heap string */
        (yyval.string_value) = (yyvsp[(1) - (1)].string_value);
    ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 314 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("name:2");
        (yyval.string_value) = strdup((yyvsp[(1) - (1)].string_value));
        MOF_ASSERT((yyval.string_value) != NULL);
    ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 320 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("name:3");
        (yyval.string_value) = strdup((yyvsp[(1) - (1)].string_value));
        MOF_ASSERT((yyval.string_value) != NULL);
    ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 326 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("name:4");
        (yyval.string_value) = strdup((yyvsp[(1) - (1)].string_value));
        MOF_ASSERT((yyval.string_value) != NULL);
    ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 332 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("name:5");
        (yyval.string_value) = strdup((yyvsp[(1) - (1)].string_value));
        MOF_ASSERT((yyval.string_value) != NULL);
    ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 349 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_decl:1");
        (yyval.qual_decl) = (yyvsp[(2) - (3)].qual_decl);
    ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 357 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_decl_body:1");
        (yyval.qual_decl) = new MOF_Qualifier_Decl();
        MOF_ASSERT((yyval.qual_decl) != NULL);
        (yyval.qual_decl)->name = (yyvsp[(1) - (6)].string_value);
        (yyval.qual_decl)->data_type = (int)(yyvsp[(3) - (6)].int_value);
        (yyval.qual_decl)->array_index = (yyvsp[(4) - (6)].decl_init).array_index;
        (yyval.qual_decl)->initializer = (yyvsp[(4) - (6)].decl_init).initializer;
        (yyval.qual_decl)->scope = (yyvsp[(5) - (6)].scope);
        (yyval.qual_decl)->flavor = MOF_Flavor::fixup((yyvsp[(6) - (6)].flavor), true);
    ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 372 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_decl_init:1");
        (yyval.decl_init) = (yyvsp[(1) - (1)].decl_init);
    ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 377 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_decl_init:2");
        (yyval.decl_init) = (yyvsp[(1) - (1)].decl_init);
    ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 385 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_decl_array_init:1");
        (yyval.decl_init).array_index = (int)(yyvsp[(1) - (3)].int_value);
        (yyval.decl_init).initializer = (yyvsp[(3) - (3)].literal);
    ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 391 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_decl_array_init:2");
        (yyval.decl_init).array_index = (int)(yyvsp[(1) - (1)].int_value);
        (yyval.decl_init).initializer = NULL;
    ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 400 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_decl_scalar_init:1");
        (yyval.decl_init).array_index = 0;
        (yyval.decl_init).initializer = (yyvsp[(2) - (2)].literal);
    ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 406 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_decl_scalar_init:2");
        (yyval.decl_init).array_index = 0;
        (yyval.decl_init).initializer = NULL;
    ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 415 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_decl_scope:1");
        (yyval.scope) = (yyvsp[(4) - (5)].scope);
    ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 423 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("scope_list:1");
        (yyval.scope) = (yyvsp[(1) - (1)].scope);
    ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 428 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("scope_list:2");
        (yyval.scope) |= (yyvsp[(3) - (3)].scope);
    ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 436 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("scope:1");
        (yyval.scope) = MOF_SCOPE_SCHEMA;
    ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 441 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("scope:2");
        (yyval.scope) = MOF_SCOPE_CLASS;
    ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 446 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("scope:3");
        (yyval.scope) = MOF_SCOPE_ASSOCIATION;
    ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 451 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("scope:4");
        (yyval.scope) = MOF_SCOPE_INDICATION;
    ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 456 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("scope:5");
        (yyval.scope) = MOF_SCOPE_PROPERTY;
    ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 461 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("scope:6");
        (yyval.scope) = MOF_SCOPE_REFERENCE;
    ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 466 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("scope:7");
        (yyval.scope) = MOF_SCOPE_METHOD;
    ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 471 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("scope:8");
        (yyval.scope) = MOF_SCOPE_PARAMETER;
    ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 476 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("scope:9");
        (yyval.scope) = MOF_SCOPE_ANY;
    ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 484 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_decl_flavor:1");
        (yyval.flavor) = (yyvsp[(4) - (5)].flavor);
    ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 489 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_decl_flavor:2");
        (yyval.flavor) = 0;
    ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 497 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("flavor_list:1");
        (yyval.flavor) = (yyvsp[(1) - (1)].flavor);
    ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 502 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("flavor_list:2");
        (yyval.flavor) = (yyvsp[(1) - (3)].flavor);
        (yyval.flavor) |= (yyvsp[(3) - (3)].flavor);
    ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 511 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("flavor:1");
        (yyval.flavor) = MOF_FLAVOR_ENABLEOVERRIDE;
    ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 516 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("flavor:2");
        (yyval.flavor) = MOF_FLAVOR_DISABLEOVERRIDE;
    ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 521 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("flavor:3");
        (yyval.flavor) = MOF_FLAVOR_RESTRICTED;
    ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 526 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("flavor:4");
        (yyval.flavor) = MOF_FLAVOR_TOSUBCLASS;
    ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 531 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("flavor:5");
        (yyval.flavor) = MOF_FLAVOR_TRANSLATABLE;
    ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 547 "cimple/tools/libmof/MOF.y"
    {
        (yyval.qual) = (yyvsp[(2) - (3)].qual);
        MOF_trace("qual_list:1");
    ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 555 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_list_body:1");
        (yyvsp[(1) - (1)].qual)->validate();
        (yyval.qual) = (yyvsp[(1) - (1)].qual);
    ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 561 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_list_body:2");
        (yyvsp[(3) - (3)].qual)->validate();
        (yyvsp[(1) - (3)].qual)->append((yyvsp[(3) - (3)].qual));
        (yyval.qual) = (yyvsp[(1) - (3)].qual);
    ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 571 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual:1");
        (yyval.qual) = new MOF_Qualifier();
        MOF_ASSERT((yyval.qual) != NULL);
        (yyval.qual)->name = (yyvsp[(1) - (3)].string_value);
        (yyval.qual)->params = (yyvsp[(2) - (3)].literal);
        (yyval.qual)->flavor = MOF_Flavor::fixup((yyvsp[(3) - (3)].flavor), false);
    ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 583 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_param:1");
        (yyval.literal) = (yyvsp[(2) - (3)].literal);
    ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 588 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_param:2");
        (yyval.literal) = (yyvsp[(1) - (1)].literal);
    ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 593 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_param:3");
        (yyval.literal) = NULL;
    ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 601 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_flavor:1");
        (yyval.flavor) = (yyvsp[(2) - (2)].flavor);
    ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 606 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_flavor:2");
        (yyval.flavor) = 0;
    ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 614 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_flavor_list:1");
        (yyval.flavor) = (yyvsp[(1) - (1)].flavor);
    ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 619 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("qual_flavor_list:2");
        (yyval.flavor) = (yyvsp[(1) - (2)].flavor);
        (yyval.flavor) |= (yyvsp[(2) - (2)].flavor);
    ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 640 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("class_decl:1");
        (yyval.class_decl) = (yyvsp[(1) - (3)].class_decl);
        (yyval.class_decl)->qualifiers = NULL;
        (yyval.class_decl)->features = (yyvsp[(2) - (3)].feature);
    ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 647 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("class_decl:2");
        (yyval.class_decl) = (yyvsp[(2) - (4)].class_decl);
        (yyval.class_decl)->qualifiers = (yyvsp[(1) - (4)].qual);
        (yyval.class_decl)->features = (yyvsp[(3) - (4)].feature);
    ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 657 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("class_head:1");
        (yyval.class_decl) = new MOF_Class_Decl();
        MOF_ASSERT((yyval.class_decl) != NULL);
        (yyval.class_decl)->name = (yyvsp[(2) - (4)].string_value);
        (yyval.class_decl)->alias = (yyvsp[(3) - (4)].string_value);
        (yyval.class_decl)->super_class_name = (yyvsp[(4) - (4)].string_value);
    ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 669 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("class_body:1");
        (yyval.feature) = (yyvsp[(2) - (3)].feature);
    ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 677 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("class_alias:1");
        (yyval.string_value) = (yyvsp[(2) - (2)].string_value);
    ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 682 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("class_alias:2");
        (yyval.string_value) = NULL;
    ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 690 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("super_class_name:1");
        (yyval.string_value) = (yyvsp[(2) - (2)].string_value);
    ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 695 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("super_class_name:2");
        (yyval.string_value) = NULL;
    ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 703 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("feature_list:1");
        (yyval.feature) = NULL;
    ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 708 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("feature_list:2");

        if ((yyvsp[(1) - (2)].feature))
            (yyvsp[(1) - (2)].feature)->append((yyvsp[(2) - (2)].feature));
        else
            (yyval.feature) = (yyvsp[(2) - (2)].feature);
    ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 720 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("feature:1");
        (yyval.feature) = (MOF_Feature*)(yyvsp[(1) - (1)].prop_decl);
    ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 725 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("feature:2");
        (yyval.feature) = (MOF_Feature*)(yyvsp[(1) - (1)].ref_decl);
    ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 730 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("feature:3");
        (yyval.feature) = (MOF_Feature*)(yyvsp[(1) - (1)].method_decl);
    ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 746 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("prop_decl:1");
        (yyval.prop_decl) = (yyvsp[(1) - (3)].prop_decl);
        (yyval.prop_decl)->array_index = (yyvsp[(2) - (3)].decl_init).array_index;
        (yyval.prop_decl)->initializer = (yyvsp[(2) - (3)].decl_init).initializer;
    ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 753 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("prop_decl:2");
        (yyvsp[(1) - (4)].qual)->validate_list(MOF_SCOPE_PROPERTY);
        (yyval.prop_decl) = (yyvsp[(2) - (4)].prop_decl);
        (yyval.prop_decl)->qualifiers = (yyvsp[(1) - (4)].qual);
        (yyval.prop_decl)->array_index = (yyvsp[(3) - (4)].decl_init).array_index;
        (yyval.prop_decl)->initializer = (yyvsp[(3) - (4)].decl_init).initializer;
    ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 765 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("prop_decl_head:1");
        (yyval.prop_decl) = new MOF_Property_Decl();
        MOF_ASSERT((yyval.prop_decl) != NULL);
        (yyval.prop_decl)->type = MOF_FEATURE_PROP;
        (yyval.prop_decl)->name = (yyvsp[(2) - (2)].string_value);
        (yyval.prop_decl)->data_type = (int)(yyvsp[(1) - (2)].int_value);
    ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 777 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("prop_decl_body:1");
        (yyval.decl_init) = (yyvsp[(1) - (1)].decl_init);
    ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 782 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("prop_decl_body:2");
        (yyval.decl_init) = (yyvsp[(1) - (1)].decl_init);
    ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 790 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("prop_decl_array_init:1");
        (yyval.decl_init).array_index = (int)(yyvsp[(1) - (3)].int_value);
        (yyval.decl_init).initializer = (yyvsp[(3) - (3)].literal);
    ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 796 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("prop_decl_array_init:2");
        (yyval.decl_init).array_index = (int)(yyvsp[(1) - (1)].int_value);
        (yyval.decl_init).initializer = NULL;
    ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 805 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("prop_decl_scalar_init:1");
        (yyval.decl_init).array_index = 0;
        (yyval.decl_init).initializer = (yyvsp[(2) - (2)].literal);
    ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 811 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("prop_decl_scalar_init:2");
        (yyval.decl_init).array_index = 0;
        (yyval.decl_init).initializer = NULL;
    ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 828 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("ref_decl:1");
        (yyval.ref_decl) = (yyvsp[(1) - (3)].ref_decl);

        (yyval.ref_decl)->alias = (yyvsp[(2) - (3)].ref_init).alias;
        (yyval.ref_decl)->obj_ref = (yyvsp[(2) - (3)].ref_init).obj_ref;
        (yyval.ref_decl)->qualifiers = NULL;
    ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 837 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("ref_decl:2");
        (yyvsp[(1) - (4)].qual)->validate_list(MOF_SCOPE_REFERENCE);
        (yyval.ref_decl) = (yyvsp[(2) - (4)].ref_decl);
        (yyval.ref_decl)->qualifiers = (yyvsp[(1) - (4)].qual);
        (yyval.ref_decl)->alias = (yyvsp[(3) - (4)].ref_init).alias;
        (yyval.ref_decl)->obj_ref = (yyvsp[(3) - (4)].ref_init).obj_ref;
    ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 849 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("ref_head:1");
        (yyval.ref_decl) = new MOF_Reference_Decl();
        MOF_ASSERT((yyval.ref_decl) != NULL);
        (yyval.ref_decl)->class_name = (yyvsp[(1) - (2)].string_value);
        (yyval.ref_decl)->type = MOF_FEATURE_REF;
        (yyval.ref_decl)->name = (yyvsp[(2) - (2)].string_value);
    ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 860 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("ref_body:1");
        (yyval.ref_init) = (yyvsp[(2) - (2)].ref_init);
    ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 865 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("ref_body:2");
        (yyval.ref_init).alias = NULL;
        (yyval.ref_init).obj_ref = NULL;
    ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 874 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("ref_init:1");

        /*
         * Create object reference for this alias (or at least try to):
         */

        (yyval.ref_init).alias = NULL;
        (yyval.ref_init).obj_ref = MOF_Instance_Decl::alias_to_obj_ref((yyvsp[(1) - (1)].string_value));
    ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 885 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("ref_init:2");
        (yyval.ref_init).alias = NULL;
        (yyval.ref_init).obj_ref = (yyvsp[(1) - (1)].obj_ref);
    ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 902 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("method_decl:1");
        (yyval.method_decl) = (yyvsp[(1) - (3)].method_decl);
        (yyval.method_decl)->qualifiers = NULL;
        (yyval.method_decl)->parameters = (yyvsp[(2) - (3)].param);
    ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 909 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("method_decl:2");
        (yyvsp[(1) - (4)].qual)->validate_list(MOF_SCOPE_METHOD);
        (yyval.method_decl) = (yyvsp[(2) - (4)].method_decl);
        (yyval.method_decl)->qualifiers = (yyvsp[(1) - (4)].qual);
        (yyval.method_decl)->parameters = (yyvsp[(3) - (4)].param);
    ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 920 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("method_head:1");
        (yyval.method_decl) = new MOF_Method_Decl();
        MOF_ASSERT((yyval.method_decl) != NULL);
        (yyval.method_decl)->type = MOF_FEATURE_METHOD;
        (yyval.method_decl)->name = (yyvsp[(2) - (2)].string_value);
        (yyval.method_decl)->data_type = (int)(yyvsp[(1) - (2)].int_value);
    ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 932 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("method_body:1");
        (yyval.param) = (yyvsp[(2) - (3)].param);
    ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 937 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("method_body:2");
        (yyval.param) = NULL;
    ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 945 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("param_list:1");
        (yyval.param) = (yyvsp[(1) - (1)].param);
    ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 950 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("param_list:2");
        (yyval.param) = (yyvsp[(1) - (3)].param);
        (yyval.param)->append((yyvsp[(3) - (3)].param));
    ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 967 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("param:1");
        (yyval.param) = (yyvsp[(1) - (2)].param);
        (yyval.param)->array_index = (int)(yyvsp[(2) - (2)].int_value);
    ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 973 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("param:2");
        (yyvsp[(1) - (3)].qual)->validate_list(MOF_SCOPE_PARAMETER);
        (yyval.param) = (yyvsp[(2) - (3)].param);
        (yyval.param)->qualifiers = (yyvsp[(1) - (3)].qual);
        (yyval.param)->array_index = (int)(yyvsp[(3) - (3)].int_value);
    ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 984 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("param_head:1");
        (yyval.param) = (yyvsp[(1) - (2)].param);
        (yyval.param)->name = (yyvsp[(2) - (2)].string_value);
    ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 993 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("param_body:1");
        (yyval.int_value) = (yyvsp[(1) - (1)].int_value);
    ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 998 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("param_body:2");
        /* It's not an array at all */
        (yyval.int_value) = 0;
    ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 1007 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("param_data_type:1");
        (yyval.param) = new MOF_Parameter();
        (yyval.param)->data_type = (int)(yyvsp[(1) - (1)].int_value);
        (yyval.param)->ref_name = NULL;
    ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 1014 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("param_data_type:2");
        (yyval.param) = new MOF_Parameter();
        (yyval.param)->data_type = TOK_REF;
        (yyval.param)->ref_name = (yyvsp[(1) - (1)].string_value);
    ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 1032 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("inst_decl:1");
        (yyval.inst_decl) = (yyvsp[(1) - (3)].inst_decl);
        (yyval.inst_decl)->qualifiers = NULL;
        (yyval.inst_decl)->properties = (yyvsp[(2) - (3)].prop);
    ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 1039 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("inst_decl:2");
        (yyvsp[(1) - (4)].qual)->validate_list(MOF_SCOPE_CLASS);
        (yyval.inst_decl) = (yyvsp[(2) - (4)].inst_decl);
        (yyval.inst_decl)->qualifiers = (yyvsp[(1) - (4)].qual);
        (yyval.inst_decl)->properties = (yyvsp[(3) - (4)].prop);
    ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 1050 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("inst_head:1");
        (yyval.inst_decl) = new MOF_Instance_Decl();
        MOF_ASSERT((yyval.inst_decl) != NULL);
        (yyval.inst_decl)->class_name = (yyvsp[(3) - (4)].string_value);
        (yyval.inst_decl)->alias = (yyvsp[(4) - (4)].string_value);
    ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 1061 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("inst_body:1");
        (yyval.prop) = (yyvsp[(2) - (3)].prop);
    ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 1069 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("inst_alias:1");
        (yyval.string_value) = (yyvsp[(2) - (2)].string_value);
    ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 1074 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("inst_alias:2");
        (yyval.string_value) = NULL;
    ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 1090 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("prop_list:1");
        (yyval.prop) = (yyvsp[(1) - (1)].prop);
    ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 1095 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("prop_list:2");
        (yyvsp[(1) - (2)].prop)->append((yyvsp[(2) - (2)].prop));
        (yyval.prop) = (yyvsp[(1) - (2)].prop);
    ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 1104 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("prop:1");
        (yyval.prop) = (yyvsp[(1) - (1)].prop);
        (yyval.prop)->qualifiers = NULL;
    ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 1110 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("prop:2");
        (yyvsp[(1) - (2)].qual)->validate_list(MOF_SCOPE_PROPERTY);
        (yyval.prop) = (yyvsp[(2) - (2)].prop);
        (yyval.prop)->qualifiers = (yyvsp[(1) - (2)].qual);
    ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 1120 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("prop_body:1");

        (yyval.prop) = new MOF_Property();
        MOF_ASSERT((yyval.prop) != NULL);
        (yyval.prop)->name = (yyvsp[(1) - (4)].string_value);
        (yyval.prop)->initializer = (yyvsp[(3) - (4)].literal);
    ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 1129 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("prop_body:2");

        (yyval.prop) = new MOF_Property();
        MOF_ASSERT((yyval.prop) != NULL);
        (yyval.prop)->name = (yyvsp[(1) - (4)].string_value);
        (yyval.prop)->initializer = NULL;
        (yyval.prop)->alias = (yyvsp[(3) - (4)].string_value);
    ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 1150 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("object_ref:1");
        (yyval.string_value) = (yyvsp[(1) - (2)].string_value);
    ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 1158 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("obj_ref:1");

        if (REF_parse((yyvsp[(1) - (1)].string_literal).escaped, &(yyval.obj_ref)) != 0)
        {
            MOF_error_printf("malformed object reference: \"%s\": %s", 
                (yyvsp[(1) - (1)].string_literal).escaped, ref_error_message);
        }

        {
            char* p = MOF_Object_Reference::normalize((yyvsp[(1) - (1)].string_literal).escaped);
            MOF_ASSERT(p != NULL);
            free(p);
        }

        free((yyvsp[(1) - (1)].string_literal).escaped);
        free((yyvsp[(1) - (1)].string_literal).raw);
        (yyval.obj_ref)->validate();
        (yyval.obj_ref)->normalize();
        /* MOF_obj_ref_print(stdout, $$); */
    ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 1191 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("array_index:1");
        /* It's a varying length array (VLA) */
        (yyval.int_value) = -1;
    ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 1197 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("array_index:2");

        /* subscript must be positive */

        if ((yyvsp[(2) - (3)].int_value) <= 0)
            MOF_error("array subscript must be a positive number");

        (yyval.int_value) = (yyvsp[(2) - (3)].int_value);
    ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 1219 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("data_type:1");
        (yyval.int_value) = TOK_UINT8;
    ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 1224 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("data_type:2");
        (yyval.int_value) = TOK_SINT8;
    ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 1229 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("data_type:3");
        (yyval.int_value) = TOK_UINT16;
    ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 1234 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("data_type:4");
        (yyval.int_value) = TOK_SINT16;
    ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 1239 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("data_type:5");
        (yyval.int_value) = TOK_UINT32;
    ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 1244 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("data_type:6");
        (yyval.int_value) = TOK_SINT32;
    ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 1249 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("data_type:7");
        (yyval.int_value) = TOK_UINT64;
    ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 1254 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("data_type:8");
        (yyval.int_value) = TOK_SINT64;
    ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 1259 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("data_type:9");
        (yyval.int_value) = TOK_REAL32;
    ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 1264 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("data_type:10");
        (yyval.int_value) = TOK_REAL64;
    ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 1269 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("data_type:11");
        (yyval.int_value) = TOK_CHAR16;
    ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 1274 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("data_type:12");
        (yyval.int_value) = TOK_STRING;
    ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 1279 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("data_type:13");
        (yyval.int_value) = TOK_BOOLEAN;
    ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 1284 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("data_type:14");
        (yyval.int_value) = TOK_DATETIME;
    ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 1300 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("init:1");
        (yyval.literal) = (yyvsp[(1) - (1)].literal);
    ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 1305 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("init:2");
        (yyval.literal) = (yyvsp[(1) - (1)].literal);
    ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 1313 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("array_init:1");
        (yyval.literal) = (yyvsp[(2) - (3)].literal);
    ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 1321 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("scalar_init_list:1");
        (yyval.literal) = (yyvsp[(1) - (1)].literal);
    ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 1326 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("scalar_init_list:2");
        (yyvsp[(1) - (3)].literal)->append((yyvsp[(3) - (3)].literal));
        (yyval.literal) = (yyvsp[(1) - (3)].literal);
    ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 1343 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("literal:1");
        (yyval.literal) = new MOF_Literal();
        MOF_ASSERT((yyval.literal) != NULL);
        (yyval.literal)->value_type = TOK_INT_VALUE;
        (yyval.literal)->int_value = (yyvsp[(1) - (1)].int_value);
    ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 1351 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("literal:2");
        (yyval.literal) = new MOF_Literal();
        MOF_ASSERT((yyval.literal) != NULL);
        (yyval.literal)->value_type = TOK_REAL_VALUE;
        (yyval.literal)->real_value = (yyvsp[(1) - (1)].real_value);
    ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 1359 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("literal:3");
        (yyval.literal) = new MOF_Literal();
        MOF_ASSERT((yyval.literal) != NULL);
        (yyval.literal)->value_type = TOK_CHAR_VALUE;
        (yyval.literal)->char_value = (yyvsp[(1) - (1)].char_value);
    ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 1367 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("literal:4");
        (yyval.literal) = new MOF_Literal();
        MOF_ASSERT((yyval.literal) != NULL);
        (yyval.literal)->value_type = TOK_BOOL_VALUE;
        (yyval.literal)->bool_value = (yyvsp[(1) - (1)].bool_value) ? true : false;
    ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 1375 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("literal:6");
        (yyval.literal) = new MOF_Literal();
        MOF_ASSERT((yyval.literal) != NULL);
        (yyval.literal)->value_type = TOK_STRING_VALUE;
        (yyval.literal)->string_value = (yyvsp[(1) - (1)].string_literal).escaped;
        free((yyvsp[(1) - (1)].string_literal).raw);
    ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 1384 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("literal:5");
        (yyval.literal) = new MOF_Literal();
        MOF_ASSERT((yyval.literal) != NULL);
        (yyval.literal)->value_type = TOK_NULL_VALUE;
    ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 1402 "cimple/tools/libmof/MOF.y"
    {
        MOF_trace("string_literal:1");
        (yyval.string_literal) = (yyvsp[(1) - (1)].string_literal);
    ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 1407 "cimple/tools/libmof/MOF.y"
    {
        /* Raw case */
        {
            size_t n1 = strlen((yyvsp[(1) - (2)].string_literal).raw);
            size_t n2 = strlen((yyvsp[(2) - (2)].string_literal).raw);

            MOF_trace("string_literal:2");

            (yyval.string_literal).raw = (char*)realloc((yyvsp[(1) - (2)].string_literal).raw, n1 + n2 + 16);
            MOF_ASSERT((yyval.string_literal).raw != NULL);

            strcat((yyval.string_literal).raw, (yyvsp[(2) - (2)].string_literal).raw);
            free((yyvsp[(2) - (2)].string_literal).raw);
        }

        /* Escaped case */
        {
            size_t n1 = strlen((yyvsp[(1) - (2)].string_literal).escaped);
            size_t n2 = strlen((yyvsp[(2) - (2)].string_literal).escaped);

            MOF_trace("string_literal:2");

            (yyval.string_literal).escaped = (char*)realloc((yyvsp[(1) - (2)].string_literal).escaped, n1 + n2 + 16);
            MOF_ASSERT((yyval.string_literal).escaped != NULL);

            strcat((yyval.string_literal).escaped, (yyvsp[(2) - (2)].string_literal).escaped);
            free((yyvsp[(2) - (2)].string_literal).escaped);
        }
    ;}
    break;



/* Line 1455 of yacc.c  */
#line 3280 "cimple/tools/libmof/MOF_Yacc.cpp"
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
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
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



/* Line 1675 of yacc.c  */
#line 1438 "cimple/tools/libmof/MOF.y"


/*
**==============================================================================
**
** MOF_trace():  Ouputs statements for detailed debug of yacc progress
**
**==============================================================================
*/

static void MOF_trace(const char* str)
{
#if MOF_DEBUG_MODE
    printf("MOF_trace(): \"%s\"\n", str);
#endif
}


