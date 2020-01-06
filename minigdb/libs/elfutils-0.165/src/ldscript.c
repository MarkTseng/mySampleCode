/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         ldparse
#define yylex           ldlex
#define yyerror         lderror
#define yydebug         lddebug
#define yynerrs         ldnerrs

#define yylval          ldlval
#define yychar          ldchar

/* Copy the first part of user declarations.  */
#line 1 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:339  */

/* Parser for linker scripts.
   Copyright (C) 2001-2011 Red Hat, Inc.
   This file is part of elfutils.
   Written by Ulrich Drepper <drepper@redhat.com>, 2001.

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   elfutils is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <error.h>
#include <libintl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <system.h>
#include <ld.h>

/* The error handler.  */
static void yyerror (const char *s);

/* Some helper functions we need to construct the data structures
   describing information from the file.  */
static struct expression *new_expr (int tag);
static struct input_section_name *new_input_section_name (const char *name,
							  bool sort_flag);
static struct input_rule *new_input_rule (int tag);
static struct output_rule *new_output_rule (int tag);
static struct assignment *new_assignment (const char *variable,
					  struct expression *expression,
					  bool provide_flag);
static void new_segment (int mode, struct output_rule *output_rule);
static struct filename_list *new_filename_listelem (const char *string);
static void add_inputfiles (struct filename_list *fnames);
static struct id_list *new_id_listelem (const char *str);
 static struct filename_list *mark_as_needed (struct filename_list *listp);
static struct version *new_version (struct id_list *local,
				    struct id_list *global);
static struct version *merge_versions (struct version *one,
				       struct version *two);
static void add_versions (struct version *versions);

extern int yylex (void);

#line 136 "ldscript.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_LD_LDSCRIPT_H_INCLUDED
# define YY_LD_LDSCRIPT_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int lddebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    kADD_OP = 258,
    kALIGN = 259,
    kAS_NEEDED = 260,
    kENTRY = 261,
    kEXCLUDE_FILE = 262,
    kFILENAME = 263,
    kGLOBAL = 264,
    kGROUP = 265,
    kID = 266,
    kINPUT = 267,
    kINTERP = 268,
    kKEEP = 269,
    kLOCAL = 270,
    kMODE = 271,
    kMUL_OP = 272,
    kNUM = 273,
    kOUTPUT_FORMAT = 274,
    kPAGESIZE = 275,
    kPROVIDE = 276,
    kSEARCH_DIR = 277,
    kSEGMENT = 278,
    kSIZEOF_HEADERS = 279,
    kSORT = 280,
    kVERSION = 281,
    kVERSION_SCRIPT = 282,
    ADD_OP = 283,
    MUL_OP = 284
  };
#endif
/* Tokens.  */
#define kADD_OP 258
#define kALIGN 259
#define kAS_NEEDED 260
#define kENTRY 261
#define kEXCLUDE_FILE 262
#define kFILENAME 263
#define kGLOBAL 264
#define kGROUP 265
#define kID 266
#define kINPUT 267
#define kINTERP 268
#define kKEEP 269
#define kLOCAL 270
#define kMODE 271
#define kMUL_OP 272
#define kNUM 273
#define kOUTPUT_FORMAT 274
#define kPAGESIZE 275
#define kPROVIDE 276
#define kSEARCH_DIR 277
#define kSEGMENT 278
#define kSIZEOF_HEADERS 279
#define kSORT 280
#define kVERSION 281
#define kVERSION_SCRIPT 282
#define ADD_OP 283
#define MUL_OP 284

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 63 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:355  */

  uintmax_t num;
  enum expression_tag op;
  char *str;
  struct expression *expr;
  struct input_section_name *sectionname;
  struct filemask_section_name *filemask_section_name;
  struct input_rule *input_rule;
  struct output_rule *output_rule;
  struct assignment *assignment;
  struct filename_list *filename_list;
  struct version *version;
  struct id_list *id_list;

#line 249 "ldscript.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE ldlval;

int ldparse (void);

