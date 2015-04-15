#!/usr/bin/env python3

# This grammar is only intended to emit code.
#
# This grammar has been modified slightly:
# - to eliminate deprecated features, such as `auto x;` and `f(x) int x; {}`
# - to eliminate silly things such as `long int long`
# - to allow compiler extensions such as `inline` and `__attribute__`
# - to generally simplify the number of node types

import sys

syms_to_tags = dict()
asts = dict()
used_syms = set()
# remove this, instead do late ident lookups for output only


class Tag(object):
    __slots__ = ('spaced', 'lower', 'upper', 'camel')

    def __init__(self, spaced):
        assert spaced.replace(' ', '').islower()
        self.spaced = spaced
        self.lower = spaced.replace(' ', '_')
        self.upper = self.lower.upper()
        self.camel = spaced.title().replace(' ', '')

class Ast(object):
    __slots__ = ('sym', 'tag')

    def __new__(cls, *args, **kwargs):
        # lots of stuff assumes there are no more subclasses
        assert cls in {Token, Alternative, Sequence, Option}, repr(cls)
        return object.__new__(cls)

class ConcreteAst(Ast):
    __slots__ = ()

class VirtualAst(Ast):
    __slots__ = ()

class Token(ConcreteAst):
    __slots__ = ()

    def __init__(self, sym, tag):
        self.sym = sym
        self.tag = tag

class Alternative(VirtualAst):
    __slots__ = ('bits',)

    def __init__(self, sym, tag, bits):
        self.sym = sym
        self.tag = tag
        self.bits = bits

class Sequence(ConcreteAst):
    __slots__ = ('bits',)

    def __init__(self, sym, tag, bits):
        self.sym = sym
        self.tag = tag
        self.bits = bits

class Option(VirtualAst):
    __slots__ = ('child',)

    def __init__(self, sym, tag, child):
        self.sym = sym
        self.tag = tag
        self.child = child


def check_sym(sym):
    assert isinstance(sym, str), repr(sym)
    assert sym not in syms_to_tags, repr(sym)
    assert isinstance(sym, str) and sym and '-' not in sym.strip('-'), repr(sym)

def check_tag(tag):
    assert isinstance(tag, Tag), repr(tag)
    assert tag.spaced not in asts, repr(tag.spaced)

def insert(obj):
    assert isinstance(obj, Ast), repr(obj)
    check_sym(obj.sym)
    check_tag(obj.tag)
    syms_to_tags[obj.sym] = obj.tag.spaced
    asts[obj.tag.spaced] = obj

# tag might not be available yet
def note_sym(sym):
    assert isinstance(sym, str)
    used_syms.add(sym)

def tok(sym):
    return tok2(sym, sym)

def tok2(sym, tag):
    assert isinstance(sym, str), repr(sym)
    assert isinstance(tag, str), repr(tag)
    tag = Tag('tok %s' % tag)
    insert(Token(sym, tag))
    return sym

def alt(sym, bits):
    assert isinstance(bits, list), repr(bits)
    assert len(bits) > 1, repr(bits)
    for b in bits:
        note_sym(b)
        assert not b.startswith('opt ')
    tag = Tag('any %s' % sym)
    insert(Alternative(sym, tag, bits))
    return sym

def seq(sym, bits):
    assert isinstance(bits, list), repr(bits)
    assert len(bits) > 1, repr(bits)
    for b in bits:
        note_sym(b)
    assert not all(b.startswith('opt ') for b in bits)
    tag = Tag('tree %s' % sym)
    insert(Sequence(sym, tag, bits))
    return sym

def opt(child_sym):
    assert not child_sym.startswith('opt ')
    sym = 'opt %s' % child_sym
    child_tag = 'comma' if child_sym == ',' else child_sym
    tag = Tag('opt %s' % child_tag)
    if tag.spaced not in asts:
        insert(Option(sym, tag, child_sym))
        note_sym(child_sym)
    return sym


def split(sym):
    return seq(sym, sym.split(' '))


