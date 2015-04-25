#pragma once
/*
    Copyright © 2015 Ben Longbons

    This file is part of Nicate.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "c89.gen.h"


struct Builder
{
    Pool *pool;
    C89_Nothing *nothing;

    /* grouping */
    C89_SymArrow *arrow;
    C89_SymComma *comma;
    C89_SymDot *dot;
    C89_SymEllipsis *ellipsis;
    C89_SymLbrace *lbrace;
    C89_SymLbracket *lbracket;
    C89_SymLparen *lparen;
    C89_SymRbrace *rbrace;
    C89_SymRbracket *rbracket;
    C89_SymRparen *rparen;
    C89_SymSemicolon *semicolon;

    /* operators */
    C89_SymAmpersand *ampersand;
    C89_SymBang *bang;
    C89_SymCaret *caret;
    C89_SymColon *colon;
    C89_SymDecr *decr;
    C89_SymEq *eq;
    C89_SymGe *ge;
    C89_SymGt *gt;
    C89_SymIncr *incr;
    C89_SymLe *le;
    C89_SymLogicalAnd *logical_and;
    C89_SymLogicalOr *logical_or;
    C89_SymLshift *lshift;
    C89_SymLt *lt;
    C89_SymMinus *minus;
    C89_SymNe *ne;
    C89_SymPercent *percent;
    C89_SymPipe *pipe;
    C89_SymPlus *plus;
    C89_SymQuery *query;
    C89_SymRshift *rshift;
    C89_SymSlash *slash;
    C89_SymStar *star;
    C89_SymTilde *tilde;

    /* assignment operators */
    C89_SymAssign *assign;
    C89_SymAmpersandAssign *ampersand_assign;
    C89_SymCaretAssign *caret_assign;
    C89_SymLshiftAssign *lshift_assign;
    C89_SymMinusAssign *minus_assign;
    C89_SymPercentAssign *percent_assign;
    C89_SymPipeAssign *pipe_assign;
    C89_SymPlusAssign *plus_assign;
    C89_SymRshiftAssign *rshift_assign;
    C89_SymSlashAssign *slash_assign;
    C89_SymStarAssign *star_assign;

    /* keywords */
    C89_KwBreak *kw_break;
    C89_KwCase *kw_case;
    C89_KwConst *kw_const;
    C89_KwContinue *kw_continue;
    C89_KwDefault *kw_default;
    C89_KwDo *kw_do;
    C89_KwElse *kw_else;
    C89_KwEnum *kw_enum;
    C89_KwExtern *kw_extern;
    C89_KwFor *kw_for;
    C89_KwGoto *kw_goto;
    C89_KwIf *kw_if;
    C89_KwRegister *kw_register;
    C89_KwReturn *kw_return;
    C89_KwSizeof *kw_sizeof;
    C89_KwStatic *kw_static;
    C89_KwStruct *kw_struct;
    C89_KwSwitch *kw_switch;
    C89_KwTypedef *kw_typedef;
    C89_KwUnion *kw_union;
    C89_KwVolatile *kw_volatile;
    C89_KwWhile *kw_while;

    /* types */
    C89_AnyTypeSpecifier *ty_void;
    C89_AnyTypeSpecifier *ty_char;
    C89_AnyTypeSpecifier *ty_signed_char;
    C89_AnyTypeSpecifier *ty_unsigned_char;

    C89_AnyTypeSpecifier *ty_signed_short;
    C89_AnyTypeSpecifier *ty_unsigned_short;
    C89_AnyTypeSpecifier *ty_signed_int;
    C89_AnyTypeSpecifier *ty_unsigned_int;
    C89_AnyTypeSpecifier *ty_signed_long;
    C89_AnyTypeSpecifier *ty_unsigned_long;
    C89_AnyTypeSpecifier *ty_signed_long_long;
    C89_AnyTypeSpecifier *ty_unsigned_long_long;

    C89_AnyTypeSpecifier *ty_float;
    C89_AnyTypeSpecifier *ty_double;
    C89_AnyTypeSpecifier *ty_long_double;
};

