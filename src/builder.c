#include "builder.h"
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
#include <stdlib.h>
#include <string.h>

#include "bridge.h"
#include "c89.gen.h"
#include "pool.h"


Builder *builder_create(void)
{
    Builder *rv = (Builder *)calloc(1, sizeof(Builder));
    Pool *pool = rv->pool = pool_create();
    rv->nothing = c89_create_nothing(pool);

    rv->arrow = c89_create_sym_arrow(pool);
    rv->comma = c89_create_sym_comma(pool);
    rv->dot = c89_create_sym_dot(pool);
    rv->ellipsis = c89_create_sym_ellipsis(pool);
    rv->lbrace = c89_create_sym_lbrace(pool);
    rv->lbracket = c89_create_sym_lbracket(pool);
    rv->lparen = c89_create_sym_lparen(pool);
    rv->rbrace = c89_create_sym_rbrace(pool);
    rv->rbracket = c89_create_sym_rbracket(pool);
    rv->rparen = c89_create_sym_rparen(pool);
    rv->semicolon = c89_create_sym_semicolon(pool);

    rv->ampersand = c89_create_sym_ampersand(pool);
    rv->bang = c89_create_sym_bang(pool);
    rv->caret = c89_create_sym_caret(pool);
    rv->colon = c89_create_sym_colon(pool);
    rv->decr = c89_create_sym_decr(pool);
    rv->eq = c89_create_sym_eq(pool);
    rv->ge = c89_create_sym_ge(pool);
    rv->gt = c89_create_sym_gt(pool);
    rv->incr = c89_create_sym_incr(pool);
    rv->le = c89_create_sym_le(pool);
    rv->logical_and = c89_create_sym_logical_and(pool);
    rv->logical_or = c89_create_sym_logical_or(pool);
    rv->lshift = c89_create_sym_lshift(pool);
    rv->lt = c89_create_sym_lt(pool);
    rv->minus = c89_create_sym_minus(pool);
    rv->ne = c89_create_sym_ne(pool);
    rv->percent = c89_create_sym_percent(pool);
    rv->pipe = c89_create_sym_pipe(pool);
    rv->plus = c89_create_sym_plus(pool);
    rv->query = c89_create_sym_query(pool);
    rv->rshift = c89_create_sym_rshift(pool);
    rv->slash = c89_create_sym_slash(pool);
    rv->star = c89_create_sym_star(pool);
    rv->tilde = c89_create_sym_tilde(pool);

    rv->assign = c89_create_sym_assign(pool);
    rv->ampersand_assign = c89_create_sym_ampersand_assign(pool);
    rv->caret_assign = c89_create_sym_caret_assign(pool);
    rv->lshift_assign = c89_create_sym_lshift_assign(pool);
    rv->minus_assign = c89_create_sym_minus_assign(pool);
    rv->percent_assign = c89_create_sym_percent_assign(pool);
    rv->pipe_assign = c89_create_sym_pipe_assign(pool);
    rv->plus_assign = c89_create_sym_plus_assign(pool);
    rv->rshift_assign = c89_create_sym_rshift_assign(pool);
    rv->slash_assign = c89_create_sym_slash_assign(pool);
    rv->star_assign = c89_create_sym_star_assign(pool);

    rv->kw_break = c89_create_kw_break(pool);
    rv->kw_case = c89_create_kw_case(pool);
    rv->kw_const = c89_create_kw_const(pool);
    rv->kw_continue = c89_create_kw_continue(pool);
    rv->kw_default = c89_create_kw_default(pool);
    rv->kw_do = c89_create_kw_do(pool);
    rv->kw_else = c89_create_kw_else(pool);
    rv->kw_enum = c89_create_kw_enum(pool);
    rv->kw_extern = c89_create_kw_extern(pool);
    rv->kw_for = c89_create_kw_for(pool);
    rv->kw_goto = c89_create_kw_goto(pool);
    rv->kw_if = c89_create_kw_if(pool);
    rv->kw_register = c89_create_kw_register(pool);
    rv->kw_return = c89_create_kw_return(pool);
    rv->kw_sizeof = c89_create_kw_sizeof(pool);
    rv->kw_static = c89_create_kw_static(pool);
    rv->kw_struct = c89_create_kw_struct(pool);
    rv->kw_switch = c89_create_kw_switch(pool);
    rv->kw_typedef = c89_create_kw_typedef(pool);
    rv->kw_union = c89_create_kw_union(pool);
    rv->kw_volatile = c89_create_kw_volatile(pool);
    rv->kw_while = c89_create_kw_while(pool);

    {
        C89_KwSigned *kw_signed = c89_create_kw_signed(pool);
        C89_KwUnsigned *kw_unsigned = c89_create_kw_unsigned(pool);
        C89_KwVoid *kw_void = c89_create_kw_void(pool);
        C89_KwChar *kw_char = c89_create_kw_char(pool);
        C89_KwShort *kw_short = c89_create_kw_short(pool);
        C89_KwInt *kw_int = c89_create_kw_int(pool);
        C89_KwLong *kw_long = c89_create_kw_long(pool);
        C89_KwFloat *kw_float = c89_create_kw_float(pool);
        C89_KwDouble *kw_double = c89_create_kw_double(pool);

        rv->ty_void = (C89_AnyTypeSpecifier *)kw_void;
        rv->ty_char = (C89_AnyTypeSpecifier *)kw_char;
        rv->ty_signed_char = (C89_AnyTypeSpecifier *)c89_create_tree_signed_char(pool, kw_signed, kw_char);
        rv->ty_unsigned_char = (C89_AnyTypeSpecifier *)c89_create_tree_unsigned_char(pool, kw_unsigned, kw_char);

        rv->ty_signed_short = (C89_AnyTypeSpecifier *)c89_create_tree_signed_short_int(pool, kw_signed, kw_short, kw_int);
        rv->ty_unsigned_short = (C89_AnyTypeSpecifier *)c89_create_tree_unsigned_short_int(pool, kw_unsigned, kw_short, kw_int);
        rv->ty_signed_int = (C89_AnyTypeSpecifier *)c89_create_tree_signed_int(pool, kw_signed, kw_int);
        rv->ty_unsigned_int = (C89_AnyTypeSpecifier *)c89_create_tree_unsigned_int(pool, kw_unsigned, kw_int);
        rv->ty_signed_long = (C89_AnyTypeSpecifier *)c89_create_tree_signed_long_int(pool, kw_signed, kw_long, kw_int);
        rv->ty_unsigned_long = (C89_AnyTypeSpecifier *)c89_create_tree_unsigned_long_int(pool, kw_unsigned, kw_long, kw_int);
        rv->ty_signed_long_long = (C89_AnyTypeSpecifier *)c89_create_tree_signed_long_long_int(pool, kw_signed, kw_long, kw_long, kw_int);
        rv->ty_unsigned_long_long = (C89_AnyTypeSpecifier *)c89_create_tree_unsigned_long_long_int(pool, kw_unsigned, kw_long, kw_long, kw_int);

        rv->ty_float = (C89_AnyTypeSpecifier *)kw_float;
        rv->ty_double = (C89_AnyTypeSpecifier *)kw_double;
        rv->ty_long_double = (C89_AnyTypeSpecifier *)c89_create_tree_long_double(pool, kw_long, kw_double);
    }

    return rv;
}
void builder_destroy(Builder *b)
{
    pool_destroy(b->pool);
    free(b);
}

void builder_emit_tu_to_file(BuildTranslationUnit *tu, FILE *fp)
{
    c89_emit((C89_Ast *)tu->ast_tu, fp);
}

char *builder_emit_tu_to_string(BuildTranslationUnit *tu)
{
    char *rv;
    size_t sz;
    FILE *fp = open_memstream(&rv, &sz);
    builder_emit_tu_to_file(tu, fp);
    fclose(fp);
    return rv;
}

void builder_free_string(char *p)
{
    free(p);
}