#endif /* !YY_LD_LDSCRIPT_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 266 "ldscript.c" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
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
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  32
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   226

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  40
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  23
/* YYNRULES -- Number of rules.  */
#define YYNRULES  66
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  159

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   284

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    29,     2,
      33,    34,    32,     2,    39,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    35,
       2,    38,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    36,    28,    37,     2,     2,     2,     2,
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
      25,    26,    27,    30,    31
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   135,   135,   136,   140,   141,   144,   149,   153,   158,
     164,   168,   174,   185,   187,   189,   191,   195,   200,   204,
     209,   221,   245,   247,   251,   256,   260,   265,   272,   279,
     290,   292,   296,   299,   302,   307,   309,   315,   321,   327,
     333,   339,   344,   349,   351,   355,   360,   364,   365,   368,
     379,   381,   386,   391,   395,   401,   407,   416,   418,   422,
     424,   429,   435,   439,   441,   445,   447
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "kADD_OP", "kALIGN", "kAS_NEEDED",
  "kENTRY", "kEXCLUDE_FILE", "kFILENAME", "kGLOBAL", "kGROUP", "kID",
  "kINPUT", "kINTERP", "kKEEP", "kLOCAL", "kMODE", "kMUL_OP", "kNUM",
  "kOUTPUT_FORMAT", "kPAGESIZE", "kPROVIDE", "kSEARCH_DIR", "kSEGMENT",
  "kSIZEOF_HEADERS", "kSORT", "kVERSION", "kVERSION_SCRIPT", "'|'", "'&'",
  "ADD_OP", "MUL_OP", "'*'", "'('", "')'", "';'", "'{'", "'}'", "'='",
  "','", "$accept", "script_or_version", "file", "content",
  "outputsections", "outputsection", "assignment", "inputsections",
  "inputsection", "sectionname", "sort_opt_name", "exclude_opt", "expr",
  "filename_id_list", "comma_opt", "filename_id_listelem", "versionlist",
  "version", "version_stmt_list", "version_stmt", "filename_id_star_list",
  "filename_id", "filename_id_star", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   124,    38,
     283,   284,    42,    40,    41,    59,   123,   125,    61,    44
};
# endif

#define YYPACT_NINF -40

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-40)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     112,   -14,   -13,    27,    31,    48,    66,   100,   119,     9,
      26,    15,    98,   135,   -40,   118,   139,   118,   118,   104,
     104,   141,   104,   133,   138,    15,   -40,   -40,     7,    15,
     -40,   142,   -40,   -40,   120,   144,   103,   -40,   -40,   145,
     117,   134,   146,   147,   148,   149,   109,   109,    65,    89,
      89,    59,   -40,   -40,     7,   118,   118,   -40,   -40,   118,
     150,   -40,   -40,   151,   -40,   152,   153,   108,   156,    -6,
     -40,   155,    73,   -40,   -40,    89,   -40,   157,    89,   158,
     -40,    76,   136,   137,   -40,   -40,   -40,   -40,   -40,   -40,
      95,    45,   161,   -40,   -40,   -40,   -40,   159,   -40,   -40,
      84,   -40,   -40,   160,   162,   164,    22,   -40,   -40,   163,
     167,   -40,   -40,   -40,   -40,    45,    54,   165,   -40,   -40,
     166,    89,   -40,   -40,   -40,   177,    45,     0,    45,    45,
      45,    45,    45,    45,   -40,   168,   171,     2,    18,   -40,
      54,    54,    58,    21,    90,    38,   -40,   104,   -40,   172,
     173,   -40,   -40,   174,   180,   -40,   -40,   175,   -40
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     2,     5,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    63,    64,     0,     3,
      53,     0,     1,     4,     0,     0,    48,    46,    51,     0,
      48,    48,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    58,    52,     0,     0,     0,    14,    47,     0,
       0,    12,    13,     0,    16,     0,     0,     0,     0,     0,
      18,     0,     0,    15,    66,    59,    65,     0,    60,     0,
      57,     0,    48,    48,    45,     6,     9,     8,     7,    21,
       0,     0,     0,    11,    17,    19,    10,     0,    62,    54,
       0,    50,    49,    64,     0,     0,     0,    25,    26,     0,
       0,    42,    41,    44,    43,     0,    22,     0,    61,    55,
       0,     0,    28,    20,    24,    33,     0,     0,     0,     0,
       0,     0,     0,     0,    56,     0,     0,     0,     0,    35,
      38,    37,    40,    39,    36,     0,    27,     0,    30,     0,
       0,    34,    23,     0,     0,    29,    32,     0,    31
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -40,   -40,   -40,   184,   169,    42,    -2,   -40,   105,    85,
     -40,   -40,    34,   -16,   -40,   154,   185,    29,   170,   -39,
     176,   -11,    30
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    12,    13,    14,    69,    70,    71,   106,   107,   108,
     150,   137,   116,    36,    59,    37,    29,    30,    51,    52,
      75,    76,   109
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      31,    40,    41,   128,    38,    67,    38,    38,    42,    43,
      23,    45,    80,   148,    31,    68,    49,   129,    31,    15,
      16,   128,    50,    26,   128,    24,    27,   149,   130,   131,
      26,    93,   132,   103,   139,   129,   104,    31,   129,    82,
      83,   128,    80,    68,    38,    38,   130,   131,    38,   110,
     132,    28,   151,   132,    74,   129,   111,   128,    53,   123,
      17,   128,    25,   112,    18,   113,   130,   131,    49,   114,
     132,   129,   152,    26,    50,   129,    27,    53,   115,    77,
      77,    19,   130,   131,    67,    49,   132,   131,   105,   120,
     132,    50,    26,   128,    68,    27,    79,    26,    32,    20,
      27,    28,    73,    26,   105,    97,   103,   129,    97,   104,
      96,    94,    26,   100,    94,    27,    68,     1,     2,   119,
      67,    74,     3,    34,     4,     5,    26,    74,    35,    27,
      68,     6,     7,    21,     8,     9,   153,    57,    10,    11,
       1,     2,    58,    89,    90,     3,    91,     4,     5,   127,
      39,    61,    22,    55,     6,     7,    58,     8,     9,    44,
     138,    10,   140,   141,   142,   143,   144,   145,    62,    46,
     101,   102,   117,    58,    47,    58,    58,    56,    54,    60,
      63,    64,    65,    66,   136,    85,    86,    87,    88,    92,
      95,   157,    98,    99,   118,   121,   125,    33,    91,   122,
     126,   134,   146,   133,   147,   154,   135,   155,   156,   158,
      48,   124,     0,    84,     0,     0,    72,     0,     0,     0,
       0,     0,     0,     0,    81,     0,    78
};

