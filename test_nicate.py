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

import gc

import pytest

import nicate


@pytest.fixture
def builder():
    return nicate.Builder()


def test_top(builder):
    b = builder
    # b.top_decl is sufficiently tested by `test_type`

    ty_uint = b.type_unsigned_int()

    expr_0 = b.expr_int(0)
    expr_1 = b.expr_int(1)

    member1 = b.member_declaration(ty_uint, 'bar')
    member2 = b.member_declaration_bitfield(ty_uint, 'baz', expr_1)
    member3 = b.member_declaration_bitfield_anon(ty_uint, expr_0)

    enum1 = b.enumerator_init('BAR', expr_1)
    enum2 = b.enumerator_noinit('BAZ')

    top = b.top_struct_definition('Foo', [member1, member2, member3])
    tu = b.tu([top])
    assert tu.emit_to_string() == 'struct Foo\n{\n    unsigned int bar;\n    unsigned int baz : 1;\n    unsigned int : 0;\n};\n'

    top = b.top_union_definition('Foo', [member1, member2, member3])
    tu = b.tu([top])
    assert tu.emit_to_string() == 'union Foo\n{\n    unsigned int bar;\n    unsigned int baz : 1;\n    unsigned int : 0;\n};\n'

    top = b.top_enum_definition('Foo', [enum1, enum2])
    tu = b.tu([top])
    assert tu.emit_to_string() == 'enum Foo\n{\n    BAR = 1, BAZ\n};\n'

    no_sc = b.storage_class_none()
    fty = b.type_function(ty_uint, [b.param_declaration(no_sc, ty_uint, 'bar')], True)
    body = b.stmt_return(expr_0)
    top = b.top_function_definition(no_sc, 'foo', fty, body)
    tu = b.tu([top])
    assert tu.emit_to_string() == 'unsigned int foo(unsigned int bar, ...)\n{\n    return 0;\n}\n'


def test_type(builder):
    b = builder
    no_sc = b.storage_class_none()
    ty_void = b.type_void()
    ty_int = b.type_signed_int()
    ty_foo = b.type_typedef('Foo')
    ty_sig_handler = b.type_pointer(b.type_function(ty_void, [b.param_declaration(no_sc, ty_int, 'sig')], False))
    expr_0 = b.expr_int(0)
    init_0 = b.initializer_expr(expr_0)
    init_00 = b.initializer_braced([init_0, init_0])

    for t, s, *a in [
            ('void', 'void{s}'),
            ('char', 'char{s}'),
            ('signed_char', 'signed char{s}'),
            ('unsigned_char', 'unsigned char{s}'),
            ('signed_short', 'signed short int{s}'),
            ('unsigned_short', 'unsigned short int{s}'),
            ('signed_int', 'signed int{s}'),
            ('unsigned_int', 'unsigned int{s}'),
            ('signed_long', 'signed long int{s}'),
            ('unsigned_long', 'unsigned long int{s}'),
            ('signed_long_long', 'signed long long int{s}'),
            ('unsigned_long_long', 'unsigned long long int{s}'),
            ('float', 'float{s}'),
            ('double', 'double{s}'),
            ('long_double', 'long double{s}'),
            ('typedef', 'Foo{s}', 'Foo'),
            ('struct', 'struct Foo{s}', 'Foo'),
            ('union', 'union Foo{s}', 'Foo'),
            ('enum', 'enum Foo{s}', 'Foo'),
            ('const', 'Foo const{s}', ty_foo),
            ('volatile', 'Foo volatile{s}', ty_foo),
            ('const', 'Foo const volatile{s}', b.type_volatile(ty_foo)),
            ('volatile', 'Foo const volatile{s}', b.type_const(ty_foo)),
            ('array', 'Foo{s}[0]', ty_foo, expr_0),
            ('array_unknown_bound', 'Foo{s}[]', ty_foo),
            ('function', 'Foo{s}(void)', ty_foo, [b.param_declaration_anon(no_sc, ty_void)], False),
            ('function', 'Foo{s}(signed int, ...)', ty_foo, [b.param_declaration_anon(no_sc, ty_int)], True),
            ('const', 'Foo volatile *const{s}', b.type_pointer(b.type_volatile(ty_foo))),
            ('const', 'Foo const{s}[]', b.type_array_unknown_bound(ty_foo)),
            ('array_unknown_bound', 'Foo const{s}[]', b.type_const(ty_foo)),
            ('pointer', 'Foo (*{ns})(void)', b.type_function(ty_foo, [b.param_declaration_anon(no_sc, ty_void)], False)),
            ('const', 'Foo volatile (*const{s})[]', b.type_pointer(b.type_array_unknown_bound(b.type_volatile(ty_foo)))),
            ('array_unknown_bound', 'Foo *{ns}[]', b.type_pointer(ty_foo)),
            ('function', 'void (*{ns}(signed int signum, void (*handler)(signed int sig)))(signed int sig)', ty_sig_handler, [b.param_declaration(no_sc, ty_int, 'signum'), b.param_declaration(no_sc, ty_sig_handler, 'handler')], False),
    ]:
        ty = getattr(b, 'type_%s' % t)(*a)

        decl = b.declaration_noinit(no_sc, ty, 'foo')
        top = b.top_decl(decl)
        tu = b.tu([top])
        assert tu.emit_to_string() == '%s;\n' % s.format(s=' foo', ns='foo')

        decl = b.declaration_init(no_sc, ty, 'foo', init_0)
        top = b.top_decl(decl)
        tu = b.tu([top])
        assert tu.emit_to_string() == '%s = 0;\n' % s.format(s=' foo', ns='foo')

        decl = b.declaration_init(no_sc, ty, 'foo', init_00)
        top = b.top_decl(decl)
        tu = b.tu([top])
        assert tu.emit_to_string() == '%s = { 0, 0 };\n' % s.format(s=' foo', ns='foo')

        decl = b.declaration_noinit(no_sc, b.type_function(ty_void, [b.param_declaration_anon(no_sc, ty)], False), 'foo')
        top = b.top_decl(decl)
        tu = b.tu([top])
        assert tu.emit_to_string() == 'void foo(%s);\n' % s.format(s='', ns='')

        decl = b.declaration_noinit(no_sc, b.type_function(ty_void, [b.param_declaration(no_sc, ty, 'bar')], False), 'foo')
        top = b.top_decl(decl)
        tu = b.tu([top])
        assert tu.emit_to_string() == 'void foo(%s);\n' % s.format(s=' bar', ns='bar')

        decl = b.member_declaration(ty, 'bar')
        top = b.top_struct_definition('Foo', [decl])
        tu = b.tu([top])
        assert tu.emit_to_string() == 'struct Foo\n{\n    %s;\n};\n' % s.format(s=' bar', ns='bar')