BuildTranslationUnit *build_tu(Builder *b, size_t ntops, BuildTopLevel **tops)
{
    C89_AnyExternalDeclaration *one;
    C89_TreeMultiTranslationUnit *many;
    C89_AnyTranslationUnit *all;
    size_t i;
    assert (ntops);

    one = tops[0]->ast_ext;
    all = (C89_AnyTranslationUnit *)one;
    for (i = 1; i < ntops; ++i)
    {
        one = tops[i]->ast_ext;
        many = c89_create_tree_multi_translation_unit(b->pool, all, one);
        all = (C89_AnyTranslationUnit *)many;
    }
    return build_tu_ast(b, all);
}
BuildTopLevel *build_top_decl(Builder *b, BuildDeclaration *decl)
{
    C89_TreeDeclaration *ast_decl = decl->ast_decl;
    C89_AnyExternalDeclaration *ast_ext = (C89_AnyExternalDeclaration *)ast_decl;
    return build_top_ast(b, ast_ext);
}
static C89_AnyStructDeclarationList *build_members(Builder *b, size_t nmembs, BuildMemberDeclaration **decls)
{
    C89_TreeStructDeclaration *one;
    C89_TreeMultiStructDeclaration *many;
    C89_AnyStructDeclarationList *all;
    size_t i;
    assert (nmembs);

    one = decls[0]->ast_struct_decl;
    all = (C89_AnyStructDeclarationList *)one;
    for (i = 1; i < nmembs; ++i)
    {
        one = decls[i]->ast_struct_decl;
        many = c89_create_tree_multi_struct_declaration(b->pool, all, one);
        all = (C89_AnyStructDeclarationList *)many;
    }
    return all;
}
static BuildTopLevel *build_top_struct_or_union_definition(Builder *b, C89_AnyStructOrUnion *sou, const char *name, size_t nmembs, BuildMemberDeclaration **decls)
{
    C89_OptIdentifier *id = (C89_OptIdentifier *)build_id(b, name);
    C89_AnyStructDeclarationList *members = build_members(b, nmembs, decls);
    C89_TreeFullStruct *ast_struct = c89_create_tree_full_struct(b->pool, sou, id, b->lbrace, members, b->rbrace);
    C89_AnyTypeSpecifier *ast_spec = (C89_AnyTypeSpecifier *)ast_struct;
    C89_OptStorageClassSpecifier *no_scs = (C89_OptStorageClassSpecifier *)b->nothing;
    C89_OptTypeQualifierList *no_ql = (C89_OptTypeQualifierList *)b->nothing;
    C89_TreeSpecifierQualifierList *ast_sql = c89_create_tree_specifier_qualifier_list(b->pool, ast_spec, no_ql);
    C89_OptInitDeclaratorList *no_idl = (C89_OptInitDeclaratorList *)b->nothing;
    C89_TreeDeclaration *ast_decl = c89_create_tree_declaration(b->pool, no_scs, ast_sql, no_idl, b->semicolon);
    C89_AnyExternalDeclaration *ast_ext = (C89_AnyExternalDeclaration *)ast_decl;
    return build_top_ast(b, ast_ext);
}
BuildTopLevel *build_top_struct_definition(Builder *b, const char *name, size_t nmembs, BuildMemberDeclaration **decls)
{
    C89_AnyStructOrUnion *sou = (C89_AnyStructOrUnion *)b->kw_struct;
    return build_top_struct_or_union_definition(b, sou, name, nmembs, decls);
}
BuildTopLevel *build_top_union_definition(Builder *b, const char *name, size_t nmembs, BuildMemberDeclaration **decls)
{
    C89_AnyStructOrUnion *sou = (C89_AnyStructOrUnion *)b->kw_union;
    return build_top_struct_or_union_definition(b, sou, name, nmembs, decls);
}
static C89_AnyEnumeratorList *build_variants(Builder *b, size_t nvars, BuildEnumerator **vars)
{
    C89_AnyEnumerator *one;
    C89_TreeMultiEnumeratorList *many;
    C89_AnyEnumeratorList *all;
    size_t i;
    assert (nvars);

    one = vars[0]->ast_enum;
    all = (C89_AnyEnumeratorList *)one;
    for (i = 1; i < nvars; ++i)
    {
        one = vars[i]->ast_enum;
        many = c89_create_tree_multi_enumerator_list(b->pool, all, b->comma, one);
        all = (C89_AnyEnumeratorList *)many;
    }
    return all;
}
BuildTopLevel *build_top_enum_definition(Builder *b, const char *name, size_t nvars, BuildEnumerator **vars)
{
    C89_OptIdentifier *id = (C89_OptIdentifier *)build_id(b, name);
    C89_AnyEnumeratorList *variants = build_variants(b, nvars, vars);
    C89_TreeFullEnum *ast_enum = c89_create_tree_full_enum(b->pool, b->kw_enum, id, b->lbrace, variants, b->rbrace);
    C89_AnyTypeSpecifier *ast_spec = (C89_AnyTypeSpecifier *)ast_enum;
    C89_OptStorageClassSpecifier *no_scs = (C89_OptStorageClassSpecifier *)b->nothing;
    C89_OptTypeQualifierList *no_ql = (C89_OptTypeQualifierList *)b->nothing;
    C89_TreeSpecifierQualifierList *ast_sql = c89_create_tree_specifier_qualifier_list(b->pool, ast_spec, no_ql);
    C89_OptInitDeclaratorList *no_idl = (C89_OptInitDeclaratorList *)b->nothing;
    C89_TreeDeclaration *ast_decl = c89_create_tree_declaration(b->pool, no_scs, ast_sql, no_idl, b->semicolon);
    C89_AnyExternalDeclaration *ast_ext = (C89_AnyExternalDeclaration *)ast_decl;
    return build_top_ast(b, ast_ext);
}
static C89_OptStorageClassSpecifier *build_scs(BuildStorageClass *sc)
{
    return sc->ast_scs;
}
BuildTopLevel *build_top_function_definition(Builder *b, BuildStorageClass *sc, const char *name, BuildType *fnty, BuildStatement *body)
{
    C89_OptStorageClassSpecifier *no_scs = build_scs(sc);
    BuildTypePairDeclarator pair = build_type_to_decl(b, fnty, name);
    C89_TreeCompoundStatement *stmt = build_stmt_to_compound(b, body);
    C89_TreeFunctionDefinition *ast_fun = c89_create_tree_function_definition(b->pool, no_scs, pair.sql, pair.decl, stmt);
    C89_AnyExternalDeclaration *ast_ext = (C89_AnyExternalDeclaration *)ast_fun;
    return build_top_ast(b, ast_ext);
}

BuildStorageClass *build_storage_class_none(Builder *b)
{
    C89_OptStorageClassSpecifier *ast_sc = (C89_OptStorageClassSpecifier *)b->nothing;
    return build_storage_class_ast(b, ast_sc);
}
BuildStorageClass *build_storage_class_typedef(Builder *b)
{
    C89_OptStorageClassSpecifier *ast_sc = (C89_OptStorageClassSpecifier *)b->kw_typedef;
    return build_storage_class_ast(b, ast_sc);
}
BuildStorageClass *build_storage_class_extern(Builder *b)
{
    C89_OptStorageClassSpecifier *ast_sc = (C89_OptStorageClassSpecifier *)b->kw_extern;
    return build_storage_class_ast(b, ast_sc);
}
BuildStorageClass *build_storage_class_static(Builder *b)
{
    C89_OptStorageClassSpecifier *ast_sc = (C89_OptStorageClassSpecifier *)b->kw_static;
    return build_storage_class_ast(b, ast_sc);
}
BuildStorageClass *build_storage_class_register(Builder *b)
{
    C89_OptStorageClassSpecifier *ast_sc = (C89_OptStorageClassSpecifier *)b->kw_register;
    return build_storage_class_ast(b, ast_sc);
}

