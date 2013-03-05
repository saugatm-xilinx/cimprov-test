
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

/* Line 1676 of yacc.c  */
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



/* Line 1676 of yacc.c  */
#line 154 "cimple/tools/libmof/MOF_Yacc.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE MOF_lval;


