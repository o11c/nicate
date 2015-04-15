#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "fwd.h"


Builder *builder_create(void);
void builder_destroy(Builder *b);

void builder_emit_tu(BuildTranslationUnit *tu, FILE *fp);

BuildTranslationUnit *build_tu(Builder *b, size_t ntops, BuildTopLevel **tops);

BuildTopLevel *build_top_decl(Builder *b, BuildDeclaration *decl);
BuildTopLevel *build_top_struct_definition(Builder *b, const char *name, size_t nmembs, BuildMemberDeclaration **decls);
BuildTopLevel *build_top_union_definition(Builder *b, const char *name, size_t nmembs, BuildMemberDeclaration **decls);
BuildTopLevel *build_top_enum_definition(Builder *b, const char *name, size_t nvars, BuildEnumerator **vars);
BuildTopLevel *build_top_function_definition(Builder *b, BuildStorageClass *scs, const char *name, BuildType *fnty, BuildStatement *body);

BuildStorageClass *build_storage_class_none(Builder *b);
BuildStorageClass *build_storage_class_typedef(Builder *b);
BuildStorageClass *build_storage_class_extern(Builder *b);
BuildStorageClass *build_storage_class_static(Builder *b);
BuildStorageClass *build_storage_class_register(Builder *b);

BuildDeclaration *build_declaration_noinit(Builder *b, BuildStorageClass *scs, BuildType *type, const char *name);
BuildDeclaration *build_declaration_init(Builder *b, BuildStorageClass *scs, BuildType *type, const char *name, BuildInitializer *init);
BuildParamDeclaration *build_param_declaration(Builder *b, BuildStorageClass *scs, BuildType *type, const char *name);
BuildParamDeclaration *build_param_declaration_anon(Builder *b, BuildStorageClass *scs, BuildType *type);
BuildMemberDeclaration *build_member_declaration(Builder *b, BuildType *type, const char *name);
BuildMemberDeclaration *build_member_declaration_bitfield(Builder *b, BuildType *type, const char *name, BuildExpression *width);
BuildMemberDeclaration *build_member_declaration_bitfield_anon(Builder *b, BuildType *type, BuildExpression *width);
BuildEnumerator *build_enumerator_noinit(Builder *b, const char *name);
BuildEnumerator *build_enumerator_init(Builder *b, const char *name, BuildExpression *init);

BuildType *build_type_typedef(Builder *b, const char *name);
BuildType *build_type_struct(Builder *b, const char *name);
BuildType *build_type_union(Builder *b, const char *name);
BuildType *build_type_enum(Builder *b, const char *name);
BuildType *build_type_void(Builder *b);
BuildType *build_type_char(Builder *b);
BuildType *build_type_signed_char(Builder *b);
BuildType *build_type_unsigned_char(Builder *b);
BuildType *build_type_signed_short(Builder *b);
BuildType *build_type_unsigned_short(Builder *b);
BuildType *build_type_signed_int(Builder *b);
BuildType *build_type_unsigned_int(Builder *b);
BuildType *build_type_signed_long(Builder *b);
BuildType *build_type_unsigned_long(Builder *b);
BuildType *build_type_signed_long_long(Builder *b);
BuildType *build_type_unsigned_long_long(Builder *b);
BuildType *build_type_float(Builder *b);
BuildType *build_type_double(Builder *b);
BuildType *build_type_long_double(Builder *b);
BuildType *build_type_const(Builder *b, BuildType *type);
BuildType *build_type_volatile(Builder *b, BuildType *type);
BuildType *build_type_pointer(Builder *b, BuildType *type);
BuildType *build_type_array(Builder *b, BuildType *type, BuildExpression *size);
BuildType *build_type_array_unknown_bound(Builder *b, BuildType *type);
BuildType *build_type_function(Builder *b, BuildType *ret, size_t nargs, BuildParamDeclaration **args, bool vararg);