BuildDeclaration *build_declaration_noinit(Builder *b, BuildStorageClass *sc, BuildType *type, const char *name)
{
    C89_OptStorageClassSpecifier *no_scs = build_scs(sc);
    BuildTypePairDeclarator pair = build_type_to_decl(b, type, name);
    C89_AnyInitDeclarator *idecl = (C89_AnyInitDeclarator *)pair.decl;
    C89_OptInitDeclaratorList *idl = (C89_OptInitDeclaratorList *)idecl;
    C89_TreeDeclaration *ast_decl = c89_create_tree_declaration(b->pool, no_scs, pair.sql, idl, b->semicolon);
    return build_decl_ast(b, ast_decl);
}
BuildDeclaration *build_declaration_init(Builder *b, BuildStorageClass *sc, BuildType *type, const char *name, BuildInitializer *init)
{
    C89_OptStorageClassSpecifier *no_scs = build_scs(sc);
    BuildTypePairDeclarator pair = build_type_to_decl(b, type, name);
    C89_AnyInitializer *ini = build_init_to_init(b, init);
    C89_TreeInitializedDeclarator *tid = c89_create_tree_initialized_declarator(b->pool, pair.decl, b->assign, ini);
    C89_AnyInitDeclarator *idecl = (C89_AnyInitDeclarator *)tid;
    C89_OptInitDeclaratorList *idl = (C89_OptInitDeclaratorList *)idecl;
    C89_TreeDeclaration *ast_decl = c89_create_tree_declaration(b->pool, no_scs, pair.sql, idl, b->semicolon);
    return build_decl_ast(b, ast_decl);
}
BuildParamDeclaration *build_param_declaration(Builder *b, BuildStorageClass *sc, BuildType *type, const char *name)
{
    C89_OptStorageClassSpecifier *no_scs = build_scs(sc);
    BuildTypePairDeclarator pair = build_type_to_decl(b, type, name);
    C89_TreeNamedParameter *rv = c89_create_tree_named_parameter(b->pool, no_scs, pair.sql, pair.decl);
    C89_AnyParameterDeclaration *ast = (C89_AnyParameterDeclaration *)rv;
    return build_param_ast(b, ast);
}
BuildParamDeclaration *build_param_declaration_anon(Builder *b, BuildStorageClass *sc, BuildType *type)
{
    C89_OptStorageClassSpecifier *no_scs = build_scs(sc);
    BuildTypePairAbstractDeclarator pair = build_type_to_decl_abstract(b, type);
    C89_TreeUnnamedParameter *rv = c89_create_tree_unnamed_parameter(b->pool, no_scs, pair.sql, pair.decl);
    C89_AnyParameterDeclaration *ast = (C89_AnyParameterDeclaration *)rv;
    return build_param_ast(b, ast);
}
BuildMemberDeclaration *build_member_declaration(Builder *b, BuildType *type, const char *name)
{
    BuildTypePairDeclarator pair = build_type_to_decl(b, type, name);
    C89_AnyStructDeclaratorList *sdl = (C89_AnyStructDeclaratorList *)pair.decl;
    C89_TreeStructDeclaration *sd = c89_create_tree_struct_declaration(b->pool, pair.sql, sdl, b->semicolon);
    return build_member_ast(b, sd);
}
BuildMemberDeclaration *build_member_declaration_bitfield(Builder *b, BuildType *type, const char *name, BuildExpression *width)
{
    BuildTypePairDeclarator pair = build_type_to_decl(b, type, name);
    C89_AnyConditionalExpression *expr = build_expr_to_conditional(b, width);
    C89_OptDeclarator *odecl = (C89_OptDeclarator *)pair.decl;
    C89_TreeBitfieldDeclarator *bdecl = c89_create_tree_bitfield_declarator(b->pool, odecl, b->colon, expr);
    C89_AnyStructDeclaratorList *sdl = (C89_AnyStructDeclaratorList *)bdecl;
    C89_TreeStructDeclaration *sd = c89_create_tree_struct_declaration(b->pool, pair.sql, sdl, b->semicolon);
    return build_member_ast(b, sd);
}
BuildMemberDeclaration *build_member_declaration_bitfield_anon(Builder *b, BuildType *type, BuildExpression *width)
{
    BuildTypePairAbstractDeclarator pair = build_type_to_decl_abstract(b, type);
    C89_AnyConditionalExpression *expr = build_expr_to_conditional(b, width);
    C89_OptDeclarator *odecl = (C89_OptDeclarator *)b->nothing;
    C89_TreeBitfieldDeclarator *bdecl = c89_create_tree_bitfield_declarator(b->pool, odecl, b->colon, expr);
    C89_AnyStructDeclaratorList *sdl = (C89_AnyStructDeclaratorList *)bdecl;
    C89_TreeStructDeclaration *sd = c89_create_tree_struct_declaration(b->pool, pair.sql, sdl, b->semicolon);
    assert (pair.decl == (C89_OptAbstractDeclarator *)b->nothing);
    return build_member_ast(b, sd);
}
BuildEnumerator *build_enumerator_noinit(Builder *b, const char *name)
{
    C89_AtomIdentifier *id = build_id(b, name);
    C89_AnyEnumerator *e = (C89_AnyEnumerator *)id;
    return build_enum_ast(b, e);
}
BuildEnumerator *build_enumerator_init(Builder *b, const char *name, BuildExpression *init)
{
    C89_AtomIdentifier *id = build_id(b, name);
    C89_AnyConditionalExpression *expr = build_expr_to_conditional(b, init);
    C89_TreeInitializedEnumerationConstant *iec = c89_create_tree_initialized_enumeration_constant(b->pool, id, b->assign, expr);
    C89_AnyEnumerator *e = (C89_AnyEnumerator *)iec;
    return build_enum_ast(b, e);
}

BuildType *build_type_typedef(Builder *b, const char *name)
{
    C89_AtomTypedefName *ty = build_typedef_name(b, name);
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)ty;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_struct(Builder *b, const char *name)
{
    C89_AtomIdentifier *id = build_id(b, name);
    C89_AnyStructOrUnion *sou = (C89_AnyStructOrUnion *)b->kw_struct;
    C89_TreeForwardStruct *ty = c89_create_tree_forward_struct(b->pool, sou, id);
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)ty;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_union(Builder *b, const char *name)
{
    C89_AtomIdentifier *id = build_id(b, name);
    C89_AnyStructOrUnion *sou = (C89_AnyStructOrUnion *)b->kw_union;
    C89_TreeForwardStruct *ty = c89_create_tree_forward_struct(b->pool, sou, id);
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)ty;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_enum(Builder *b, const char *name)
{
    C89_AtomIdentifier *id = build_id(b, name);
    C89_TreeForwardEnum *ty = c89_create_tree_forward_enum(b->pool, b->kw_enum, id);
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)ty;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_void(Builder *b)
{
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)b->ty_void;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_char(Builder *b)
{
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)b->ty_char;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_signed_char(Builder *b)
{
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)b->ty_signed_char;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_unsigned_char(Builder *b)
{
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)b->ty_unsigned_char;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_signed_short(Builder *b)
{
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)b->ty_signed_short;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_unsigned_short(Builder *b)
{
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)b->ty_unsigned_short;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_signed_int(Builder *b)
{
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)b->ty_signed_int;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_unsigned_int(Builder *b)
{
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)b->ty_unsigned_int;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_signed_long(Builder *b)
{
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)b->ty_signed_long;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_unsigned_long(Builder *b)
{
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)b->ty_unsigned_long;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_signed_long_long(Builder *b)
{
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)b->ty_signed_long_long;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_unsigned_long_long(Builder *b)
{
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)b->ty_unsigned_long_long;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_float(Builder *b)
{
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)b->ty_float;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_double(Builder *b)
{
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)b->ty_double;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_long_double(Builder *b)
{
    C89_AnyTypeSpecifier *spec = (C89_AnyTypeSpecifier *)b->ty_long_double;
    return build_type_spec_ast(b, spec, false, false);
}
BuildType *build_type_const(Builder *b, BuildType *type)
{
    assert (type->type != BTYTY_FUN);
    if (type->type == BTYTY_ARRAY)
    {
        BuildType *const_element = build_type_const(b, type->target);
        return build_type_array_ast(b, const_element, type->array_size, false, false);
    }
    else
    {
        BuildType copy = *type;
        copy.is_const = true;
        return build_type_copy_ast(b, copy);
    }
}
BuildType *build_type_volatile(Builder *b, BuildType *type)
{
    assert (type->type != BTYTY_FUN);
    if (type->type == BTYTY_ARRAY)
    {
        BuildType *volatile_element = build_type_volatile(b, type->target);
        return build_type_array_ast(b, volatile_element, type->array_size, false, false);
    }
    else
    {
        BuildType copy = *type;
        copy.is_volatile = true;
        return build_type_copy_ast(b, copy);
    }
}
BuildType *build_type_pointer(Builder *b, BuildType *type)
{
    return build_type_ptr_ast(b, type, false, false);
}
BuildType *build_type_array(Builder *b, BuildType *type, BuildExpression *size)
{
    C89_AnyConditionalExpression *expr = build_expr_to_conditional(b, size);
    C89_OptConditionalExpression *opt_expr = (C89_OptConditionalExpression *)expr;
    return build_type_array_ast(b, type, opt_expr, false, false);
}
BuildType *build_type_array_unknown_bound(Builder *b, BuildType *type)
{
    C89_OptConditionalExpression *opt_expr = (C89_OptConditionalExpression *)b->nothing;
    return build_type_array_ast(b, type, opt_expr, false, false);
}
static C89_OptParameterTypeList *build_args(Builder *b, size_t nargs, BuildParamDeclaration **args, bool vararg)
{
    C89_AnyParameterDeclaration *one;
    C89_TreeMultiParameterList *many;
    C89_AnyParameterList *all;
    size_t i;
    C89_AnyParameterTypeList *rv;
    assert (nargs);

    one = args[0]->ast_param_decl;
    all = (C89_AnyParameterList *)one;
    for (i = 1; i < nargs; ++i)
    {
        one = args[i]->ast_param_decl;
        many = c89_create_tree_multi_parameter_list(b->pool, all, b->comma, one);
        all = (C89_AnyParameterList *)many;
    }
    if (vararg)
    {
        C89_TreeVarargsParameterList *more = c89_create_tree_varargs_parameter_list(b->pool, all, b->comma, b->ellipsis);
        rv = (C89_AnyParameterTypeList *)more;
    }
    else
    {
        rv = (C89_AnyParameterTypeList *)all;
    }
    return (C89_OptParameterTypeList *)rv;
}
BuildType *build_type_function(Builder *b, BuildType *ret, size_t nargs, BuildParamDeclaration **args, bool vararg)
{
    C89_OptParameterTypeList *opt_args = build_args(b, nargs, args, vararg);
    return build_type_fun_ast(b, ret, opt_args, false, false);
}

