#include "builder.h"

#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])
#define ARRAY_PAIR(a) ARRAY_SIZE(a), a
#define ZERO_PAIR(a) 0, NULL

int main()
{
    Builder *b = builder_create();

    BuildStorageClass *no_sc = build_storage_class_none(b);
    BuildStorageClass *extern_sc = build_storage_class_extern(b);

    BuildType *ty_void = build_type_void(b);
    BuildType *ty_char = build_type_char(b);
    BuildType *ty_ccp = build_type_pointer(b, build_type_const(b, ty_char));
    BuildType *ty_cpp = build_type_pointer(b, build_type_pointer(b, ty_char));
    BuildType *ty_int = build_type_signed_int(b);

    BuildParamDeclaration *p_format = build_param_declaration(b, no_sc, ty_ccp, "format");
    BuildParamDeclaration *printf_params[] = {p_format};
    BuildType *printf_ty = build_type_function(b, ty_int, ARRAY_PAIR(printf_params), true);
    BuildDeclaration *decl_printf = build_declaration_noinit(b, extern_sc, printf_ty, "printf");
    BuildTopLevel *top_decl_printf = build_top_decl(b, decl_printf);

    BuildExpression *zero = build_expr_int(b, 0);
    BuildExpression *hello = build_expr_string(b, "Hello, World!");
    BuildExpression *expr_argc = build_expr_id(b, "argc");
    BuildExpression *expr_argv = build_expr_id(b, "argv");
    BuildExpression *expr_printf = build_expr_id(b, "printf");
    BuildExpression *cast_argc = build_expr_cast(b, ty_void, expr_argc);
    BuildExpression *cast_argv = build_expr_cast(b, ty_void, expr_argv);
    BuildExpression *printf_args[] = {hello};
    BuildExpression *call_printf = build_expr_call(b, expr_printf, ARRAY_PAIR(printf_args));
    BuildStatement *stmt_cast_argc = build_stmt_expr(b, cast_argc);
    BuildStatement *stmt_cast_argv = build_stmt_expr(b, cast_argv);
    BuildStatement *stmt_call_printf = build_stmt_expr(b, call_printf);
    BuildStatement *return_0 = build_stmt_return(b, zero);
    BuildStatement *main_stmts[] = {stmt_cast_argc, stmt_cast_argv, stmt_call_printf, return_0};

    BuildParamDeclaration *p_argc = build_param_declaration(b, no_sc, ty_int, "argc");
    BuildParamDeclaration *p_argv = build_param_declaration(b, no_sc, ty_cpp, "argv");
    BuildParamDeclaration *main_params[] = {p_argc, p_argv};
    BuildType *main_ty = build_type_function(b, ty_int, ARRAY_PAIR(main_params), false);
    BuildStatement *main_body = build_stmt_compound(b, ZERO_PAIR(), ARRAY_PAIR(main_stmts));
    BuildTopLevel *def_main = build_top_function_definition(b, no_sc, "main", main_ty, main_body);

    BuildTopLevel *tops[] = {top_decl_printf, def_main};
    BuildTranslationUnit *tu = build_tu(b, ARRAY_PAIR(tops));
    builder_emit_tu(tu, stdout);
    builder_destroy(b);
    return 0;
}
