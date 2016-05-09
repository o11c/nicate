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
#include <string.h>

#include "pool.h"


GnuCAtomIdentifier *build_id(Builder *b, const char *id)
{
    size_t i = 0;
    assert (id[i] == '_' || isalpha(id[i]));
    while (id[++i])
        assert (id[i] == '_' || isalnum(id[i]));
    return gnu_c_create_atom_identifier(b->pool, id);
}
GnuCAtomTypedefName *build_typedef_name(Builder *b, const char *id)
{
    size_t i = 0;
    assert (id[i] == '_' || isalpha(id[i]));
    while (id[++i])
        assert (id[i] == '_' || isalnum(id[i]));
    return gnu_c_create_atom_typedef_name(b->pool, id);
}

BuildTranslationUnit *build_tu_ast(Builder *b, GnuCAliasTranslationUnit *ast)
{
    BuildTranslationUnit rv = {ast};
    return (BuildTranslationUnit *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildStatement *build_stmt_ast(Builder *b, GnuCOptLabels *labels, GnuCAnyBlockItem *ast)
{
    BuildStatement rv = {labels, ast, NULL};
    return (BuildStatement *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildStatement *build_decl_ast(Builder *b, GnuCTreeDeclaration *ast)
{
    BuildStatement rv = {NULL, (GnuCAnyBlockItem *)ast, (GnuCAnyExternalDeclaration *)ast};
    return (BuildStatement *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildStatement *build_comment_ast(Builder *b, GnuCAtomComment *ast)
{
    BuildStatement rv = {NULL, (GnuCAnyBlockItem *)ast, (GnuCAnyExternalDeclaration *)ast};
    return (BuildStatement *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildStorageClass *build_storage_class_ast(Builder *b, GnuCAnyHeadDeclarationSpecifier *ast)
{
    BuildStorageClass rv = {ast};
    return (BuildStorageClass *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildParamDeclaration *build_param_ast(Builder *b, GnuCAnyParameterDeclaration *ast)
{
    BuildParamDeclaration rv = {ast};
    return (BuildParamDeclaration *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildMemberDeclaration *build_member_ast(Builder *b, GnuCAnyStructDeclaration *ast)
{
    BuildMemberDeclaration rv = {ast};
    return (BuildMemberDeclaration *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildEnumerator *build_enum_ast(Builder *b, GnuCAnyEnumerator *ast)
{
    BuildEnumerator rv = {ast};
    return (BuildEnumerator *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildType *build_type_spec_ast(Builder *b, GnuCTreeTailDeclarationSpecifiers *ds, GnuCAnySpecifierQualifierList *sql)
{
    BuildType rv;
    rv.type = BTYTY_SPEC;
    rv.spec.ast_dspec = ds;
    rv.spec.ast_sql = sql;
    memset(&rv.flags, '\0', sizeof(rv.flags));
    return (BuildType *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildType *build_type_ptr_ast(Builder *b, BuildType *target)
{
    BuildType rv;
    rv.type = BTYTY_PTR;
    rv.ptr.element = target;
    rv.ptr._unused = NULL;
    memset(&rv.flags, '\0', sizeof(rv.flags));
    return (BuildType *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildType *build_type_array_ast(Builder *b, BuildType *element, GnuCAnyArrayDeclarator *size)
{
    BuildType rv;
    rv.type = BTYTY_ARRAY;
    rv.array.element = element;
    rv.array.size = size;
    memset(&rv.flags, '\0', sizeof(rv.flags));
    return (BuildType *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildType *build_type_fun_ast(Builder *b, BuildType *target, GnuCAnyParameterTypeList *args)
{
    BuildType rv;
    rv.type = BTYTY_FUN;
    rv.fun.ret = target;
    rv.fun.args = args;
    memset(&rv.flags, '\0', sizeof(rv.flags));
    return (BuildType *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildType *build_type_copy_ast(Builder *b, BuildType copy)
{
    return (BuildType *)pool_intern(b->pool, &copy, sizeof(copy));
}
BuildInitializer *build_init_ast(Builder *b, GnuCAnyInitializer *ast)
{
    BuildInitializer rv = {ast};
    return (BuildInitializer *)pool_intern(b->pool, &rv, sizeof(rv));
}
BuildExpression *build_expr_ast(Builder *b, GnuCAnyExprList *ast)
{
    BuildExpression rv = {ast};
    return (BuildExpression *)pool_intern(b->pool, &rv, sizeof(rv));
}

/* The hard part. */
static GnuCOptTypeQualifierList *build_tql(Builder *b, struct BuildTypeFlags flags)
{
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCOptTypeQualifierList *rv = (GnuCOptTypeQualifierList *)b->nothing;
    if (flags.is_const)
    {
        GnuCTreeTagQListQualifier *tmp = gnu_c_create_tree_tag_q_list_qualifier(b->pool, rv, (GnuCAnyTypeQualifier *)b->kw_const, no_attrs);
        rv = (GnuCOptTypeQualifierList *)tmp;
    }
    if (flags.is_volatile)
    {
        GnuCTreeTagQListQualifier *tmp = gnu_c_create_tree_tag_q_list_qualifier(b->pool, rv, (GnuCAnyTypeQualifier *)b->kw_const, no_attrs);
        rv = (GnuCOptTypeQualifierList *)tmp;
    }
    return rv;
}
static GnuCTreeTailDeclarationSpecifiers *build_ds(Builder *b, GnuCTreeTailDeclarationSpecifiers *specs, struct BuildTypeFlags flags)
{
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCOptHeadDeclarationSpecifiers *no_head = (GnuCOptHeadDeclarationSpecifiers *)b->nothing;
    if (flags.is_const)
    {
        specs = gnu_c_create_tree_tail_declaration_specifiers(b->pool, (GnuCOptTailDeclarationSpecifiers *)specs, (GnuCAnyTailDeclarationSpecifier *)b->kw_const, no_attrs, no_head);
    }
    if (flags.is_volatile)
    {
        specs = gnu_c_create_tree_tail_declaration_specifiers(b->pool, (GnuCOptTailDeclarationSpecifiers *)specs, (GnuCAnyTailDeclarationSpecifier *)b->kw_volatile, no_attrs, no_head);
    }
    return specs;
}
static GnuCAnySpecifierQualifierList *build_sql(Builder *b, GnuCAnySpecifierQualifierList *specs, struct BuildTypeFlags flags)
{
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    if (flags.is_const)
    {
        GnuCTreeTagSqListQualifier *tmp = gnu_c_create_tree_tag_sq_list_qualifier(b->pool, (GnuCOptSpecifierQualifierList *)specs, (GnuCAnyTypeQualifier *)b->kw_const, no_attrs);
        specs = (GnuCAnySpecifierQualifierList *)tmp;
    }
    if (flags.is_volatile)
    {
        GnuCTreeTagSqListQualifier *tmp = gnu_c_create_tree_tag_sq_list_qualifier(b->pool, (GnuCOptSpecifierQualifierList *)specs, (GnuCAnyTypeQualifier *)b->kw_volatile, no_attrs);
        specs = (GnuCAnySpecifierQualifierList *)tmp;
    }
    return specs;
}
static GnuCAnyDirectDeclarator *build_decl_direct(Builder *b, GnuCTreeDeclarator *decl)
{
    if (gnu_c_is_any_direct_declarator((GnuCAst *)decl))
    {
        return (GnuCAnyDirectDeclarator *)decl;
    }
    else
    {
        GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
        GnuCTreeTagParenDirectDeclarator *pd = gnu_c_create_tree_tag_paren_direct_declarator(b->pool, b->lparen, no_attrs, decl, b->rparen);
        return (GnuCAnyDirectDeclarator *)pd;
    }
}
static GnuCTreeDeclarator *build_ptr(Builder *b, GnuCTreeDeclarator *decl, struct BuildTypeFlags flags)
{
    GnuCTreePointer *pointer = gnu_c_create_tree_pointer(b->pool, b->star, (GnuCOptTypeQualifierList *)build_tql(b, flags), (GnuCOptPointer *)b->nothing);
    GnuCAnyDirectDeclarator *ddecl = build_decl_direct(b, decl);
    GnuCTreeDeclarator *rv = gnu_c_create_tree_declarator(b->pool, (GnuCOptPointer *)pointer, ddecl);
    decl = (GnuCTreeDeclarator *)rv;
    return decl;
}

static BuildTypePairDeclarator build_type_to_decl_step(Builder *b, BuildType *type, GnuCTreeDeclarator *decl)
{
    switch (type->type)
    {
    case BTYTY_SPEC:
        {
            BuildTypePairDeclarator rv;
            rv.specs = (GnuCAnyDeclarationSpecifiers *)build_ds(b, type->spec.ast_dspec, type->flags);
            rv.decl = decl;
            return rv;
        }
    case BTYTY_PTR:
        {
            decl = build_ptr(b, decl, type->flags);
            type = type->ptr.element;
            return build_type_to_decl_step(b, type, decl);
        }
    case BTYTY_ARRAY:
        {
            GnuCAnyDirectDeclarator *ddecl = build_decl_direct(b, decl);
            GnuCTreeTagArrayDirectDeclarator *adecl = gnu_c_create_tree_tag_array_direct_declarator(b->pool, ddecl, type->array.size);
            decl = (GnuCTreeDeclarator *)adecl;
            assert (!type->flags.is_const && !type->flags.is_volatile);
            type = type->array.element;
            return build_type_to_decl_step(b, type, decl);
        }
    case BTYTY_FUN:
        {
            GnuCAnyDirectDeclarator *ddecl = build_decl_direct(b, decl);
            GnuCTreeTagFunDirectDeclarator *fdecl = gnu_c_create_tree_tag_fun_direct_declarator(b->pool, ddecl, b->lparen, type->fun.args, b->rparen);
            decl = (GnuCTreeDeclarator *)fdecl;
            assert (!type->flags.is_const && !type->flags.is_volatile);
            type = type->fun.ret;
            return build_type_to_decl_step(b, type, decl);
        }
    default:
        abort();
    }
}
static BuildTypePairStructDeclarator build_type_to_decl_struct_step(Builder *b, BuildType *type, GnuCTreeDeclarator *decl)
{
    switch (type->type)
    {
    case BTYTY_SPEC:
        {
            BuildTypePairStructDeclarator rv;
            rv.specs = build_sql(b, type->spec.ast_sql, type->flags);
            rv.decl = decl;
            return rv;
        }
    case BTYTY_PTR:
        {
            decl = build_ptr(b, decl, type->flags);
            type = type->ptr.element;
            return build_type_to_decl_struct_step(b, type, decl);
        }
    case BTYTY_ARRAY:
        {
            GnuCAnyDirectDeclarator *ddecl = build_decl_direct(b, decl);
            GnuCTreeTagArrayDirectDeclarator *adecl = gnu_c_create_tree_tag_array_direct_declarator(b->pool, ddecl, type->array.size);
            decl = (GnuCTreeDeclarator *)adecl;
            assert (!type->flags.is_const && !type->flags.is_volatile);
            type = type->array.element;
            return build_type_to_decl_struct_step(b, type, decl);
        }
    case BTYTY_FUN:
        {
            GnuCAnyDirectDeclarator *ddecl = build_decl_direct(b, decl);
            GnuCTreeTagFunDirectDeclarator *fdecl = gnu_c_create_tree_tag_fun_direct_declarator(b->pool, ddecl, b->lparen, type->fun.args, b->rparen);
            decl = (GnuCTreeDeclarator *)fdecl;
            assert (!type->flags.is_const && !type->flags.is_volatile);
            type = type->fun.ret;
            return build_type_to_decl_struct_step(b, type, decl);
        }
    default:
        abort();
    }
}

static GnuCOptDirectAbstractDeclarator *build_opt_abstract_decl_direct(Builder *b, GnuCOptAbstractDeclarator *decl)
{
    if (gnu_c_is_opt_direct_abstract_declarator((GnuCAst *)decl))
    {
        return (GnuCOptDirectAbstractDeclarator *)decl;
    }
    else
    {
        GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
        GnuCAnyAbstractDeclarator *some_decl = (GnuCAnyAbstractDeclarator *)decl;
        GnuCTreeTagParenDirectAbstractDeclarator *pad = gnu_c_create_tree_tag_paren_direct_abstract_declarator(b->pool, b->lparen, no_attrs, some_decl, b->rparen);
        return (GnuCOptDirectAbstractDeclarator *)pad;
    }
}
static BuildTypePairAbstractDeclarator build_type_to_abstract_decl_step(Builder *b, BuildType *type, GnuCOptAbstractDeclarator *decl)
{
    switch (type->type)
    {
    case BTYTY_SPEC:
        {
            BuildTypePairAbstractDeclarator rv;
            rv.specs = (GnuCAnyDeclarationSpecifiers *)build_ds(b, type->spec.ast_dspec, type->flags);
            rv.decl = decl;
            return rv;
        }
    case BTYTY_PTR:
        {
            GnuCOptDirectAbstractDeclarator *ddecl = build_opt_abstract_decl_direct(b, decl);
            if (gnu_c_is_nothing((GnuCAst *)ddecl))
            {
                GnuCTreePointer *pointer = gnu_c_create_tree_pointer(b->pool, b->star, build_tql(b, type->flags), (GnuCOptPointer *)b->nothing);
                decl = (GnuCOptAbstractDeclarator *)pointer;
            }
            else
            {
                GnuCTreePointer *pointer = gnu_c_create_tree_pointer(b->pool, b->star, build_tql(b, type->flags), (GnuCOptPointer *)b->nothing);
                GnuCTreeTagPointerAbstractDeclarator *pdecl = gnu_c_create_tree_tag_pointer_abstract_declarator(b->pool, (GnuCTreePointer *)pointer, (GnuCOptDirectAbstractDeclarator *)ddecl);
                decl = (GnuCOptAbstractDeclarator *)pdecl;
            }
            type = type->ptr.element;
            return build_type_to_abstract_decl_step(b, type, decl);
        }
    case BTYTY_ARRAY:
        {
            GnuCOptDirectAbstractDeclarator *odadecl = build_opt_abstract_decl_direct(b, decl);
            GnuCTreeTagArrayDirectAbstractDeclarator *adecl = gnu_c_create_tree_tag_array_direct_abstract_declarator(b->pool, odadecl, type->array.size);
            decl = (GnuCOptAbstractDeclarator *)adecl;
            assert (!type->flags.is_const && !type->flags.is_volatile);
            type = type->array.element;
            return build_type_to_abstract_decl_step(b, type, decl);
        }
    case BTYTY_FUN:
        {
            GnuCOptDirectAbstractDeclarator *odadecl = build_opt_abstract_decl_direct(b, decl);
            GnuCTreeTagFunDirectAbstractDeclarator *fdecl = gnu_c_create_tree_tag_fun_direct_abstract_declarator(b->pool, odadecl, b->lparen, (GnuCOptParameterTypeList *)type->fun.args, b->rparen);
            decl = (GnuCOptAbstractDeclarator *)fdecl;
            assert (!type->flags.is_const && !type->flags.is_volatile);
            type = type->fun.ret;
            return build_type_to_abstract_decl_step(b, type, decl);
        }
    default:
        abort();
    }
}
static BuildTypePairAbstractStructDeclarator build_type_to_abstract_decl_struct_step(Builder *b, BuildType *type, GnuCOptAbstractDeclarator *decl)
{
    switch (type->type)
    {
    case BTYTY_SPEC:
        {
            BuildTypePairAbstractStructDeclarator rv;
            rv.specs = build_sql(b, type->spec.ast_sql, type->flags);
            rv.decl = decl;
            return rv;
        }
    case BTYTY_PTR:
        {
            GnuCOptDirectAbstractDeclarator *ddecl = build_opt_abstract_decl_direct(b, decl);
            if (gnu_c_is_nothing((GnuCAst *)ddecl))
            {
                GnuCTreePointer *pointer = gnu_c_create_tree_pointer(b->pool, b->star, build_tql(b, type->flags), (GnuCOptPointer *)b->nothing);
                decl = (GnuCOptAbstractDeclarator *)pointer;
            }
            else
            {
                GnuCTreePointer *pointer = gnu_c_create_tree_pointer(b->pool, b->star, build_tql(b, type->flags), (GnuCOptPointer *)b->nothing);
                GnuCTreeTagPointerAbstractDeclarator *pdecl = gnu_c_create_tree_tag_pointer_abstract_declarator(b->pool, (GnuCTreePointer *)pointer, (GnuCOptDirectAbstractDeclarator *)ddecl);
                decl = (GnuCOptAbstractDeclarator *)pdecl;
            }
            type = type->ptr.element;
            return build_type_to_abstract_decl_struct_step(b, type, decl);
        }
    case BTYTY_ARRAY:
        {
            GnuCOptDirectAbstractDeclarator *odadecl = build_opt_abstract_decl_direct(b, decl);
            GnuCTreeTagArrayDirectAbstractDeclarator *adecl = gnu_c_create_tree_tag_array_direct_abstract_declarator(b->pool, odadecl, type->array.size);
            decl = (GnuCOptAbstractDeclarator *)adecl;
            assert (!type->flags.is_const && !type->flags.is_volatile);
            type = type->array.element;
            return build_type_to_abstract_decl_struct_step(b, type, decl);
        }
    case BTYTY_FUN:
        {
            GnuCOptDirectAbstractDeclarator *odadecl = build_opt_abstract_decl_direct(b, decl);
            GnuCTreeTagFunDirectAbstractDeclarator *fdecl = gnu_c_create_tree_tag_fun_direct_abstract_declarator(b->pool, odadecl, b->lparen, (GnuCOptParameterTypeList *)type->fun.args, b->rparen);
            decl = (GnuCOptAbstractDeclarator *)fdecl;
            assert (!type->flags.is_const && !type->flags.is_volatile);
            type = type->fun.ret;
            return build_type_to_abstract_decl_struct_step(b, type, decl);
        }
    default:
        abort();
    }
}

static GnuCAnyDeclarationSpecifiers *head_specs(Builder *b, size_t nscs, BuildStorageClass **scs, GnuCAnyDeclarationSpecifiers *tspecs)
{
    GnuCOptHeadDeclarationSpecifiers *hspecs = (GnuCOptHeadDeclarationSpecifiers *)b->nothing;
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    if (!nscs)
    {
        return tspecs;
    }
    while (nscs--)
    {
        hspecs = (GnuCOptHeadDeclarationSpecifiers *)gnu_c_create_tree_head_declaration_specifiers(b->pool, hspecs, scs[0]->ast_scs, no_attrs);
        scs++;
    }
    return (GnuCAnyDeclarationSpecifiers *)gnu_c_create_tree_tag_head_tail_declaration_specifiers(b->pool, no_attrs, (GnuCOptHeadDeclarationSpecifiers *)hspecs, (GnuCTreeTailDeclarationSpecifiers *)tspecs);
}
BuildTypePairDeclarator build_type_to_decl(Builder *b, size_t nscs, BuildStorageClass **scs, BuildType *type, const char *name)
{
    GnuCAtomIdentifier *id = build_id(b, name);
    GnuCTreeDeclarator *decl = (GnuCTreeDeclarator *)id;
    BuildTypePairDeclarator rv = build_type_to_decl_step(b, type, decl);
    rv.specs = head_specs(b, nscs, scs, rv.specs);
    return rv;
}

BuildTypePairAbstractDeclarator build_type_to_decl_abstract(Builder *b, size_t nscs, BuildStorageClass **scs, BuildType *type)
{
    GnuCOptAbstractDeclarator *decl = (GnuCOptAbstractDeclarator *)b->nothing;
    BuildTypePairAbstractDeclarator rv = build_type_to_abstract_decl_step(b, type, decl);
    rv.specs = head_specs(b, nscs, scs, rv.specs);
    return rv;
}

BuildTypePairStructDeclarator build_type_to_decl_struct(Builder *b, BuildType *type, const char *name)
{
    GnuCAtomIdentifier *id = build_id(b, name);
    GnuCTreeDeclarator *decl = (GnuCTreeDeclarator *)id;
    return build_type_to_decl_struct_step(b, type, decl);
}
BuildTypePairAbstractStructDeclarator build_type_to_decl_struct_abstract(Builder *b, BuildType *type)
{
    GnuCOptAbstractDeclarator *decl = (GnuCOptAbstractDeclarator *)b->nothing;
    return build_type_to_abstract_decl_struct_step(b, type, decl);
}

GnuCAnyBlockItem *build_stmt_to_stmt(Builder *b, BuildStatement *stmt)
{
    GnuCTreeStatement *rv;
    assert (stmt->ast_ext == NULL);
    assert (stmt->ast_labels != NULL);
    assert (stmt->ast_unlabeled != NULL);
    assert (gnu_c_is_tree_statement((GnuCAst *)stmt->ast_unlabeled));
    rv = gnu_c_create_tree_statement(b->pool, stmt->ast_labels, (GnuCAnyUnlabeledStatement *)stmt->ast_unlabeled);
    return (GnuCAnyBlockItem *)rv;
}
GnuCTreeCompoundStatement *build_stmt_to_compound(Builder *b, BuildStatement *stmt)
{
    assert (stmt->ast_ext == NULL);
    assert (stmt->ast_labels != NULL);
    assert (stmt->ast_unlabeled != NULL);
    assert (gnu_c_is_tree_statement((GnuCAst *)stmt->ast_unlabeled));
    if (gnu_c_is_nothing((GnuCAst *)stmt->ast_labels))
    {
        if (gnu_c_is_tree_compound_statement((GnuCAst *)stmt->ast_unlabeled))
        {
            return (GnuCTreeCompoundStatement *)stmt->ast_unlabeled;
        }
    }

    {
        GnuCOptLabelDeclarations *no_labels = (GnuCOptLabelDeclarations *)b->nothing;
        GnuCOptBlockItemList *stmts = (GnuCOptBlockItemList *)build_stmt_to_stmt(b, stmt);
        return gnu_c_create_tree_compound_statement(b->pool, b->lbrace, no_labels, stmts, b->rbrace);
    }
}
GnuCTreeStatement *build_stmt_to_else_body(Builder *b, BuildStatement *stmt)
{
    GnuCTreeStatement *ast_stmt = (GnuCTreeStatement *)stmt->ast_unlabeled;
    GnuCAst *ast = (GnuCAst *)ast_stmt;
    assert (stmt->ast_ext == NULL);
    assert (stmt->ast_labels != NULL);
    assert (stmt->ast_unlabeled != NULL);
    if (gnu_c_is_nothing((GnuCAst *)stmt->ast_labels))
    {
        if (gnu_c_is_tree_if_statement(ast))
        {
            return ast_stmt;
        }
        if (gnu_c_is_tree_if_else_statement(ast))
        {
            return ast_stmt;
        }
    }
    return (GnuCTreeStatement *)build_stmt_to_compound(b, stmt);
}

GnuCAnyInitializer *build_init_to_init(Builder *b, BuildInitializer *init)
{
    (void)b;
    assert (gnu_c_is_any_initializer((GnuCAst *)init->ast_init));
    return (GnuCAnyInitializer *)init->ast_init;
}

BuildExpression *build_expr_from_primary(Builder *b, GnuCAnyPrimaryExpression *expr)
{
    assert (gnu_c_is_any_primary_expression((GnuCAst *)expr));
    return build_expr_ast(b, (GnuCAnyExprList *)expr);
}
BuildExpression *build_expr_from_postfix(Builder *b, GnuCAnyPostfixExpression *expr)
{
    assert (gnu_c_is_any_postfix_expression((GnuCAst *)expr));
    return build_expr_ast(b, (GnuCAnyExprList *)expr);
}
BuildExpression *build_expr_from_cast(Builder *b, GnuCAnyMaybeCastExpression *expr)
{
    assert (gnu_c_is_any_maybe_cast_expression((GnuCAst *)expr));
    return build_expr_ast(b, (GnuCAnyExprList *)expr);
}
BuildExpression *build_expr_from_unary(Builder *b, GnuCAnyNoCastExpression *expr)
{
    assert (gnu_c_is_any_no_cast_expression((GnuCAst *)expr));
    return build_expr_ast(b, (GnuCAnyExprList *)expr);
}
BuildExpression *build_expr_from_mul(Builder *b, GnuCAnyMultiplicativeExpression *expr)
{
    assert (gnu_c_is_any_multiplicative_expression((GnuCAst *)expr));
    return build_expr_ast(b, (GnuCAnyExprList *)expr);
}
BuildExpression *build_expr_from_add(Builder *b, GnuCAnyAdditiveExpression *expr)
{
    assert (gnu_c_is_any_additive_expression((GnuCAst *)expr));
    return build_expr_ast(b, (GnuCAnyExprList *)expr);
}
BuildExpression *build_expr_from_shift(Builder *b, GnuCAnyShiftExpression *expr)
{
    assert (gnu_c_is_any_shift_expression((GnuCAst *)expr));
    return build_expr_ast(b, (GnuCAnyExprList *)expr);
}
BuildExpression *build_expr_from_rel(Builder *b, GnuCAnyRelationalExpression *expr)
{
    assert (gnu_c_is_any_relational_expression((GnuCAst *)expr));
    return build_expr_ast(b, (GnuCAnyExprList *)expr);
}
BuildExpression *build_expr_from_eq(Builder *b, GnuCAnyEqualityExpression *expr)
{
    assert (gnu_c_is_any_equality_expression((GnuCAst *)expr));
    return build_expr_ast(b, (GnuCAnyExprList *)expr);
}
BuildExpression *build_expr_from_bit_and(Builder *b, GnuCAnyAndExpression *expr)
{
    assert (gnu_c_is_any_and_expression((GnuCAst *)expr));
    return build_expr_ast(b, (GnuCAnyExprList *)expr);
}
BuildExpression *build_expr_from_bit_xor(Builder *b, GnuCAnyExclusiveOrExpression *expr)
{
    assert (gnu_c_is_any_exclusive_or_expression((GnuCAst *)expr));
    return build_expr_ast(b, (GnuCAnyExprList *)expr);
}
BuildExpression *build_expr_from_bit_or(Builder *b, GnuCAnyInclusiveOrExpression *expr)
{
    assert (gnu_c_is_any_inclusive_or_expression((GnuCAst *)expr));
    return build_expr_ast(b, (GnuCAnyExprList *)expr);
}
BuildExpression *build_expr_from_log_and(Builder *b, GnuCAnyLogicalAndExpression *expr)
{
    assert (gnu_c_is_any_logical_and_expression((GnuCAst *)expr));
    return build_expr_ast(b, (GnuCAnyExprList *)expr);
}
BuildExpression *build_expr_from_log_or(Builder *b, GnuCAnyLogicalOrExpression *expr)
{
    assert (gnu_c_is_any_logical_or_expression((GnuCAst *)expr));
    return build_expr_ast(b, (GnuCAnyExprList *)expr);
}
BuildExpression *build_expr_from_conditional(Builder *b, GnuCAnyConditionalExpression *expr)
{
    assert (gnu_c_is_any_conditional_expression((GnuCAst *)expr));
    return build_expr_ast(b, (GnuCAnyExprList *)expr);
}
BuildExpression *build_expr_from_assignment(Builder *b, GnuCAnyAssignmentExpression *expr)
{
    assert (gnu_c_is_any_assignment_expression((GnuCAst *)expr));
    return build_expr_ast(b, (GnuCAnyExprList *)expr);
}
BuildExpression *build_expr_from_comma(Builder *b, GnuCAnyExprList *expr)
{
    assert (gnu_c_is_any_expr_list((GnuCAst *)expr));
    return build_expr_ast(b, (GnuCAnyExprList *)expr);
}

static GnuCTreeTagParenExpression *gnu_c_expr_parenthesize(Builder *b, BuildExpression *expr)
{
    GnuCAnyExprList *comma = build_expr_to_comma(b, expr);
    return gnu_c_create_tree_tag_paren_expression(b->pool, b->lparen, comma, b->rparen);
}
GnuCAnyPrimaryExpression *build_expr_to_primary(Builder *b, BuildExpression *expr)
{
    if (gnu_c_is_any_primary_expression((GnuCAst *)expr->ast_expr))
        return (GnuCAnyPrimaryExpression *)expr->ast_expr;
    return (GnuCAnyPrimaryExpression *)gnu_c_expr_parenthesize(b, expr);
}
GnuCAnyPostfixExpression *build_expr_to_postfix(Builder *b, BuildExpression *expr)
{
    if (gnu_c_is_any_postfix_expression((GnuCAst *)expr->ast_expr))
        return (GnuCAnyPostfixExpression *)expr->ast_expr;
    return (GnuCAnyPostfixExpression *)gnu_c_expr_parenthesize(b, expr);
}
GnuCAnyMaybeCastExpression *build_expr_to_cast(Builder *b, BuildExpression *expr)
{
    if (gnu_c_is_any_maybe_cast_expression((GnuCAst *)expr->ast_expr))
        return (GnuCAnyMaybeCastExpression *)expr->ast_expr;
    return (GnuCAnyMaybeCastExpression *)gnu_c_expr_parenthesize(b, expr);
}
GnuCAnyNoCastExpression *build_expr_to_unary(Builder *b, BuildExpression *expr)
{
    if (gnu_c_is_any_no_cast_expression((GnuCAst *)expr->ast_expr))
        return (GnuCAnyNoCastExpression *)expr->ast_expr;
    return (GnuCAnyNoCastExpression *)gnu_c_expr_parenthesize(b, expr);
}
GnuCAnyMultiplicativeExpression *build_expr_to_mul(Builder *b, BuildExpression *expr)
{
    if (gnu_c_is_any_multiplicative_expression((GnuCAst *)expr->ast_expr))
        return (GnuCAnyMultiplicativeExpression *)expr->ast_expr;
    return (GnuCAnyMultiplicativeExpression *)gnu_c_expr_parenthesize(b, expr);
}
GnuCAnyAdditiveExpression *build_expr_to_add(Builder *b, BuildExpression *expr)
{
    if (gnu_c_is_any_additive_expression((GnuCAst *)expr->ast_expr))
        return (GnuCAnyAdditiveExpression *)expr->ast_expr;
    return (GnuCAnyAdditiveExpression *)gnu_c_expr_parenthesize(b, expr);
}
GnuCAnyShiftExpression *build_expr_to_shift(Builder *b, BuildExpression *expr)
{
    if (gnu_c_is_any_shift_expression((GnuCAst *)expr->ast_expr))
        return (GnuCAnyShiftExpression *)expr->ast_expr;
    return (GnuCAnyShiftExpression *)gnu_c_expr_parenthesize(b, expr);
}
GnuCAnyRelationalExpression *build_expr_to_rel(Builder *b, BuildExpression *expr)
{
    if (gnu_c_is_any_relational_expression((GnuCAst *)expr->ast_expr))
        return (GnuCAnyRelationalExpression *)expr->ast_expr;
    return (GnuCAnyRelationalExpression *)gnu_c_expr_parenthesize(b, expr);
}
GnuCAnyEqualityExpression *build_expr_to_eq(Builder *b, BuildExpression *expr)
{
    if (gnu_c_is_any_equality_expression((GnuCAst *)expr->ast_expr))
        return (GnuCAnyEqualityExpression *)expr->ast_expr;
    return (GnuCAnyEqualityExpression *)gnu_c_expr_parenthesize(b, expr);
}
GnuCAnyAndExpression *build_expr_to_bit_and(Builder *b, BuildExpression *expr)
{
    if (gnu_c_is_any_and_expression((GnuCAst *)expr->ast_expr))
        return (GnuCAnyAndExpression *)expr->ast_expr;
    return (GnuCAnyAndExpression *)gnu_c_expr_parenthesize(b, expr);
}
GnuCAnyExclusiveOrExpression *build_expr_to_bit_xor(Builder *b, BuildExpression *expr)
{
    if (gnu_c_is_any_exclusive_or_expression((GnuCAst *)expr->ast_expr))
        return (GnuCAnyExclusiveOrExpression *)expr->ast_expr;
    return (GnuCAnyExclusiveOrExpression *)gnu_c_expr_parenthesize(b, expr);
}
GnuCAnyInclusiveOrExpression *build_expr_to_bit_or(Builder *b, BuildExpression *expr)
{
    if (gnu_c_is_any_inclusive_or_expression((GnuCAst *)expr->ast_expr))
        return (GnuCAnyInclusiveOrExpression *)expr->ast_expr;
    return (GnuCAnyInclusiveOrExpression *)gnu_c_expr_parenthesize(b, expr);
}
GnuCAnyLogicalAndExpression *build_expr_to_log_and(Builder *b, BuildExpression *expr)
{
    if (gnu_c_is_any_logical_and_expression((GnuCAst *)expr->ast_expr))
        return (GnuCAnyLogicalAndExpression *)expr->ast_expr;
    return (GnuCAnyLogicalAndExpression *)gnu_c_expr_parenthesize(b, expr);
}
GnuCAnyLogicalOrExpression *build_expr_to_log_or(Builder *b, BuildExpression *expr)
{
    if (gnu_c_is_any_logical_or_expression((GnuCAst *)expr->ast_expr))
        return (GnuCAnyLogicalOrExpression *)expr->ast_expr;
    return (GnuCAnyLogicalOrExpression *)gnu_c_expr_parenthesize(b, expr);
}
GnuCAnyConditionalExpression *build_expr_to_conditional(Builder *b, BuildExpression *expr)
{
    if (gnu_c_is_any_conditional_expression((GnuCAst *)expr->ast_expr))
        return (GnuCAnyConditionalExpression *)expr->ast_expr;
    return (GnuCAnyConditionalExpression *)gnu_c_expr_parenthesize(b, expr);
}
GnuCAnyAssignmentExpression *build_expr_to_assignment(Builder *b, BuildExpression *expr)
{
    if (gnu_c_is_any_assignment_expression((GnuCAst *)expr->ast_expr))
        return (GnuCAnyAssignmentExpression *)expr->ast_expr;
    return (GnuCAnyAssignmentExpression *)gnu_c_expr_parenthesize(b, expr);
}
GnuCAnyExprList *build_expr_to_comma(Builder *b, BuildExpression *expr)
{
    (void)b;
    assert (gnu_c_is_any_expr_list((GnuCAst *)expr->ast_expr));
    return (GnuCAnyExprList *)expr->ast_expr;
}