BuildStatement *build_stmt_none(Builder *b)
{
    C89_OptCommaExpression *no_expr = (C89_OptCommaExpression *)b->nothing;
    C89_TreeExpressionStatement *stmt = c89_create_tree_expression_statement(b->pool, no_expr, b->semicolon);
    C89_AnyStatement *ast = (C89_AnyStatement *)stmt;
    return build_stmt_ast(b, ast);
}
BuildStatement *build_stmt_expr(Builder *b, BuildExpression *expr)
{
    C89_AnyCommaExpression *comma_expr = build_expr_to_comma(b, expr);
    C89_OptCommaExpression *some_expr = (C89_OptCommaExpression *)comma_expr;
    C89_TreeExpressionStatement *stmt = c89_create_tree_expression_statement(b->pool, some_expr, b->semicolon);
    C89_AnyStatement *ast = (C89_AnyStatement *)stmt;
    return build_stmt_ast(b, ast);
}
BuildStatement *build_stmt_label(Builder *b, const char *label, BuildStatement *target)
{
    C89_AtomIdentifier *id = build_id(b, label);
    C89_TreeLabelStatement *stmt = c89_create_tree_label_statement(b->pool, id, b->colon, target->ast_stmt);
    C89_AnyStatement *ast = (C89_AnyStatement *)stmt;
    return build_stmt_ast(b, ast);
}
BuildStatement *build_stmt_case(Builder *b, BuildExpression *expr, BuildStatement *target)
{
    C89_AnyConditionalExpression *cond = build_expr_to_conditional(b, expr);
    C89_TreeCaseStatement *stmt = c89_create_tree_case_statement(b->pool, b->kw_case, cond, b->colon, target->ast_stmt);
    C89_AnyStatement *ast = (C89_AnyStatement *)stmt;
    return build_stmt_ast(b, ast);
}
BuildStatement *build_stmt_default(Builder *b, BuildStatement *target)
{
    C89_TreeDefaultStatement *stmt = c89_create_tree_default_statement(b->pool, b->kw_default, b->colon, target->ast_stmt);
    C89_AnyStatement *ast = (C89_AnyStatement *)stmt;
    return build_stmt_ast(b, ast);
}
static C89_OptDeclarationList *build_decls(Builder *b, size_t ndecls, BuildDeclaration **decls)
{
    if (!ndecls)
    {
        return (C89_OptDeclarationList *)b->nothing;
    }
    else
    {
        C89_TreeDeclaration *one;
        C89_TreeMultiDeclarationList *many;
        C89_AnyDeclarationList *all;
        size_t i;

        one = decls[0]->ast_decl;
        all = (C89_AnyDeclarationList *)one;
        for (i = 1; i < ndecls; ++i)
        {
            one = decls[i]->ast_decl;
            many = c89_create_tree_multi_declaration_list(b->pool, all, one);
            all = (C89_AnyDeclarationList *)many;
        }
        return (C89_OptDeclarationList *)all;
    }
}
static C89_OptStatementList *build_stmts(Builder *b, size_t nstmts, BuildStatement **stmts)
{
    if (!nstmts)
    {
        return (C89_OptStatementList *)b->nothing;
    }
    else
    {
        C89_AnyStatement *one;
        C89_TreeMultiStatementList *many;
        C89_AnyStatementList *all;
        size_t i;

        one = stmts[0]->ast_stmt;
        all = (C89_AnyStatementList *)one;
        for (i = 1; i < nstmts; ++i)
        {
            one = stmts[i]->ast_stmt;
            many = c89_create_tree_multi_statement_list(b->pool, all, one);
            all = (C89_AnyStatementList *)many;
        }
        return (C89_OptStatementList *)all;
    }
}
BuildStatement *build_stmt_compound(Builder *b, size_t ndecls, BuildDeclaration **decls, size_t nstmts, BuildStatement **stmts)
{
    C89_OptDeclarationList *ast_decls = build_decls(b, ndecls, decls);
    C89_OptStatementList *ast_stmts = build_stmts(b, nstmts, stmts);
    C89_TreeCompoundStatement *stmt = c89_create_tree_compound_statement(b->pool, b->lbrace, ast_decls, ast_stmts, b->rbrace);
    C89_AnyStatement *ast = (C89_AnyStatement *)stmt;
    return build_stmt_ast(b, ast);
}
BuildStatement *build_stmt_if(Builder *b, BuildExpression *cond, BuildStatement *if_true)
{
    C89_AnyCommaExpression *expr = build_expr_to_comma(b, cond);
    C89_TreeCompoundStatement *comp = build_stmt_to_compound(b, if_true);
    C89_AnyStatement *body = (C89_AnyStatement *)comp;
    C89_TreeIfStatement *stmt = c89_create_tree_if_statement(b->pool, b->kw_if, b->lparen, expr, b->rparen, body);
    C89_AnyStatement *ast = (C89_AnyStatement *)stmt;
    return build_stmt_ast(b, ast);
}
BuildStatement *build_stmt_if_else(Builder *b, BuildExpression *cond, BuildStatement *if_true, BuildStatement *if_false)
{
    C89_AnyCommaExpression *expr = build_expr_to_comma(b, cond);
    C89_TreeCompoundStatement *if_comp = build_stmt_to_compound(b, if_true);
    C89_AnyStatement *if_body = (C89_AnyStatement *)if_comp;
    C89_AnyStatement *else_body = build_stmt_to_else_body(b, if_false);
    C89_TreeIfElseStatement *stmt = c89_create_tree_if_else_statement(b->pool, b->kw_if, b->lparen, expr, b->rparen, if_body, b->kw_else, else_body);
    C89_AnyStatement *ast = (C89_AnyStatement *)stmt;
    return build_stmt_ast(b, ast);
}
BuildStatement *build_stmt_while(Builder *b, BuildExpression *cond, BuildStatement *body)
{
    C89_AnyCommaExpression *expr = build_expr_to_comma(b, cond);
    C89_TreeCompoundStatement *comp = build_stmt_to_compound(b, body);
    C89_AnyStatement *loop_body = (C89_AnyStatement *)comp;
    C89_TreeWhileStatement *stmt = c89_create_tree_while_statement(b->pool, b->kw_while, b->lparen, expr, b->rparen, loop_body);
    C89_AnyStatement *ast = (C89_AnyStatement *)stmt;
    return build_stmt_ast(b, ast);
}
BuildStatement *build_stmt_do_while(Builder *b, BuildStatement *body, BuildExpression *cond)
{
    C89_TreeCompoundStatement *comp = build_stmt_to_compound(b, body);
    C89_AnyStatement *loop_body = (C89_AnyStatement *)comp;
    C89_AnyCommaExpression *expr = build_expr_to_comma(b, cond);
    C89_TreeDoWhileStatement *stmt = c89_create_tree_do_while_statement(b->pool, b->kw_do, loop_body, b->kw_while, b->lparen, expr, b->rparen, b->semicolon);
    C89_AnyStatement *ast = (C89_AnyStatement *)stmt;
    return build_stmt_ast(b, ast);
}
static C89_OptCommaExpression *build_expr_opt_expr(Builder *b, BuildExpression *expr)
{
    if (expr == build_expr_none(b))
    {
        return (C89_OptCommaExpression *)b->nothing;
    }
    else
    {
        C89_AnyCommaExpression *rv = build_expr_to_comma(b, expr);
        return (C89_OptCommaExpression *)rv;
    }
}
BuildStatement *build_stmt_for(Builder *b, BuildExpression *init, BuildExpression *cond, BuildExpression *post, BuildStatement *body)
{
    C89_OptCommaExpression *opt_init = build_expr_opt_expr(b, init);
    C89_OptCommaExpression *opt_cond = build_expr_opt_expr(b, cond);
    C89_OptCommaExpression *opt_post = build_expr_opt_expr(b, post);
    C89_TreeCompoundStatement *comp = build_stmt_to_compound(b, body);
    C89_AnyStatement *loop_body = (C89_AnyStatement *)comp;
    C89_TreeForStatement *stmt = c89_create_tree_for_statement(b->pool, b->kw_for, b->lparen, opt_init, b->semicolon, opt_cond, b->semicolon, opt_post, b->rparen, loop_body);
    C89_AnyStatement *ast = (C89_AnyStatement *)stmt;
    return build_stmt_ast(b, ast);
}
BuildStatement *build_stmt_goto(Builder *b, const char *label)
{
    C89_AtomIdentifier *id = build_id(b, label);
    C89_TreeGotoStatement *stmt = c89_create_tree_goto_statement(b->pool, b->kw_goto, id, b->semicolon);
    C89_AnyStatement *ast = (C89_AnyStatement *)stmt;
    return build_stmt_ast(b, ast);
}
BuildStatement *build_stmt_continue(Builder *b)
{
    C89_TreeContinueStatement *stmt = c89_create_tree_continue_statement(b->pool, b->kw_continue, b->semicolon);
    C89_AnyStatement *ast = (C89_AnyStatement *)stmt;
    return build_stmt_ast(b, ast);
}
BuildStatement *build_stmt_break(Builder *b)
{
    C89_TreeBreakStatement *stmt = c89_create_tree_break_statement(b->pool, b->kw_break, b->semicolon);
    C89_AnyStatement *ast = (C89_AnyStatement *)stmt;
    return build_stmt_ast(b, ast);
}
BuildStatement *build_stmt_return(Builder *b, BuildExpression *expr)
{
    C89_OptCommaExpression *opt_expr = build_expr_opt_expr(b, expr);
    C89_TreeReturnStatement *stmt = c89_create_tree_return_statement(b->pool, b->kw_return, opt_expr, b->semicolon);
    C89_AnyStatement *ast = (C89_AnyStatement *)stmt;
    return build_stmt_ast(b, ast);
}