static const yytype_int16 yycheck[] =
{
      11,    17,    18,     3,    15,    11,    17,    18,    19,    20,
       1,    22,    51,    11,    25,    21,     9,    17,    29,    33,
      33,     3,    15,     8,     3,    16,    11,    25,    28,    29,
       8,    37,    32,    11,    34,    17,    14,    48,    17,    55,
      56,     3,    81,    21,    55,    56,    28,    29,    59,     4,
      32,    36,    34,    32,    32,    17,    11,     3,    29,    37,
      33,     3,    36,    18,    33,    20,    28,    29,     9,    24,
      32,    17,    34,     8,    15,    17,    11,    48,    33,    49,
      50,    33,    28,    29,    11,     9,    32,    29,    90,   100,
      32,    15,     8,     3,    21,    11,    37,     8,     0,    33,
      11,    36,    37,     8,   106,    75,    11,    17,    78,    14,
      37,    69,     8,    37,    72,    11,    21,     5,     6,    35,
      11,    32,    10,     5,    12,    13,     8,    32,    10,    11,
      21,    19,    20,    33,    22,    23,   147,    34,    26,    27,
       5,     6,    39,    35,    36,    10,    38,    12,    13,   115,
      11,    34,    33,    33,    19,    20,    39,    22,    23,    18,
     126,    26,   128,   129,   130,   131,   132,   133,    34,    36,
      34,    34,    11,    39,    36,    39,    39,    33,    36,    34,
      34,    34,    34,    34,     7,    35,    35,    35,    35,    33,
      35,    11,    35,    35,    35,    33,    33,    13,    38,    35,
      33,    35,    34,    38,    33,    33,   121,    34,    34,    34,
      25,   106,    -1,    59,    -1,    -1,    47,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    54,    -1,    50
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,     6,    10,    12,    13,    19,    20,    22,    23,
      26,    27,    41,    42,    43,    33,    33,    33,    33,    33,
      33,    33,    33,     1,    16,    36,     8,    11,    36,    56,
      57,    61,     0,    43,     5,    10,    53,    55,    61,    11,
      53,    53,    61,    61,    18,    61,    36,    36,    56,     9,
      15,    58,    59,    57,    36,    33,    33,    34,    39,    54,
      34,    34,    34,    34,    34,    34,    34,    11,    21,    44,
      45,    46,    44,    37,    32,    60,    61,    62,    60,    37,
      59,    58,    53,    53,    55,    35,    35,    35,    35,    35,
      36,    38,    33,    37,    45,    35,    37,    62,    35,    35,
      37,    34,    34,    11,    14,    46,    47,    48,    49,    62,
       4,    11,    18,    20,    24,    33,    52,    11,    35,    35,
      61,    33,    35,    37,    48,    33,    33,    52,     3,    17,
      28,    29,    32,    38,    35,    49,     7,    51,    52,    34,
      52,    52,    52,    52,    52,    52,    34,    33,    11,    25,
      50,    34,    34,    61,    33,    34,    34,    11,    34
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    40,    41,    41,    42,    42,    43,    43,    43,    43,
      43,    43,    43,    43,    43,    43,    43,    44,    44,    45,
      45,    45,    46,    46,    47,    47,    48,    48,    48,    49,
      50,    50,    51,    51,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    53,    53,    54,    54,    55,
      55,    55,    56,    56,    57,    57,    57,    58,    58,    59,
      59,    60,    60,    61,    61,    62,    62
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     2,     1,     5,     5,     5,     5,
       5,     5,     4,     4,     4,     4,     4,     2,     1,     2,
       4,     2,     3,     6,     2,     1,     1,     4,     2,     5,
       1,     4,     4,     0,     4,     3,     3,     3,     3,     3,
       3,     1,     1,     1,     1,     3,     1,     1,     0,     4,
       4,     1,     2,     1,     4,     5,     6,     2,     1,     2,
       2,     3,     2,     1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
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
      if (yytable_value_is_error (yyn))
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
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 137 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { add_versions ((yyvsp[0].version)); }
#line 1458 "ldscript.c" /* yacc.c:1646  */
    break;

  case 6:
#line 145 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      if (likely (ld_state.entry == NULL))
			ld_state.entry = (yyvsp[-2].str);
		    }