struct BuildTranslationUnit
{
    C89_AnyTranslationUnit *ast_tu;
};

struct BuildTopLevel
{
    C89_AnyExternalDeclaration *ast_ext;
};

struct BuildStorageClass
{
    C89_OptStorageClassSpecifier *ast_scs;
};

struct BuildDeclaration
{
    C89_TreeDeclaration *ast_decl;
};

struct BuildParamDeclaration
{
    C89_AnyParameterDeclaration *ast_param_decl;
};

struct BuildMemberDeclaration
{
    C89_TreeStructDeclaration *ast_struct_decl;
};

struct BuildEnumerator
{
    C89_AnyEnumerator *ast_enum;
};

enum BuildTypeType
{
    BTYTY_SPEC,
    BTYTY_PTR,
    BTYTY_ARRAY,
    BTYTY_FUN,
};
struct BuildType
{
    /* Types can be used in a lot of ways, many involving constructing
     * a declarator, which is backwards and inside out so can't be built. */
    enum BuildTypeType type;
    union
    {
        /* only for BTYTY_SPEC */
        C89_AnyTypeSpecifier *ast_spec;
        /* only for BTYTY_PTR, BTYTY_ARRAY, and BTYTY_FUN */
        BuildType *target;
    };
    union
    {
        /* only for BTYTY_ARRAY */
        C89_OptConditionalExpression *array_size;
        /* only for BTYTY_FUN */
        C89_OptParameterTypeList *args;
    };
    bool is_const;
    bool is_volatile;
};

struct BuildStatement
{
    C89_AnyStatement *ast_stmt;
};

struct BuildInitializer
{
    C89_AnyInitializer *ast_init;
};

struct BuildExpression
{
    C89_AnyCommaExpression *ast_expr;
};


struct BuildTypePairDeclarator
{
    C89_TreeSpecifierQualifierList *sql;
    C89_AnyDeclarator *decl;
};
struct BuildTypePairAbstractDeclarator
{
    C89_TreeSpecifierQualifierList *sql;
    C89_OptAbstractDeclarator *decl;
};


/* build helpers */
C89_AtomIdentifier *build_id(Builder *b, const char *id);
C89_AtomTypedefName *build_typedef_name(Builder *b, const char *id);


/* raw builders */
BuildTranslationUnit *build_tu_ast(Builder *b, C89_AnyTranslationUnit *ast);
BuildTopLevel *build_top_ast(Builder *b, C89_AnyExternalDeclaration *ast);
BuildStorageClass *build_storage_class_ast(Builder *b, C89_OptStorageClassSpecifier *ast);
BuildDeclaration *build_decl_ast(Builder *b, C89_TreeDeclaration *ast);
BuildParamDeclaration *build_param_ast(Builder *b, C89_AnyParameterDeclaration *ast);
BuildMemberDeclaration *build_member_ast(Builder *b, C89_TreeStructDeclaration *ast);
BuildEnumerator *build_enum_ast(Builder *b, C89_AnyEnumerator *ast);
BuildType *build_type_spec_ast(Builder *b, C89_AnyTypeSpecifier *ast, bool isc, bool isv);
BuildType *build_type_ptr_ast(Builder *b, BuildType *target, bool isc, bool isv);
BuildType *build_type_array_ast(Builder *b, BuildType *element, C89_OptConditionalExpression *size, bool isc, bool isv);
BuildType *build_type_fun_ast(Builder *b, BuildType *target, C89_OptParameterTypeList *args, bool isc, bool isv);
BuildType *build_type_copy_ast(Builder *b, BuildType copy);
BuildStatement *build_stmt_ast(Builder *b, C89_AnyStatement *ast);
BuildInitializer *build_init_ast(Builder *b, C89_AnyInitializer *ast);
BuildExpression *build_expr_ast(Builder *b, C89_AnyCommaExpression *ast);