BuildInitializer *build_initializer_expr(Builder *b, BuildExpression *expr)
{
    C89_AnyAssignmentExpression *assign = build_expr_to_assignment(b, expr);
    C89_AnyInitializer *ast = (C89_AnyInitializer *)assign;
    return build_init_ast(b, ast);
}
static C89_AnyInitializerList *build_init_list(Builder *b, size_t nelems, BuildInitializer **elems)
{
    C89_AnyInitializer *one;
    C89_TreeMultiInitializerList *many;
    C89_AnyInitializerList *all;
    size_t i;
    assert (nelems);

    one = elems[0]->ast_init;
    all = (C89_AnyInitializerList *)one;
    for (i = 1; i < nelems; ++i)
    {
        one = elems[i]->ast_init;
        many = c89_create_tree_multi_initializer_list(b->pool, all, b->comma, one);
        all = (C89_AnyInitializerList *)many;
    }
    return all;
}
BuildInitializer *build_initializer_braced(Builder *b, size_t nelems, BuildInitializer **elems)
{
    C89_AnyInitializerList *list = build_init_list(b, nelems, elems);
    C89_OptComma *no_comma = (C89_OptComma *)b->nothing;
    C89_TreeListInitializer *init = c89_create_tree_list_initializer(b->pool, b->lbrace, list, no_comma, b->rbrace);
    C89_AnyInitializer *ast = (C89_AnyInitializer *)init;
    return build_init_ast(b, ast);
}

static C89_TreeTypeName *build_type_to_typename(Builder *b, BuildType *type)
{
    BuildTypePairAbstractDeclarator pair = build_type_to_decl_abstract(b, type);
    return c89_create_tree_type_name(b->pool, pair.sql, pair.decl);
}

BuildExpression *build_expr_none(Builder *b)
{
    BuildExpression *zero = build_expr_int(b, 0);
    BuildExpression *sum = build_expr_plus(b, zero, zero);
    BuildType *ty_void = build_type_void(b);
    return build_expr_cast(b, ty_void, sum);
}
BuildExpression *build_expr_parenthesize(Builder *b, BuildExpression *expr)
{
    if (c89_is_tree_parenthesis_expression((C89_Ast *)expr->ast_expr))
        return expr;
    return build_expr_from_comma(b, (C89_AnyCommaExpression *)c89_create_tree_parenthesis_expression(b->pool, b->lparen, build_expr_to_comma(b, expr), b->rparen));
}
BuildExpression *build_expr_id(Builder *b, const char *id)
{
    C89_AtomIdentifier *i = build_id(b, id);
    return build_expr_from_primary(b, (C89_AnyPrimaryExpression *)i);
}


static char *really_do_quote(const char *s, size_t l, char q)
{
    size_t a = 3 + l;
    size_t i;
    char *m;
    char *p;
    for (i = 0; i < l; ++i)
    {
        unsigned char c = s[i];
        switch (c)
        {
        case '\a':
        case '\b':
        case '\f':
        case '\n':
        case '\r':
        case '\t':
        case '\v':
            a++;
            continue;
        }
        if (c == q || c == '\\')
            a++;
        else if (c < 0x20 || c >= 0x7f)
            a += 3;

    }
    p = m = (char *)malloc(a);
    *p++ = q;
    for (i = 0; i < l; ++i)
    {
        unsigned char c = s[i];
        switch (c)
        {
        case '\a':
            *p++ = '\\';
            *p++ = 'a';
            continue;
        case '\b':
            *p++ = '\\';
            *p++ = 'b';
            continue;
        case '\f':
            *p++ = '\\';
            *p++ = 'f';
            continue;
        case '\n':
            *p++ = '\\';
            *p++ = 'n';
            continue;
        case '\r':
            *p++ = '\\';
            *p++ = 'r';
            continue;
        case '\t':
            *p++ = '\\';
            *p++ = 't';
            continue;
        case '\v':
            *p++ = '\\';
            *p++ = 'v';
            continue;
        }
        if (c == q || c == '\\')
        {
            *p++ = '\\';
            *p++ = c;
            continue;
        }
        if (c < 0x20 || c >= 0x7f)
        {
            /* octal because \x is not limited to 2 digits */
            *p++ = '\\';
            *p++ = '0' + ((c >> 6) & 3);
            *p++ = '0' + ((c >> 3) & 7);
            *p++ = '0' + ((c >> 0) & 7);
            continue;
        }
        *p++ = c;
    }
    *p++ = q;
    *p = '\0';
    return m;
}