def sf(b, s):
    n = b.storage_class_none()
    v = b.type_void()
    p = b.param_declaration_anon(n, v)
    t = b.type_function(v, [p], False)
    c = b.stmt_compound([], [s])
    f = b.top_function_definition(n, 'foo', t, c)
    return b.tu([f])

def ef(b, e):
    return sf(b, b.stmt_expr(e))

def pat(s):
    return 'void foo(void)\n{\n    %s\n}\n' % s.replace('\n', '\n    ')

def test_stmt(builder):
    b = builder

    no_sc = b.storage_class_none()
    ty_int = b.type_signed_int()
    expr_none = b.expr_none()
    expr_0 = b.expr_int(0)
    expr_1 = b.expr_int(1)
    expr_2 = b.expr_int(2)
    expr_3 = b.expr_int(3)
    stmt_0 = b.stmt_expr(expr_0)
    stmt_1 = b.stmt_expr(expr_1)
    stmt_2 = b.stmt_expr(expr_2)
    stmt_3 = b.stmt_expr(expr_3)

    tu = sf(b, b.stmt_none())
    assert tu.emit_to_string() == pat(';')

    # more in test_expr below
    tu = sf(b, b.stmt_expr(expr_0))
    assert tu.emit_to_string() == pat('0;')

    tu = sf(b, b.stmt_label('bar', b.stmt_none()))
    assert tu.emit_to_string() == pat('bar:;')

    tu = sf(b, b.stmt_label('bar', b.stmt_compound([], [])))
    assert tu.emit_to_string() == pat('bar:\n{\n}')

    tu = sf(b, b.stmt_case(expr_0, b.stmt_none()))
    assert tu.emit_to_string() == pat('case 0:;')

    tu = sf(b, b.stmt_case(expr_0, b.stmt_compound([], [])))
    assert tu.emit_to_string() == pat('case 0:\n{\n}')

    tu = sf(b, b.stmt_default(b.stmt_none()))
    assert tu.emit_to_string() == pat('default:;')

    tu = sf(b, b.stmt_default(b.stmt_compound([], [])))
    assert tu.emit_to_string() == pat('default:\n{\n}')

    tu = sf(b, b.stmt_compound([b.declaration_init(no_sc, ty_int, 'i', b.initializer_expr(expr_0)), b.declaration_noinit(no_sc, ty_int, 'j')], [stmt_0, stmt_1, stmt_2, stmt_3]))
    assert tu.emit_to_string() == pat('{\n    signed int i = 0;\n    signed int j;\n    0;\n    1;\n    2;\n    3;\n}')

    tu = sf(b, b.stmt_if(expr_0, b.stmt_if_else(expr_1, stmt_0, stmt_1)))
    assert tu.emit_to_string() == pat('if (0)\n{\n    if (1)\n    {\n        0;\n    }\n    else\n    {\n        1;\n    }\n}')

    tu = sf(b, b.stmt_if_else(expr_0, b.stmt_if(expr_1, stmt_0), b.stmt_if(expr_2, stmt_1)))
    assert tu.emit_to_string() == pat('if (0)\n{\n    if (1)\n    {\n        0;\n    }\n}\nelse if (2)\n{\n    1;\n}')

    tu = sf(b, b.stmt_if(expr_0, b.stmt_compound([], [])))
    assert tu.emit_to_string() == pat('if (0)\n{\n}')

    tu = sf(b, b.stmt_while(expr_0, stmt_0))
    assert tu.emit_to_string() == pat('while (0)\n{\n    0;\n}')

    tu = sf(b, b.stmt_while(expr_0, b.stmt_compound([], [])))
    assert tu.emit_to_string() == pat('while (0)\n{\n}')

    tu = sf(b, b.stmt_do_while(stmt_0, expr_0))
    assert tu.emit_to_string() == pat('do\n{\n    0;\n}\nwhile (0);')

    tu = sf(b, b.stmt_do_while(b.stmt_compound([], []), expr_0))
    assert tu.emit_to_string() == pat('do\n{\n}\nwhile (0);')

    tu = sf(b, b.stmt_for(expr_0, expr_1, expr_2, stmt_3))
    assert tu.emit_to_string() == pat('for (0; 1; 2)\n{\n    3;\n}')

    tu = sf(b, b.stmt_for(expr_none, expr_none, expr_none, b.stmt_compound([], [])))
    assert tu.emit_to_string() == pat('for (;;)\n{\n}')

    tu = sf(b, b.stmt_goto('fail'))
    assert tu.emit_to_string() == pat('goto fail;')

    tu = sf(b, b.stmt_continue())
    assert tu.emit_to_string() == pat('continue;')

    tu = sf(b, b.stmt_break())
    assert tu.emit_to_string() == pat('break;')

    tu = sf(b, b.stmt_return(expr_none))
    assert tu.emit_to_string() == pat('return;')

    tu = sf(b, b.stmt_return(expr_0))
    assert tu.emit_to_string() == pat('return 0;')

