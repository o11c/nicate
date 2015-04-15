#include "bridge.h"
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

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "pool.h"


C89_TokIdentifier *build_id(Builder *b, const char *id)
{
    size_t i = 0;
    assert (id[i] == '_' || isalpha(id[i]));
    while (id[++i])
        assert (id[i] == '_' || isalnum(id[i]));
    return c89_create_tok_identifier(b->pool, id);
}
C89_TokTypedefName *build_typedef_name(Builder *b, const char *id)
{
    size_t i = 0;
    assert (id[i] == '_' || isalpha(id[i]));
    while (id[++i])
        assert (id[i] == '_' || isalnum(id[i]));
    return c89_create_tok_typedef_name(b->pool, id);
}

BuildTranslationUnit *build_tu_ast(Builder *b, C89_AnyTranslationUnit *ast)
{
    BuildTranslationUnit rv = {ast};
    return (BuildTranslationUnit *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildTopLevel *build_top_ast(Builder *b, C89_AnyExternalDeclaration *ast)
{
    BuildTopLevel rv = {ast};
    return (BuildTopLevel *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildStorageClass *build_storage_class_ast(Builder *b, C89_OptStorageClassSpecifier *ast)
{
    BuildStorageClass rv = {ast};
    return (BuildStorageClass *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildDeclaration *build_decl_ast(Builder *b, C89_TreeDeclaration *ast)
{
    BuildDeclaration rv = {ast};
    return (BuildDeclaration *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildParamDeclaration *build_param_ast(Builder *b, C89_AnyParameterDeclaration *ast)
{
    BuildParamDeclaration rv = {ast};
    return (BuildParamDeclaration *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildMemberDeclaration *build_member_ast(Builder *b, C89_TreeStructDeclaration *ast)
{
    BuildMemberDeclaration rv = {ast};
    return (BuildMemberDeclaration *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildEnumerator *build_enum_ast(Builder *b, C89_AnyEnumerator *ast)
{
    BuildEnumerator rv = {ast};
    return (BuildEnumerator *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildType *build_type_spec_ast(Builder *b, C89_AnyTypeSpecifier *ast, bool isc, bool isv)
{
    BuildType rv;
    rv.type = BTYTY_SPEC;
    rv.ast_spec = ast;
    rv.array_size = NULL;
    rv.is_const = isc;
    rv.is_volatile = isv;
    return (BuildType *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildType *build_type_ptr_ast(Builder *b, BuildType *target, bool isc, bool isv)
{
    BuildType rv;
    rv.type = BTYTY_PTR;
    rv.target = target;
    rv.array_size = NULL;
    rv.is_const = isc;
    rv.is_volatile = isv;
    return (BuildType *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildType *build_type_array_ast(Builder *b, BuildType *element, C89_OptConditionalExpression *size, bool isc, bool isv)
{
    BuildType rv;
    rv.type = BTYTY_ARRAY;
    rv.target = element;
    rv.array_size = size;
    rv.is_const = isc;
    rv.is_volatile = isv;
    assert (!element->is_const && !element->is_volatile);
    return (BuildType *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildType *build_type_fun_ast(Builder *b, BuildType *target, C89_OptParameterTypeList *args, bool isc, bool isv)
{
    BuildType rv;
    rv.type = BTYTY_FUN;
    rv.target = target;
    rv.args = args;
    rv.is_const = isc;
    rv.is_volatile = isv;
    return (BuildType *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildType *build_type_copy_ast(Builder *b, BuildType copy)
{
    return (BuildType *)pool_intern(b->pool, &copy, sizeof(copy));
}
BuildStatement *build_stmt_ast(Builder *b, C89_AnyStatement *ast)
{
    BuildStatement rv = {ast};
    return (BuildStatement *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildInitializer *build_init_ast(Builder *b, C89_AnyInitializer *ast)
{
    BuildInitializer rv = {ast};
    return (BuildInitializer *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildExpression *build_expr_ast(Builder *b, C89_AnyCommaExpression *ast)
{
    BuildExpression rv = {ast};
    return (BuildExpression *)pool_intern(b->pool, &rv, sizeof(rv));
}

/* The hard part. */
static C89_OptTypeQualifierList *build_tql(Builder *b, bool is_const, bool is_volatile)
{
    /* this approach won't scale, but meh */
    if (!is_const)
    {
        if (!is_volatile)
        {
            return (C89_OptTypeQualifierList *)b->nothing;
        }
        else
        {
            return (C89_OptTypeQualifierList *)b->kw_volatile;
        }
    }
    else
    {
        if (!is_volatile)
        {
            return (C89_OptTypeQualifierList *)b->kw_const;
        }
        else
        {
            C89_AnyTypeQualifierList *c = (C89_AnyTypeQualifierList *)b->kw_const;
            C89_AnyTypeQualifier *v = (C89_AnyTypeQualifier *)b->kw_volatile;
            C89_TreeMultiTypeQualifierList *mtql = c89_create_tree_multi_type_qualifier_list(b->pool, c, v);
            return (C89_OptTypeQualifierList *)mtql;
        }
    }
}

static C89_AnyDirectDeclarator *build_decl_direct(Builder *b, C89_AnyDeclarator *decl)
{
    if (c89_is_any_direct_declarator((C89_Ast *)decl))
    {
        return (C89_AnyDirectDeclarator *)decl;
    }
    else
    {
        C89_TreeParenthesisDeclarator *pd = c89_create_tree_parenthesis_declarator(b->pool, b->lparen, decl, b->rparen);
        return (C89_AnyDirectDeclarator *)pd;
    }
}
static BuildTypePairDeclarator build_type_to_decl_step(Builder *b, BuildType *type, C89_AnyDeclarator *decl)
{
    C89_OptTypeQualifierList *tql = build_tql(b, type->is_const, type->is_volatile);
    switch (type->type)
    {
    case BTYTY_SPEC:
        {
            BuildTypePairDeclarator rv;
            rv.sql = c89_create_tree_specifier_qualifier_list(b->pool, type->ast_spec, tql);
            rv.decl = decl;
            return rv;
        }
    case BTYTY_PTR:
        {
            C89_TreePointerDeclarator *pdecl = c89_create_tree_pointer_declarator(b->pool, b->star, tql, decl);
            decl = (C89_AnyDeclarator *)pdecl;
            type = type->target;
            return build_type_to_decl_step(b, type, decl);
        }
    case BTYTY_ARRAY:
        {
            C89_AnyDirectDeclarator *ddecl = build_decl_direct(b, decl);
            C89_TreeArrayDeclarator *adecl = c89_create_tree_array_declarator(b->pool, ddecl, b->lbracket, type->array_size, b->rbracket);
            decl = (C89_AnyDeclarator *)adecl;
            assert (!type->is_const && !type->is_volatile);
            type = type->target;
            return build_type_to_decl_step(b, type, decl);
        }
    case BTYTY_FUN:
        {
            C89_AnyDirectDeclarator *ddecl = build_decl_direct(b, decl);
            C89_TreeFunctionDeclarator *fdecl = c89_create_tree_function_declarator(b->pool, ddecl, b->lparen, type->args, b->rparen);
            decl = (C89_AnyDeclarator *)fdecl;
            assert (!type->is_const && !type->is_volatile);
            type = type->target;
            return build_type_to_decl_step(b, type, decl);
        }
    default:
        abort();
    }
}
static C89_OptDirectAbstractDeclarator *build_opt_abstract_decl_direct(Builder *b, C89_OptAbstractDeclarator *decl)
{
    if (c89_is_opt_direct_abstract_declarator((C89_Ast *)decl))
    {
        return (C89_OptDirectAbstractDeclarator *)decl;
    }
    else
    {
        C89_AnyAbstractDeclarator *some_decl = (C89_AnyAbstractDeclarator *)decl;
        C89_TreeParenthesisAbstractDeclarator *pad = c89_create_tree_parenthesis_abstract_declarator(b->pool, b->lparen, some_decl, b->rparen);
        return (C89_OptDirectAbstractDeclarator *)pad;
    }
}
static BuildTypePairAbstractDeclarator build_type_to_abstract_decl_step(Builder *b, BuildType *type, C89_OptAbstractDeclarator *decl)
{
    C89_OptTypeQualifierList *tql = build_tql(b, type->is_const, type->is_volatile);
    switch (type->type)
    {
    case BTYTY_SPEC:
        {
            BuildTypePairAbstractDeclarator rv;
            rv.sql = c89_create_tree_specifier_qualifier_list(b->pool, type->ast_spec, tql);
            rv.decl = decl;
            return rv;
        }
    case BTYTY_PTR:
        {
            if (c89_is_nothing((C89_Ast *)decl))
            {
                C89_TreeLeafPointerAbstractDeclarator *pdecl = c89_create_tree_leaf_pointer_abstract_declarator(b->pool, b->star, tql);
                decl = (C89_OptAbstractDeclarator *)pdecl;
            }
            else
            {
                C89_AnyAbstractDeclarator *tdecl = (C89_AnyAbstractDeclarator *)decl;
                C89_TreeRecursivePointerAbstractDeclarator *pdecl = c89_create_tree_recursive_pointer_abstract_declarator(b->pool, b->star, tql, tdecl);
                decl = (C89_OptAbstractDeclarator *)pdecl;
            }
            type = type->target;
            return build_type_to_abstract_decl_step(b, type, decl);
        }
    case BTYTY_ARRAY:
        {
            C89_OptDirectAbstractDeclarator *odadecl = build_opt_abstract_decl_direct(b, decl);
            C89_TreeArrayAbstractDeclarator *adecl = c89_create_tree_array_abstract_declarator(b->pool, odadecl, b->lbracket, type->array_size, b->rbracket);
            decl = (C89_OptAbstractDeclarator *)adecl;
            assert (!type->is_const && !type->is_volatile);
            type = type->target;
            return build_type_to_abstract_decl_step(b, type, decl);
        }
    case BTYTY_FUN:
        {
            C89_OptDirectAbstractDeclarator *odadecl = build_opt_abstract_decl_direct(b, decl);
            C89_TreeFunctionAbstractDeclarator *fdecl = c89_create_tree_function_abstract_declarator(b->pool, odadecl, b->lparen, type->args, b->rparen);
            decl = (C89_OptAbstractDeclarator *)fdecl;
            assert (!type->is_const && !type->is_volatile);
            type = type->target;
            return build_type_to_abstract_decl_step(b, type, decl);
        }
    default:
        abort();
    }
}
BuildTypePairDeclarator build_type_to_decl(Builder *b, BuildType *type, const char *name)
{
    C89_TokIdentifier *id = build_id(b, name);
    C89_AnyDeclarator *decl = (C89_AnyDeclarator *)id;
    return build_type_to_decl_step(b, type, decl);
}
BuildTypePairAbstractDeclarator build_type_to_decl_abstract(Builder *b, BuildType *type)
{
    C89_OptAbstractDeclarator *decl = (C89_OptAbstractDeclarator *)b->nothing;
    return build_type_to_abstract_decl_step(b, type, decl);
}

C89_AnyStatement *build_stmt_to_stmt(Builder *b, BuildStatement *stmt)
{
    (void)b;
    assert (c89_is_any_statement((C89_Ast *)stmt->ast_stmt));
    return (C89_AnyStatement *)stmt->ast_stmt;
}
C89_TreeCompoundStatement *build_stmt_to_compound(Builder *b, BuildStatement *stmt)
{
    if (c89_is_tree_compound_statement((C89_Ast *)stmt->ast_stmt))
    {
        return (C89_TreeCompoundStatement *)stmt->ast_stmt;
    }
    else
    {
        C89_OptDeclarationList *decls = (C89_OptDeclarationList *)b->nothing;
        C89_OptStatementList *stmts = (C89_OptStatementList *)build_stmt_to_stmt(b, stmt);
        return c89_create_tree_compound_statement(b->pool, b->lbrace, decls, stmts, b->rbrace);
    }
}

C89_AnyInitializer *build_init_to_init(Builder *b, BuildInitializer *init)
{
    (void)b;
    assert (c89_is_any_initializer((C89_Ast *)init->ast_init));
    return (C89_AnyInitializer *)init->ast_init;
}

BuildExpression *build_expr_from_primary(Builder *b, C89_AnyPrimaryExpression *expr)
{
    assert (c89_is_any_primary_expression((C89_Ast *)expr));
    return build_expr_ast(b, (C89_AnyCommaExpression *)expr);
}
BuildExpression *build_expr_from_postfix(Builder *b, C89_AnyPostfixExpression *expr)
{
    assert (c89_is_any_postfix_expression((C89_Ast *)expr));
    return build_expr_ast(b, (C89_AnyCommaExpression *)expr);
}
BuildExpression *build_expr_from_unary(Builder *b, C89_AnyUnaryExpression *expr)
{
    assert (c89_is_any_unary_expression((C89_Ast *)expr));
    return build_expr_ast(b, (C89_AnyCommaExpression *)expr);
}
BuildExpression *build_expr_from_mul(Builder *b, C89_AnyMultiplicativeExpression *expr)
{
    assert (c89_is_any_multiplicative_expression((C89_Ast *)expr));
    return build_expr_ast(b, (C89_AnyCommaExpression *)expr);
}
BuildExpression *build_expr_from_add(Builder *b, C89_AnyAdditiveExpression *expr)
{
    assert (c89_is_any_additive_expression((C89_Ast *)expr));
    return build_expr_ast(b, (C89_AnyCommaExpression *)expr);
}
BuildExpression *build_expr_from_shift(Builder *b, C89_AnyShiftExpression *expr)
{
    assert (c89_is_any_shift_expression((C89_Ast *)expr));
    return build_expr_ast(b, (C89_AnyCommaExpression *)expr);
}
BuildExpression *build_expr_from_rel(Builder *b, C89_AnyRelationalExpression *expr)
{
    assert (c89_is_any_relational_expression((C89_Ast *)expr));
    return build_expr_ast(b, (C89_AnyCommaExpression *)expr);
}
BuildExpression *build_expr_from_eq(Builder *b, C89_AnyEqualityExpression *expr)
{
    assert (c89_is_any_equality_expression((C89_Ast *)expr));
    return build_expr_ast(b, (C89_AnyCommaExpression *)expr);
}
BuildExpression *build_expr_from_bit_and(Builder *b, C89_AnyAndExpression *expr)
{
    assert (c89_is_any_and_expression((C89_Ast *)expr));
    return build_expr_ast(b, (C89_AnyCommaExpression *)expr);
}
BuildExpression *build_expr_from_bit_xor(Builder *b, C89_AnyExclusiveOrExpression *expr)
{
    assert (c89_is_any_exclusive_or_expression((C89_Ast *)expr));
    return build_expr_ast(b, (C89_AnyCommaExpression *)expr);
}
BuildExpression *build_expr_from_bit_or(Builder *b, C89_AnyInclusiveOrExpression *expr)
{
    assert (c89_is_any_inclusive_or_expression((C89_Ast *)expr));
    return build_expr_ast(b, (C89_AnyCommaExpression *)expr);
}
BuildExpression *build_expr_from_log_and(Builder *b, C89_AnyLogicalAndExpression *expr)
{
    assert (c89_is_any_logical_and_expression((C89_Ast *)expr));
    return build_expr_ast(b, (C89_AnyCommaExpression *)expr);
}
BuildExpression *build_expr_from_log_or(Builder *b, C89_AnyLogicalOrExpression *expr)
{
    assert (c89_is_any_logical_or_expression((C89_Ast *)expr));
    return build_expr_ast(b, (C89_AnyCommaExpression *)expr);
}
BuildExpression *build_expr_from_conditional(Builder *b, C89_AnyConditionalExpression *expr)
{
    assert (c89_is_any_conditional_expression((C89_Ast *)expr));
    return build_expr_ast(b, (C89_AnyCommaExpression *)expr);
}
BuildExpression *build_expr_from_assignment(Builder *b, C89_AnyAssignmentExpression *expr)
{
    assert (c89_is_any_assignment_expression((C89_Ast *)expr));
    return build_expr_ast(b, (C89_AnyCommaExpression *)expr);
}
BuildExpression *build_expr_from_comma(Builder *b, C89_AnyCommaExpression *expr)
{
    assert (c89_is_any_comma_expression((C89_Ast *)expr));
    return build_expr_ast(b, (C89_AnyCommaExpression *)expr);
}

static C89_TreeParenthesisExpression *c89_expr_parenthesize(Builder *b, BuildExpression *expr)
{
    C89_AnyCommaExpression *comma = build_expr_to_comma(b, expr);
    return c89_create_tree_parenthesis_expression(b->pool, b->lparen, comma, b->rparen);
}
C89_AnyPrimaryExpression *build_expr_to_primary(Builder *b, BuildExpression *expr)
{
    if (c89_is_any_primary_expression((C89_Ast *)expr->ast_expr))
        return (C89_AnyPrimaryExpression *)expr->ast_expr;
    return (C89_AnyPrimaryExpression *)c89_expr_parenthesize(b, expr);
}
C89_AnyPostfixExpression *build_expr_to_postfix(Builder *b, BuildExpression *expr)
{
    if (c89_is_any_postfix_expression((C89_Ast *)expr->ast_expr))
        return (C89_AnyPostfixExpression *)expr->ast_expr;
    return (C89_AnyPostfixExpression *)c89_expr_parenthesize(b, expr);
}
C89_AnyUnaryExpression *build_expr_to_unary(Builder *b, BuildExpression *expr)
{
    if (c89_is_any_unary_expression((C89_Ast *)expr->ast_expr))
        return (C89_AnyUnaryExpression *)expr->ast_expr;
    return (C89_AnyUnaryExpression *)c89_expr_parenthesize(b, expr);
}
C89_AnyMultiplicativeExpression *build_expr_to_mul(Builder *b, BuildExpression *expr)
{
    if (c89_is_any_multiplicative_expression((C89_Ast *)expr->ast_expr))
        return (C89_AnyMultiplicativeExpression *)expr->ast_expr;
    return (C89_AnyMultiplicativeExpression *)c89_expr_parenthesize(b, expr);
}
C89_AnyAdditiveExpression *build_expr_to_add(Builder *b, BuildExpression *expr)
{
    if (c89_is_any_additive_expression((C89_Ast *)expr->ast_expr))
        return (C89_AnyAdditiveExpression *)expr->ast_expr;
    return (C89_AnyAdditiveExpression *)c89_expr_parenthesize(b, expr);
}
C89_AnyShiftExpression *build_expr_to_shift(Builder *b, BuildExpression *expr)
{
    if (c89_is_any_shift_expression((C89_Ast *)expr->ast_expr))
        return (C89_AnyShiftExpression *)expr->ast_expr;
    return (C89_AnyShiftExpression *)c89_expr_parenthesize(b, expr);
}
C89_AnyRelationalExpression *build_expr_to_rel(Builder *b, BuildExpression *expr)
{
    if (c89_is_any_relational_expression((C89_Ast *)expr->ast_expr))
        return (C89_AnyRelationalExpression *)expr->ast_expr;
    return (C89_AnyRelationalExpression *)c89_expr_parenthesize(b, expr);
}
C89_AnyEqualityExpression *build_expr_to_eq(Builder *b, BuildExpression *expr)
{
    if (c89_is_any_equality_expression((C89_Ast *)expr->ast_expr))
        return (C89_AnyEqualityExpression *)expr->ast_expr;
    return (C89_AnyEqualityExpression *)c89_expr_parenthesize(b, expr);
}
C89_AnyAndExpression *build_expr_to_bit_and(Builder *b, BuildExpression *expr)
{
    if (c89_is_any_and_expression((C89_Ast *)expr->ast_expr))
        return (C89_AnyAndExpression *)expr->ast_expr;
    return (C89_AnyAndExpression *)c89_expr_parenthesize(b, expr);
}
C89_AnyExclusiveOrExpression *build_expr_to_bit_xor(Builder *b, BuildExpression *expr)
{
    if (c89_is_any_exclusive_or_expression((C89_Ast *)expr->ast_expr))
        return (C89_AnyExclusiveOrExpression *)expr->ast_expr;
    return (C89_AnyExclusiveOrExpression *)c89_expr_parenthesize(b, expr);
}
C89_AnyInclusiveOrExpression *build_expr_to_bit_or(Builder *b, BuildExpression *expr)
{
    if (c89_is_any_inclusive_or_expression((C89_Ast *)expr->ast_expr))
        return (C89_AnyInclusiveOrExpression *)expr->ast_expr;
    return (C89_AnyInclusiveOrExpression *)c89_expr_parenthesize(b, expr);
}
C89_AnyLogicalAndExpression *build_expr_to_log_and(Builder *b, BuildExpression *expr)
{
    if (c89_is_any_logical_and_expression((C89_Ast *)expr->ast_expr))
        return (C89_AnyLogicalAndExpression *)expr->ast_expr;
    return (C89_AnyLogicalAndExpression *)c89_expr_parenthesize(b, expr);
}
C89_AnyLogicalOrExpression *build_expr_to_log_or(Builder *b, BuildExpression *expr)
{
    if (c89_is_any_logical_or_expression((C89_Ast *)expr->ast_expr))
        return (C89_AnyLogicalOrExpression *)expr->ast_expr;
    return (C89_AnyLogicalOrExpression *)c89_expr_parenthesize(b, expr);
}
C89_AnyConditionalExpression *build_expr_to_conditional(Builder *b, BuildExpression *expr)
{
    if (c89_is_any_conditional_expression((C89_Ast *)expr->ast_expr))
        return (C89_AnyConditionalExpression *)expr->ast_expr;
    return (C89_AnyConditionalExpression *)c89_expr_parenthesize(b, expr);
}
C89_AnyAssignmentExpression *build_expr_to_assignment(Builder *b, BuildExpression *expr)
{
    if (c89_is_any_assignment_expression((C89_Ast *)expr->ast_expr))
        return (C89_AnyAssignmentExpression *)expr->ast_expr;
    return (C89_AnyAssignmentExpression *)c89_expr_parenthesize(b, expr);
}
C89_AnyCommaExpression *build_expr_to_comma(Builder *b, BuildExpression *expr)
{
    (void)b;
    assert (c89_is_any_comma_expression((C89_Ast *)expr->ast_expr));
    return (C89_AnyCommaExpression *)expr->ast_expr;
}