static void *transform_single_quote(Pool *pool, const void *str, size_t len)
{
    void *rv = really_do_quote((const char *)str, len, '\'');
    pool_free(pool, free, rv);
    return rv;
}
static void *transform_double_quote(Pool *pool, const void *str, size_t len)
{
    void *rv = really_do_quote((const char *)str, len, '"');
    pool_free(pool, free, rv);
    return rv;
}

static const char *pool_single_quote(Pool *pool, const char *str, size_t len)
{
    return (const char *)pool_intern_map(pool, transform_single_quote, str, len);
}

static const char *pool_double_quote(Pool *pool, const char *str, size_t len)
{
    return (const char *)pool_intern_map(pool, transform_double_quote, str, len);
}


BuildExpression *build_expr_char(Builder *b, char c)
{
    return build_expr_multi_char_slice(b, &c, 1);
}
BuildExpression *build_expr_multi_char(Builder *b, const char *c)
{
    return build_expr_multi_char_slice(b, c, strlen(c));
}
BuildExpression *build_expr_multi_char_slice(Builder *b, const char *c, size_t l)
{
    const char *q = pool_single_quote(b->pool, c, l);
    C89_AtomCharacterConstant *p = c89_create_atom_character_constant(b->pool, q);
    return build_expr_from_primary(b, (C89_AnyPrimaryExpression *)p);
}
BuildExpression *build_expr_string(Builder *b, const char *s)
{
    return build_expr_string_slice(b, s, strlen(s));
}
BuildExpression *build_expr_string_slice(Builder *b, const char *s, size_t l)
{
    const char *q = pool_double_quote(b->pool, s, l);
    C89_AtomStringLiteral *p = c89_create_atom_string_literal(b->pool, q);
    return build_expr_from_primary(b, (C89_AnyPrimaryExpression *)p);
}
BuildExpression *build_expr_int(Builder *b, uintmax_t i)
{
    char buf[20 + 1];
    int rv;
    C89_AtomDecimalConstant *p;
    if (i == 0)
        return build_expr_int_oct(b, i, 0);
    rv = sprintf(buf, "%ju", i);
    assert (rv >= 0 && (size_t)rv < sizeof(buf));
    p = c89_create_atom_decimal_constant(b->pool, buf);
    return build_expr_from_primary(b, (C89_AnyPrimaryExpression *)p);
}
BuildExpression *build_expr_int_oct(Builder *b, uintmax_t i, unsigned min_len)
{
    char buf[1 + 22 + 1];
    int rv;
    C89_AtomOctalConstant *p;
    if (min_len > 22)
        min_len = 22;
    rv = sprintf(buf, "%0#*jo", min_len + 1, i);
    assert (rv >= 0 && (size_t)rv < sizeof(buf));
    p = c89_create_atom_octal_constant(b->pool, buf);
    return build_expr_from_primary(b, (C89_AnyPrimaryExpression *)p);
}
BuildExpression *build_expr_int_hex(Builder *b, uintmax_t i, unsigned min_len)
{
    char buf[2 + 16 + 1];
    int rv;
    C89_AtomHexadecimalConstant *p;
    if (min_len > 16)
        min_len = 16;
    rv = sprintf(buf, "%0#*jx", min_len + 2, i);
    assert (rv >= 0 && (size_t)rv < sizeof(buf));
    p = c89_create_atom_hexadecimal_constant(b->pool, buf);
    return build_expr_from_primary(b, (C89_AnyPrimaryExpression *)p);
}
BuildExpression *build_expr_float(Builder *b, long double f)
{
    char buf[1 + 1 + 20 + 1 + 1 + 4 + 1];
    int rv;
    C89_AtomFloatingConstant *p;
    long double f2;
    rv = sprintf(buf, "%.21Le", f);
    assert (rv >= 0 && (size_t)rv < sizeof(buf));
    rv = sscanf(buf, "%Lf", &f2);
    assert (rv == 1 && f == f2);
    p = c89_create_atom_floating_constant(b->pool, buf);
    return build_expr_from_primary(b, (C89_AnyPrimaryExpression *)p);
}