def define():
    #tok('auto')
    tok('break')
    tok('case')
    tok('char')
    tok('const')
    tok('continue')
    tok('default')
    tok('do')
    tok('double')
    tok('else')
    tok('enum')
    tok('extern')
    tok('float')
    tok('for')
    tok('goto')
    tok('if')
    tok('int')
    tok('long')
    tok('register')
    tok('return')
    tok('short')
    tok('signed')
    tok('sizeof')
    tok('static')
    tok('struct')
    tok('switch')
    tok('typedef')
    tok('union')
    tok('unsigned')
    tok('void')
    tok('volatile')
    tok('while')

    tok('identifier') # includes enumeration constant
    tok('typedef name') # really identifier
    # tok('constant') # char/integer/float only, any base
    tok('floating constant')
    # tok('integer constant')
    tok('decimal constant')
    tok('octal constant')
    tok('hexadecimal constant')
    tok('character constant')
    tok('string literal')

    tok2('[', 'lbracket')
    tok2(']', 'rbracket')
    tok2('(', 'lparen')
    tok2(')', 'rparen')
    tok2('{', 'lbrace')
    tok2('}', 'rbrace')
    tok2('.', 'dot')
    tok2('...', 'ellipsis')
    tok2('->', 'arrow')
    tok2('++', 'incr')
    tok2('--', 'decr')
    tok2('&', 'ampersand')
    tok2('*', 'star')
    tok2('+', 'plus')
    tok2('-', 'minus')
    tok2('~', 'tilde')
    tok2('!', 'bang')
    tok2('/', 'slash')
    tok2('%', 'percent')
    tok2('<<', 'lshift')
    tok2('>>', 'rshift')
    tok2('<', 'lt')
    tok2('>', 'gt')
    tok2('<=', 'le')
    tok2('>=', 'ge')
    tok2('==', 'eq')
    tok2('!=', 'ne')
    tok2('^', 'caret')
    tok2('|', 'pipe')
    tok2('&&', 'logical and')
    tok2('||', 'logical or')
    tok2('?', 'query')
    tok2(':', 'colon')
    tok2('=', 'assign')
    tok2('*=', 'star assign')
    tok2('/=', 'slash assign')
    tok2('%=', 'percent assign')
    tok2('+=', 'plus assign')
    tok2('-=', 'minus assign')
    tok2('<<=', 'lshift assign')
    tok2('>>=', 'rshift assign')
    tok2('&=', 'ampersand assign')
    tok2('^=', 'caret assign')
    tok2('|=', 'pipe assign')
    tok2(',', 'comma')
    tok2(';', 'semicolon')
    # tok2('#', '')
    # tok2('##', '')

    alt('constant', [
        'floating constant',
        'integer constant',
        'character constant',
    ])

    alt('integer constant', [
        'decimal constant',
        'octal constant',
        'hexadecimal constant',
    ])

    alt('primary expression', [
        'identifier',
        'constant',
        'string literal',
        seq('parenthesis expression', ['(', 'comma expression', ')']),
    ])

    alt('postfix expression', [
        'primary expression',
        seq('index expression', ['postfix expression', '[', 'comma expression', ']']),
        seq('call expression', ['postfix expression', '(', opt('comma expression'), ')']),
        seq('dot expression', ['postfix expression', '.', 'identifier']),
        seq('arrow expression', ['postfix expression', '->', 'identifier']),
        seq('post increment expression', ['postfix expression', '++']),
        seq('post decrement expression', ['postfix expression', '--']),
    ])

    alt('unary expression', [
        'postfix expression',
        seq('pre increment expression', ['++', 'unary expression']),
        seq('pre decrement expression', ['--', 'unary expression']),
        seq('addressof expression', ['&', 'unary expression']),
        seq('dereference expression', ['*', 'unary expression']),
        seq('unary plus expression', ['+', 'unary expression']),
        seq('unary minus expression', ['-', 'unary expression']),
        seq('bitwise not expression', ['~', 'unary expression']),
        seq('logical not expression', ['!', 'unary expression']),
        seq('sizeof expression expression', ['sizeof', 'unary expression']),
        seq('sizeof type expression', ['sizeof', '(', 'type name', ')']),
        seq('cast expression', ['(', 'type name', ')', 'unary expression']),
    ])

    alt('multiplicative expression', [
        'unary expression',
        seq('times expression', ['multiplicative expression', '*', 'unary expression']),
        seq('divide expression', ['multiplicative expression', '/', 'unary expression']),
        seq('modulus expression', ['multiplicative expression', '%', 'unary expression']),
    ])

    alt('additive expression', [
        'multiplicative expression',
        seq('plus expression', ['additive expression', '+', 'multiplicative expression']),
        seq('minus expression', ['additive expression', '-', 'multiplicative expression']),
    ])

    alt('shift expression', [
        'additive expression',
        seq('left shift expression', ['shift expression', '<<', 'additive expression']),
        seq('right shift expression', ['shift expression', '>>', 'additive expression']),
    ])

    alt('relational expression', [
        'shift expression',
        seq('compare lt expression', ['relational expression', '<', 'shift expression']),
        seq('compare gt expression', ['relational expression', '>', 'shift expression']),
        seq('compare le expression', ['relational expression', '<=', 'shift expression']),
        seq('compare ge expression', ['relational expression', '>=', 'shift expression']),
    ])

    alt('equality expression', [
        'relational expression',
        seq('compare eq expression', ['equality expression', '==', 'relational expression']),
        seq('compare ne expression', ['equality expression', '!=', 'relational expression']),
    ])

    alt('and expression', [
        'equality expression',
        seq('bitwise and expression', ['and expression', '&', 'equality expression']),
    ])

    alt('exclusive or expression', [
        'and expression',
        seq('bitwise xor expression', ['exclusive or expression', '^', 'and expression']),
    ])

    alt('inclusive or expression', [
        'exclusive or expression',
        seq('bitwise or expression', ['inclusive or expression', '|', 'exclusive or expression']),
    ])

    alt('logical and expression', [
        'inclusive or expression',
        seq('binary and expression', ['logical and expression', '&&', 'inclusive or expression']),
    ])

    alt('logical or expression', [
        'logical and expression',
        seq('binary or expression', ['logical or expression', '||', 'logical and expression']),
    ])

    alt('conditional expression', [
        'logical or expression',
        seq('ternary expression', ['logical or expression', '?', 'comma expression', ':', 'conditional expression']),
    ])

    alt('assignment expression', [
        'conditional expression',
        seq('simple assignment', ['unary expression', '=', 'assignment expression']),
        seq('compound times assignment', ['unary expression', '*=', 'assignment expression']),
        seq('compound divide assignment', ['unary expression', '/=', 'assignment expression']),
        seq('compound modulus assignment', ['unary expression', '%=', 'assignment expression']),
        seq('compound plus assignment', ['unary expression', '+=', 'assignment expression']),
        seq('compound minus assignment', ['unary expression', '-=', 'assignment expression']),
        seq('compound left shift assignment', ['unary expression', '<<=', 'assignment expression']),
        seq('compound right shift assignment', ['unary expression', '>>=', 'assignment expression']),
        seq('compound bitwise and assignment', ['unary expression', '&=', 'assignment expression']),
        seq('compound bitwise xor assignment', ['unary expression', '^=', 'assignment expression']),
        seq('compound bitwise or assignment', ['unary expression', '|=', 'assignment expression']),
    ])

    alt('comma expression', [
        'assignment expression',
        seq('binary comma expression', ['comma expression', ',', 'assignment expression']),
    ])

    seq('declaration', [opt('storage class specifier'), 'specifier qualifier list', opt('init declarator list'), ';'])

    alt('init declarator list', [
        'init declarator',
        seq('multi init declarator list', ['init declarator list', 'init declarator']),
    ])

    alt('init declarator', [
        'declarator',
        seq('initialized declarator', ['declarator', '=', 'initializer']),
    ])

    alt('storage class specifier', [
        'typedef',
        'extern',
        'static',
        'register',
    ])

    alt('type specifier', [
        'void',
        'char',
        split('signed char'),
        split('unsigned char'),
        split('signed short int'),
        split('unsigned short int'),
        split('signed int'),
        split('unsigned int'),
        split('signed long int'),
        split('unsigned long int'),
        split('signed long long int'),
        split('unsigned long long int'),
        'float',
        'double',
        split('long double'),
        'struct or union specifier',
        'enum specifier',
        'typedef name',
    ])

    alt('struct or union specifier', [
        seq('full struct', ['struct or union', opt('identifier'), '{', 'struct declaration list', '}']),
        seq('forward struct', ['struct or union', 'identifier']),
    ])

    alt('struct or union', [
        'struct',
        'union',
    ])

    alt('struct declaration list', [
        'struct declaration',
        seq('multi struct declaration', ['struct declaration list', 'struct declaration']),
    ])

    seq('struct declaration', ['specifier qualifier list', 'struct declarator list', ';'])

    seq('specifier qualifier list', ['type specifier', opt('type qualifier list')])

    alt('struct declarator list', [
        'struct declarator',
        seq('multi struct declarator', ['struct declarator list', ',', 'struct declarator']),
    ])

    alt('struct declarator', [
        'declarator',
        seq('bitfield declarator', [opt('declarator'), ':', 'conditional expression']),
    ])

    alt('enum specifier', [
        seq('full enum', ['enum', opt('identifier'), '{', 'enumerator list', '}']),
        seq('forward enum', ['enum', 'identifier']),
    ])

    alt('enumerator list', [
        'enumerator',
        seq('multi enumerator list', ['enumerator list', ',', 'enumerator']),
    ])

    alt('enumerator', [
        'identifier',
        seq('initialized enumeration constant', ['identifier', '=', 'conditional expression']),
    ])

    alt('type qualifier', [
        'const',
        'volatile',
    ])

    alt('declarator', [
        'direct declarator',
        seq('pointer declarator', ['*', opt('type qualifier list'), 'declarator']),
    ])

    alt('direct declarator', [
        'identifier',
        seq('parenthesis declarator', ['(', 'declarator', ')']),
        seq('array declarator', ['direct declarator', '[', opt('conditional expression'), ']']),
        seq('function declarator', ['direct declarator', '(', opt('parameter type list'), ')']),
    ])

    alt('type qualifier list', [
        'type qualifier',
        seq('multi type qualifier list', ['type qualifier list', 'type qualifier']),
    ])

    alt('parameter type list', [
        'parameter list',
        seq('varargs parameter list', ['parameter list', ',', '...']),
    ])

    alt('parameter list', [
        'parameter declaration',
        seq('multi parameter list', ['parameter list', ',', 'parameter declaration']),
    ])

    alt('parameter declaration', [
        seq('named parameter', [opt('storage class specifier'), 'specifier qualifier list', 'declarator']),
        seq('unnamed parameter', [opt('storage class specifier'), 'specifier qualifier list', opt('abstract declarator')]),
    ])

    seq('type name', ['specifier qualifier list', opt('abstract declarator')])

    alt('abstract declarator', [
        'direct abstract declarator',
        seq('leaf pointer abstract declarator', ['*', opt('type qualifier list')]),
        seq('recursive pointer abstract declarator', ['*', opt('type qualifier list'), 'abstract declarator']),
    ])

    alt('direct abstract declarator', [
        seq('parenthesis abstract declarator', ['(', 'abstract declarator', ')']),
        seq('array abstract declarator', [opt('direct abstract declarator'), '[', opt('conditional expression'), ']']),
        seq('function abstract declarator', [opt('direct abstract declarator'), '(', opt('parameter type list'), ')']),
    ])

    alt('initializer', [
        'assignment expression',
        seq('list initializer', ['{', 'initializer list', opt(','), '}']),
    ])

    alt('initializer list', [
        'initializer',
        seq('multi initializer list', ['initializer list', ',', 'initializer']),
    ])

    alt('statement', [
        'labeled statement',
        'compound statement',
        'expression statement',
        'selection statement',
        'iteration statement',
        'jump statement',
    ])

    alt('labeled statement', [
        seq('label statement', ['identifier', ':', 'statement']),
        seq('case statement', ['case', 'conditional expression', ':', 'statement']),
        seq('default statement', ['default', ':', 'statement']),
    ])

    seq('compound statement', ['{', opt('declaration list'), opt('statement list'), '}'])

    alt('declaration list', [
        'declaration',
        seq('multi declaration list', ['declaration list', 'declaration']),
    ])

    alt('statement list', [
        'statement',
        seq('multi statement list', ['statement list', 'statement']),
    ])

    seq('expression statement', [opt('comma expression'), ';'])

    alt('selection statement', [
        seq('if statement', ['if', '(', 'comma expression', ')', 'statement']),
        seq('if else statement', ['if', '(', 'comma expression', ')', 'statement', 'else', 'statement']),
        seq('switch statement', ['switch', '(', 'comma expression', ')', 'statement']),
    ])

    alt('iteration statement', [
        seq('while statement', ['while', '(', 'comma expression', ')', 'statement']),
        seq('do while statement', ['do', 'statement', 'while', '(', 'comma expression', ')', ';']),
        seq('for statement', ['for', '(', opt('comma expression'), ';', opt('comma expression'), ';', opt('comma expression'), ')', 'statement']),
    ])

    alt('jump statement', [
        seq('goto statement', ['goto', 'identifier', ';']),
        seq('continue statement', ['continue', ';']),
        seq('break statement', ['break', ';']),
        seq('return statement', ['return', opt('comma expression'), ';']),
    ])

    alt('translation unit', [
        'external declaration',
        seq('multi translation unit', ['translation unit', 'external declaration']),
    ])

    alt('external declaration', [
        'function definition',
        'declaration',
    ])

    seq('function definition', [opt('storage class specifier'), 'specifier qualifier list', 'declarator', 'compound statement'])