#line 1467 "ldscript.c" /* yacc.c:1646  */
    break;

  case 7:
#line 150 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      ld_new_searchdir ((yyvsp[-2].str));
		    }
#line 1475 "ldscript.c" /* yacc.c:1646  */
    break;

  case 8:
#line 154 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      if (likely (ld_state.pagesize == 0))
			ld_state.pagesize = (yyvsp[-2].num);
		    }
#line 1484 "ldscript.c" /* yacc.c:1646  */
    break;

  case 9:
#line 159 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      if (likely (ld_state.interp == NULL)
			  && ld_state.file_type != dso_file_type)
			ld_state.interp = (yyvsp[-2].str);
		    }
#line 1494 "ldscript.c" /* yacc.c:1646  */
    break;

  case 10:
#line 165 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      new_segment ((yyvsp[-3].num), (yyvsp[-1].output_rule));
		    }
#line 1502 "ldscript.c" /* yacc.c:1646  */
    break;

  case 11:
#line 169 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      fputs_unlocked (gettext ("mode for segment invalid\n"),
				      stderr);
		      new_segment (0, (yyvsp[-1].output_rule));
		    }
#line 1512 "ldscript.c" /* yacc.c:1646  */
    break;

  case 12:
#line 175 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      /* First little optimization.  If there is only one
			 file in the group don't do anything.  */
		      if ((yyvsp[-1].filename_list) != (yyvsp[-1].filename_list)->next)
			{
			  (yyvsp[-1].filename_list)->next->group_start = 1;
			  (yyvsp[-1].filename_list)->group_end = 1;
			}
		      add_inputfiles ((yyvsp[-1].filename_list));
		    }
#line 1527 "ldscript.c" /* yacc.c:1646  */
    break;

  case 13:
#line 186 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { add_inputfiles ((yyvsp[-1].filename_list)); }
#line 1533 "ldscript.c" /* yacc.c:1646  */
    break;

  case 14:
#line 188 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { add_inputfiles (mark_as_needed ((yyvsp[-1].filename_list))); }
#line 1539 "ldscript.c" /* yacc.c:1646  */
    break;

  case 15:
#line 190 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { add_versions ((yyvsp[-1].version)); }
#line 1545 "ldscript.c" /* yacc.c:1646  */
    break;

  case 16:
#line 192 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { /* XXX TODO */ }
#line 1551 "ldscript.c" /* yacc.c:1646  */
    break;

  case 17:
#line 196 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyvsp[0].output_rule)->next = (yyvsp[-1].output_rule)->next;
		      (yyval.output_rule) = (yyvsp[-1].output_rule)->next = (yyvsp[0].output_rule);
		    }
#line 1560 "ldscript.c" /* yacc.c:1646  */
    break;

  case 18:
#line 201 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.output_rule) = (yyvsp[0].output_rule); }
#line 1566 "ldscript.c" /* yacc.c:1646  */
    break;

  case 19:
#line 205 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyval.output_rule) = new_output_rule (output_assignment);
		      (yyval.output_rule)->val.assignment = (yyvsp[-1].assignment);
		    }
#line 1575 "ldscript.c" /* yacc.c:1646  */
    break;

  case 20:
#line 210 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyval.output_rule) = new_output_rule (output_section);
		      (yyval.output_rule)->val.section.name = (yyvsp[-3].str);
		      (yyval.output_rule)->val.section.input = (yyvsp[-1].input_rule)->next;
		      if (ld_state.strip == strip_debug
			  && ebl_debugscn_p (ld_state.ebl, (yyvsp[-3].str)))
			(yyval.output_rule)->val.section.ignored = true;
		      else
			(yyval.output_rule)->val.section.ignored = false;
		      (yyvsp[-1].input_rule)->next = NULL;
		    }
#line 1591 "ldscript.c" /* yacc.c:1646  */
    break;

  case 21:
#line 222 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      /* This is a short cut for "ID { *(ID) }".  */
		      (yyval.output_rule) = new_output_rule (output_section);
		      (yyval.output_rule)->val.section.name = (yyvsp[-1].str);
		      (yyval.output_rule)->val.section.input = new_input_rule (input_section);
		      (yyval.output_rule)->val.section.input->next = NULL;
		      (yyval.output_rule)->val.section.input->val.section =
			(struct filemask_section_name *)
			  obstack_alloc (&ld_state.smem,
					 sizeof (struct filemask_section_name));
		      (yyval.output_rule)->val.section.input->val.section->filemask = NULL;
		      (yyval.output_rule)->val.section.input->val.section->excludemask = NULL;
		      (yyval.output_rule)->val.section.input->val.section->section_name =
			new_input_section_name ((yyvsp[-1].str), false);
		      (yyval.output_rule)->val.section.input->val.section->keep_flag = false;
		      if (ld_state.strip == strip_debug
			  && ebl_debugscn_p (ld_state.ebl, (yyvsp[-1].str)))
			(yyval.output_rule)->val.section.ignored = true;
		      else
			(yyval.output_rule)->val.section.ignored = false;
		    }
#line 1617 "ldscript.c" /* yacc.c:1646  */
    break;

  case 22:
#line 246 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.assignment) = new_assignment ((yyvsp[-2].str), (yyvsp[0].expr), false); }
#line 1623 "ldscript.c" /* yacc.c:1646  */
    break;

  case 23:
#line 248 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.assignment) = new_assignment ((yyvsp[-3].str), (yyvsp[-1].expr), true); }
#line 1629 "ldscript.c" /* yacc.c:1646  */
    break;

  case 24:
#line 252 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyvsp[0].input_rule)->next = (yyvsp[-1].input_rule)->next;
		      (yyval.input_rule) = (yyvsp[-1].input_rule)->next = (yyvsp[0].input_rule);
		    }
#line 1638 "ldscript.c" /* yacc.c:1646  */
    break;

  case 25:
#line 257 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.input_rule) = (yyvsp[0].input_rule); }
#line 1644 "ldscript.c" /* yacc.c:1646  */
    break;

  case 26:
#line 261 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyval.input_rule) = new_input_rule (input_section);
		      (yyval.input_rule)->val.section = (yyvsp[0].filemask_section_name);
		    }
#line 1653 "ldscript.c" /* yacc.c:1646  */
    break;

  case 27:
#line 266 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyvsp[-1].filemask_section_name)->keep_flag = true;

		      (yyval.input_rule) = new_input_rule (input_section);
		      (yyval.input_rule)->val.section = (yyvsp[-1].filemask_section_name);
		    }
#line 1664 "ldscript.c" /* yacc.c:1646  */
    break;

  case 28:
#line 273 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyval.input_rule) = new_input_rule (input_assignment);
		      (yyval.input_rule)->val.assignment = (yyvsp[-1].assignment);
		    }
#line 1673 "ldscript.c" /* yacc.c:1646  */
    break;

  case 29:
#line 280 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyval.filemask_section_name) = (struct filemask_section_name *)
			obstack_alloc (&ld_state.smem, sizeof (*(yyval.filemask_section_name)));
		      (yyval.filemask_section_name)->filemask = (yyvsp[-4].str);
		      (yyval.filemask_section_name)->excludemask = (yyvsp[-2].str);
		      (yyval.filemask_section_name)->section_name = (yyvsp[-1].sectionname);
		      (yyval.filemask_section_name)->keep_flag = false;
		    }
#line 1686 "ldscript.c" /* yacc.c:1646  */
    break;

  case 30:
#line 291 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.sectionname) = new_input_section_name ((yyvsp[0].str), false); }
#line 1692 "ldscript.c" /* yacc.c:1646  */
    break;

  case 31:
#line 293 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.sectionname) = new_input_section_name ((yyvsp[-1].str), true); }
#line 1698 "ldscript.c" /* yacc.c:1646  */
    break;

  case 32:
#line 297 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[-1].str); }
#line 1704 "ldscript.c" /* yacc.c:1646  */
    break;

  case 33:
#line 299 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.str) = NULL; }
#line 1710 "ldscript.c" /* yacc.c:1646  */
    break;

  case 34:
#line 303 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyval.expr) = new_expr (exp_align);
		      (yyval.expr)->val.child = (yyvsp[-1].expr);
		    }
#line 1719 "ldscript.c" /* yacc.c:1646  */
    break;

  case 35:
#line 308 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 1725 "ldscript.c" /* yacc.c:1646  */
    break;

  case 36:
#line 310 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyval.expr) = new_expr (exp_mult);
		      (yyval.expr)->val.binary.left = (yyvsp[-2].expr);
		      (yyval.expr)->val.binary.right = (yyvsp[0].expr);
		    }
#line 1735 "ldscript.c" /* yacc.c:1646  */
    break;

  case 37:
#line 316 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyval.expr) = new_expr ((yyvsp[-1].op));
		      (yyval.expr)->val.binary.left = (yyvsp[-2].expr);
		      (yyval.expr)->val.binary.right = (yyvsp[0].expr);
		    }
#line 1745 "ldscript.c" /* yacc.c:1646  */
    break;

  case 38:
#line 322 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyval.expr) = new_expr ((yyvsp[-1].op));
		      (yyval.expr)->val.binary.left = (yyvsp[-2].expr);
		      (yyval.expr)->val.binary.right = (yyvsp[0].expr);
		    }
