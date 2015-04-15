#pragma once

#include "c89.h"

struct Builder
{
    Pool *pool;
    C89_Nothing *nothing;

    /* grouping */
    C89_TokArrow *arrow;
    C89_TokComma *comma;
    C89_TokDot *dot;
    C89_TokEllipsis *ellipsis;
    C89_TokLbrace *lbrace;
    C89_TokLbracket *lbracket;
    C89_TokLparen *lparen;
    C89_TokRbrace *rbrace;
    C89_TokRbracket *rbracket;
    C89_TokRparen *rparen;
    C89_TokSemicolon *semicolon;

    /* operators */
    C89_TokAmpersand *ampersand;
    C89_TokBang *bang;
    C89_TokCaret *caret;
    C89_TokColon *colon;
    C89_TokDecr *decr;
    C89_TokEq *eq;
    C89_TokGe *ge;
    C89_TokGt *gt;
    C89_TokIncr *incr;
    C89_TokLe *le;
    C89_TokLogicalAnd *logical_and;
    C89_TokLogicalOr *logical_or;
    C89_TokLshift *lshift;
    C89_TokLt *lt;
    C89_TokMinus *minus;
    C89_TokNe *ne;
    C89_TokPercent *percent;
    C89_TokPipe *pipe;
    C89_TokPlus *plus;
    C89_TokQuery *query;
    C89_TokRshift *rshift;
    C89_TokSlash *slash;
    C89_TokStar *star;
    C89_TokTilde *tilde;

    /* assignment operators */
    C89_TokAssign *assign;
    C89_TokAmpersandAssign *ampersand_assign;
    C89_TokCaretAssign *caret_assign;
    C89_TokLshiftAssign *lshift_assign;
    C89_TokMinusAssign *minus_assign;
    C89_TokPercentAssign *percent_assign;
    C89_TokPipeAssign *pipe_assign;
    C89_TokPlusAssign *plus_assign;
    C89_TokRshiftAssign *rshift_assign;
    C89_TokSlashAssign *slash_assign;
    C89_TokStarAssign *star_assign;

    /* keywords */
    C89_TokBreak *kw_break;
    C89_TokCase *kw_case;
    C89_TokConst *kw_const;
    C89_TokContinue *kw_continue;
    C89_TokDefault *kw_default;
    C89_TokDo *kw_do;
    C89_TokElse *kw_else;
    C89_TokEnum *kw_enum;
    C89_TokExtern *kw_extern;
    C89_TokFor *kw_for;
    C89_TokGoto *kw_goto;
    C89_TokIf *kw_if;
    C89_TokRegister *kw_register;
    C89_TokReturn *kw_return;
    C89_TokSizeof *kw_sizeof;
    C89_TokStatic *kw_static;
    C89_TokStruct *kw_struct;
    C89_TokSwitch *kw_switch;
    C89_TokTypedef *kw_typedef;
    C89_TokUnion *kw_union;
    C89_TokVolatile *kw_volatile;
    C89_TokWhile *kw_while;

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
C89_TokIdentifier *build_id(Builder *b, const char *id);
C89_TokTypedefName *build_typedef_name(Builder *b, const char *id);


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
