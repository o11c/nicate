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
#include "gnu-c.gen.h"
#include "pool.h"

#define ZERO_PAIR(a) 0, NULL


static GnuCTreeTailDeclarationSpecifiers *helper_create_type_specifiers2a(Builder *b, GnuCAnyTypeSpecifier *ta, GnuCAnyTypeSpecifier *tb)
{
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCOptHeadDeclarationSpecifiers *no_head = (GnuCOptHeadDeclarationSpecifiers *)b->nothing;
    GnuCTreeTailDeclarationSpecifiers *rv = (GnuCTreeTailDeclarationSpecifiers *)ta;
    rv = gnu_c_create_tree_tail_declaration_specifiers(b->pool, (GnuCOptTailDeclarationSpecifiers *)rv, (GnuCAnyTailDeclarationSpecifier *)tb, no_attrs, no_head);
    return rv;
}
static GnuCTreeTailDeclarationSpecifiers *helper_create_type_specifiers3a(Builder *b, GnuCAnyTypeSpecifier *ta, GnuCAnyTypeSpecifier *tb, GnuCAnyTypeSpecifier *tc)
{
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCOptHeadDeclarationSpecifiers *no_head = (GnuCOptHeadDeclarationSpecifiers *)b->nothing;
    GnuCTreeTailDeclarationSpecifiers *rv = (GnuCTreeTailDeclarationSpecifiers *)ta;
    rv = gnu_c_create_tree_tail_declaration_specifiers(b->pool, (GnuCOptTailDeclarationSpecifiers *)rv, (GnuCAnyTailDeclarationSpecifier *)tb, no_attrs, no_head);
    rv = gnu_c_create_tree_tail_declaration_specifiers(b->pool, (GnuCOptTailDeclarationSpecifiers *)rv, (GnuCAnyTailDeclarationSpecifier *)tc, no_attrs, no_head);
    return rv;
}
static GnuCTreeTailDeclarationSpecifiers *helper_create_type_specifiers4a(Builder *b, GnuCAnyTypeSpecifier *ta, GnuCAnyTypeSpecifier *tb, GnuCAnyTypeSpecifier *tc, GnuCAnyTypeSpecifier *td)
{
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCOptHeadDeclarationSpecifiers *no_head = (GnuCOptHeadDeclarationSpecifiers *)b->nothing;
    GnuCTreeTailDeclarationSpecifiers *rv = (GnuCTreeTailDeclarationSpecifiers *)ta;
    rv = gnu_c_create_tree_tail_declaration_specifiers(b->pool, (GnuCOptTailDeclarationSpecifiers *)rv, (GnuCAnyTailDeclarationSpecifier *)tb, no_attrs, no_head);
    rv = gnu_c_create_tree_tail_declaration_specifiers(b->pool, (GnuCOptTailDeclarationSpecifiers *)rv, (GnuCAnyTailDeclarationSpecifier *)tc, no_attrs, no_head);
    rv = gnu_c_create_tree_tail_declaration_specifiers(b->pool, (GnuCOptTailDeclarationSpecifiers *)rv, (GnuCAnyTailDeclarationSpecifier *)td, no_attrs, no_head);
    return rv;
}

static GnuCAnySpecifierQualifierList *helper_create_type_specifiers2b(Builder *b, GnuCAnyTypeSpecifier *ta, GnuCAnyTypeSpecifier *tb)
{
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCTreeTagSqListSpecifier *rv = (GnuCTreeTagSqListSpecifier *)ta;
    rv = gnu_c_create_tree_tag_sq_list_specifier(b->pool, (GnuCOptSpecifierQualifierList *)rv, tb, no_attrs);
    return (GnuCAnySpecifierQualifierList *)rv;
}
static GnuCAnySpecifierQualifierList *helper_create_type_specifiers3b(Builder *b, GnuCAnyTypeSpecifier *ta, GnuCAnyTypeSpecifier *tb, GnuCAnyTypeSpecifier *tc)
{
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCTreeTagSqListSpecifier *rv = (GnuCTreeTagSqListSpecifier *)ta;
    rv = gnu_c_create_tree_tag_sq_list_specifier(b->pool, (GnuCOptSpecifierQualifierList *)rv, tb, no_attrs);
    rv = gnu_c_create_tree_tag_sq_list_specifier(b->pool, (GnuCOptSpecifierQualifierList *)rv, tc, no_attrs);
    return (GnuCAnySpecifierQualifierList *)rv;
}
static GnuCAnySpecifierQualifierList *helper_create_type_specifiers4b(Builder *b, GnuCAnyTypeSpecifier *ta, GnuCAnyTypeSpecifier *tb, GnuCAnyTypeSpecifier *tc, GnuCAnyTypeSpecifier *td)
{
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCTreeTagSqListSpecifier *rv = (GnuCTreeTagSqListSpecifier *)ta;
    rv = gnu_c_create_tree_tag_sq_list_specifier(b->pool, (GnuCOptSpecifierQualifierList *)rv, tb, no_attrs);
    rv = gnu_c_create_tree_tag_sq_list_specifier(b->pool, (GnuCOptSpecifierQualifierList *)rv, tc, no_attrs);
    rv = gnu_c_create_tree_tag_sq_list_specifier(b->pool, (GnuCOptSpecifierQualifierList *)rv, td, no_attrs);
    return (GnuCAnySpecifierQualifierList *)rv;
}