def output(header, source):
    def h(*args, **kwargs):
        print(*args, file=header, **kwargs)
    def c(*args, **kwargs):
        print(*args, file=source, **kwargs)
    def s(sig):
        h(sig, ';', sep='')
        c(sig)

    ast_tags = set(asts)
    used_tags = {syms_to_tags[sym] for sym in used_syms}
    unused_tags = sorted(ast_tags - used_tags)
    undefined_tags = sorted(used_tags - ast_tags)
    if unused_tags:
        sys.exit('unused: %s' % ', '.join(unused_tags))
    if undefined_tags:
        sys.exit('undefined: %s' % ', '.join(undefined_tags))

    ast_pairs = sorted(asts.items())
    ast_values = [v for _, v in ast_pairs]

    h('/* Generated file, edit c89.py instead. */')
    h('#pragma once')
    h()
    c('/* Generated file, edit c89.py instead. */')
    c('#include "c89.h"')
    c()
    c('#include <assert.h>')
    h('#include <stdbool.h>')
    c('#include <stddef.h>')
    h('#include <stdio.h>')
    c('#include <string.h>')
    h()
    h('#include "fwd.h"')
    c('#include "pool.h"')
    h()
    h()
    c()
    c()
    h('typedef struct C89_Nothing C89_Nothing;')
    for ast in ast_values:
        h('typedef struct C89_{0} C89_{0};'.format(ast.tag.camel))
    h()
    c('/* Concrete types only. */')
    c('typedef enum C89_AstType C89_AstType;')
    c('enum C89_AstType')
    c('{')
    c('    C89_NOTHING,')
    for ast in ast_values:
        if isinstance(ast, ConcreteAst):
            c('    C89_%s,' % ast.tag.upper)
    c('};')
    c()
    c('struct C89_Ast')
    c('{')
    c('    C89_AstType type;')
    c('    size_t total_tokens;')
    c('    size_t num_children;')
    c('    union { char *raw; C89_Ast **children; };')
    c('};')
    c()
    c('static C89_Ast c89_nothing = {C89_NOTHING, 0, 0, {""}};')
    c()
    h('/* Construction. */')
    s('C89_Nothing *c89_create_nothing(Pool *pool)')
    c('{')
    c('    (void)pool;')
    c('    return (C89_Nothing *)&c89_nothing;')
    c('}')
    c('static C89_Ast *c89_create_terminal(Pool *pool, C89_AstType type, const char *raw)')
    c('{')
    c('    C89_Ast rv;')
    c('    memset(&rv, 0, sizeof(rv));')
    c('    rv.type = type;')
    c('    rv.total_tokens = 1;')
    c('    rv.num_children = 0;')
    c('    rv.raw = (char *)pool_intern_string(pool, raw);')
    c('    return (C89_Ast *)(const C89_Ast *)pool_intern(pool, &rv, sizeof(rv));')
    c('}')
    c('static C89_Ast *c89_create_nonterminal(Pool *pool, C89_AstType type, size_t nargs, C89_Ast **args)')
    c('{')
    c('    C89_Ast rv;')
    c('    size_t i;')
    c('    assert (nargs >= 2);')
    c('    memset(&rv, 0, sizeof(rv));')
    c('    rv.type = type;')
    c('    rv.total_tokens = args[0]->total_tokens;')
    c('    for (i = 1; i < nargs; ++i)')
    c('    {')
    c('        rv.total_tokens += args[i]->total_tokens;')
    c('    }')
    c('    rv.num_children = nargs;')
    c('    rv.children = (C89_Ast **)pool_intern(pool, args, nargs * sizeof(C89_Ast *));')
    c('    return (C89_Ast *)(const C89_Ast *)pool_intern(pool, &rv, sizeof(rv));')
    c('}')
    for ast in ast_values:
        if isinstance(ast, ConcreteAst):
            if isinstance(ast, Token):
                s('C89_%s *c89_create_%s(Pool *pool, const char *raw)' % (ast.tag.camel, ast.tag.lower))
                c('{')
                c('    return (C89_%s *)c89_create_terminal(pool, C89_%s, raw);' % (ast.tag.camel, ast.tag.upper))
                c('}')
            if isinstance(ast, Sequence):
                ba = [asts[syms_to_tags[b]] for b in ast.bits]
                # this is needed for e.g. tree_for_statement, which has 3 opt_comma_expression and 2 tok_semicolon
                bn = ['%s%d' % (b.tag.lower, 1 + ba[:i].count(b)) if ba.count(b) > 1 else b.tag.lower for i, b in enumerate(ba)]
                assert len(ba) == len(set(bn))
                s('C89_%s *c89_create_%s(Pool *pool, %s)' % (ast.tag.camel, ast.tag.lower, ', '.join('C89_%s *%s' % (b.tag.camel, n) for b, n in zip(ba, bn))))
                c('{')
                c('    C89_Ast *args[%d] = {%s};' % (len(bn), ', '.join('(C89_Ast *)%s' % n for n in bn)))
                for b, n in zip(ba, bn):
                    c('    assert (c89_is_%s((C89_Ast *)%s));' % (b.tag.lower, n))
                c('    return (C89_%s *)c89_create_nonterminal(pool, C89_%s, %d, args);' % (ast.tag.camel, ast.tag.upper, len(bn)))
                c('}')
    h()
    h('/* Type checking. */')
    s('bool c89_is_nothing(C89_Ast *ast)')
    c('{')
    c('    return ast->type == C89_NOTHING;')
    c('}')
    for ast in ast_values:
        s('bool c89_is_%s(C89_Ast *ast)' % ast.tag.lower)
        c('{')
        if isinstance(ast, ConcreteAst):
            c('    return ast->type == C89_%s;' % ast.tag.upper)
        if isinstance(ast, Option):
            c('    return c89_is_nothing(ast) || c89_is_%s(ast);' % asts[syms_to_tags[ast.child]].tag.lower)
        if isinstance(ast, Alternative):
            c('    return %s;' % (' || '.join('c89_is_%s(ast)' % asts[syms_to_tags[b]].tag.lower for b in ast.bits)))
        c('}')
    h()
    h('/* Output. */')
    c('typedef struct IndentationInfo IndentationInfo;')
    c('struct IndentationInfo')
    c('{')
    c('    size_t indentation;')
    c('    const char *prev;')
    c('};')
    c('static void c89_emit_impl_impl(const char *next, FILE *fp, IndentationInfo *info)')
    c('{')
    c('    const char *const prev = info->prev;')
    c('    if (!*next)')
    c('        return;')
    c('    if (strcmp(next, "}") == 0)')
    c('        info->indentation--;')
    c('    if (prev)')
    c('    {')
    c('        bool newline = false;')
    c('        bool space = true;')
    c('        newline |= strcmp(prev, ";") == 0;')
    c('        newline |= strcmp(next, "{") == 0;')
    c('        newline |= strcmp(prev, "{") == 0;')
    c('        newline |= strcmp(prev, "}") == 0;')
    c('        space &= strcmp(prev, "(") != 0;')
    c('        space &= strcmp(next, ")") != 0;')
    c('        space &= strcmp(next, ",") != 0;')
    c('        space &= strcmp(next, ";") != 0;')
    c('        if (newline)')
    c('        {')
    c('            size_t i;')
    c('            fputc(\'\\n\', fp);')
    c('            for (i = 0; i < info->indentation; ++i)')
    c('                fputs("    ", fp);')
    c('        }')
    c('        else if (space)')
    c('            fputc(\' \', fp);')
    c('    }')
    c('    if (strcmp(next, "{") == 0)')
    c('        info->indentation++;')
    c('    info->prev = next;')
    c('    fputs(next, fp);')
    c('}')
    c('static void c89_emit_impl(C89_Ast *ast, FILE *fp, IndentationInfo *info)')
    c('{')
    c('    size_t i;')
    c('    if (ast->num_children == 0)')
    c('    {')
    c('         c89_emit_impl_impl(ast->raw, fp, info);')
    c('    }')
    c('    for (i = 0; i < ast->num_children; ++i)')
    c('    {')
    c('        c89_emit_impl(ast->children[i], fp, info);')
    c('    }')
    c('}')
    s('void c89_emit(C89_Ast *ast, FILE *fp)')
    c('{')
    c('    IndentationInfo info;')
    c('    memset(&info, 0, sizeof(info));');
    c('    c89_emit_impl(ast, fp, &info);')
    c('    fputc(\'\\n\', fp);')
    c('}')

def main():
    define()
    with open('c89.h', 'w') as h, open('c89.c', 'w') as c:
        output(h, c)

if __name__ == '__main__':
    main()