def test_expr_primary(builder):
    b = builder

    expr_0 = b.expr_int(0)

    tu = ef(b, expr_0)
    assert tu.emit_to_string() == pat('0;')

    tu = ef(b, b.expr_parenthesize(expr_0))
    assert tu.emit_to_string() == pat('(0);')

    tu = ef(b, b.expr_id('hi'))
    assert tu.emit_to_string() == pat('hi;')

    tu = ef(b, b.expr_char('\0'))
    assert tu.emit_to_string() == pat("'\\000';")
    tu = ef(b, b.expr_char('\a'))
    assert tu.emit_to_string() == pat("'\\a';")
    tu = ef(b, b.expr_char('\b'))
    assert tu.emit_to_string() == pat("'\\b';")
    tu = ef(b, b.expr_char('\t'))
    assert tu.emit_to_string() == pat("'\\t';")
    tu = ef(b, b.expr_char('\n'))
    assert tu.emit_to_string() == pat("'\\n';")
    tu = ef(b, b.expr_char('\v'))
    assert tu.emit_to_string() == pat("'\\v';")
    tu = ef(b, b.expr_char('\f'))
    assert tu.emit_to_string() == pat("'\\f';")
    tu = ef(b, b.expr_char('\r'))
    assert tu.emit_to_string() == pat("'\\r';")
    tu = ef(b, b.expr_char('\x1b'))
    assert tu.emit_to_string() == pat("'\\033';")
    tu = ef(b, b.expr_char('\x1f'))
    assert tu.emit_to_string() == pat("'\\037';")
    tu = ef(b, b.expr_char('\x20'))
    assert tu.emit_to_string() == pat("' ';")
    tu = ef(b, b.expr_char('"'))
    assert tu.emit_to_string() == pat("'\"';")
    tu = ef(b, b.expr_char('\''))
    assert tu.emit_to_string() == pat("'\\'';")
    tu = ef(b, b.expr_char('\\'))
    assert tu.emit_to_string() == pat("'\\\\';")
    tu = ef(b, b.expr_char('h'))
    assert tu.emit_to_string() == pat("'h';")
    tu = ef(b, b.expr_char('\x7e'))
    assert tu.emit_to_string() == pat("'~';")
    tu = ef(b, b.expr_char('\x7f'))
    assert tu.emit_to_string() == pat("'\\177';")
    tu = ef(b, b.expr_char('\udc80'))
    assert tu.emit_to_string() == pat("'\\200';")
    tu = ef(b, b.expr_char('\udcff'))
    assert tu.emit_to_string() == pat("'\\377';")

    tu = ef(b, b.expr_multi_char('ab\0c'))
    assert tu.emit_to_string() == pat("'ab\\000c';")

    tu = ef(b, b.expr_string('\0'))
    assert tu.emit_to_string() == pat('"\\000";')
    tu = ef(b, b.expr_string('\a'))
    assert tu.emit_to_string() == pat('"\\a";')
    tu = ef(b, b.expr_string('\b'))
    assert tu.emit_to_string() == pat('"\\b";')
    tu = ef(b, b.expr_string('\t'))
    assert tu.emit_to_string() == pat('"\\t";')
    tu = ef(b, b.expr_string('\n'))
    assert tu.emit_to_string() == pat('"\\n";')
    tu = ef(b, b.expr_string('\v'))
    assert tu.emit_to_string() == pat('"\\v";')
    tu = ef(b, b.expr_string('\f'))
    assert tu.emit_to_string() == pat('"\\f";')
    tu = ef(b, b.expr_string('\r'))
    assert tu.emit_to_string() == pat('"\\r";')
    tu = ef(b, b.expr_string('\x1b'))
    assert tu.emit_to_string() == pat('"\\033";')
    tu = ef(b, b.expr_string('\x1f'))
    assert tu.emit_to_string() == pat('"\\037";')
    tu = ef(b, b.expr_string('\x20'))
    assert tu.emit_to_string() == pat('" ";')
    tu = ef(b, b.expr_string('"'))
    assert tu.emit_to_string() == pat('"\\"";')
    tu = ef(b, b.expr_string('\''))
    assert tu.emit_to_string() == pat('"\'";')
    tu = ef(b, b.expr_string('\\'))
    assert tu.emit_to_string() == pat('"\\\\";')
    tu = ef(b, b.expr_string('h'))
    assert tu.emit_to_string() == pat('"h";')
    tu = ef(b, b.expr_string('\x7e'))
    assert tu.emit_to_string() == pat('"~";')
    tu = ef(b, b.expr_string('\x7f'))
    assert tu.emit_to_string() == pat('"\\177";')
    tu = ef(b, b.expr_string('\udc80'))
    assert tu.emit_to_string() == pat('"\\200";')
    tu = ef(b, b.expr_string('\udcff'))
    assert tu.emit_to_string() == pat('"\\377";')

    tu = ef(b, b.expr_string('ab\0c'))
    assert tu.emit_to_string() == pat('"ab\\000c";')

    tu = ef(b, b.expr_int(0))
    assert tu.emit_to_string() == pat('0;')

    tu = ef(b, b.expr_int(102030405))
    assert tu.emit_to_string() == pat('102030405;')

    tu = ef(b, b.expr_int_oct(0o102030405, 0))
    assert tu.emit_to_string() == pat('0102030405;')

    tu = ef(b, b.expr_int_oct(0o7, 3))
    assert tu.emit_to_string() == pat('0007;')

    tu = ef(b, b.expr_int_hex(0x10203040, 0))
    assert tu.emit_to_string() == pat('0x10203040;')

    tu = ef(b, b.expr_int_hex(0xff, 4))
    assert tu.emit_to_string() == pat('0x00ff;')

    tu = ef(b, b.expr_float(1.5))
    assert tu.emit_to_string() == pat('1.500000000000000000000e+00;')

