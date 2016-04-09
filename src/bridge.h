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

#include "gnu-c.gen.h"


struct Builder
{
    Pool *pool;
    GnuCNothing *nothing;

    /* grouping */
    GnuCSymArrow *arrow;
    GnuCSymComma *comma;
    GnuCSymDot *dot;
    GnuCSymEllipsis *ellipsis;
    GnuCSymLbrace *lbrace;
    GnuCSymLbracket *lbracket;
    GnuCSymLparen *lparen;
    GnuCSymRbrace *rbrace;
    GnuCSymRbracket *rbracket;
    GnuCSymRparen *rparen;
    GnuCSymSemicolon *semicolon;

    /* operators */
    GnuCSymAmpersand *ampersand;
    GnuCSymBang *bang;
    GnuCSymCaret *caret;
    GnuCSymColon *colon;
    GnuCSymDecr *decr;
    GnuCSymEq *eq;
    GnuCSymGe *ge;
    GnuCSymGt *gt;
    GnuCSymIncr *incr;
    GnuCSymLe *le;
    GnuCSymLogicalAnd *logical_and;
    GnuCSymLogicalOr *logical_or;
    GnuCSymLshift *lshift;
    GnuCSymLt *lt;
    GnuCSymMinus *minus;
    GnuCSymNe *ne;
    GnuCSymPercent *percent;
    GnuCSymPipe *pipe;
    GnuCSymPlus *plus;
    GnuCSymQuery *query;
    GnuCSymRshift *rshift;
    GnuCSymSlash *slash;
    GnuCSymStar *star;
    GnuCSymTilde *tilde;

    /* assignment operators */
    GnuCSymAssign *assign;
    GnuCSymAmpersandAssign *ampersand_assign;
    GnuCSymCaretAssign *caret_assign;
    GnuCSymLshiftAssign *lshift_assign;
    GnuCSymMinusAssign *minus_assign;
    GnuCSymPercentAssign *percent_assign;
    GnuCSymPipeAssign *pipe_assign;
    GnuCSymPlusAssign *plus_assign;
    GnuCSymRshiftAssign *rshift_assign;
    GnuCSymSlashAssign *slash_assign;
    GnuCSymStarAssign *star_assign;

    /* keywords */
    GnuCKwBreak *kw_break;
    GnuCKwCase *kw_case;
    GnuCKwConst *kw_const;
    GnuCKwContinue *kw_continue;
    GnuCKwDefault *kw_default;
    GnuCKwDo *kw_do;
    GnuCKwElse *kw_else;
    GnuCKwEnum *kw_enum;
    GnuCKwExtern *kw_extern;
    GnuCKwFor *kw_for;
    GnuCKwGoto *kw_goto;
    GnuCKwIf *kw_if;
    GnuCKwRegister *kw_register;
    GnuCKwReturn *kw_return;
    GnuCKwSizeof *kw_sizeof;
    GnuCKwStatic *kw_static;
    GnuCKwStruct *kw_struct;
    GnuCKwSwitch *kw_switch;
    GnuCKwTypedef *kw_typedef;
    GnuCKwUnion *kw_union;
    GnuCKwVolatile *kw_volatile;
    GnuCKwWhile *kw_while;

    /* types (in non-struct declarations) */
    GnuCAnyTailDeclarationSpecifiers *ty_void_ds;
    GnuCAnyTailDeclarationSpecifiers *ty_char_ds;
    GnuCAnyTailDeclarationSpecifiers *ty_signed_char_ds;
    GnuCAnyTailDeclarationSpecifiers *ty_unsigned_char_ds;

    GnuCAnyTailDeclarationSpecifiers *ty_signed_short_ds;
    GnuCAnyTailDeclarationSpecifiers *ty_unsigned_short_ds;
    GnuCAnyTailDeclarationSpecifiers *ty_signed_int_ds;
    GnuCAnyTailDeclarationSpecifiers *ty_unsigned_int_ds;
    GnuCAnyTailDeclarationSpecifiers *ty_signed_long_ds;
    GnuCAnyTailDeclarationSpecifiers *ty_unsigned_long_ds;
    GnuCAnyTailDeclarationSpecifiers *ty_signed_long_long_ds;
    GnuCAnyTailDeclarationSpecifiers *ty_unsigned_long_long_ds;

    GnuCAnyTailDeclarationSpecifiers *ty_float_ds;
    GnuCAnyTailDeclarationSpecifiers *ty_double_ds;
    GnuCAnyTailDeclarationSpecifiers *ty_long_double_ds;

    /* types (in struct declarations and bare type names) */
    GnuCAnySpecifierQualifierList *ty_void_sql;
    GnuCAnySpecifierQualifierList *ty_char_sql;
    GnuCAnySpecifierQualifierList *ty_signed_char_sql;
    GnuCAnySpecifierQualifierList *ty_unsigned_char_sql;