BuildExpression *build_expr_index(Builder *b, BuildExpression *arr, BuildExpression *idx)
{
    C89_AnyPostfixExpression *a = build_expr_to_postfix(b, arr);
    C89_AnyCommaExpression *i = build_expr_to_comma(b, idx);
    C89_TreeIndexExpression *x = c89_create_tree_index_expression(b->pool, a, b->lbracket, i, b->rbracket);
    return build_expr_from_postfix(b, (C89_AnyPostfixExpression *)x);
}
BuildExpression *build_expr_call(Builder *b, BuildExpression *fun, size_t nargs, BuildExpression **args)
{
    C89_AnyPostfixExpression *f = build_expr_to_postfix(b, fun);
    C89_OptCommaExpression *a;
    C89_TreeCallExpression *x;
    if (nargs == 0)
    {
        a = (C89_OptCommaExpression *)b->nothing;
    }
    else
    {
        size_t i;
        BuildExpression *tmp = args[0];
        /* ensure parentheses if first argument is a comma expression */
        tmp = build_expr_from_assignment(b, build_expr_to_assignment(b, tmp));
        for (i = 1; i < nargs; ++i)
        {
            tmp = build_expr_comma(b, tmp, args[i]);
        }
        a = (C89_OptCommaExpression *)build_expr_to_comma(b, tmp);
    }
    x = c89_create_tree_call_expression(b->pool, f, b->lparen, a, b->rparen);
    return build_expr_from_postfix(b, (C89_AnyPostfixExpression *)x);
}
BuildExpression *build_expr_direct_member(Builder *b, BuildExpression *obj, const char *field)
{
    C89_AnyPostfixExpression *o = build_expr_to_postfix(b, obj);
    C89_AtomIdentifier *f = build_id(b, field);
    C89_TreeDotExpression *x = c89_create_tree_dot_expression(b->pool, o, b->dot, f);
    return build_expr_from_postfix(b, (C89_AnyPostfixExpression *)x);
}
BuildExpression *build_expr_indirect_member(Builder *b, BuildExpression *ptr, const char *field)
{
    C89_AnyPostfixExpression *p = build_expr_to_postfix(b, ptr);
    C89_AtomIdentifier *f = build_id(b, field);
    C89_TreeArrowExpression *x = c89_create_tree_arrow_expression(b->pool, p, b->arrow, f);
    return build_expr_from_postfix(b, (C89_AnyPostfixExpression *)x);
}
BuildExpression *build_expr_post_inc(Builder *b, BuildExpression *lhs)
{
    C89_AnyPostfixExpression *l = build_expr_to_postfix(b, lhs);
    C89_TreePostIncrementExpression *x = c89_create_tree_post_increment_expression(b->pool, l, b->incr);
    return build_expr_from_postfix(b, (C89_AnyPostfixExpression *)x);
}
BuildExpression *build_expr_post_dec(Builder *b, BuildExpression *lhs)
{
    C89_AnyPostfixExpression *l = build_expr_to_postfix(b, lhs);
    C89_TreePostDecrementExpression *x = c89_create_tree_post_decrement_expression(b->pool, l, b->decr);
    return build_expr_from_postfix(b, (C89_AnyPostfixExpression *)x);
}
BuildExpression *build_expr_pre_inc(Builder *b, BuildExpression *rhs)
{
    C89_AnyUnaryExpression *r = build_expr_to_unary(b, rhs);
    C89_TreePreIncrementExpression *x = c89_create_tree_pre_increment_expression(b->pool, b->incr, r);
    return build_expr_from_unary(b, (C89_AnyUnaryExpression *)x);
}
BuildExpression *build_expr_pre_dec(Builder *b, BuildExpression *rhs)
{
    C89_AnyUnaryExpression *r = build_expr_to_unary(b, rhs);
    C89_TreePreDecrementExpression *x = c89_create_tree_pre_decrement_expression(b->pool, b->decr, r);
    return build_expr_from_unary(b, (C89_AnyUnaryExpression *)x);
}
BuildExpression *build_expr_addressof(Builder *b, BuildExpression *obj)
{
    C89_AnyUnaryExpression *o = build_expr_to_unary(b, obj);
    C89_TreeAddressofExpression *x = c89_create_tree_addressof_expression(b->pool, b->ampersand, o);
    return build_expr_from_unary(b, (C89_AnyUnaryExpression *)x);
}
BuildExpression *build_expr_deref(Builder *b, BuildExpression *ptr)
{
    C89_AnyUnaryExpression *p = build_expr_to_unary(b, ptr);
    C89_TreeDereferenceExpression *x = c89_create_tree_dereference_expression(b->pool, b->star, p);
    return build_expr_from_unary(b, (C89_AnyUnaryExpression *)x);
}
BuildExpression *build_expr_unary_plus(Builder *b, BuildExpression *rhs)
{
    C89_AnyUnaryExpression *r = build_expr_to_unary(b, rhs);
    C89_TreeUnaryPlusExpression *x = c89_create_tree_unary_plus_expression(b->pool, b->plus, r);
    return build_expr_from_unary(b, (C89_AnyUnaryExpression *)x);
}
BuildExpression *build_expr_unary_minus(Builder *b, BuildExpression *rhs)
{
    C89_AnyUnaryExpression *r = build_expr_to_unary(b, rhs);
    C89_TreeUnaryMinusExpression *x = c89_create_tree_unary_minus_expression(b->pool, b->minus, r);
    return build_expr_from_unary(b, (C89_AnyUnaryExpression *)x);
}
BuildExpression *build_expr_bitwise_not(Builder *b, BuildExpression *rhs)
{
    C89_AnyUnaryExpression *r = build_expr_to_unary(b, rhs);
    C89_TreeBitwiseNotExpression *x = c89_create_tree_bitwise_not_expression(b->pool, b->tilde, r);
    return build_expr_from_unary(b, (C89_AnyUnaryExpression *)x);
}
BuildExpression *build_expr_logical_not(Builder *b, BuildExpression *rhs)
{
    C89_AnyUnaryExpression *r = build_expr_to_unary(b, rhs);
    C89_TreeLogicalNotExpression *x = c89_create_tree_logical_not_expression(b->pool, b->bang, r);
    return build_expr_from_unary(b, (C89_AnyUnaryExpression *)x);
}
BuildExpression *build_expr_sizeof_expr(Builder *b, BuildExpression *obj)
{
    C89_AnyUnaryExpression *o = build_expr_to_unary(b, build_expr_parenthesize(b, obj));
    C89_TreeSizeofExpressionExpression *x = c89_create_tree_sizeof_expression_expression(b->pool, b->kw_sizeof, o);
    return build_expr_from_unary(b, (C89_AnyUnaryExpression *)x);
}
BuildExpression *build_expr_sizeof_type(Builder *b, BuildType *type)
{
    C89_TreeTypeName *t = build_type_to_typename(b, type);
    C89_TreeSizeofTypeExpression *x = c89_create_tree_sizeof_type_expression(b->pool, b->kw_sizeof, b->lparen, t, b->rparen);
    return build_expr_from_unary(b, (C89_AnyUnaryExpression *)x);
}
BuildExpression *build_expr_cast(Builder *b, BuildType *type, BuildExpression *rhs)
{
    C89_TreeTypeName *t = build_type_to_typename(b, type);
    C89_AnyUnaryExpression *r = build_expr_to_unary(b, rhs);
    C89_TreeCastExpression *x = c89_create_tree_cast_expression(b->pool, b->lparen, t, b->rparen, r);
    return build_expr_from_unary(b, (C89_AnyUnaryExpression *)x);
}
BuildExpression *build_expr_times(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyMultiplicativeExpression *l = build_expr_to_mul(b, lhs);
    C89_AnyUnaryExpression *r = build_expr_to_unary(b, rhs);
    C89_TreeTimesExpression *x = c89_create_tree_times_expression(b->pool, l, b->star, r);
    return build_expr_from_mul(b, (C89_AnyMultiplicativeExpression *)x);
}
BuildExpression *build_expr_divide(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyMultiplicativeExpression *l = build_expr_to_mul(b, lhs);
    C89_AnyUnaryExpression *r = build_expr_to_unary(b, rhs);
    C89_TreeDivideExpression *x = c89_create_tree_divide_expression(b->pool, l, b->slash, r);
    return build_expr_from_mul(b, (C89_AnyMultiplicativeExpression *)x);
}
BuildExpression *build_expr_modulus(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyMultiplicativeExpression *l = build_expr_to_mul(b, lhs);
    C89_AnyUnaryExpression *r = build_expr_to_unary(b, rhs);
    C89_TreeModulusExpression *x = c89_create_tree_modulus_expression(b->pool, l, b->percent, r);
    return build_expr_from_mul(b, (C89_AnyMultiplicativeExpression *)x);
}
BuildExpression *build_expr_plus(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyAdditiveExpression *l = build_expr_to_add(b, lhs);
    C89_AnyMultiplicativeExpression *r = build_expr_to_mul(b, rhs);
    C89_TreePlusExpression *x = c89_create_tree_plus_expression(b->pool, l, b->plus, r);
    return build_expr_from_add(b, (C89_AnyAdditiveExpression *)x);
}
BuildExpression *build_expr_minus(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyAdditiveExpression *l = build_expr_to_add(b, lhs);
    C89_AnyMultiplicativeExpression *r = build_expr_to_mul(b, rhs);
    C89_TreeMinusExpression *x = c89_create_tree_minus_expression(b->pool, l, b->minus, r);
    return build_expr_from_add(b, (C89_AnyAdditiveExpression *)x);
}
BuildExpression *build_expr_lshift(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyShiftExpression *l = build_expr_to_shift(b, lhs);
    C89_AnyAdditiveExpression *r = build_expr_to_add(b, rhs);
    C89_TreeLeftShiftExpression *x = c89_create_tree_left_shift_expression(b->pool, l, b->lshift, r);
    return build_expr_from_shift(b, (C89_AnyShiftExpression *)x);
}
BuildExpression *build_expr_rshift(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyShiftExpression *l = build_expr_to_shift(b, lhs);
    C89_AnyAdditiveExpression *r = build_expr_to_add(b, rhs);
    C89_TreeRightShiftExpression *x = c89_create_tree_right_shift_expression(b->pool, l, b->rshift, r);
    return build_expr_from_shift(b, (C89_AnyShiftExpression *)x);
}
BuildExpression *build_expr_cmp_lt(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyRelationalExpression *l = build_expr_to_rel(b, lhs);
    C89_AnyShiftExpression *r = build_expr_to_shift(b, rhs);
    C89_TreeCompareLtExpression *x = c89_create_tree_compare_lt_expression(b->pool, l, b->lt, r);
    return build_expr_from_rel(b, (C89_AnyRelationalExpression *)x);
}
BuildExpression *build_expr_cmp_gt(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyRelationalExpression *l = build_expr_to_rel(b, lhs);
    C89_AnyShiftExpression *r = build_expr_to_shift(b, rhs);
    C89_TreeCompareGtExpression *x = c89_create_tree_compare_gt_expression(b->pool, l, b->gt, r);
    return build_expr_from_rel(b, (C89_AnyRelationalExpression *)x);
}
BuildExpression *build_expr_cmp_le(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyRelationalExpression *l = build_expr_to_rel(b, lhs);
    C89_AnyShiftExpression *r = build_expr_to_shift(b, rhs);
    C89_TreeCompareLeExpression *x = c89_create_tree_compare_le_expression(b->pool, l, b->le, r);
    return build_expr_from_rel(b, (C89_AnyRelationalExpression *)x);
}
BuildExpression *build_expr_cmp_ge(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyRelationalExpression *l = build_expr_to_rel(b, lhs);
    C89_AnyShiftExpression *r = build_expr_to_shift(b, rhs);
    C89_TreeCompareGeExpression *x = c89_create_tree_compare_ge_expression(b->pool, l, b->ge, r);
    return build_expr_from_rel(b, (C89_AnyRelationalExpression *)x);
}
BuildExpression *build_expr_cmp_eq(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyEqualityExpression *l = build_expr_to_eq(b, lhs);
    C89_AnyRelationalExpression *r = build_expr_to_rel(b, rhs);
    C89_TreeCompareEqExpression *x = c89_create_tree_compare_eq_expression(b->pool, l, b->eq, r);
    return build_expr_from_eq(b, (C89_AnyEqualityExpression *)x);
}
BuildExpression *build_expr_cmp_ne(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyEqualityExpression *l = build_expr_to_eq(b, lhs);
    C89_AnyRelationalExpression *r = build_expr_to_rel(b, rhs);
    C89_TreeCompareNeExpression *x = c89_create_tree_compare_ne_expression(b->pool, l, b->ne, r);
    return build_expr_from_eq(b, (C89_AnyEqualityExpression *)x);
}
BuildExpression *build_expr_bitwise_and(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyAndExpression *l = build_expr_to_bit_and(b, lhs);
    C89_AnyEqualityExpression *r = build_expr_to_eq(b, rhs);
    C89_TreeBitwiseAndExpression *x = c89_create_tree_bitwise_and_expression(b->pool, l, b->ampersand, r);
    return build_expr_from_bit_and(b, (C89_AnyAndExpression *)x);
}
BuildExpression *build_expr_bitwise_xor(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyExclusiveOrExpression *l = build_expr_to_bit_xor(b, lhs);
    C89_AnyAndExpression *r = build_expr_to_bit_and(b, rhs);
    C89_TreeBitwiseXorExpression *x = c89_create_tree_bitwise_xor_expression(b->pool, l, b->caret, r);
    return build_expr_from_bit_xor(b, (C89_AnyExclusiveOrExpression *)x);
}
BuildExpression *build_expr_bitwise_or(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyInclusiveOrExpression *l = build_expr_to_bit_or(b, lhs);
    C89_AnyExclusiveOrExpression *r = build_expr_to_bit_xor(b, rhs);
    C89_TreeBitwiseOrExpression *x = c89_create_tree_bitwise_or_expression(b->pool, l, b->pipe, r);
    return build_expr_from_bit_or(b, (C89_AnyInclusiveOrExpression *)x);
}
BuildExpression *build_expr_logical_and(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyLogicalAndExpression *l = build_expr_to_log_and(b, lhs);
    C89_AnyInclusiveOrExpression *r = build_expr_to_bit_or(b, rhs);
    C89_TreeBinaryAndExpression *x = c89_create_tree_binary_and_expression(b->pool, l, b->logical_and, r);
    return build_expr_from_log_and(b, (C89_AnyLogicalAndExpression *)x);
}
BuildExpression *build_expr_logical_or(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyLogicalOrExpression *l = build_expr_to_log_or(b, lhs);
    C89_AnyLogicalAndExpression *r = build_expr_to_log_and(b, rhs);
    C89_TreeBinaryOrExpression *x = c89_create_tree_binary_or_expression(b->pool, l, b->logical_or, r);
    return build_expr_from_log_or(b, (C89_AnyLogicalOrExpression *)x);
}
BuildExpression *build_expr_conditional(Builder *b, BuildExpression *cond, BuildExpression *if_true, BuildExpression *if_false)
{
    C89_AnyLogicalOrExpression *c = build_expr_to_log_or(b, cond);
    C89_AnyCommaExpression *t = build_expr_to_comma(b, if_true);
    C89_AnyConditionalExpression *f = build_expr_to_conditional(b, if_false);
    C89_TreeTernaryExpression *x = c89_create_tree_ternary_expression(b->pool, c, b->query, t, b->colon, f);
    return build_expr_from_conditional(b, (C89_AnyConditionalExpression *)x);
}