def test_expr_rest(builder):
    b = builder

    ta = b.type_typedef('A')
    xa = b.expr_id('a')
    xb = b.expr_id('b')
    xc = b.expr_id('c')
    xd = b.expr_id('d')
    xe = b.expr_id('e')

    tu = ef(b, b.expr_index(xa, xb))
    assert tu.emit_to_string() == pat('a[b];')
    tu = ef(b, b.expr_index(b.expr_index(xa, xb), xc))
    assert tu.emit_to_string() == pat('a[b][c];')
    tu = ef(b, b.expr_index(xa, b.expr_index(xb, xc)))
    assert tu.emit_to_string() == pat('a[b[c]];')

    tu = ef(b, b.expr_call(xa, []))
    assert tu.emit_to_string() == pat('a();')
    tu = ef(b, b.expr_call(xa, [xb]))
    assert tu.emit_to_string() == pat('a(b);')
    tu = ef(b, b.expr_call(xa, [xb, xc]))
    assert tu.emit_to_string() == pat('a(b, c);')
    tu = ef(b, b.expr_call(xa, [b.expr_call(xb, [])]))
    assert tu.emit_to_string() == pat('a(b());')
    tu = ef(b, b.expr_call(b.expr_call(xa, []), []))
    assert tu.emit_to_string() == pat('a()();')

    tu = ef(b, b.expr_direct_member(xa, 'f'))
    assert tu.emit_to_string() == pat('a.f;')
    tu = ef(b, b.expr_direct_member(b.expr_direct_member(xa, 'f'), 'g'))
    assert tu.emit_to_string() == pat('a.f.g;')

    tu = ef(b, b.expr_indirect_member(xa, 'f'))
    assert tu.emit_to_string() == pat('a->f;')
    tu = ef(b, b.expr_indirect_member(b.expr_indirect_member(xa, 'f'), 'g'))
    assert tu.emit_to_string() == pat('a->f->g;')

    tu = ef(b, b.expr_post_inc(xa))
    assert tu.emit_to_string() == pat('a++;')
    tu = ef(b, b.expr_post_inc(b.expr_post_inc(xa)))
    assert tu.emit_to_string() == pat('a++++;')

    tu = ef(b, b.expr_post_dec(xa))
    assert tu.emit_to_string() == pat('a--;')
    tu = ef(b, b.expr_post_dec(b.expr_post_dec(xa)))
    assert tu.emit_to_string() == pat('a----;')

    tu = ef(b, b.expr_pre_inc(xa))
    assert tu.emit_to_string() == pat('++a;')
    tu = ef(b, b.expr_pre_inc(b.expr_pre_inc(xa)))
    assert tu.emit_to_string() == pat('++++a;')

    tu = ef(b, b.expr_pre_dec(xa))
    assert tu.emit_to_string() == pat('--a;')
    tu = ef(b, b.expr_pre_dec(b.expr_pre_dec(xa)))
    assert tu.emit_to_string() == pat('----a;')

    tu = ef(b, b.expr_addressof(xa))
    assert tu.emit_to_string() == pat('&a;')
    tu = ef(b, b.expr_addressof(b.expr_addressof(xa)))
    assert tu.emit_to_string() == pat('& &a;')

    tu = ef(b, b.expr_deref(xa))
    assert tu.emit_to_string() == pat('*a;')
    tu = ef(b, b.expr_deref(b.expr_deref(xa)))
    assert tu.emit_to_string() == pat('**a;')

    tu = ef(b, b.expr_unary_plus(xa))
    assert tu.emit_to_string() == pat('+a;')
    tu = ef(b, b.expr_unary_plus(b.expr_unary_plus(xa)))
    assert tu.emit_to_string() == pat('+ +a;')

    tu = ef(b, b.expr_unary_minus(xa))
    assert tu.emit_to_string() == pat('-a;')
    tu = ef(b, b.expr_unary_minus(b.expr_unary_minus(xa)))
    assert tu.emit_to_string() == pat('- -a;')

    tu = ef(b, b.expr_bitwise_not(xa))
    assert tu.emit_to_string() == pat('~a;')
    tu = ef(b, b.expr_bitwise_not(b.expr_bitwise_not(xa)))
    assert tu.emit_to_string() == pat('~~a;')

    tu = ef(b, b.expr_logical_not(xa))
    assert tu.emit_to_string() == pat('!a;')
    tu = ef(b, b.expr_logical_not(b.expr_logical_not(xa)))
    assert tu.emit_to_string() == pat('!!a;')

    tu = ef(b, b.expr_sizeof_expr(xa))
    assert tu.emit_to_string() == pat('sizeof(a);')
    tu = ef(b, b.expr_sizeof_expr(b.expr_sizeof_expr(xa)))
    assert tu.emit_to_string() == pat('sizeof(sizeof(a));')

    tu = ef(b, b.expr_sizeof_type(ta))
    assert tu.emit_to_string() == pat('sizeof(A);')

    tu = ef(b, b.expr_cast(ta, xa))
    assert tu.emit_to_string() == pat('(A)a;')

    tu = ef(b, b.expr_times(xa, xb))
    assert tu.emit_to_string() == pat('a * b;')
    tu = ef(b, b.expr_times(b.expr_times(xa, xb), xc))
    assert tu.emit_to_string() == pat('a * b * c;')
    tu = ef(b, b.expr_times(xa, b.expr_times(xb, xc)))
    assert tu.emit_to_string() == pat('a * (b * c);')

    tu = ef(b, b.expr_divide(xa, xb))
    assert tu.emit_to_string() == pat('a / b;')
    tu = ef(b, b.expr_divide(b.expr_divide(xa, xb), xc))
    assert tu.emit_to_string() == pat('a / b / c;')
    tu = ef(b, b.expr_divide(xa, b.expr_divide(xb, xc)))
    assert tu.emit_to_string() == pat('a / (b / c);')

    tu = ef(b, b.expr_modulus(xa, xb))
    assert tu.emit_to_string() == pat('a % b;')
    tu = ef(b, b.expr_modulus(b.expr_modulus(xa, xb), xc))
    assert tu.emit_to_string() == pat('a % b % c;')
    tu = ef(b, b.expr_modulus(xa, b.expr_modulus(xb, xc)))
    assert tu.emit_to_string() == pat('a % (b % c);')

    tu = ef(b, b.expr_plus(xa, xb))
    assert tu.emit_to_string() == pat('a + b;')
    tu = ef(b, b.expr_plus(b.expr_plus(xa, xb), xc))
    assert tu.emit_to_string() == pat('a + b + c;')
    tu = ef(b, b.expr_plus(xa, b.expr_plus(xb, xc)))
    assert tu.emit_to_string() == pat('a + (b + c);')

    tu = ef(b, b.expr_minus(xa, xb))
    assert tu.emit_to_string() == pat('a - b;')
    tu = ef(b, b.expr_minus(b.expr_minus(xa, xb), xc))
    assert tu.emit_to_string() == pat('a - b - c;')
    tu = ef(b, b.expr_minus(xa, b.expr_minus(xb, xc)))
    assert tu.emit_to_string() == pat('a - (b - c);')

    tu = ef(b, b.expr_lshift(xa, xb))
    assert tu.emit_to_string() == pat('a << b;')
    tu = ef(b, b.expr_lshift(b.expr_lshift(xa, xb), xc))
    assert tu.emit_to_string() == pat('a << b << c;')
    tu = ef(b, b.expr_lshift(xa, b.expr_lshift(xb, xc)))
    assert tu.emit_to_string() == pat('a << (b << c);')

    tu = ef(b, b.expr_rshift(xa, xb))
    assert tu.emit_to_string() == pat('a >> b;')
    tu = ef(b, b.expr_rshift(b.expr_rshift(xa, xb), xc))
    assert tu.emit_to_string() == pat('a >> b >> c;')
    tu = ef(b, b.expr_rshift(xa, b.expr_rshift(xb, xc)))
    assert tu.emit_to_string() == pat('a >> (b >> c);')

    tu = ef(b, b.expr_cmp_lt(xa, xb))
    assert tu.emit_to_string() == pat('a < b;')
    tu = ef(b, b.expr_cmp_lt(b.expr_cmp_lt(xa, xb), xc))
    assert tu.emit_to_string() == pat('a < b < c;')
    tu = ef(b, b.expr_cmp_lt(xa, b.expr_cmp_lt(xb, xc)))
    assert tu.emit_to_string() == pat('a < (b < c);')

    tu = ef(b, b.expr_cmp_gt(xa, xb))
    assert tu.emit_to_string() == pat('a > b;')
    tu = ef(b, b.expr_cmp_gt(b.expr_cmp_gt(xa, xb), xc))
    assert tu.emit_to_string() == pat('a > b > c;')
    tu = ef(b, b.expr_cmp_gt(xa, b.expr_cmp_gt(xb, xc)))
    assert tu.emit_to_string() == pat('a > (b > c);')

    tu = ef(b, b.expr_cmp_le(xa, xb))
    assert tu.emit_to_string() == pat('a <= b;')
    tu = ef(b, b.expr_cmp_le(b.expr_cmp_le(xa, xb), xc))
    assert tu.emit_to_string() == pat('a <= b <= c;')
    tu = ef(b, b.expr_cmp_le(xa, b.expr_cmp_le(xb, xc)))
    assert tu.emit_to_string() == pat('a <= (b <= c);')

    tu = ef(b, b.expr_cmp_ge(xa, xb))
    assert tu.emit_to_string() == pat('a >= b;')
    tu = ef(b, b.expr_cmp_ge(b.expr_cmp_ge(xa, xb), xc))
    assert tu.emit_to_string() == pat('a >= b >= c;')
    tu = ef(b, b.expr_cmp_ge(xa, b.expr_cmp_ge(xb, xc)))
    assert tu.emit_to_string() == pat('a >= (b >= c);')

    tu = ef(b, b.expr_cmp_eq(xa, xb))
    assert tu.emit_to_string() == pat('a == b;')
    tu = ef(b, b.expr_cmp_eq(b.expr_cmp_eq(xa, xb), xc))
    assert tu.emit_to_string() == pat('a == b == c;')
    tu = ef(b, b.expr_cmp_eq(xa, b.expr_cmp_eq(xb, xc)))
    assert tu.emit_to_string() == pat('a == (b == c);')

    tu = ef(b, b.expr_cmp_ne(xa, xb))
    assert tu.emit_to_string() == pat('a != b;')
    tu = ef(b, b.expr_cmp_ne(b.expr_cmp_ne(xa, xb), xc))
    assert tu.emit_to_string() == pat('a != b != c;')
    tu = ef(b, b.expr_cmp_ne(xa, b.expr_cmp_ne(xb, xc)))
    assert tu.emit_to_string() == pat('a != (b != c);')

    tu = ef(b, b.expr_bitwise_and(xa, xb))
    assert tu.emit_to_string() == pat('a & b;')
    tu = ef(b, b.expr_bitwise_and(b.expr_bitwise_and(xa, xb), xc))
    assert tu.emit_to_string() == pat('a & b & c;')
    tu = ef(b, b.expr_bitwise_and(xa, b.expr_bitwise_and(xb, xc)))
    assert tu.emit_to_string() == pat('a & (b & c);')

    tu = ef(b, b.expr_bitwise_xor(xa, xb))
    assert tu.emit_to_string() == pat('a ^ b;')
    tu = ef(b, b.expr_bitwise_xor(b.expr_bitwise_xor(xa, xb), xc))
    assert tu.emit_to_string() == pat('a ^ b ^ c;')
    tu = ef(b, b.expr_bitwise_xor(xa, b.expr_bitwise_xor(xb, xc)))
    assert tu.emit_to_string() == pat('a ^ (b ^ c);')

    tu = ef(b, b.expr_bitwise_or(xa, xb))
    assert tu.emit_to_string() == pat('a | b;')
    tu = ef(b, b.expr_bitwise_or(b.expr_bitwise_or(xa, xb), xc))
    assert tu.emit_to_string() == pat('a | b | c;')
    tu = ef(b, b.expr_bitwise_or(xa, b.expr_bitwise_or(xb, xc)))
    assert tu.emit_to_string() == pat('a | (b | c);')

    tu = ef(b, b.expr_logical_and(xa, xb))
    assert tu.emit_to_string() == pat('a && b;')
    tu = ef(b, b.expr_logical_and(b.expr_logical_and(xa, xb), xc))
    assert tu.emit_to_string() == pat('a && b && c;')
    tu = ef(b, b.expr_logical_and(xa, b.expr_logical_and(xb, xc)))
    assert tu.emit_to_string() == pat('a && (b && c);')

    tu = ef(b, b.expr_logical_or(xa, xb))
    assert tu.emit_to_string() == pat('a || b;')
    tu = ef(b, b.expr_logical_or(b.expr_logical_or(xa, xb), xc))
    assert tu.emit_to_string() == pat('a || b || c;')
    tu = ef(b, b.expr_logical_or(xa, b.expr_logical_or(xb, xc)))
    assert tu.emit_to_string() == pat('a || (b || c);')

    tu = ef(b, b.expr_conditional(xa, xb, xc))
    assert tu.emit_to_string() == pat('a ? b : c;')
    tu = ef(b, b.expr_conditional(b.expr_conditional(xa, xb, xc), xd, xe))
    assert tu.emit_to_string() == pat('(a ? b : c) ? d : e;')
    tu = ef(b, b.expr_conditional(xa, b.expr_conditional(xb, xc, xd), xe))
    assert tu.emit_to_string() == pat('a ? b ? c : d : e;')
    tu = ef(b, b.expr_conditional(xa, xb, b.expr_conditional(xc, xd, xe)))
    assert tu.emit_to_string() == pat('a ? b : c ? d : e;')

    tu = ef(b, b.expr_assign(xa, xb))
    assert tu.emit_to_string() == pat('a = b;')
    tu = ef(b, b.expr_assign(b.expr_assign(xa, xb), xc))
    assert tu.emit_to_string() == pat('(a = b) = c;')
    tu = ef(b, b.expr_assign(xa, b.expr_assign(xb, xc)))
    assert tu.emit_to_string() == pat('a = b = c;')

    tu = ef(b, b.expr_assign_times(xa, xb))
    assert tu.emit_to_string() == pat('a *= b;')
    tu = ef(b, b.expr_assign_times(b.expr_assign_times(xa, xb), xc))
    assert tu.emit_to_string() == pat('(a *= b) *= c;')
    tu = ef(b, b.expr_assign_times(xa, b.expr_assign_times(xb, xc)))
    assert tu.emit_to_string() == pat('a *= b *= c;')

    tu = ef(b, b.expr_assign_divide(xa, xb))
    assert tu.emit_to_string() == pat('a /= b;')
    tu = ef(b, b.expr_assign_divide(b.expr_assign_divide(xa, xb), xc))
    assert tu.emit_to_string() == pat('(a /= b) /= c;')
    tu = ef(b, b.expr_assign_divide(xa, b.expr_assign_divide(xb, xc)))
    assert tu.emit_to_string() == pat('a /= b /= c;')

    tu = ef(b, b.expr_assign_modulus(xa, xb))
    assert tu.emit_to_string() == pat('a %= b;')
    tu = ef(b, b.expr_assign_modulus(b.expr_assign_modulus(xa, xb), xc))
    assert tu.emit_to_string() == pat('(a %= b) %= c;')
    tu = ef(b, b.expr_assign_modulus(xa, b.expr_assign_modulus(xb, xc)))
    assert tu.emit_to_string() == pat('a %= b %= c;')

    tu = ef(b, b.expr_assign_plus(xa, xb))
    assert tu.emit_to_string() == pat('a += b;')
    tu = ef(b, b.expr_assign_plus(b.expr_assign_plus(xa, xb), xc))
    assert tu.emit_to_string() == pat('(a += b) += c;')
    tu = ef(b, b.expr_assign_plus(xa, b.expr_assign_plus(xb, xc)))
    assert tu.emit_to_string() == pat('a += b += c;')

    tu = ef(b, b.expr_assign_minus(xa, xb))
    assert tu.emit_to_string() == pat('a -= b;')
    tu = ef(b, b.expr_assign_minus(b.expr_assign_minus(xa, xb), xc))
    assert tu.emit_to_string() == pat('(a -= b) -= c;')
    tu = ef(b, b.expr_assign_minus(xa, b.expr_assign_minus(xb, xc)))
    assert tu.emit_to_string() == pat('a -= b -= c;')

    tu = ef(b, b.expr_assign_lshift(xa, xb))
    assert tu.emit_to_string() == pat('a <<= b;')
    tu = ef(b, b.expr_assign_lshift(b.expr_assign_lshift(xa, xb), xc))
    assert tu.emit_to_string() == pat('(a <<= b) <<= c;')
    tu = ef(b, b.expr_assign_lshift(xa, b.expr_assign_lshift(xb, xc)))
    assert tu.emit_to_string() == pat('a <<= b <<= c;')

    tu = ef(b, b.expr_assign_rshift(xa, xb))
    assert tu.emit_to_string() == pat('a >>= b;')
    tu = ef(b, b.expr_assign_rshift(b.expr_assign_rshift(xa, xb), xc))
    assert tu.emit_to_string() == pat('(a >>= b) >>= c;')
    tu = ef(b, b.expr_assign_rshift(xa, b.expr_assign_rshift(xb, xc)))
    assert tu.emit_to_string() == pat('a >>= b >>= c;')

    tu = ef(b, b.expr_assign_bitwise_and(xa, xb))
    assert tu.emit_to_string() == pat('a &= b;')
    tu = ef(b, b.expr_assign_bitwise_and(b.expr_assign_bitwise_and(xa, xb), xc))
    assert tu.emit_to_string() == pat('(a &= b) &= c;')
    tu = ef(b, b.expr_assign_bitwise_and(xa, b.expr_assign_bitwise_and(xb, xc)))
    assert tu.emit_to_string() == pat('a &= b &= c;')

    tu = ef(b, b.expr_assign_bitwise_xor(xa, xb))
    assert tu.emit_to_string() == pat('a ^= b;')
    tu = ef(b, b.expr_assign_bitwise_xor(b.expr_assign_bitwise_xor(xa, xb), xc))
    assert tu.emit_to_string() == pat('(a ^= b) ^= c;')
    tu = ef(b, b.expr_assign_bitwise_xor(xa, b.expr_assign_bitwise_xor(xb, xc)))
    assert tu.emit_to_string() == pat('a ^= b ^= c;')

    tu = ef(b, b.expr_assign_bitwise_or(xa, xb))
    assert tu.emit_to_string() == pat('a |= b;')
    tu = ef(b, b.expr_assign_bitwise_or(b.expr_assign_bitwise_or(xa, xb), xc))
    assert tu.emit_to_string() == pat('(a |= b) |= c;')
    tu = ef(b, b.expr_assign_bitwise_or(xa, b.expr_assign_bitwise_or(xb, xc)))
    assert tu.emit_to_string() == pat('a |= b |= c;')

    tu = ef(b, b.expr_comma(xa, xb))
    assert tu.emit_to_string() == pat('a, b;')
    tu = ef(b, b.expr_comma(b.expr_comma(xa, xb), xc))
    assert tu.emit_to_string() == pat('a, b, c;')
    tu = ef(b, b.expr_comma(xa, b.expr_comma(xb, xc)))
    assert tu.emit_to_string() == pat('a, (b, c);')


if __name__ == '__main__':
    print('Note: assert rewriting may be slow the first time.')
    __import__('sys').exit(pytest.main())