    GnuCAnySpecifierQualifierList *ty_signed_short_sql;
    GnuCAnySpecifierQualifierList *ty_unsigned_short_sql;
    GnuCAnySpecifierQualifierList *ty_signed_int_sql;
    GnuCAnySpecifierQualifierList *ty_unsigned_int_sql;
    GnuCAnySpecifierQualifierList *ty_signed_long_sql;
    GnuCAnySpecifierQualifierList *ty_unsigned_long_sql;
    GnuCAnySpecifierQualifierList *ty_signed_long_long_sql;
    GnuCAnySpecifierQualifierList *ty_unsigned_long_long_sql;

    GnuCAnySpecifierQualifierList *ty_float_sql;
    GnuCAnySpecifierQualifierList *ty_double_sql;
    GnuCAnySpecifierQualifierList *ty_long_double_sql;
};

struct BuildTranslationUnit
{
    GnuCAliasTranslationUnit *ast_tu;
};

struct BuildStatement
{
    GnuCOptLabels *ast_labels;
    GnuCAnyBlockItem *ast_unlabeled;
    GnuCAnyExternalDeclaration *ast_ext;
};

struct BuildStorageClass
{
    GnuCAnyHeadDeclarationSpecifier *ast_scs;
};

struct BuildParamDeclaration
{
    GnuCAnyParameterDeclaration *ast_param_decl;
};

struct BuildMemberDeclaration
{
    GnuCAnyStructDeclaration *ast_struct_decl;
};

struct BuildEnumerator
{
    GnuCAnyEnumerator *ast_enum;
};

enum BuildTypeType
{
    BTYTY_SPEC,
    BTYTY_PTR,
    BTYTY_ARRAY,
    BTYTY_FUN,
};
struct BuildTypeFlags
{
    bool is_const;
    bool is_volatile;
};
struct BuildType
{
    /* Types can be used in a lot of ways, many involving constructing
     * a declarator, which is backwards and inside out so can't be built. */
    enum BuildTypeType type;
    __extension__ union
    {
        struct
        {
            GnuCAnyTailDeclarationSpecifiers *ast_dspec;
            GnuCAnySpecifierQualifierList *ast_sql;
        } spec;
        struct
        {
            BuildType *element;
            void *_unused;
        } ptr;
        struct
        {
            BuildType *element;
            GnuCAnyArrayDeclarator *size;
        } array;
        struct
        {
            BuildType *ret;
            GnuCAnyParameterTypeList *args;
        } fun;
    };
    struct BuildTypeFlags flags;
};

struct BuildInitializer
{
    GnuCAnyInitializer *ast_init;
};

struct BuildExpression
{
    GnuCAnyExprList *ast_expr;
};


struct BuildTypePairDeclarator
{
    GnuCAnyDeclarationSpecifiers *specs;
    GnuCAnyDeclarator *decl;
};
struct BuildTypePairAbstractDeclarator
{
    GnuCAnyDeclarationSpecifiers *specs;
    GnuCOptAbstractDeclarator *decl;
};
struct BuildTypePairStructDeclarator
{
    GnuCAnySpecifierQualifierList *specs;
    GnuCAnyDeclarator *decl;
};
struct BuildTypePairAbstractStructDeclarator
{
    GnuCAnySpecifierQualifierList *specs;
    GnuCOptAbstractDeclarator *decl;
};


/* build helpers */
GnuCAtomIdentifier *build_id(Builder *b, const char *id);
GnuCAtomTypedefName *build_typedef_name(Builder *b, const char *id);


/* raw builders */
BuildTranslationUnit *build_tu_ast(Builder *b, GnuCAliasTranslationUnit *ast);
BuildStatement *build_stmt_ast(Builder *b, GnuCOptLabels *labels, GnuCAnyBlockItem *ast);
BuildStatement *build_decl_ast(Builder *b, GnuCAnyDeclaration *ast);
BuildStorageClass *build_storage_class_ast(Builder *b, GnuCAnyHeadDeclarationSpecifier *ast);
BuildParamDeclaration *build_param_ast(Builder *b, GnuCAnyParameterDeclaration *ast);
BuildMemberDeclaration *build_member_ast(Builder *b, GnuCAnyStructDeclaration *ast);
BuildEnumerator *build_enum_ast(Builder *b, GnuCAnyEnumerator *ast);
BuildType *build_type_spec_ast(Builder *b, GnuCAnyTailDeclarationSpecifiers *ds, GnuCAnySpecifierQualifierList *ast);
BuildType *build_type_ptr_ast(Builder *b, BuildType *target);
BuildType *build_type_array_ast(Builder *b, BuildType *element, GnuCAnyArrayDeclarator *size);
BuildType *build_type_fun_ast(Builder *b, BuildType *target, GnuCAnyParameterTypeList *args);
BuildType *build_type_copy_ast(Builder *b, BuildType copy);
BuildInitializer *build_init_ast(Builder *b, GnuCAnyInitializer *ast);
BuildExpression *build_expr_ast(Builder *b, GnuCAnyExprList *ast);


