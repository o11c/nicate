#!/usr/bin/env python3
#   Copyright © 2015 Ben Longbons
#
#   This file is part of Nicate.
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Lesser General Public License as published
#   by the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.

import sys

import nicate


def main():
    b = nicate.Builder()

    no_sc = b.storage_class_none()
    extern_sc = b.storage_class_extern()

    ty_void = b.type_void()
    ty_char = b.type_char()
    ty_ccp = b.type_pointer(b.type_const(ty_char))
    ty_cpp = b.type_pointer(b.type_pointer(ty_char))
    ty_int = b.type_signed_int()

    p_format = b.param_declaration(no_sc, ty_ccp, "format")
    printf_params = [p_format]
    printf_ty = b.type_function(ty_int, printf_params, True)
    decl_printf = b.declaration_noinit(extern_sc, printf_ty, "printf")
    top_decl_printf = b.top_decl(decl_printf)

    zero = b.expr_int(0)
    hello = b.expr_string("Hello, World!\n")
    expr_argc = b.expr_id("argc")
    expr_argv = b.expr_id("argv")
    expr_printf = b.expr_id("printf")
    cast_argc = b.expr_cast(ty_void, expr_argc)
    cast_argv = b.expr_cast(ty_void, expr_argv)
    printf_args = [hello]
    call_printf = b.expr_call(expr_printf, printf_args)
    stmt_cast_argc = b.stmt_expr(cast_argc)
    stmt_cast_argv = b.stmt_expr(cast_argv)
    stmt_call_printf = b.stmt_expr(call_printf)
    return_0 = b.stmt_return(zero)
    main_stmts = [stmt_cast_argc, stmt_cast_argv, stmt_call_printf, return_0]

    p_argc = b.param_declaration(no_sc, ty_int, "argc")
    p_argv = b.param_declaration(no_sc, ty_cpp, "argv")
    main_params = [p_argc, p_argv]
    main_ty = b.type_function(ty_int, main_params, False)
    main_body = b.stmt_compound([], main_stmts)
    def_main = b.top_function_definition(no_sc, "main", main_ty, main_body)

    tops = [top_decl_printf, def_main]
    tu = b.tu(tops)
    tu.emit_to_file(sys.stdout)

if __name__ == '__main__':
    main()