/* unbuilders & rebuilder wrappers */
BuildTypePairDeclarator build_type_to_decl(Builder *b, BuildType *type, const char *name);
BuildTypePairAbstractDeclarator build_type_to_decl_abstract(Builder *b, BuildType *type);

C89_AnyStatement *build_stmt_to_stmt(Builder *b, BuildStatement *stmt);
C89_TreeCompoundStatement *build_stmt_to_compound(Builder *b, BuildStatement *stmt);
C89_AnyStatement *build_stmt_to_else_body(Builder *b, BuildStatement *stmt);

C89_AnyInitializer *build_init_to_init(Builder *b, BuildInitializer *init);

BuildExpression *build_expr_from_primary(Builder *b, C89_AnyPrimaryExpression *expr);
BuildExpression *build_expr_from_postfix(Builder *b, C89_AnyPostfixExpression *expr);
BuildExpression *build_expr_from_unary(Builder *b, C89_AnyUnaryExpression *expr);
BuildExpression *build_expr_from_mul(Builder *b, C89_AnyMultiplicativeExpression *expr);
BuildExpression *build_expr_from_add(Builder *b, C89_AnyAdditiveExpression *expr);
BuildExpression *build_expr_from_shift(Builder *b, C89_AnyShiftExpression *expr);
BuildExpression *build_expr_from_rel(Builder *b, C89_AnyRelationalExpression *expr);
BuildExpression *build_expr_from_eq(Builder *b, C89_AnyEqualityExpression *expr);
BuildExpression *build_expr_from_bit_and(Builder *b, C89_AnyAndExpression *expr);
BuildExpression *build_expr_from_bit_xor(Builder *b, C89_AnyExclusiveOrExpression *expr);
BuildExpression *build_expr_from_bit_or(Builder *b, C89_AnyInclusiveOrExpression *expr);
BuildExpression *build_expr_from_log_and(Builder *b, C89_AnyLogicalAndExpression *expr);
BuildExpression *build_expr_from_log_or(Builder *b, C89_AnyLogicalOrExpression *expr);
BuildExpression *build_expr_from_conditional(Builder *b, C89_AnyConditionalExpression *expr);
BuildExpression *build_expr_from_assignment(Builder *b, C89_AnyAssignmentExpression *expr);
BuildExpression *build_expr_from_comma(Builder *b, C89_AnyCommaExpression *expr);

C89_AnyPrimaryExpression *build_expr_to_primary(Builder *b, BuildExpression *expr);
C89_AnyPostfixExpression *build_expr_to_postfix(Builder *b, BuildExpression *expr);
C89_AnyUnaryExpression *build_expr_to_unary(Builder *b, BuildExpression *expr);
C89_AnyMultiplicativeExpression *build_expr_to_mul(Builder *b, BuildExpression *expr);
C89_AnyAdditiveExpression *build_expr_to_add(Builder *b, BuildExpression *expr);
C89_AnyShiftExpression *build_expr_to_shift(Builder *b, BuildExpression *expr);
C89_AnyRelationalExpression *build_expr_to_rel(Builder *b, BuildExpression *expr);
C89_AnyEqualityExpression *build_expr_to_eq(Builder *b, BuildExpression *expr);
C89_AnyAndExpression *build_expr_to_bit_and(Builder *b, BuildExpression *expr);
C89_AnyExclusiveOrExpression *build_expr_to_bit_xor(Builder *b, BuildExpression *expr);
C89_AnyInclusiveOrExpression *build_expr_to_bit_or(Builder *b, BuildExpression *expr);
C89_AnyLogicalAndExpression *build_expr_to_log_and(Builder *b, BuildExpression *expr);
C89_AnyLogicalOrExpression *build_expr_to_log_or(Builder *b, BuildExpression *expr);
C89_AnyConditionalExpression *build_expr_to_conditional(Builder *b, BuildExpression *expr);
C89_AnyAssignmentExpression *build_expr_to_assignment(Builder *b, BuildExpression *expr);
C89_AnyCommaExpression *build_expr_to_comma(Builder *b, BuildExpression *expr);