/* unbuilders & rebuilder wrappers */
BuildTypePairDeclarator build_type_to_decl(Builder *b, size_t nscs, BuildStorageClass **scs, BuildType *type, const char *name);
BuildTypePairAbstractDeclarator build_type_to_decl_abstract(Builder *b, size_t nscs, BuildStorageClass **scs, BuildType *type);
BuildTypePairStructDeclarator build_type_to_decl_struct(Builder *b, BuildType *type, const char *name);
BuildTypePairAbstractStructDeclarator build_type_to_decl_struct_abstract(Builder *b, BuildType *type);

GnuCAnyBlockItem *build_stmt_to_stmt(Builder *b, BuildStatement *stmt);
GnuCTreeCompoundStatement *build_stmt_to_compound(Builder *b, BuildStatement *stmt);
GnuCAnyStatement *build_stmt_to_else_body(Builder *b, BuildStatement *stmt);

GnuCAnyInitializer *build_init_to_init(Builder *b, BuildInitializer *init);

BuildExpression *build_expr_from_primary(Builder *b, GnuCAnyPrimaryExpression *expr);
BuildExpression *build_expr_from_postfix(Builder *b, GnuCAnyPostfixExpression *expr);
BuildExpression *build_expr_from_cast(Builder *b, GnuCAnyCastExpression *expr);
BuildExpression *build_expr_from_unary(Builder *b, GnuCAnyUnaryExpression *expr);
BuildExpression *build_expr_from_mul(Builder *b, GnuCAnyMultiplicativeExpression *expr);
BuildExpression *build_expr_from_add(Builder *b, GnuCAnyAdditiveExpression *expr);
BuildExpression *build_expr_from_shift(Builder *b, GnuCAnyShiftExpression *expr);
BuildExpression *build_expr_from_rel(Builder *b, GnuCAnyRelationalExpression *expr);
BuildExpression *build_expr_from_eq(Builder *b, GnuCAnyEqualityExpression *expr);
BuildExpression *build_expr_from_bit_and(Builder *b, GnuCAnyAndExpression *expr);
BuildExpression *build_expr_from_bit_xor(Builder *b, GnuCAnyExclusiveOrExpression *expr);
BuildExpression *build_expr_from_bit_or(Builder *b, GnuCAnyInclusiveOrExpression *expr);
BuildExpression *build_expr_from_log_and(Builder *b, GnuCAnyLogicalAndExpression *expr);
BuildExpression *build_expr_from_log_or(Builder *b, GnuCAnyLogicalOrExpression *expr);
BuildExpression *build_expr_from_conditional(Builder *b, GnuCAnyConditionalExpression *expr);
BuildExpression *build_expr_from_assignment(Builder *b, GnuCAnyAssignmentExpression *expr);
BuildExpression *build_expr_from_comma(Builder *b, GnuCAnyExprList *expr);

GnuCAnyPrimaryExpression *build_expr_to_primary(Builder *b, BuildExpression *expr);
GnuCAnyPostfixExpression *build_expr_to_postfix(Builder *b, BuildExpression *expr);
GnuCAnyCastExpression *build_expr_to_cast(Builder *b, BuildExpression *expr);
GnuCAnyUnaryExpression *build_expr_to_unary(Builder *b, BuildExpression *expr);
GnuCAnyMultiplicativeExpression *build_expr_to_mul(Builder *b, BuildExpression *expr);
GnuCAnyAdditiveExpression *build_expr_to_add(Builder *b, BuildExpression *expr);
GnuCAnyShiftExpression *build_expr_to_shift(Builder *b, BuildExpression *expr);
GnuCAnyRelationalExpression *build_expr_to_rel(Builder *b, BuildExpression *expr);
GnuCAnyEqualityExpression *build_expr_to_eq(Builder *b, BuildExpression *expr);
GnuCAnyAndExpression *build_expr_to_bit_and(Builder *b, BuildExpression *expr);
GnuCAnyExclusiveOrExpression *build_expr_to_bit_xor(Builder *b, BuildExpression *expr);
GnuCAnyInclusiveOrExpression *build_expr_to_bit_or(Builder *b, BuildExpression *expr);
GnuCAnyLogicalAndExpression *build_expr_to_log_and(Builder *b, BuildExpression *expr);
GnuCAnyLogicalOrExpression *build_expr_to_log_or(Builder *b, BuildExpression *expr);
GnuCAnyConditionalExpression *build_expr_to_conditional(Builder *b, BuildExpression *expr);
GnuCAnyAssignmentExpression *build_expr_to_assignment(Builder *b, BuildExpression *expr);
GnuCAnyExprList *build_expr_to_comma(Builder *b, BuildExpression *expr);