#line 1755 "ldscript.c" /* yacc.c:1646  */
    break;

  case 39:
#line 328 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyval.expr) = new_expr (exp_and);
		      (yyval.expr)->val.binary.left = (yyvsp[-2].expr);
		      (yyval.expr)->val.binary.right = (yyvsp[0].expr);
		    }
#line 1765 "ldscript.c" /* yacc.c:1646  */
    break;

  case 40:
#line 334 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyval.expr) = new_expr (exp_or);
		      (yyval.expr)->val.binary.left = (yyvsp[-2].expr);
		      (yyval.expr)->val.binary.right = (yyvsp[0].expr);
		    }
#line 1775 "ldscript.c" /* yacc.c:1646  */
    break;

  case 41:
#line 340 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyval.expr) = new_expr (exp_num);
		      (yyval.expr)->val.num = (yyvsp[0].num);
		    }
#line 1784 "ldscript.c" /* yacc.c:1646  */
    break;

  case 42:
#line 345 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyval.expr) = new_expr (exp_id);
		      (yyval.expr)->val.str = (yyvsp[0].str);
		    }
#line 1793 "ldscript.c" /* yacc.c:1646  */
    break;

  case 43:
#line 350 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.expr) = new_expr (exp_sizeof_headers); }
#line 1799 "ldscript.c" /* yacc.c:1646  */
    break;

  case 44:
#line 352 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.expr) = new_expr (exp_pagesize); }
#line 1805 "ldscript.c" /* yacc.c:1646  */
    break;

  case 45:
#line 356 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyvsp[0].filename_list)->next = (yyvsp[-2].filename_list)->next;
		      (yyval.filename_list) = (yyvsp[-2].filename_list)->next = (yyvsp[0].filename_list);
		    }
#line 1814 "ldscript.c" /* yacc.c:1646  */
    break;

  case 46:
#line 361 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.filename_list) = (yyvsp[0].filename_list); }
#line 1820 "ldscript.c" /* yacc.c:1646  */
    break;

  case 49:
#line 369 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      /* First little optimization.  If there is only one
			 file in the group don't do anything.  */
		      if ((yyvsp[-1].filename_list) != (yyvsp[-1].filename_list)->next)
			{
			  (yyvsp[-1].filename_list)->next->group_start = 1;
			  (yyvsp[-1].filename_list)->group_end = 1;
			}
		      (yyval.filename_list) = (yyvsp[-1].filename_list);
		    }
#line 1835 "ldscript.c" /* yacc.c:1646  */
    break;

  case 50:
#line 380 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.filename_list) = mark_as_needed ((yyvsp[-1].filename_list)); }
#line 1841 "ldscript.c" /* yacc.c:1646  */
    break;

  case 51:
#line 382 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.filename_list) = new_filename_listelem ((yyvsp[0].str)); }
#line 1847 "ldscript.c" /* yacc.c:1646  */
    break;

  case 52:
#line 387 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyvsp[0].version)->next = (yyvsp[-1].version)->next;
		      (yyval.version) = (yyvsp[-1].version)->next = (yyvsp[0].version);
		    }
#line 1856 "ldscript.c" /* yacc.c:1646  */
    break;

  case 53:
#line 392 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.version) = (yyvsp[0].version); }
#line 1862 "ldscript.c" /* yacc.c:1646  */
    break;

  case 54:
#line 396 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyvsp[-2].version)->versionname = "";
		      (yyvsp[-2].version)->parentname = NULL;
		      (yyval.version) = (yyvsp[-2].version);
		    }
#line 1872 "ldscript.c" /* yacc.c:1646  */
    break;

  case 55:
#line 402 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyvsp[-2].version)->versionname = (yyvsp[-4].str);
		      (yyvsp[-2].version)->parentname = NULL;
		      (yyval.version) = (yyvsp[-2].version);
		    }
#line 1882 "ldscript.c" /* yacc.c:1646  */
    break;

  case 56:
#line 408 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      (yyvsp[-3].version)->versionname = (yyvsp[-5].str);
		      (yyvsp[-3].version)->parentname = (yyvsp[-1].str);
		      (yyval.version) = (yyvsp[-3].version);
		    }
#line 1892 "ldscript.c" /* yacc.c:1646  */
    break;

  case 57:
#line 417 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.version) = merge_versions ((yyvsp[-1].version), (yyvsp[0].version)); }
#line 1898 "ldscript.c" /* yacc.c:1646  */
    break;

  case 58:
#line 419 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.version) = (yyvsp[0].version); }
#line 1904 "ldscript.c" /* yacc.c:1646  */
    break;

  case 59:
#line 423 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.version) = new_version (NULL, (yyvsp[0].id_list)); }
#line 1910 "ldscript.c" /* yacc.c:1646  */
    break;

  case 60:
#line 425 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.version) = new_version ((yyvsp[0].id_list), NULL); }
#line 1916 "ldscript.c" /* yacc.c:1646  */
    break;

  case 61:
#line 430 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    {
		      struct id_list *newp = new_id_listelem ((yyvsp[-1].str));
		      newp->next = (yyvsp[-2].id_list)->next;
		      (yyval.id_list) = (yyvsp[-2].id_list)->next = newp;
		    }
#line 1926 "ldscript.c" /* yacc.c:1646  */
    break;

  case 62:
#line 436 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.id_list) = new_id_listelem ((yyvsp[-1].str)); }
#line 1932 "ldscript.c" /* yacc.c:1646  */
    break;

  case 63:
#line 440 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[0].str); }
#line 1938 "ldscript.c" /* yacc.c:1646  */
    break;

  case 64:
#line 442 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[0].str); }
#line 1944 "ldscript.c" /* yacc.c:1646  */
    break;

  case 65:
#line 446 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[0].str); }
#line 1950 "ldscript.c" /* yacc.c:1646  */
    break;

  case 66:
#line 448 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1646  */
    { (yyval.str) = NULL; }
#line 1956 "ldscript.c" /* yacc.c:1646  */
    break;


#line 1960 "ldscript.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
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

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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

#if !defined yyoverflow || YYERROR_VERBOSE
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
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
#line 451 "/home/mark/src/elfutils/src/ldscript.y" /* yacc.c:1906  */


static void
yyerror (const char *s)
{
  error (0, 0, (ld_scan_version_script
		? gettext ("while reading version script '%s': %s at line %d")
		: gettext ("while reading linker script '%s': %s at line %d")),
	 ldin_fname, gettext (s), ldlineno);
}


static struct expression *
new_expr (int tag)
{
  struct expression *newp = (struct expression *)
    obstack_alloc (&ld_state.smem, sizeof (*newp));

  newp->tag = tag;
  return newp;
}


static struct input_section_name *
new_input_section_name (const char *name, bool sort_flag)
{
  struct input_section_name *newp = (struct input_section_name *)
    obstack_alloc (&ld_state.smem, sizeof (*newp));

  newp->name = name;
  newp->sort_flag = sort_flag;
  return newp;
}


static struct input_rule *
new_input_rule (int tag)
{
  struct input_rule *newp = (struct input_rule *)
    obstack_alloc (&ld_state.smem, sizeof (*newp));

  newp->tag = tag;
  newp->next = newp;
  return newp;
}


static struct output_rule *
new_output_rule (int tag)
{
  struct output_rule *newp = (struct output_rule *)
    memset (obstack_alloc (&ld_state.smem, sizeof (*newp)),
	    '\0', sizeof (*newp));

  newp->tag = tag;
  newp->next = newp;
  return newp;
}


static struct assignment *
new_assignment (const char *variable, struct expression *expression,
		bool provide_flag)
{
  struct assignment *newp = (struct assignment *)
    obstack_alloc (&ld_state.smem, sizeof (*newp));

  newp->variable = variable;
  newp->expression = expression;
  newp->sym = NULL;
  newp->provide_flag = provide_flag;

  /* Insert the symbol into a hash table.  We will later have to matc*/
  return newp;
}


static void
new_segment (int mode, struct output_rule *output_rule)
{
  struct output_segment *newp;

  newp
    = (struct output_segment *) obstack_alloc (&ld_state.smem, sizeof (*newp));
  newp->mode = mode;
  newp->next = newp;

  newp->output_rules = output_rule->next;
  output_rule->next = NULL;

  /* Enqueue the output segment description.  */
  if (ld_state.output_segments == NULL)
    ld_state.output_segments = newp;
  else
    {
      newp->next = ld_state.output_segments->next;
      ld_state.output_segments = ld_state.output_segments->next = newp;
    }

  /* If the output file should be stripped of all symbol set the flag
     in the structures of all output sections.  */
  if (mode == 0 && ld_state.strip == strip_all)
    {
      struct output_rule *runp;

      for (runp = newp->output_rules; runp != NULL; runp = runp->next)
	if (runp->tag == output_section)
	  runp->val.section.ignored = true;
    }
}


static struct filename_list *
new_filename_listelem (const char *string)
{
  struct filename_list *newp;

  /* We use calloc and not the obstack since this object can be freed soon.  */
  newp = (struct filename_list *) xcalloc (1, sizeof (*newp));
  newp->name = string;
  newp->next = newp;
  return newp;
}


static struct filename_list *
mark_as_needed (struct filename_list *listp)
{
  struct filename_list *runp = listp;
  do
    {
      runp->as_needed = true;
      runp = runp->next;
    }
  while (runp != listp);

  return listp;
}