BuildStatement *build_stmt_none(Builder *b);
BuildStatement *build_stmt_expr(Builder *b, BuildExpression *expr);
BuildStatement *build_stmt_label(Builder *b, const char *label, BuildStatement *target);
BuildStatement *build_stmt_case(Builder *b, BuildExpression *expr, BuildStatement *target);
BuildStatement *build_stmt_default(Builder *b, BuildStatement *target);
BuildStatement *build_stmt_compound(Builder *b, size_t ndecls, BuildDeclaration **decls, size_t nstmts, BuildStatement **stmts);
BuildStatement *build_stmt_if(Builder *b, BuildExpression *cond, BuildStatement *if_true);
BuildStatement *build_stmt_if_else(Builder *b, BuildExpression *cond, BuildStatement *if_true, BuildStatement *if_false);
BuildStatement *build_stmt_while(Builder *b, BuildExpression *cond, BuildStatement *body);
BuildStatement *build_stmt_do_while(Builder *b, BuildStatement *body, BuildExpression *cond);
BuildStatement *build_stmt_for(Builder *b, BuildExpression *init, BuildExpression *cond, BuildExpression *post, BuildStatement *body);
BuildStatement *build_stmt_goto(Builder *b, const char *label);
BuildStatement *build_stmt_continue(Builder *b);
BuildStatement *build_stmt_break(Builder *b);
BuildStatement *build_stmt_return(Builder *b, BuildExpression *expr);

BuildInitializer *build_initializer_expr(Builder *b, BuildExpression *expr);
BuildInitializer *build_initializer_braced(Builder *b, size_t nelems, BuildInitializer **elems);

BuildExpression *build_expr_none(Builder *b);
BuildExpression *build_expr_parenthesize(Builder *b, BuildExpression *expr);
BuildExpression *build_expr_id(Builder *b, const char *id);
BuildExpression *build_expr_char(Builder *b, char c);
BuildExpression *build_expr_multi_char(Builder *b, const char *c);
BuildExpression *build_expr_multi_char_slice(Builder *b, const char *c, size_t l);
BuildExpression *build_expr_string(Builder *b, const char *s);
BuildExpression *build_expr_string_slice(Builder *b, const char *s, size_t l);
BuildExpression *build_expr_int(Builder *b, uintmax_t i);
BuildExpression *build_expr_int_oct(Builder *b, uintmax_t i, unsigned min_len);
BuildExpression *build_expr_int_hex(Builder *b, uintmax_t i, unsigned min_len);
BuildExpression *build_expr_float(Builder *b, long double f);

BuildExpression *build_expr_index(Builder *b, BuildExpression *arr, BuildExpression *idx);
BuildExpression *build_expr_call(Builder *b, BuildExpression *fun, size_t nargs, BuildExpression **args);
BuildExpression *build_expr_direct_member(Builder *b, BuildExpression *obj, const char *field);
BuildExpression *build_expr_indirect_member(Builder *b, BuildExpression *ptr, const char *field);
BuildExpression *build_expr_post_inc(Builder *b, BuildExpression *lhs);
BuildExpression *build_expr_post_dec(Builder *b, BuildExpression *lhs);
BuildExpression *build_expr_pre_inc(Builder *b, BuildExpression *rhs);
BuildExpression *build_expr_pre_dec(Builder *b, BuildExpression *rhs);
BuildExpression *build_expr_addressof(Builder *b, BuildExpression *obj);
BuildExpression *build_expr_deref(Builder *b, BuildExpression *ptr);
BuildExpression *build_expr_unary_plus(Builder *b, BuildExpression *rhs);
BuildExpression *build_expr_unary_minus(Builder *b, BuildExpression *rhs);
BuildExpression *build_expr_bitwise_not(Builder *b, BuildExpression *rhs);
BuildExpression *build_expr_logical_not(Builder *b, BuildExpression *rhs);
BuildExpression *build_expr_sizeof_expr(Builder *b, BuildExpression *obj);
BuildExpression *build_expr_sizeof_type(Builder *b, BuildType *type);
BuildExpression *build_expr_cast(Builder *b, BuildType *type, BuildExpression *rhs);
BuildExpression *build_expr_times(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_divide(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_modulus(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_plus(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_minus(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_lshift(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_rshift(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_cmp_lt(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_cmp_gt(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_cmp_le(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_cmp_ge(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_cmp_eq(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_cmp_ne(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_bitwise_and(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_bitwise_xor(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_bitwise_or(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_logical_and(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_logical_or(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_conditional(Builder *b, BuildExpression *cond, BuildExpression *if_true, BuildExpression *if_false);

BuildExpression *build_expr_assign(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_assign_times(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_assign_divide(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_assign_modulus(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_assign_plus(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_assign_minus(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_assign_lshift(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_assign_rshift(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_assign_bitwise_and(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_assign_bitwise_xor(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_assign_bitwise_or(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
BuildExpression *build_expr_comma(Builder *b, BuildExpression *lhs, BuildExpression *rhs);
