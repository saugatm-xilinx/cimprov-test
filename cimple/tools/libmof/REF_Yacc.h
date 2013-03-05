
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     REF_TOK_NULL_VALUE = 258,
     REF_TOK_INT_VALUE = 259,
     REF_TOK_BOOL_VALUE = 260,
     REF_TOK_REAL_VALUE = 261,
     REF_TOK_STRING_VALUE = 262,
     REF_TOK_CHAR_VALUE = 263,
     REF_TOK_IDENT = 264,
     REF_TOK_COMMA = 265,
     REF_TOK_COLON = 266,
     REF_TOK_EQUAL = 267,
     REF_TOK_DOT = 268,
     REF_TOK_OPEN_BRACE = 269,
     REF_TOK_CLOSE_BRACE = 270
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 17 "cimple/tools/libmof/REF.y"

    MOF_char16 char_value;
    char* string_value;
    MOF_sint64 int_value;
    int bool_value;
    double real_value;

    class MOF_Literal* literal;
    class MOF_Key_Value_Pair* key_val_pair;
    class MOF_Object_Reference* obj_ref;



/* Line 1676 of yacc.c  */
#line 81 "cimple/tools/libmof/REF_Yacc.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE REF_lval;