Builder *builder_create(void)
{
    Builder *rv = (Builder *)calloc(1, sizeof(Builder));
    Pool *pool = rv->pool = pool_create();
    rv->nothing = gnu_c_create_nothing(pool);

    rv->arrow = gnu_c_create_sym_arrow(pool);
    rv->comma = gnu_c_create_sym_comma(pool);
    rv->dot = gnu_c_create_sym_dot(pool);
    rv->ellipsis = gnu_c_create_sym_ellipsis(pool);
    rv->lbrace = gnu_c_create_sym_lbrace(pool);
    rv->lbracket = gnu_c_create_sym_lbracket(pool);
    rv->lparen = gnu_c_create_sym_lparen(pool);
    rv->rbrace = gnu_c_create_sym_rbrace(pool);
    rv->rbracket = gnu_c_create_sym_rbracket(pool);
    rv->rparen = gnu_c_create_sym_rparen(pool);
    rv->semicolon = gnu_c_create_sym_semicolon(pool);

    rv->ampersand = gnu_c_create_sym_ampersand(pool);
    rv->bang = gnu_c_create_sym_bang(pool);
    rv->caret = gnu_c_create_sym_caret(pool);
    rv->colon = gnu_c_create_sym_colon(pool);
    rv->decr = gnu_c_create_sym_decr(pool);
    rv->eq = gnu_c_create_sym_eq(pool);
    rv->ge = gnu_c_create_sym_ge(pool);
    rv->gt = gnu_c_create_sym_gt(pool);
    rv->incr = gnu_c_create_sym_incr(pool);
    rv->le = gnu_c_create_sym_le(pool);
    rv->logical_and = gnu_c_create_sym_logical_and(pool);
    rv->logical_or = gnu_c_create_sym_logical_or(pool);
    rv->lshift = gnu_c_create_sym_lshift(pool);
    rv->lt = gnu_c_create_sym_lt(pool);
    rv->minus = gnu_c_create_sym_minus(pool);
    rv->ne = gnu_c_create_sym_ne(pool);
    rv->percent = gnu_c_create_sym_percent(pool);
    rv->pipe = gnu_c_create_sym_pipe(pool);
    rv->plus = gnu_c_create_sym_plus(pool);
    rv->query = gnu_c_create_sym_query(pool);
    rv->rshift = gnu_c_create_sym_rshift(pool);
    rv->slash = gnu_c_create_sym_slash(pool);
    rv->star = gnu_c_create_sym_star(pool);
    rv->tilde = gnu_c_create_sym_tilde(pool);

    rv->assign = gnu_c_create_sym_assign(pool);
    rv->ampersand_assign = gnu_c_create_sym_ampersand_assign(pool);
    rv->caret_assign = gnu_c_create_sym_caret_assign(pool);
    rv->lshift_assign = gnu_c_create_sym_lshift_assign(pool);
    rv->minus_assign = gnu_c_create_sym_minus_assign(pool);
    rv->percent_assign = gnu_c_create_sym_percent_assign(pool);
    rv->pipe_assign = gnu_c_create_sym_pipe_assign(pool);
    rv->plus_assign = gnu_c_create_sym_plus_assign(pool);
    rv->rshift_assign = gnu_c_create_sym_rshift_assign(pool);
    rv->slash_assign = gnu_c_create_sym_slash_assign(pool);
    rv->star_assign = gnu_c_create_sym_star_assign(pool);

    rv->kw_break = gnu_c_create_kw_break(pool);
    rv->kw_case = gnu_c_create_kw_case(pool);
    rv->kw_const = gnu_c_create_kw_const(pool);
    rv->kw_continue = gnu_c_create_kw_continue(pool);
    rv->kw_default = gnu_c_create_kw_default(pool);
    rv->kw_do = gnu_c_create_kw_do(pool);
    rv->kw_else = gnu_c_create_kw_else(pool);
    rv->kw_enum = gnu_c_create_kw_enum(pool);
    rv->kw_extern = gnu_c_create_kw_extern(pool);
    rv->kw_for = gnu_c_create_kw_for(pool);
    rv->kw_goto = gnu_c_create_kw_goto(pool);
    rv->kw_if = gnu_c_create_kw_if(pool);
    rv->kw_register = gnu_c_create_kw_register(pool);
    rv->kw_return = gnu_c_create_kw_return(pool);
    rv->kw_sizeof = gnu_c_create_kw_sizeof(pool);
    rv->kw_static = gnu_c_create_kw_static(pool);
    rv->kw_struct = gnu_c_create_kw_struct(pool);
    rv->kw_switch = gnu_c_create_kw_switch(pool);
    rv->kw_typedef = gnu_c_create_kw_typedef(pool);
    rv->kw_union = gnu_c_create_kw_union(pool);
    rv->kw_volatile = gnu_c_create_kw_volatile(pool);
    rv->kw_while = gnu_c_create_kw_while(pool);

    {
        GnuCAnyTypeSpecifier *kw_signed = (GnuCAnyTypeSpecifier *)gnu_c_create_kw_signed(pool);
        GnuCAnyTypeSpecifier *kw_unsigned = (GnuCAnyTypeSpecifier *)gnu_c_create_kw_unsigned(pool);
        GnuCAnyTypeSpecifier *kw_void = (GnuCAnyTypeSpecifier *)gnu_c_create_kw_void(pool);
        GnuCAnyTypeSpecifier *kw_bool = (GnuCAnyTypeSpecifier *)gnu_c_create_kw_bool(pool);
        GnuCAnyTypeSpecifier *kw_char = (GnuCAnyTypeSpecifier *)gnu_c_create_kw_char(pool);
        GnuCAnyTypeSpecifier *kw_short = (GnuCAnyTypeSpecifier *)gnu_c_create_kw_short(pool);
        GnuCAnyTypeSpecifier *kw_int = (GnuCAnyTypeSpecifier *)gnu_c_create_kw_int(pool);
        GnuCAnyTypeSpecifier *kw_long = (GnuCAnyTypeSpecifier *)gnu_c_create_kw_long(pool);
        GnuCAnyTypeSpecifier *kw_float = (GnuCAnyTypeSpecifier *)gnu_c_create_kw_float(pool);
        GnuCAnyTypeSpecifier *kw_double = (GnuCAnyTypeSpecifier *)gnu_c_create_kw_double(pool);

        rv->ty_void_ds = (GnuCTreeTailDeclarationSpecifiers *)kw_void;
        rv->ty_bool_ds = (GnuCTreeTailDeclarationSpecifiers *)kw_bool;
        rv->ty_char_ds = (GnuCTreeTailDeclarationSpecifiers *)kw_char;
        rv->ty_signed_char_ds = helper_create_type_specifiers2a(rv, kw_signed, kw_char);
        rv->ty_unsigned_char_ds = helper_create_type_specifiers2a(rv, kw_unsigned, kw_char);

        rv->ty_signed_short_ds = helper_create_type_specifiers3a(rv, kw_signed, kw_short, kw_int);
        rv->ty_unsigned_short_ds = helper_create_type_specifiers3a(rv, kw_unsigned, kw_short, kw_int);
        rv->ty_signed_int_ds = helper_create_type_specifiers2a(rv, kw_signed, kw_int);
        rv->ty_unsigned_int_ds = helper_create_type_specifiers2a(rv, kw_unsigned, kw_int);
        rv->ty_signed_long_ds = helper_create_type_specifiers3a(rv, kw_signed, kw_long, kw_int);
        rv->ty_unsigned_long_ds = helper_create_type_specifiers3a(rv, kw_unsigned, kw_long, kw_int);
        rv->ty_signed_long_long_ds = helper_create_type_specifiers4a(rv, kw_signed, kw_long, kw_long, kw_int);
        rv->ty_unsigned_long_long_ds = helper_create_type_specifiers4a(rv, kw_unsigned, kw_long, kw_long, kw_int);

        rv->ty_float_ds = (GnuCTreeTailDeclarationSpecifiers *)kw_float;
        rv->ty_double_ds = (GnuCTreeTailDeclarationSpecifiers *)kw_double;
        rv->ty_long_double_ds = (GnuCTreeTailDeclarationSpecifiers *)helper_create_type_specifiers2a(rv, kw_long, kw_double);


        rv->ty_void_sql = (GnuCAnySpecifierQualifierList *)kw_void;
        rv->ty_bool_sql = (GnuCAnySpecifierQualifierList *)kw_bool;
        rv->ty_char_sql = (GnuCAnySpecifierQualifierList *)kw_char;
        rv->ty_signed_char_sql = helper_create_type_specifiers2b(rv, kw_signed, kw_char);
        rv->ty_unsigned_char_sql = helper_create_type_specifiers2b(rv, kw_unsigned, kw_char);

        rv->ty_signed_short_sql = helper_create_type_specifiers3b(rv, kw_signed, kw_short, kw_int);
        rv->ty_unsigned_short_sql = helper_create_type_specifiers3b(rv, kw_unsigned, kw_short, kw_int);
        rv->ty_signed_int_sql = helper_create_type_specifiers2b(rv, kw_signed, kw_int);
        rv->ty_unsigned_int_sql = helper_create_type_specifiers2b(rv, kw_unsigned, kw_int);
        rv->ty_signed_long_sql = helper_create_type_specifiers3b(rv, kw_signed, kw_long, kw_int);
        rv->ty_unsigned_long_sql = helper_create_type_specifiers3b(rv, kw_unsigned, kw_long, kw_int);
        rv->ty_signed_long_long_sql = helper_create_type_specifiers4b(rv, kw_signed, kw_long, kw_long, kw_int);
        rv->ty_unsigned_long_long_sql = helper_create_type_specifiers4b(rv, kw_unsigned, kw_long, kw_long, kw_int);

        rv->ty_float_sql = (GnuCAnySpecifierQualifierList *)kw_float;
        rv->ty_double_sql = (GnuCAnySpecifierQualifierList *)kw_double;
        rv->ty_long_double_sql = (GnuCAnySpecifierQualifierList *)helper_create_type_specifiers2b(rv, kw_long, kw_double);
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
    gnu_c_emit((GnuCAst *)tu->ast_tu, fp);
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

BuildTranslationUnit *build_tu(Builder *b, size_t ntops, BuildStatement **tops)
{
    GnuCOptExternalDeclarations *all;
    size_t i;
    assert (ntops);

    all = (GnuCOptExternalDeclarations *)b->nothing;
    for (i = 0; i < ntops; ++i)
    {
        GnuCAnyExternalDeclaration *one;
        GnuCTreeExternalDeclarations *many;
        one = tops[i]->ast_ext;
        many = gnu_c_create_tree_external_declarations(b->pool, all, one);
        all = (GnuCOptExternalDeclarations *)many;
    }
    return build_tu_ast(b, all);
}
BuildStatement *build_comment(Builder *b, const char *txt)
{
    GnuCAtomComment *ast = gnu_c_create_atom_comment(b->pool, txt);
    return build_comment_ast(b, ast);
}

BuildStatement *build_struct_declaration(Builder *b, const char *name)
{
    GnuCAnyStructOrUnion *ast_kw = (GnuCAnyStructOrUnion *)b->kw_struct;
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCAtomIdentifier *id = build_id(b, name);
    GnuCTreeTagForwardStructOrUnionSpecifier *ast_struct = gnu_c_create_tree_tag_forward_struct_or_union_specifier(b->pool, ast_kw, no_attrs, id);
    GnuCOptInitDeclaratorList *no_idl = (GnuCOptInitDeclaratorList *)b->nothing;
    GnuCAnyDeclarationSpecifiers *ast_spec = (GnuCAnyDeclarationSpecifiers *)ast_struct;
    GnuCTreeDeclaration *ast_decl = gnu_c_create_tree_declaration(b->pool, ast_spec, no_idl, b->semicolon);
    return build_decl_ast(b, ast_decl);
}
BuildStatement *build_union_declaration(Builder *b, const char *name)
{
    GnuCAnyStructOrUnion *ast_kw = (GnuCAnyStructOrUnion *)b->kw_union;
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCAtomIdentifier *id = build_id(b, name);
    GnuCTreeTagForwardStructOrUnionSpecifier *ast_union = gnu_c_create_tree_tag_forward_struct_or_union_specifier(b->pool, ast_kw, no_attrs, id);
    GnuCOptInitDeclaratorList *no_idl = (GnuCOptInitDeclaratorList *)b->nothing;
    GnuCAnyDeclarationSpecifiers *ast_spec = (GnuCAnyDeclarationSpecifiers *)ast_union;
    GnuCTreeDeclaration *ast_decl = gnu_c_create_tree_declaration(b->pool, ast_spec, no_idl, b->semicolon);
    return build_decl_ast(b, ast_decl);
}
BuildStatement *build_enum_declaration(Builder *b, const char *name)
{
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCAtomIdentifier *id = build_id(b, name);
    GnuCTreeTagForwardEnumSpecifier *ast_enum = gnu_c_create_tree_tag_forward_enum_specifier(b->pool, b->kw_enum, no_attrs, id);
    GnuCOptInitDeclaratorList *no_idl = (GnuCOptInitDeclaratorList *)b->nothing;
    GnuCAnyDeclarationSpecifiers *ast_spec = (GnuCAnyDeclarationSpecifiers *)ast_enum;
    GnuCTreeDeclaration *ast_decl = gnu_c_create_tree_declaration(b->pool, ast_spec, no_idl, b->semicolon);
    return build_decl_ast(b, ast_decl);
}

static GnuCOptStructDeclarationList *build_members(Builder *b, size_t nmembs, BuildMemberDeclaration **decls)
{
    GnuCOptStructDeclarationList *all;
    size_t i;
    assert (nmembs);

    all = (GnuCOptStructDeclarationList *)b->nothing;
    for (i = 0; i < nmembs; ++i)
    {
        GnuCAnyStructDeclaration *one;
        GnuCTreeStructDeclarationList *many;
        one = decls[i]->ast_struct_decl;
        many = gnu_c_create_tree_struct_declaration_list(b->pool, all, one);
        all = (GnuCOptStructDeclarationList *)many;
    }
    return all;
}
static BuildStatement *build_struct_or_union_definition(Builder *b, GnuCAnyStructOrUnion *sou, const char *name, size_t nmembs, BuildMemberDeclaration **decls)
{
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCOptIdentifier *id = (GnuCOptIdentifier *)build_id(b, name);
    GnuCOptStructDeclarationList *members = build_members(b, nmembs, decls);
    GnuCTreeTagFullStructOrUnionSpecifier *ast_struct = gnu_c_create_tree_tag_full_struct_or_union_specifier(b->pool, sou, no_attrs, id, b->lbrace, members, b->rbrace);
    GnuCAnyDeclarationSpecifiers *ast_spec = (GnuCAnyDeclarationSpecifiers *)ast_struct;
    GnuCOptInitDeclaratorList *no_idl = (GnuCOptInitDeclaratorList *)b->nothing;
    GnuCTreeDeclaration *ast_decl = gnu_c_create_tree_declaration(b->pool, ast_spec, no_idl, b->semicolon);
    return build_decl_ast(b, ast_decl);
}
BuildStatement *build_struct_definition(Builder *b, const char *name, size_t nmembs, BuildMemberDeclaration **decls)
{
    GnuCAnyStructOrUnion *sou = (GnuCAnyStructOrUnion *)b->kw_struct;
    return build_struct_or_union_definition(b, sou, name, nmembs, decls);
}
BuildStatement *build_union_definition(Builder *b, const char *name, size_t nmembs, BuildMemberDeclaration **decls)
{
    GnuCAnyStructOrUnion *sou = (GnuCAnyStructOrUnion *)b->kw_union;
    return build_struct_or_union_definition(b, sou, name, nmembs, decls);
}
static GnuCAnyEnumeratorList *build_variants(Builder *b, size_t nvars, BuildEnumerator **vars)
{
    GnuCAnyEnumerator *one;
    GnuCTreeTagMultiEnumeratorList *many;
    GnuCAnyEnumeratorList *all;
    size_t i;
    assert (nvars);

    one = vars[0]->ast_enum;
    all = (GnuCAnyEnumeratorList *)one;
    for (i = 1; i < nvars; ++i)
    {
        one = vars[i]->ast_enum;
        many = gnu_c_create_tree_tag_multi_enumerator_list(b->pool, all, b->comma, one);
        all = (GnuCAnyEnumeratorList *)many;
    }
    return all;
}
BuildStatement *build_enum_definition(Builder *b, const char *name, size_t nvars, BuildEnumerator **vars)
{
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCOptComma *no_comma = (GnuCOptComma *)b->nothing;
    GnuCOptIdentifier *id = (GnuCOptIdentifier *)build_id(b, name);
    GnuCAnyEnumeratorList *variants = build_variants(b, nvars, vars);
    GnuCTreeTagFullEnumSpecifier *ast_enum = gnu_c_create_tree_tag_full_enum_specifier(b->pool, b->kw_enum, no_attrs, id, b->lbrace, variants, no_comma, b->rbrace);
    GnuCAnyDeclarationSpecifiers *ast_spec = (GnuCAnyDeclarationSpecifiers *)ast_enum;
    GnuCOptInitDeclaratorList *no_idl = (GnuCOptInitDeclaratorList *)b->nothing;
    GnuCTreeDeclaration *ast_decl = gnu_c_create_tree_declaration(b->pool, ast_spec, no_idl, b->semicolon);
    return build_decl_ast(b, ast_decl);
}
BuildStatement *build_function_definition(Builder *b, size_t nscs, BuildStorageClass **scs, const char *name, BuildType *fnty, BuildStatement *body)
{
    BuildTypePairDeclarator pair = build_type_to_decl(b, nscs, scs, fnty, name);
    GnuCOptDeclarationList *no_old_decls = (GnuCOptDeclarationList *)b->nothing;
    GnuCTreeCompoundStatement *stmt = build_stmt_to_compound(b, body);
    GnuCTreeNestedFunctionDefinition *ast_fun = gnu_c_create_tree_nested_function_definition(b->pool, pair.specs, pair.decl, no_old_decls, stmt);
    /* LIES! */
    return build_decl_ast(b, (GnuCTreeDeclaration *)ast_fun);
}

BuildStorageClass *build_storage_class_typedef(Builder *b)
{
    GnuCAnyHeadDeclarationSpecifier *ast_sc = (GnuCAnyHeadDeclarationSpecifier *)b->kw_typedef;
    return build_storage_class_ast(b, ast_sc);
}
BuildStorageClass *build_storage_class_extern(Builder *b)
{
    GnuCAnyHeadDeclarationSpecifier *ast_sc = (GnuCAnyHeadDeclarationSpecifier *)b->kw_extern;
    return build_storage_class_ast(b, ast_sc);
}
BuildStorageClass *build_storage_class_static(Builder *b)
{
    GnuCAnyHeadDeclarationSpecifier *ast_sc = (GnuCAnyHeadDeclarationSpecifier *)b->kw_static;
    return build_storage_class_ast(b, ast_sc);
}
BuildStorageClass *build_storage_class_register(Builder *b)
{
    GnuCAnyHeadDeclarationSpecifier *ast_sc = (GnuCAnyHeadDeclarationSpecifier *)b->kw_register;
    return build_storage_class_ast(b, ast_sc);
}

BuildStatement *build_declaration_noinit(Builder *b, size_t nscs, BuildStorageClass **scs, BuildType *type, const char *name)
{
    BuildTypePairDeclarator pair = build_type_to_decl(b, nscs, scs, type, name);
    GnuCAnyInitDeclarator *idecl = (GnuCAnyInitDeclarator *)pair.decl;
    GnuCOptInitDeclaratorList *idl = (GnuCOptInitDeclaratorList *)idecl;
    GnuCTreeDeclaration *ast_decl = gnu_c_create_tree_declaration(b->pool, pair.specs, idl, b->semicolon);
    return build_decl_ast(b, ast_decl);
}
BuildStatement *build_declaration_init(Builder *b, size_t nscs, BuildStorageClass **scs, BuildType *type, const char *name, BuildInitializer *init)
{
    BuildTypePairDeclarator pair = build_type_to_decl(b, nscs, scs, type, name);
    GnuCOptSimpleAsmExpr *no_asm = (GnuCOptSimpleAsmExpr *)b->nothing;
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCAnyInitializer *ini = build_init_to_init(b, init);
    GnuCTreeTagInitializedDeclarator *tid = gnu_c_create_tree_tag_initialized_declarator(b->pool, pair.decl, no_asm, no_attrs, b->assign, ini);
    GnuCAnyInitDeclarator *idecl = (GnuCAnyInitDeclarator *)tid;
    GnuCOptInitDeclaratorList *idl = (GnuCOptInitDeclaratorList *)idecl;
    GnuCTreeDeclaration *ast_decl = gnu_c_create_tree_declaration(b->pool, pair.specs, idl, b->semicolon);
    return build_decl_ast(b, ast_decl);
}
BuildParamDeclaration *build_param_declaration(Builder *b, size_t nscs, BuildStorageClass **scs, BuildType *type, const char *name)
{
    BuildTypePairDeclarator pair = build_type_to_decl(b, nscs, scs, type, name);
    GnuCTreeTagConcreteParameterDeclaration *rv = gnu_c_create_tree_tag_concrete_parameter_declaration(b->pool, pair.specs, pair.decl);
    GnuCAnyParameterDeclaration *ast = (GnuCAnyParameterDeclaration *)rv;
    return build_param_ast(b, ast);
}
BuildParamDeclaration *build_param_declaration_anon(Builder *b, size_t nscs, BuildStorageClass **scs, BuildType *type)
{
    BuildTypePairAbstractDeclarator pair = build_type_to_decl_abstract(b, nscs, scs, type);
    GnuCTreeTagAbstractParameterDeclaration *rv = gnu_c_create_tree_tag_abstract_parameter_declaration(b->pool, pair.specs, pair.decl);
    GnuCAnyParameterDeclaration *ast = (GnuCAnyParameterDeclaration *)rv;
    return build_param_ast(b, ast);
}
BuildMemberDeclaration *build_member_declaration(Builder *b, BuildType *type, const char *name)
{
    BuildTypePairStructDeclarator pair = build_type_to_decl_struct(b, type, name);
    GnuCOptStructDeclaratorList *sdl = (GnuCOptStructDeclaratorList *)pair.decl;
    GnuCTreeTagMemberDeclaration *sd = gnu_c_create_tree_tag_member_declaration(b->pool, pair.specs, sdl, b->semicolon);
    return build_member_ast(b, (GnuCAnyStructDeclaration *)sd);
}
BuildMemberDeclaration *build_member_declaration_bitfield(Builder *b, BuildType *type, const char *name, BuildExpression *width)
{
    BuildTypePairStructDeclarator pair = build_type_to_decl_struct(b, type, name);
    GnuCOptDeclarator *odecl = (GnuCOptDeclarator *)pair.decl;
    GnuCAliasConstantExpression *expr = build_expr_to_conditional(b, width);
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCTreeTagBitfieldDeclarator *bdecl = gnu_c_create_tree_tag_bitfield_declarator(b->pool, odecl, b->colon, expr, no_attrs);
    GnuCOptStructDeclaratorList *sdl = (GnuCOptStructDeclaratorList *)bdecl;
    GnuCTreeTagMemberDeclaration *sd = gnu_c_create_tree_tag_member_declaration(b->pool, pair.specs, sdl, b->semicolon);
    return build_member_ast(b, (GnuCAnyStructDeclaration *)sd);
}
BuildMemberDeclaration *build_member_declaration_bitfield_anon(Builder *b, BuildType *type, BuildExpression *width)
{
    BuildTypePairAbstractStructDeclarator pair = build_type_to_decl_struct_abstract(b, type);
    GnuCOptDeclarator *odecl = (GnuCOptDeclarator *)b->nothing;
    GnuCAliasConstantExpression *expr = build_expr_to_conditional(b, width);
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCTreeTagBitfieldDeclarator *bdecl = gnu_c_create_tree_tag_bitfield_declarator(b->pool, odecl, b->colon, expr, no_attrs);
    GnuCOptStructDeclaratorList *sdl = (GnuCOptStructDeclaratorList *)bdecl;
    GnuCTreeTagMemberDeclaration *sd = gnu_c_create_tree_tag_member_declaration(b->pool, pair.specs, sdl, b->semicolon);
    assert (gnu_c_is_nothing((GnuCAst *)pair.decl));
    return build_member_ast(b, (GnuCAnyStructDeclaration *)sd);
}
BuildEnumerator *build_enumerator_noinit(Builder *b, const char *name)
{
    GnuCAtomIdentifier *id = build_id(b, name);
    GnuCAnyEnumerator *e = (GnuCAnyEnumerator *)id;
    return build_enum_ast(b, e);
}
BuildEnumerator *build_enumerator_init(Builder *b, const char *name, BuildExpression *init)
{
    GnuCAliasEnumerationConstant *id = build_id(b, name);
    GnuCAliasConstantExpression *expr = build_expr_to_conditional(b, init);
    GnuCTreeTagInitializedEnumerator *iec = gnu_c_create_tree_tag_initialized_enumerator(b->pool, id, b->assign, expr);
    GnuCAnyEnumerator *e = (GnuCAnyEnumerator *)iec;
    return build_enum_ast(b, e);
}

BuildType *build_type_typedef(Builder *b, const char *name)
{
    GnuCAtomTypedefName *ty = build_typedef_name(b, name);
    GnuCTreeTailDeclarationSpecifiers *ds = (GnuCTreeTailDeclarationSpecifiers *)ty;
    GnuCAnySpecifierQualifierList *sql = (GnuCAnySpecifierQualifierList *)ty;
    return build_type_spec_ast(b, ds, sql);
}
BuildType *build_type_struct(Builder *b, const char *name)
{
    GnuCAnyStructOrUnion *sou = (GnuCAnyStructOrUnion *)b->kw_struct;
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCAtomIdentifier *id = build_id(b, name);
    GnuCTreeTagForwardStructOrUnionSpecifier *ty = gnu_c_create_tree_tag_forward_struct_or_union_specifier(b->pool, sou, no_attrs, id);
    GnuCTreeTailDeclarationSpecifiers *ds = (GnuCTreeTailDeclarationSpecifiers *)ty;
    GnuCAnySpecifierQualifierList *sql = (GnuCAnySpecifierQualifierList *)ty;
    return build_type_spec_ast(b, ds, sql);
}
BuildType *build_type_union(Builder *b, const char *name)
{
    GnuCAnyStructOrUnion *sou = (GnuCAnyStructOrUnion *)b->kw_union;
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCAtomIdentifier *id = build_id(b, name);
    GnuCTreeTagForwardStructOrUnionSpecifier *ty = gnu_c_create_tree_tag_forward_struct_or_union_specifier(b->pool, sou, no_attrs, id);
    GnuCTreeTailDeclarationSpecifiers *ds = (GnuCTreeTailDeclarationSpecifiers *)ty;
    GnuCAnySpecifierQualifierList *sql = (GnuCAnySpecifierQualifierList *)ty;
    return build_type_spec_ast(b, ds, sql);
}
BuildType *build_type_enum(Builder *b, const char *name)
{
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCAtomIdentifier *id = build_id(b, name);
    GnuCTreeTagForwardEnumSpecifier *ty = gnu_c_create_tree_tag_forward_enum_specifier(b->pool, b->kw_enum, no_attrs, id);
    GnuCTreeTailDeclarationSpecifiers *ds = (GnuCTreeTailDeclarationSpecifiers *)ty;
    GnuCAnySpecifierQualifierList *sql = (GnuCAnySpecifierQualifierList *)ty;
    return build_type_spec_ast(b, ds, sql);
}
BuildType *build_type_void(Builder *b)
{
    return build_type_spec_ast(b, b->ty_void_ds, b->ty_void_sql);
}
BuildType *build_type_bool(Builder *b)
{
    return build_type_spec_ast(b, b->ty_bool_ds, b->ty_bool_sql);
}
BuildType *build_type_char(Builder *b)
{
    return build_type_spec_ast(b, b->ty_char_ds, b->ty_char_sql);;
}
BuildType *build_type_signed_char(Builder *b)
{
    return build_type_spec_ast(b, b->ty_signed_char_ds, b->ty_signed_char_sql);;
}
BuildType *build_type_unsigned_char(Builder *b)
{
    return build_type_spec_ast(b, b->ty_unsigned_char_ds, b->ty_unsigned_char_sql);;
}
BuildType *build_type_signed_short(Builder *b)
{
    return build_type_spec_ast(b, b->ty_signed_short_ds, b->ty_signed_short_sql);;
}
BuildType *build_type_unsigned_short(Builder *b)
{
    return build_type_spec_ast(b, b->ty_unsigned_short_ds, b->ty_unsigned_short_sql);;
}
BuildType *build_type_signed_int(Builder *b)
{
    return build_type_spec_ast(b, b->ty_signed_int_ds, b->ty_signed_int_sql);;
}
BuildType *build_type_unsigned_int(Builder *b)
{
    return build_type_spec_ast(b, b->ty_unsigned_int_ds, b->ty_unsigned_int_sql);;
}
BuildType *build_type_signed_long(Builder *b)
{
    return build_type_spec_ast(b, b->ty_signed_long_ds, b->ty_signed_long_sql);;
}
BuildType *build_type_unsigned_long(Builder *b)
{
    return build_type_spec_ast(b, b->ty_unsigned_long_ds, b->ty_unsigned_long_sql);;
}
BuildType *build_type_signed_long_long(Builder *b)
{
    return build_type_spec_ast(b, b->ty_signed_long_long_ds, b->ty_signed_long_long_sql);;
}
BuildType *build_type_unsigned_long_long(Builder *b)
{
    return build_type_spec_ast(b, b->ty_unsigned_long_long_ds, b->ty_unsigned_long_long_sql);;
}
BuildType *build_type_float(Builder *b)
{
    return build_type_spec_ast(b, b->ty_float_ds, b->ty_float_sql);;
}
BuildType *build_type_double(Builder *b)
{
    return build_type_spec_ast(b, b->ty_double_ds, b->ty_double_sql);;
}
BuildType *build_type_long_double(Builder *b)
{
    return build_type_spec_ast(b, b->ty_long_double_ds, b->ty_long_double_sql);;
}
BuildType *build_type_const(Builder *b, BuildType *type)
{
    assert (type->type != BTYTY_FUN);
    if (type->type == BTYTY_ARRAY)
    {
        BuildType *const_element = build_type_const(b, type->array.element);
        return build_type_array_ast(b, const_element, type->array.size);
    }
    else
    {
        BuildType copy = *type;
        copy.flags.is_const = true;
        return build_type_copy_ast(b, copy);
    }
}
BuildType *build_type_volatile(Builder *b, BuildType *type)
{
    assert (type->type != BTYTY_FUN);
    if (type->type == BTYTY_ARRAY)
    {
        BuildType *volatile_element = build_type_volatile(b, type->array.element);
        return build_type_array_ast(b, volatile_element, type->array.size);
    }
    else
    {
        BuildType copy = *type;
        copy.flags.is_volatile = true;
        return build_type_copy_ast(b, copy);
    }
}
BuildType *build_type_pointer(Builder *b, BuildType *type)
{
    return build_type_ptr_ast(b, type);
}
BuildType *build_type_array(Builder *b, BuildType *type, BuildExpression *size)
{
    GnuCOptTypeQualifierList *no_tql = (GnuCOptTypeQualifierList *)b->nothing;
    GnuCAnyConditionalExpression *expr = build_expr_to_conditional(b, size);
    GnuCOptAssignmentExpression *opt_expr = (GnuCOptAssignmentExpression *)expr;
    GnuCTreeTagSimpleArrayDeclarator *array_size = gnu_c_create_tree_tag_simple_array_declarator(b->pool, b->lbracket, no_tql, opt_expr, b->rbracket);
    return build_type_array_ast(b, type, (GnuCAnyArrayDeclarator *)array_size);
}
BuildType *build_type_array_unknown_bound(Builder *b, BuildType *type)
{
    GnuCOptTypeQualifierList *no_tql = (GnuCOptTypeQualifierList *)b->nothing;
    GnuCOptAssignmentExpression *opt_expr = (GnuCOptAssignmentExpression *)b->nothing;
    GnuCTreeTagSimpleArrayDeclarator *array_size = gnu_c_create_tree_tag_simple_array_declarator(b->pool, b->lbracket, no_tql, opt_expr, b->rbracket);
    return build_type_array_ast(b, type, (GnuCAnyArrayDeclarator *)array_size);
}
static GnuCAnyParameterTypeList *build_args(Builder *b, size_t nargs, BuildParamDeclaration **args, bool vararg)
{
    GnuCAnyParameterDeclaration *one;
    GnuCTreeTagMultiParameterList *many;
    GnuCAnyParameterList *all;
    size_t i;
    GnuCAnyParameterTypeList *rv;
    assert (nargs);

    one = args[0]->ast_param_decl;
    all = (GnuCAnyParameterList *)one;
    for (i = 1; i < nargs; ++i)
    {
        one = args[i]->ast_param_decl;
        many = gnu_c_create_tree_tag_multi_parameter_list(b->pool, all, b->comma, one);
        all = (GnuCAnyParameterList *)many;
    }
    if (vararg)
    {
        GnuCTreeTagVariadicParameterList *more = gnu_c_create_tree_tag_variadic_parameter_list(b->pool, all, b->comma, b->ellipsis);
        rv = (GnuCAnyParameterTypeList *)more;
    }
    else
    {
        rv = (GnuCAnyParameterTypeList *)all;
    }
    return rv;
}
BuildType *build_type_function(Builder *b, BuildType *ret, size_t nargs, BuildParamDeclaration **args, bool vararg)
{
    GnuCAnyParameterTypeList *fun_args;
    if (nargs)
    {
        fun_args = build_args(b, nargs, args, vararg);
    }
    else
    {
        BuildParamDeclaration *pd_void = build_param_declaration_anon(b, ZERO_PAIR(), build_type_void(b));
        fun_args = build_args(b, 1, &pd_void, vararg);
    }
    return build_type_fun_ast(b, ret, fun_args);
}

BuildStatement *build_stmt_none(Builder *b)
{
    GnuCOptExprList *no_expr = (GnuCOptExprList *)b->nothing;
    GnuCTreeExpressionStatement *stmt = gnu_c_create_tree_expression_statement(b->pool, no_expr, b->semicolon);
    GnuCOptLabels *no_labels = (GnuCOptLabels *)b->nothing;
    GnuCAnyBlockItem *unlabeled = (GnuCAnyBlockItem *)stmt;
    return build_stmt_ast(b, no_labels, unlabeled);
}
BuildStatement *build_stmt_expr(Builder *b, BuildExpression *expr)
{
    GnuCAnyExprList *comma_expr = build_expr_to_comma(b, expr);
    GnuCOptExprList *some_expr = (GnuCOptExprList *)comma_expr;
    GnuCTreeExpressionStatement *stmt = gnu_c_create_tree_expression_statement(b->pool, some_expr, b->semicolon);
    GnuCOptLabels *no_labels = (GnuCOptLabels *)b->nothing;
    GnuCAnyBlockItem *unlabeled = (GnuCAnyBlockItem *)stmt;
    return build_stmt_ast(b, no_labels, unlabeled);
}
BuildStatement *build_stmt_label(Builder *b, const char *label, BuildStatement *target)
{
    GnuCAtomIdentifier *id = build_id(b, label);
    GnuCOptAttributes *no_attrs = (GnuCOptAttributes *)b->nothing;
    GnuCAnyLabel *new_label = (GnuCAnyLabel *)gnu_c_create_tree_tag_named_label(b->pool, id, b->colon, no_attrs);
    GnuCTreeLabels *labels = gnu_c_create_tree_labels(b->pool, target->ast_labels, new_label);
    return build_stmt_ast(b, (GnuCOptLabels *)labels, target->ast_unlabeled);
}
BuildStatement *build_stmt_case(Builder *b, BuildExpression *expr, BuildStatement *target)
{
    GnuCAliasConstantExpression *cond = build_expr_to_conditional(b, expr);
    GnuCAnyLabel *new_label = (GnuCAnyLabel *)gnu_c_create_tree_tag_case_label(b->pool, b->kw_case, cond, b->colon);
    GnuCTreeLabels *labels = gnu_c_create_tree_labels(b->pool, target->ast_labels, new_label);
    return build_stmt_ast(b, (GnuCOptLabels *)labels, target->ast_unlabeled);
}
BuildStatement *build_stmt_default(Builder *b, BuildStatement *target)
{
    GnuCAnyLabel *new_label = (GnuCAnyLabel *)gnu_c_create_tree_tag_default_label(b->pool, b->kw_default, b->colon);
    GnuCTreeLabels *labels = gnu_c_create_tree_labels(b->pool, target->ast_labels, new_label);
    return build_stmt_ast(b, (GnuCOptLabels *)labels, target->ast_unlabeled);
}
static GnuCAnyBlockItem *finish_stmt(Builder *b, BuildStatement *stmt)
{
    if (stmt->ast_labels == NULL)
        return (GnuCAnyBlockItem *)stmt->ast_unlabeled;
    /* Note: if stmt->ast_labels is b->nothing, stmt->ast_unlabeled will be returned directly */
    return (GnuCAnyBlockItem *)gnu_c_create_tree_statement(b->pool, stmt->ast_labels, (GnuCAnyUnlabeledStatement *)stmt->ast_unlabeled);
}
static GnuCOptBlockItemList *build_stmts(Builder *b, size_t nstmts, BuildStatement **stmts)
{
    GnuCOptBlockItemList *all;
    size_t i;

    all = (GnuCOptBlockItemList *)b->nothing;
    for (i = 0; i < nstmts; ++i)
    {
        GnuCAnyBlockItem *one;
        GnuCTreeBlockItemList *many;
        one = finish_stmt(b, stmts[i]);
        many = gnu_c_create_tree_block_item_list(b->pool, all, one);
        all = (GnuCOptBlockItemList *)many;
    }
    return all;
}
BuildStatement *build_stmt_compound(Builder *b, size_t nstmts, BuildStatement **stmts)
{
    GnuCOptLabelDeclarations *no_label_declarations = (GnuCOptLabelDeclarations *)b->nothing;
    GnuCOptBlockItemList *ast_stmts = build_stmts(b, nstmts, stmts);
    GnuCTreeCompoundStatement *stmt = gnu_c_create_tree_compound_statement(b->pool, b->lbrace, no_label_declarations, ast_stmts, b->rbrace);
    GnuCOptLabels *no_labels = (GnuCOptLabels *)b->nothing;
    GnuCAnyBlockItem *unlabeled = (GnuCAnyBlockItem *)stmt;
    return build_stmt_ast(b, no_labels, unlabeled);
}
BuildStatement *build_stmt_if(Builder *b, BuildExpression *cond, BuildStatement *if_true)
{
    GnuCAnyExprList *expr = build_expr_to_comma(b, cond);
    GnuCTreeCompoundStatement *comp = build_stmt_to_compound(b, if_true);
    GnuCTreeStatement *body = (GnuCTreeStatement *)comp;
    GnuCTreeIfStatement *stmt = gnu_c_create_tree_if_statement(b->pool, b->kw_if, gnu_c_create_tree_condition(b->pool, b->lparen, expr, b->rparen), body);
    GnuCOptLabels *no_labels = (GnuCOptLabels *)b->nothing;
    GnuCAnyBlockItem *ast = (GnuCAnyBlockItem *)stmt;
    return build_stmt_ast(b, no_labels, ast);
}
BuildStatement *build_stmt_if_else(Builder *b, BuildExpression *cond, BuildStatement *if_true, BuildStatement *if_false)
{
    GnuCAnyExprList *expr = build_expr_to_comma(b, cond);
    GnuCTreeCompoundStatement *if_comp = build_stmt_to_compound(b, if_true);
    GnuCTreeStatementExceptIf *if_body = (GnuCTreeStatementExceptIf *)if_comp;
    GnuCTreeStatement *else_body = build_stmt_to_else_body(b, if_false);
    GnuCTreeIfElseStatement *stmt = gnu_c_create_tree_if_else_statement(b->pool, b->kw_if, gnu_c_create_tree_condition(b->pool, b->lparen, expr, b->rparen), if_body, b->kw_else, else_body);
    GnuCOptLabels *no_labels = (GnuCOptLabels *)b->nothing;
    GnuCAnyBlockItem *ast = (GnuCAnyBlockItem *)stmt;
    return build_stmt_ast(b, no_labels, ast);
}
BuildStatement *build_stmt_while(Builder *b, BuildExpression *cond, BuildStatement *body)
{
    GnuCAnyExprList *expr = build_expr_to_comma(b, cond);
    GnuCTreeCompoundStatement *comp = build_stmt_to_compound(b, body);
    GnuCTreeStatement *loop_body = (GnuCTreeStatement *)comp;
    GnuCTreeWhileStatement *stmt = gnu_c_create_tree_while_statement(b->pool, b->kw_while, gnu_c_create_tree_condition(b->pool, b->lparen, expr, b->rparen), loop_body);
    GnuCOptLabels *no_labels = (GnuCOptLabels *)b->nothing;
    GnuCAnyBlockItem *ast = (GnuCAnyBlockItem *)stmt;
    return build_stmt_ast(b, no_labels, ast);
}
BuildStatement *build_stmt_do_while(Builder *b, BuildStatement *body, BuildExpression *cond)
{
    GnuCTreeCompoundStatement *comp = build_stmt_to_compound(b, body);
    GnuCTreeStatement *loop_body = (GnuCTreeStatement *)comp;
    GnuCAnyExprList *expr = build_expr_to_comma(b, cond);
    GnuCTreeDoStatement *stmt = gnu_c_create_tree_do_statement(b->pool, b->kw_do, loop_body, b->kw_while, gnu_c_create_tree_condition(b->pool, b->lparen, expr, b->rparen), b->semicolon);
    GnuCOptLabels *no_labels = (GnuCOptLabels *)b->nothing;
    GnuCAnyBlockItem *ast = (GnuCAnyBlockItem *)stmt;
    return build_stmt_ast(b, no_labels, ast);
}
static GnuCOptExprList *build_expr_opt_expr(Builder *b, BuildExpression *expr)
{
    if (expr == build_expr_none(b))
    {
        return (GnuCOptExprList *)b->nothing;
    }
    else
    {
        GnuCAnyExprList *rv = build_expr_to_comma(b, expr);
        return (GnuCOptExprList *)rv;
    }
}
BuildStatement *build_stmt_for(Builder *b, BuildExpression *init, BuildExpression *cond, BuildExpression *post, BuildStatement *body)
{
    GnuCOptExprList *opt_init = build_expr_opt_expr(b, init);
    GnuCOptExprList *opt_cond = build_expr_opt_expr(b, cond);
    GnuCOptExprList *opt_post = build_expr_opt_expr(b, post);
    GnuCTreeCompoundStatement *comp = build_stmt_to_compound(b, body);
    GnuCTreeStatement *loop_body = (GnuCTreeStatement *)comp;
    GnuCTreeTagForExprStatement *stmt = gnu_c_create_tree_tag_for_expr_statement(b->pool, b->kw_for, b->lparen, opt_init, b->semicolon, opt_cond, b->semicolon, opt_post, b->rparen, loop_body);
    GnuCOptLabels *no_labels = (GnuCOptLabels *)b->nothing;
    GnuCAnyBlockItem *ast = (GnuCAnyBlockItem *)stmt;
    return build_stmt_ast(b, no_labels, ast);
}
BuildStatement *build_stmt_goto(Builder *b, const char *label)
{
    GnuCAtomIdentifier *id = build_id(b, label);
    GnuCTreeTagGotoStatement *stmt = gnu_c_create_tree_tag_goto_statement(b->pool, b->kw_goto, id, b->semicolon);
    GnuCOptLabels *no_labels = (GnuCOptLabels *)b->nothing;
    GnuCAnyBlockItem *ast = (GnuCAnyBlockItem *)stmt;
    return build_stmt_ast(b, no_labels, ast);
}
BuildStatement *build_stmt_continue(Builder *b)
{
    GnuCTreeTagContinueStatement *stmt = gnu_c_create_tree_tag_continue_statement(b->pool, b->kw_continue, b->semicolon);
    GnuCOptLabels *no_labels = (GnuCOptLabels *)b->nothing;
    GnuCAnyBlockItem *ast = (GnuCAnyBlockItem *)stmt;
    return build_stmt_ast(b, no_labels, ast);
}
BuildStatement *build_stmt_break(Builder *b)
{
    GnuCTreeTagBreakStatement *stmt = gnu_c_create_tree_tag_break_statement(b->pool, b->kw_break, b->semicolon);
    GnuCOptLabels *no_labels = (GnuCOptLabels *)b->nothing;
    GnuCAnyBlockItem *ast = (GnuCAnyBlockItem *)stmt;
    return build_stmt_ast(b, no_labels, ast);
}
BuildStatement *build_stmt_return(Builder *b, BuildExpression *expr)
{
    GnuCOptExprList *opt_expr = build_expr_opt_expr(b, expr);
    GnuCTreeTagReturnStatement *stmt = gnu_c_create_tree_tag_return_statement(b->pool, b->kw_return, opt_expr, b->semicolon);
    GnuCOptLabels *no_labels = (GnuCOptLabels *)b->nothing;
    GnuCAnyBlockItem *ast = (GnuCAnyBlockItem *)stmt;
    return build_stmt_ast(b, no_labels, ast);
}

BuildInitializer *build_initializer_expr(Builder *b, BuildExpression *expr)
{
    GnuCAnyAssignmentExpression *assign = build_expr_to_assignment(b, expr);
    GnuCAnyInitializer *ast = (GnuCAnyInitializer *)assign;
    return build_init_ast(b, ast);
}
static GnuCOptInitializerList *build_init_list(Builder *b, size_t nelems, BuildInitializer **elems)
{
    GnuCTreeDesignatedInitializer *one;
    GnuCTreeTagMultiInitializerList *many;
    GnuCAnyInitializerList *all;
    size_t i;
    if (!nelems)
    {
        return (GnuCOptInitializerList *)b->nothing;
    }

    one = (GnuCTreeDesignatedInitializer *) elems[0]->ast_init;
    all = (GnuCAnyInitializerList *)one;
    for (i = 1; i < nelems; ++i)
    {
        one = (GnuCTreeDesignatedInitializer *)elems[i]->ast_init;
        many = gnu_c_create_tree_tag_multi_initializer_list(b->pool, all, b->comma, one);
        all = (GnuCAnyInitializerList *)many;
    }
    return (GnuCOptInitializerList *)all;
}
BuildInitializer *build_initializer_braced(Builder *b, size_t nelems, BuildInitializer **elems)
{
    GnuCOptInitializerList *list = build_init_list(b, nelems, elems);
    GnuCTreeTagBracedInitializer *init = gnu_c_create_tree_tag_braced_initializer(b->pool, b->lbrace, list, b->rbrace);
    GnuCAnyInitializer *ast = (GnuCAnyInitializer *)init;
    return build_init_ast(b, ast);
}

static GnuCTreeTypeName *build_type_to_typename(Builder *b, BuildType *type)
{
    BuildTypePairAbstractStructDeclarator pair = build_type_to_decl_struct_abstract(b, type);
    return gnu_c_create_tree_type_name(b->pool, pair.specs, pair.decl);
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
    if (gnu_c_is_tree_tag_paren_expression((GnuCAst *)expr->ast_expr))
        return expr;
    return build_expr_from_comma(b, (GnuCAnyExprList *)gnu_c_create_tree_tag_paren_expression(b->pool, b->lparen, build_expr_to_comma(b, expr), b->rparen));
}
BuildExpression *build_expr_id(Builder *b, const char *id)
{
    GnuCAtomIdentifier *i = build_id(b, id);
    return build_expr_from_primary(b, (GnuCAnyPrimaryExpression *)i);
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

static void *transform_single_quote(Pool *pool, const void *str, size_t len, void *context)
{
    void *rv = really_do_quote((const char *)str, len, '\'');
    (void)context;
    pool_free(pool, free, rv);
    return rv;
}
static void *transform_double_quote(Pool *pool, const void *str, size_t len, void *context)
{
    void *rv = really_do_quote((const char *)str, len, '"');
    (void)context;
    pool_free(pool, free, rv);
    return rv;
}

static const char *pool_single_quote(Pool *pool, const char *str, size_t len)
{
    return (const char *)pool_intern_map(pool, transform_single_quote, str, len, NULL);
}

static const char *pool_double_quote(Pool *pool, const char *str, size_t len)
{
    return (const char *)pool_intern_map(pool, transform_double_quote, str, len, NULL);
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
    GnuCAtomCharacterConstant *p = gnu_c_create_atom_character_constant(b->pool, q);
    return build_expr_from_primary(b, (GnuCAnyPrimaryExpression *)p);
}
BuildExpression *build_expr_string(Builder *b, const char *s)
{
    return build_expr_string_slice(b, s, strlen(s));
}
BuildExpression *build_expr_string_slice(Builder *b, const char *s, size_t l)
{
    const char *q = pool_double_quote(b->pool, s, l);
    GnuCAtomStringLiteral *p = gnu_c_create_atom_string_literal(b->pool, q);
    return build_expr_from_primary(b, (GnuCAnyPrimaryExpression *)p);
}
BuildExpression *build_expr_int(Builder *b, uintmax_t i)
{
    char buf[20 + 1];
    int rv;
    GnuCAtomDecimalConstant *p;
    if (i == 0)
        return build_expr_int_oct(b, i, 0);
    rv = sprintf(buf, "%ju", i);
    assert (rv >= 0 && (size_t)rv < sizeof(buf));
    p = gnu_c_create_atom_decimal_constant(b->pool, buf);
    return build_expr_from_primary(b, (GnuCAnyPrimaryExpression *)p);
}
BuildExpression *build_expr_int_oct(Builder *b, uintmax_t i, unsigned min_len)
{
    char buf[1 + 22 + 1];
    int rv;
    GnuCAtomOctalConstant *p;
    if (min_len > 22)
        min_len = 22;
    rv = sprintf(buf, "%0#*jo", min_len + 1, i);
    assert (rv >= 0 && (size_t)rv < sizeof(buf));
    p = gnu_c_create_atom_octal_constant(b->pool, buf);
    return build_expr_from_primary(b, (GnuCAnyPrimaryExpression *)p);
}
BuildExpression *build_expr_int_hex(Builder *b, uintmax_t i, unsigned min_len)
{
    char buf[2 + 16 + 1];
    int rv;
    GnuCAtomHexadecimalConstant *p;
    if (min_len > 16)
        min_len = 16;
    rv = sprintf(buf, "%0#*jx", min_len + 2, i);
    assert (rv >= 0 && (size_t)rv < sizeof(buf));
    p = gnu_c_create_atom_hexadecimal_constant(b->pool, buf);
    return build_expr_from_primary(b, (GnuCAnyPrimaryExpression *)p);
}
BuildExpression *build_expr_float(Builder *b, double f)
{
    char buf[1 + 1 + 20 + 1 + 1 + 4 + 1];
    int rv;
    GnuCAtomDecimalFloatingConstant *p;
    double f2;
    rv = sprintf(buf, "%.21e", f);
    assert (rv >= 0 && (size_t)rv < sizeof(buf));
    rv = sscanf(buf, "%lf", &f2);
    assert (rv == 1 && f == f2);
    p = gnu_c_create_atom_decimal_floating_constant(b->pool, buf);
    return build_expr_from_primary(b, (GnuCAnyPrimaryExpression *)p);
}

BuildExpression *build_expr_index(Builder *b, BuildExpression *arr, BuildExpression *idx)
{
    GnuCAnyPostfixExpression *a = build_expr_to_postfix(b, arr);
    GnuCAnyExprList *i = build_expr_to_comma(b, idx);
    GnuCTreeTagIndexExpression *x = gnu_c_create_tree_tag_index_expression(b->pool, a, b->lbracket, i, b->rbracket);
    return build_expr_from_postfix(b, (GnuCAnyPostfixExpression *)x);
}
BuildExpression *build_expr_call(Builder *b, BuildExpression *fun, size_t nargs, BuildExpression **args)
{
    GnuCAnyPostfixExpression *f = build_expr_to_postfix(b, fun);
    GnuCOptExprList *a;
    GnuCTreeTagCallExpression *x;
    if (nargs == 0)
    {
        a = (GnuCOptExprList *)b->nothing;
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
        a = (GnuCOptExprList *)build_expr_to_comma(b, tmp);
    }
    x = gnu_c_create_tree_tag_call_expression(b->pool, f, b->lparen, a, b->rparen);
    return build_expr_from_postfix(b, (GnuCAnyPostfixExpression *)x);
}
BuildExpression *build_expr_direct_member(Builder *b, BuildExpression *obj, const char *field)
{
    GnuCAnyPostfixExpression *o = build_expr_to_postfix(b, obj);
    GnuCAtomIdentifier *f = build_id(b, field);
    GnuCTreeTagDirectMemberExpression *x = gnu_c_create_tree_tag_direct_member_expression(b->pool, o, b->dot, f);
    return build_expr_from_postfix(b, (GnuCAnyPostfixExpression *)x);
}
BuildExpression *build_expr_indirect_member(Builder *b, BuildExpression *ptr, const char *field)
{
    GnuCAnyPostfixExpression *p = build_expr_to_postfix(b, ptr);
    GnuCAtomIdentifier *f = build_id(b, field);
    GnuCTreeTagIndirectMemberExpression *x = gnu_c_create_tree_tag_indirect_member_expression(b->pool, p, b->arrow, f);
    return build_expr_from_postfix(b, (GnuCAnyPostfixExpression *)x);
}
BuildExpression *build_expr_post_inc(Builder *b, BuildExpression *lhs)
{
    GnuCAnyPostfixExpression *l = build_expr_to_postfix(b, lhs);
    GnuCTreeTagPostIncrExpression *x = gnu_c_create_tree_tag_post_incr_expression(b->pool, l, b->incr);
    return build_expr_from_postfix(b, (GnuCAnyPostfixExpression *)x);
}
BuildExpression *build_expr_post_dec(Builder *b, BuildExpression *lhs)
{
    GnuCAnyPostfixExpression *l = build_expr_to_postfix(b, lhs);
    GnuCTreeTagPostDecrExpression *x = gnu_c_create_tree_tag_post_decr_expression(b->pool, l, b->decr);
    return build_expr_from_postfix(b, (GnuCAnyPostfixExpression *)x);
}
BuildExpression *build_expr_pre_inc(Builder *b, BuildExpression *rhs)
{
    GnuCAnyMaybeCastExpression *r = build_expr_to_cast(b, rhs);
    GnuCTreeTagPreIncrExpression *x = gnu_c_create_tree_tag_pre_incr_expression(b->pool, b->incr, r);
    return build_expr_from_unary(b, (GnuCAnyNoCastExpression *)x);
}
BuildExpression *build_expr_pre_dec(Builder *b, BuildExpression *rhs)
{
    GnuCAnyMaybeCastExpression *r = build_expr_to_cast(b, rhs);
    GnuCTreeTagPreDecrExpression *x = gnu_c_create_tree_tag_pre_decr_expression(b->pool, b->decr, r);
    return build_expr_from_unary(b, (GnuCAnyNoCastExpression *)x);
}
BuildExpression *build_expr_addressof(Builder *b, BuildExpression *obj)
{
    GnuCAnyMaybeCastExpression *o = build_expr_to_cast(b, obj);
    GnuCTreeTagAddressofExpression *x = gnu_c_create_tree_tag_addressof_expression(b->pool, b->ampersand, o);
    return build_expr_from_unary(b, (GnuCAnyNoCastExpression *)x);
}
BuildExpression *build_expr_deref(Builder *b, BuildExpression *ptr)
{
    GnuCAnyMaybeCastExpression *p = build_expr_to_cast(b, ptr);
    GnuCTreeTagDerefExpression *x = gnu_c_create_tree_tag_deref_expression(b->pool, b->star, p);
    return build_expr_from_unary(b, (GnuCAnyNoCastExpression *)x);
}
BuildExpression *build_expr_unary_plus(Builder *b, BuildExpression *rhs)
{
    GnuCAnyMaybeCastExpression *r = build_expr_to_cast(b, rhs);
    GnuCTreeTagUnaryPlusExpression *x = gnu_c_create_tree_tag_unary_plus_expression(b->pool, b->plus, r);
    return build_expr_from_unary(b, (GnuCAnyNoCastExpression *)x);
}
BuildExpression *build_expr_unary_minus(Builder *b, BuildExpression *rhs)
{
    GnuCAnyMaybeCastExpression *r = build_expr_to_cast(b, rhs);
    GnuCTreeTagUnaryMinusExpression *x = gnu_c_create_tree_tag_unary_minus_expression(b->pool, b->minus, r);
    return build_expr_from_unary(b, (GnuCAnyNoCastExpression *)x);
}
BuildExpression *build_expr_bitwise_not(Builder *b, BuildExpression *rhs)
{
    GnuCAnyMaybeCastExpression *r = build_expr_to_cast(b, rhs);
    GnuCTreeTagBitNotExpression *x = gnu_c_create_tree_tag_bit_not_expression(b->pool, b->tilde, r);
    return build_expr_from_unary(b, (GnuCAnyNoCastExpression *)x);
}
BuildExpression *build_expr_logical_not(Builder *b, BuildExpression *rhs)
{
    GnuCAnyMaybeCastExpression *r = build_expr_to_cast(b, rhs);
    GnuCTreeTagLogNotExpression *x = gnu_c_create_tree_tag_log_not_expression(b->pool, b->bang, r);
    return build_expr_from_unary(b, (GnuCAnyNoCastExpression *)x);
}
BuildExpression *build_expr_sizeof_expr(Builder *b, BuildExpression *obj)
{
    GnuCAnyNoCastExpression *o = build_expr_to_unary(b, build_expr_parenthesize(b, obj));
    GnuCTreeTagSizeofExpressionExpression *x = gnu_c_create_tree_tag_sizeof_expression_expression(b->pool, b->kw_sizeof, o);
    return build_expr_from_unary(b, (GnuCAnyNoCastExpression *)x);
}
BuildExpression *build_expr_sizeof_type(Builder *b, BuildType *type)
{
    GnuCTreeTypeName *t = build_type_to_typename(b, type);
    GnuCTreeTagSizeofTypeExpression *x = gnu_c_create_tree_tag_sizeof_type_expression(b->pool, b->kw_sizeof, b->lparen, t, b->rparen);
    return build_expr_from_unary(b, (GnuCAnyNoCastExpression *)x);
}
BuildExpression *build_expr_cast(Builder *b, BuildType *type, BuildExpression *rhs)
{
    GnuCTreeTypeName *t = build_type_to_typename(b, type);
    GnuCAnyMaybeCastExpression *r = build_expr_to_cast(b, rhs);
    GnuCTreeTagActualCastExpression *x = gnu_c_create_tree_tag_actual_cast_expression(b->pool, b->lparen, t, b->rparen, r);
    return build_expr_from_cast(b, (GnuCAnyMaybeCastExpression *)x);
}
BuildExpression *build_expr_times(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyMultiplicativeExpression *l = build_expr_to_mul(b, lhs);
    GnuCAnyMaybeCastExpression *r = build_expr_to_cast(b, rhs);
    GnuCTreeTagTimesExpression *x = gnu_c_create_tree_tag_times_expression(b->pool, l, b->star, r);
    return build_expr_from_mul(b, (GnuCAnyMultiplicativeExpression *)x);
}
BuildExpression *build_expr_divide(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyMultiplicativeExpression *l = build_expr_to_mul(b, lhs);
    GnuCAnyMaybeCastExpression *r = build_expr_to_cast(b, rhs);
    GnuCTreeTagDivideExpression *x = gnu_c_create_tree_tag_divide_expression(b->pool, l, b->slash, r);
    return build_expr_from_mul(b, (GnuCAnyMultiplicativeExpression *)x);
}
BuildExpression *build_expr_modulus(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyMultiplicativeExpression *l = build_expr_to_mul(b, lhs);
    GnuCAnyMaybeCastExpression *r = build_expr_to_cast(b, rhs);
    GnuCTreeTagModuloExpression *x = gnu_c_create_tree_tag_modulo_expression(b->pool, l, b->percent, r);
    return build_expr_from_mul(b, (GnuCAnyMultiplicativeExpression *)x);
}
BuildExpression *build_expr_plus(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyAdditiveExpression *l = build_expr_to_add(b, lhs);
    GnuCAnyMultiplicativeExpression *r = build_expr_to_mul(b, rhs);
    GnuCTreeTagPlusExpression *x = gnu_c_create_tree_tag_plus_expression(b->pool, l, b->plus, r);
    return build_expr_from_add(b, (GnuCAnyAdditiveExpression *)x);
}
BuildExpression *build_expr_minus(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyAdditiveExpression *l = build_expr_to_add(b, lhs);
    GnuCAnyMultiplicativeExpression *r = build_expr_to_mul(b, rhs);
    GnuCTreeTagMinusExpression *x = gnu_c_create_tree_tag_minus_expression(b->pool, l, b->minus, r);
    return build_expr_from_add(b, (GnuCAnyAdditiveExpression *)x);
}
BuildExpression *build_expr_lshift(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyShiftExpression *l = build_expr_to_shift(b, lhs);
    GnuCAnyAdditiveExpression *r = build_expr_to_add(b, rhs);
    GnuCTreeTagLeftShiftExpression *x = gnu_c_create_tree_tag_left_shift_expression(b->pool, l, b->lshift, r);
    return build_expr_from_shift(b, (GnuCAnyShiftExpression *)x);
}
BuildExpression *build_expr_rshift(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyShiftExpression *l = build_expr_to_shift(b, lhs);
    GnuCAnyAdditiveExpression *r = build_expr_to_add(b, rhs);
    GnuCTreeTagRightShiftExpression *x = gnu_c_create_tree_tag_right_shift_expression(b->pool, l, b->rshift, r);
    return build_expr_from_shift(b, (GnuCAnyShiftExpression *)x);
}
BuildExpression *build_expr_cmp_lt(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyRelationalExpression *l = build_expr_to_rel(b, lhs);
    GnuCAnyShiftExpression *r = build_expr_to_shift(b, rhs);
    GnuCTreeTagLtExpression *x = gnu_c_create_tree_tag_lt_expression(b->pool, l, b->lt, r);
    return build_expr_from_rel(b, (GnuCAnyRelationalExpression *)x);
}
BuildExpression *build_expr_cmp_gt(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyRelationalExpression *l = build_expr_to_rel(b, lhs);
    GnuCAnyShiftExpression *r = build_expr_to_shift(b, rhs);
    GnuCTreeTagGtExpression *x = gnu_c_create_tree_tag_gt_expression(b->pool, l, b->gt, r);
    return build_expr_from_rel(b, (GnuCAnyRelationalExpression *)x);
}
BuildExpression *build_expr_cmp_le(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyRelationalExpression *l = build_expr_to_rel(b, lhs);
    GnuCAnyShiftExpression *r = build_expr_to_shift(b, rhs);
    GnuCTreeTagLeExpression *x = gnu_c_create_tree_tag_le_expression(b->pool, l, b->le, r);
    return build_expr_from_rel(b, (GnuCAnyRelationalExpression *)x);
}
BuildExpression *build_expr_cmp_ge(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyRelationalExpression *l = build_expr_to_rel(b, lhs);
    GnuCAnyShiftExpression *r = build_expr_to_shift(b, rhs);
    GnuCTreeTagGeExpression *x = gnu_c_create_tree_tag_ge_expression(b->pool, l, b->ge, r);
    return build_expr_from_rel(b, (GnuCAnyRelationalExpression *)x);
}
BuildExpression *build_expr_cmp_eq(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyEqualityExpression *l = build_expr_to_eq(b, lhs);
    GnuCAnyRelationalExpression *r = build_expr_to_rel(b, rhs);
    GnuCTreeTagEqExpression *x = gnu_c_create_tree_tag_eq_expression(b->pool, l, b->eq, r);
    return build_expr_from_eq(b, (GnuCAnyEqualityExpression *)x);
}
BuildExpression *build_expr_cmp_ne(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyEqualityExpression *l = build_expr_to_eq(b, lhs);
    GnuCAnyRelationalExpression *r = build_expr_to_rel(b, rhs);
    GnuCTreeTagNeExpression *x = gnu_c_create_tree_tag_ne_expression(b->pool, l, b->ne, r);
    return build_expr_from_eq(b, (GnuCAnyEqualityExpression *)x);
}
BuildExpression *build_expr_bitwise_and(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyAndExpression *l = build_expr_to_bit_and(b, lhs);
    GnuCAnyEqualityExpression *r = build_expr_to_eq(b, rhs);
    GnuCTreeTagBitAndExpression *x = gnu_c_create_tree_tag_bit_and_expression(b->pool, l, b->ampersand, r);
    return build_expr_from_bit_and(b, (GnuCAnyAndExpression *)x);
}
BuildExpression *build_expr_bitwise_xor(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyExclusiveOrExpression *l = build_expr_to_bit_xor(b, lhs);
    GnuCAnyAndExpression *r = build_expr_to_bit_and(b, rhs);
    GnuCTreeTagBitXorExpression *x = gnu_c_create_tree_tag_bit_xor_expression(b->pool, l, b->caret, r);
    return build_expr_from_bit_xor(b, (GnuCAnyExclusiveOrExpression *)x);
}
BuildExpression *build_expr_bitwise_or(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyInclusiveOrExpression *l = build_expr_to_bit_or(b, lhs);
    GnuCAnyExclusiveOrExpression *r = build_expr_to_bit_xor(b, rhs);
    GnuCTreeTagBitIorExpression *x = gnu_c_create_tree_tag_bit_ior_expression(b->pool, l, b->pipe, r);
    return build_expr_from_bit_or(b, (GnuCAnyInclusiveOrExpression *)x);
}
BuildExpression *build_expr_logical_and(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyLogicalAndExpression *l = build_expr_to_log_and(b, lhs);
    GnuCAnyInclusiveOrExpression *r = build_expr_to_bit_or(b, rhs);
    GnuCTreeTagLogAndExpression *x = gnu_c_create_tree_tag_log_and_expression(b->pool, l, b->logical_and, r);
    return build_expr_from_log_and(b, (GnuCAnyLogicalAndExpression *)x);
}
BuildExpression *build_expr_logical_or(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyLogicalOrExpression *l = build_expr_to_log_or(b, lhs);
    GnuCAnyLogicalAndExpression *r = build_expr_to_log_and(b, rhs);
    GnuCTreeTagLogOrExpression *x = gnu_c_create_tree_tag_log_or_expression(b->pool, l, b->logical_or, r);
    return build_expr_from_log_or(b, (GnuCAnyLogicalOrExpression *)x);
}
BuildExpression *build_expr_conditional(Builder *b, BuildExpression *cond, BuildExpression *if_true, BuildExpression *if_false)
{
    GnuCAnyLogicalOrExpression *c = build_expr_to_log_or(b, cond);
    GnuCAnyExprList *t = build_expr_to_comma(b, if_true);
    GnuCAnyConditionalExpression *f = build_expr_to_conditional(b, if_false);
    GnuCTreeTagTernaryExpression *x = gnu_c_create_tree_tag_ternary_expression(b->pool, c, b->query, (GnuCOptExprList *)t, b->colon, f);
    return build_expr_from_conditional(b, (GnuCAnyConditionalExpression *)x);
}

BuildExpression *build_expr_assign(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyConditionalExpression *l = (GnuCAnyConditionalExpression *)build_expr_to_cast(b, lhs);
    GnuCAnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    GnuCTreeTagSimpleAssignmentExpression *a = gnu_c_create_tree_tag_simple_assignment_expression(b->pool, l, b->assign, r);
    return build_expr_from_assignment(b, (GnuCAnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_times(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyConditionalExpression *l = (GnuCAnyConditionalExpression *)build_expr_to_cast(b, lhs);
    GnuCAnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    GnuCTreeTagTimesAssignmentExpression *a = gnu_c_create_tree_tag_times_assignment_expression(b->pool, l, b->star_assign, r);
    return build_expr_from_assignment(b, (GnuCAnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_divide(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyConditionalExpression *l = (GnuCAnyConditionalExpression *)build_expr_to_cast(b, lhs);
    GnuCAnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    GnuCTreeTagDivideAssignmentExpression *a = gnu_c_create_tree_tag_divide_assignment_expression(b->pool, l, b->slash_assign, r);
    return build_expr_from_assignment(b, (GnuCAnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_modulus(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyConditionalExpression *l = (GnuCAnyConditionalExpression *)build_expr_to_cast(b, lhs);
    GnuCAnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    GnuCTreeTagModuloAssignmentExpression *a = gnu_c_create_tree_tag_modulo_assignment_expression(b->pool, l, b->percent_assign, r);
    return build_expr_from_assignment(b, (GnuCAnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_plus(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyConditionalExpression *l = (GnuCAnyConditionalExpression *)build_expr_to_cast(b, lhs);
    GnuCAnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    GnuCTreeTagPlusAssignmentExpression  *a = gnu_c_create_tree_tag_plus_assignment_expression(b->pool, l, b->plus_assign, r);
    return build_expr_from_assignment(b, (GnuCAnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_minus(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyConditionalExpression *l = (GnuCAnyConditionalExpression *)build_expr_to_cast(b, lhs);
    GnuCAnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    GnuCTreeTagMinusAssignmentExpression  *a = gnu_c_create_tree_tag_minus_assignment_expression(b->pool, l, b->minus_assign, r);
    return build_expr_from_assignment(b, (GnuCAnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_lshift(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyConditionalExpression *l = (GnuCAnyConditionalExpression *)build_expr_to_cast(b, lhs);
    GnuCAnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    GnuCTreeTagLeftShiftAssignmentExpression *a = gnu_c_create_tree_tag_left_shift_assignment_expression(b->pool, l, b->lshift_assign, r);
    return build_expr_from_assignment(b, (GnuCAnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_rshift(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyConditionalExpression *l = (GnuCAnyConditionalExpression *)build_expr_to_cast(b, lhs);
    GnuCAnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    GnuCTreeTagRightShiftAssignmentExpression *a = gnu_c_create_tree_tag_right_shift_assignment_expression(b->pool, l, b->rshift_assign, r);
    return build_expr_from_assignment(b, (GnuCAnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_bitwise_and(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyConditionalExpression *l = (GnuCAnyConditionalExpression *)build_expr_to_cast(b, lhs);
    GnuCAnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    GnuCTreeTagBitAndAssignmentExpression *a = gnu_c_create_tree_tag_bit_and_assignment_expression(b->pool, l, b->ampersand_assign, r);
    return build_expr_from_assignment(b, (GnuCAnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_bitwise_xor(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyConditionalExpression *l = (GnuCAnyConditionalExpression *)build_expr_to_cast(b, lhs);
    GnuCAnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    GnuCTreeTagBitXorAssignmentExpression *a = gnu_c_create_tree_tag_bit_xor_assignment_expression(b->pool, l, b->caret_assign, r);
    return build_expr_from_assignment(b, (GnuCAnyAssignmentExpression *)a);
}
BuildExpression *build_expr_assign_bitwise_or(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyConditionalExpression *l = (GnuCAnyConditionalExpression *)build_expr_to_cast(b, lhs);
    GnuCAnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    GnuCTreeTagBitIorAssignmentExpression *a = gnu_c_create_tree_tag_bit_ior_assignment_expression(b->pool, l, b->pipe_assign, r);
    return build_expr_from_assignment(b, (GnuCAnyAssignmentExpression *)a);
}

BuildExpression *build_expr_comma(Builder *b, BuildExpression *lhs, BuildExpression *rhs)
{
    GnuCAnyExprList *l = build_expr_to_comma(b, lhs);
    GnuCAnyAssignmentExpression *r = build_expr_to_assignment(b, rhs);
    GnuCTreeTagMultiExprList *x = gnu_c_create_tree_tag_multi_expr_list(b->pool, l, b->comma, r);
    return build_expr_from_comma(b, (GnuCAnyExprList *)x);
}