static void
add_inputfiles (struct filename_list *fnames)
{
  assert (fnames != NULL);

  if (ld_state.srcfiles == NULL)
    ld_state.srcfiles = fnames;
  else
    {
      struct filename_list *first = ld_state.srcfiles->next;

      ld_state.srcfiles->next = fnames->next;
      fnames->next = first;
      ld_state.srcfiles->next = fnames;
    }
}


static _Bool
special_char_p (const char *str)
{
  while (*str != '\0')
    {
      if (__builtin_expect (*str == '*', 0)
	  || __builtin_expect (*str == '?', 0)
	  || __builtin_expect (*str == '[', 0))
	return true;

      ++str;
    }

  return false;
}


static struct id_list *
new_id_listelem (const char *str)
{
  struct id_list *newp;

  newp = (struct id_list *) obstack_alloc (&ld_state.smem, sizeof (*newp));
  if (str == NULL)
    newp->u.id_type = id_all;
  else if (__builtin_expect (special_char_p (str), false))
    newp->u.id_type = id_wild;
  else
    newp->u.id_type = id_str;
  newp->id = str;
  newp->next = newp;

  return newp;
}


static struct version *
new_version (struct id_list *local, struct id_list *global)
{
  struct version *newp;

  newp = (struct version *) obstack_alloc (&ld_state.smem, sizeof (*newp));
  newp->next = newp;
  newp->local_names = local;
  newp->global_names = global;
  newp->versionname = NULL;
  newp->parentname = NULL;

  return newp;
}


static struct version *
merge_versions (struct version *one, struct version *two)
{
  assert (two->local_names == NULL || two->global_names == NULL);

  if (two->local_names != NULL)
    {
      if (one->local_names == NULL)
	one->local_names = two->local_names;
      else
	{
	  two->local_names->next = one->local_names->next;
	  one->local_names = one->local_names->next = two->local_names;
	}
    }
  else
    {
      if (one->global_names == NULL)
	one->global_names = two->global_names;
      else
	{
	  two->global_names->next = one->global_names->next;
	  one->global_names = one->global_names->next = two->global_names;
	}
    }

  return one;
}


static void
add_id_list (const char *versionname, struct id_list *runp, _Bool local)
{
  struct id_list *lastp = runp;

  if (runp == NULL)
    /* Nothing to do.  */
    return;

  /* Convert into a simple single-linked list.  */
  runp = runp->next;
  assert (runp != NULL);
  lastp->next = NULL;

  do
    if (runp->u.id_type == id_str)
      {
	struct id_list *curp;
	struct id_list *defp;
	unsigned long int hval = elf_hash (runp->id);

	curp = runp;
	runp = runp->next;

	defp = ld_version_str_tab_find (&ld_state.version_str_tab, hval, curp);
	if (defp != NULL)
	  {
	    /* There is already a version definition for this symbol.  */
	    while (strcmp (defp->u.s.versionname, versionname) != 0)
	      {
		if (defp->next == NULL)
		  {
		    /* No version like this so far.  */
		    defp->next = curp;
		    curp->u.s.local = local;
		    curp->u.s.versionname = versionname;
		    curp->next = NULL;
		    defp = NULL;
		    break;
		  }

		defp = defp->next;
	      }

	    if (defp != NULL && defp->u.s.local != local)
	      error (EXIT_FAILURE, 0, versionname[0] == '\0'
		     ? gettext ("\
symbol '%s' is declared both local and global for unnamed version '%s'")
		     : gettext ("\
symbol '%s' is declared both local and global for version '%s'"),
		     runp->id, versionname);
	  }
	else
	  {
	    /* This is the first version definition for this symbol.  */
	    ld_version_str_tab_insert (&ld_state.version_str_tab, hval, curp);

	    curp->u.s.local = local;
	    curp->u.s.versionname = versionname;
	    curp->next = NULL;
	  }
      }
    else if (runp->u.id_type == id_all)
      {
	if (local)
	  {
	    if (ld_state.default_bind_global)
	      error (EXIT_FAILURE, 0,
		     gettext ("default visibility set as local and global"));
	    ld_state.default_bind_local = true;
	  }
	else
	  {
	    if (ld_state.default_bind_local)
	      error (EXIT_FAILURE, 0,
		     gettext ("default visibility set as local and global"));
	    ld_state.default_bind_global = true;
	  }

	runp = runp->next;
      }
    else
      {
	assert (runp->u.id_type == id_wild);
	/* XXX TBI */
	abort ();
      }
  while (runp != NULL);
}


static void
add_versions (struct version *versions)
{
  struct version *lastp = versions;

  if (versions == NULL)
    return;

  /* Convert into a simple single-linked list.  */
  versions = versions->next;
  assert (versions != NULL);
  lastp->next = NULL;

  do
    {
      add_id_list (versions->versionname, versions->local_names, true);
      add_id_list (versions->versionname, versions->global_names, false);

      versions = versions->next;
    }
  while (versions != NULL);
}