BuildExpression *build_expr_assign(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyUnaryExpression *l = build_expr_to_unary(b, lhs);
    C89_AnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    C89_TreeSimpleAssignment *a = c89_create_tree_simple_assignment(b->pool, l, b->assign, r);
    return build_expr_from_assignment(b, (C89_AnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_times(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyUnaryExpression *l = build_expr_to_unary(b, lhs);
    C89_AnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    C89_TreeCompoundTimesAssignment *a = c89_create_tree_compound_times_assignment(b->pool, l, b->star_assign, r);
    return build_expr_from_assignment(b, (C89_AnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_divide(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyUnaryExpression *l = build_expr_to_unary(b, lhs);
    C89_AnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    C89_TreeCompoundDivideAssignment *a = c89_create_tree_compound_divide_assignment(b->pool, l, b->slash_assign, r);
    return build_expr_from_assignment(b, (C89_AnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_modulus(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyUnaryExpression *l = build_expr_to_unary(b, lhs);
    C89_AnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    C89_TreeCompoundModulusAssignment *a = c89_create_tree_compound_modulus_assignment(b->pool, l, b->percent_assign, r);
    return build_expr_from_assignment(b, (C89_AnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_plus(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyUnaryExpression *l = build_expr_to_unary(b, lhs);
    C89_AnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    C89_TreeCompoundPlusAssignment *a = c89_create_tree_compound_plus_assignment(b->pool, l, b->plus_assign, r);
    return build_expr_from_assignment(b, (C89_AnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_minus(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyUnaryExpression *l = build_expr_to_unary(b, lhs);
    C89_AnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    C89_TreeCompoundMinusAssignment *a = c89_create_tree_compound_minus_assignment(b->pool, l, b->minus_assign, r);
    return build_expr_from_assignment(b, (C89_AnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_lshift(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyUnaryExpression *l = build_expr_to_unary(b, lhs);
    C89_AnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    C89_TreeCompoundLeftShiftAssignment *a = c89_create_tree_compound_left_shift_assignment(b->pool, l, b->lshift_assign, r);
    return build_expr_from_assignment(b, (C89_AnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_rshift(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyUnaryExpression *l = build_expr_to_unary(b, lhs);
    C89_AnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    C89_TreeCompoundRightShiftAssignment *a = c89_create_tree_compound_right_shift_assignment(b->pool, l, b->rshift_assign, r);
    return build_expr_from_assignment(b, (C89_AnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_bitwise_and(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyUnaryExpression *l = build_expr_to_unary(b, lhs);
    C89_AnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    C89_TreeCompoundBitwiseAndAssignment *a = c89_create_tree_compound_bitwise_and_assignment(b->pool, l, b->ampersand_assign, r);
    return build_expr_from_assignment(b, (C89_AnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_bitwise_xor(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyUnaryExpression *l = build_expr_to_unary(b, lhs);
    C89_AnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    C89_TreeCompoundBitwiseXorAssignment *a = c89_create_tree_compound_bitwise_xor_assignment(b->pool, l, b->caret_assign, r);
    return build_expr_from_assignment(b, (C89_AnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_bitwise_or(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyUnaryExpression *l = build_expr_to_unary(b, lhs);
    C89_AnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    C89_TreeCompoundBitwiseOrAssignment *a = c89_create_tree_compound_bitwise_or_assignment(b->pool, l, b->pipe_assign, r);
    return build_expr_from_assignment(b, (C89_AnyAssignmentExpression *)a);
}
BuildExpression *build_expr_comma(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    C89_AnyCommaExpression *l = build_expr_to_comma(b, lhs);
    C89_AnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    C89_TreeBinaryCommaExpression *x = c89_create_tree_binary_comma_expression(b->pool, l, b->comma, r);
    return build_expr_from_comma(b, (C89_AnyCommaExpression *)x);
}
