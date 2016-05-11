#   Copyright © 2016 Ben Longbons
#
#   This file is part of Nicate.
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Lesser General Public License as published
#   by the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.


from .core import Builder
from .util import b2u, isoctb, ishexb
from .scope import Scope


class Item:
    __slots__ = ()

    def _get_slots(self):
        for cls in type(self).mro()[-2::-1]:
            assert isinstance(cls.__slots__, tuple), 'class %s has __slots__ of type %s' % (cls.__name__, type(cls.__slots__).__name__)
            for slot in cls.__slots__:
                yield slot

    def __init__(self, *args):
        required_slots = []
        forbidden_slots = []
        for slot in self._get_slots():
            assert isinstance(slot, str)
            if slot.startswith('attr_'):
                forbidden_slots.append(slot)
            else:
                required_slots.append(slot)

        if len(required_slots) < len(args):
            raise TypeError('Too many arguments for %s: expect %d, got %d' % (type(self).__name__, len(required_slots), len(args)))
        if len(required_slots) > len(args):
            raise TypeError('Too few arguments for %s: expect %d, got %d' % (type(self).__name__,len(required_slots), len(args)))
        for slot, arg in zip(required_slots, args):
            assert isinstance(arg, (Item, type(None), str, int, list, Scope)), type(arg)
            setattr(self, slot, arg)
        for slot in forbidden_slots:
            setattr(self, slot, False)

class Type(Item):
    __slots__ = ()

    valid = True

    def accepts(self, other):
        return self.accepts_type(other.type) or self._accepts_value(other)

    def accepts_type(self, other):
        return self is other or self._accepts_type(other)

    def _accepts_type(self, other):
        return False

    def _accepts_value(self, val):
        return False

class Stmt(Item):
    __slots__ = ()

class Block(Stmt):
    __slots__ = ('scope', 'items')

class ExprStmt(Item):
    __slots__ = ('expr',)

class Expr(Item):
    __slots__ = ('type',)

class Builtin(Item):
    __slots__ = ()

# TODO some fields probably belong in subclasses
class Decl(Item):
    __slots__ = ('name', 'type', 'initializer', 'attr_extern', 'attr_static')

class FunDef(Item):
    __slots__ = ('name', 'type', 'arg_decls', 'body', 'attr_extern', 'attr_static')

class Comment(Item):
    __slots__ = ('rambling',)


class Arg(Decl):
    __slots__ = ()

class Field(Decl):
    __slots__ = ()

class Enumerator(Decl):
    __slots__ = ()


class QualifiedType(Type):
    __slots__ = ('unqualified_type', 'attr_const',)

    def _dup(self):
        rv = object.__new__(QualifiedType)
        for slot in self._get_slots():
            setattr(rv, slot, getattr(self, slot))

    @staticmethod
    def get_const(ty):
        if isinstance(ty, QualifiedType):
            if ty.attr_const:
                return ty
            rv = ty._dup()
        else:
            assert ty.valid
            rv = QualifiedType(ty)
        rv.attr_const = True
        return rv

    def _accepts_type(self, other):
        if self is other:
            return True
        if isinstance(other, QualifiedType):
            return self.unqualified_type.accepts_type(other.unqualified_type) and self.attr_const > other.attr_const
        return self.unqualified_type.accepts_type(other)


class VoidType(Type):
    __slots__ = ()

    valid = False

class VaListType(Type):
    __slots__ = ()

class BoolType(Type):
    __slots__ = ()

class IntType(Type):
    # TODO attributes for bitfields? Or a subtype?
    __slots__ = ('underlying',)

    def _accepts_type(self, other):
        return isinstance(other, IntLitType)

class FloatType(Type):
    __slots__ = ('underlying',)

class CharType(Type):
    __slots__ = ()

class StringLitType(Type):
    __slots__ = ()

    valid = False

class IntLitType(Type):
    __slots__ = ()

    valid = False

class ArrayLitType(Type):
    __slots__ = ('elem_types',)

    valid = False

class NullType(Type):
    __slots__ = ()

    valid = False

class PointerType(Type):
    __slots__ = ('elem',)

    def __repr__(self):
        return 'PointerType(%r)' % self.elem

    def _accepts_type(self, other):
        if isinstance(other, StringLitType):
            return isinstance(self.elem, QualifiedType) and self.elem.attr_const and isinstance(self.elem.unqualified_type, CharType)
        return isinstance(other, NullType) or (isinstance(other, PointerType) and self.elem.accepts_type(other.elem))

class ArrayType(Type):
    __slots__ = ('elem', 'size')

    @property
    def valid(self):
        return self.size is not None

    def _accepts_type(self, other):
        if isinstance(other, StringLitType):
            elem = self.elem
            if isinstance(elem, QualifiedType):
                elem = elem.unqualified_type
            return isinstance(elem, CharType)
        if isinstance(other, ArrayLitType):
            elem = self.elem
            return all([elem.accepts_type(t) for t in other.elem_types])
        return False

class FunctionType(Type):
    __slots__ = ('arg_types', 'variadic', 'ret_type')

class StructType(Type):
    __slots__ = ('name', 'body')

class UnionType(Type):
    __slots__ = ('name', 'body')

class EnumType(Type):
    __slots__ = ('name', 'values')

class LabelType(Type):
    __slots__ = ()


class ReturnStmt(Stmt):
    __slots__ = ('value', 'type')

class GotoStmt(Stmt):
    __slots__ = ('target',)

class Loop(Stmt):
    __slots__ = ('block',)

class Switch(Stmt):
    __slots__ = ('val', 'cases', 'default')

class Branch(Stmt):
    __slots__ = ('cond', 'if_true', 'if_false')


class Lit(Expr):
    __slots__ = ()

class BoolLit(Lit):
    __slots__ = ('val',)

class IntLit(Lit):
    __slots__ = ('val',)

class FloatLit(Lit):
    __slots__ = ('val',)

class CharLit(Lit):
    __slots__ = ('val',)

class StringLit(Lit):
    __slots__ = ('val',)

class NullLit(Lit):
    __slots__ = ()

class UndefLit(Lit):
    __slots__ = ()

class ArrayLit(Lit):
    __slots__ = ('elems',)

class StructLit(Lit):
    __slots__ = ('elems',)

class UnionLit(Lit):
    __slots__ = ('field', 'val')

# used for objects, functions, and labels
class IdRef(Expr):
    __slots__ = ('decl',)

class Assign(Expr):
    __slots__ = ('left', 'right')

class BinaryExpr(Expr):
    __slots__ = ('left', 'right')

class UnaryExpr(Expr):
    __slots__ = ('right',)

class PostfixExpr(Expr):
    __slots__ = ('left',)


class SimpleAssign(Assign):
    __slots__ = ()

class CompoundAssign(Assign):
    __slots__ = ()

class TimesAssign(Assign):
    __slots__ = ()

class DivideAssign(Assign):
    __slots__ = ()

class ModuloAssign(Assign):
    __slots__ = ()

class PlusAssign(Assign):
    __slots__ = ()

class MinusAssign(Assign):
    __slots__ = ()

class LeftShiftAssign(Assign):
    __slots__ = ()

class RightShiftAssign(Assign):
    __slots__ = ()

class BitAndAssign(Assign):
    __slots__ = ()

class BitXorAssign(Assign):
    __slots__ = ()

class BitIorAssign(Assign):
    __slots__ = ()

class TimesExpr(BinaryExpr):
    __slots__ = ()

class DivideExpr(BinaryExpr):
    __slots__ = ()

class ModuloExpr(BinaryExpr):
    __slots__ = ()

class PlusExpr(BinaryExpr):
    __slots__ = ()

class MinusExpr(BinaryExpr):
    __slots__ = ()

class LeftShiftExpr(BinaryExpr):
    __slots__ = ()

class LtExpr(BinaryExpr):
    __slots__ = ()

class GtExpr(BinaryExpr):
    __slots__ = ()

class LeExpr(BinaryExpr):
    __slots__ = ()

class GeExpr(BinaryExpr):
    __slots__ = ()

class EqExpr(BinaryExpr):
    __slots__ = ()

class NeExpr(BinaryExpr):
    __slots__ = ()

class BitAndExpr(BinaryExpr):
    __slots__ = ()

class BitXorExpr(BinaryExpr):
    __slots__ = ()

class BitIorExpr(BinaryExpr):
    __slots__ = ()

class LogAndExpr(BinaryExpr):
    __slots__ = ()

class LogOrExpr(BinaryExpr):
    __slots__ = ()

class PreIncrExpr(UnaryExpr):
    __slots__ = ()

class PreDecrExpr(UnaryExpr):
    __slots__ = ()

class AddressofExpr(UnaryExpr):
    __slots__ = ()

class UnaryPlusExpr(UnaryExpr):
    __slots__ = ()

class UnaryMinusExpr(UnaryExpr):
    __slots__ = ()

class BitNotExpr(UnaryExpr):
    __slots__ = ()

class LogNotExpr(UnaryExpr):
    __slots__ = ()

class IndexExpr(PostfixExpr):
    __slots__ = ('indices',)

class CallExpr(PostfixExpr):
    __slots__ = ('args',)

class MemberExpr(PostfixExpr):
    __slots__ = ()

class PostIncrExpr(PostfixExpr):
    __slots__ = ()

class PostDecrExpr(PostfixExpr):
    __slots__ = ()


class PointerBuiltin(Builtin):
    __slots__ = ()

    def builtin_index(self, elem):
        assert isinstance(elem, Type), elem
        assert elem.valid
        return PointerType(elem)

class ArrayBuiltin(Builtin):
    __slots__ = ()

    def builtin_index(self, elem, size=None):
        assert isinstance(elem, Type), elem
        assert elem.valid
        if size is not None:
            assert isinstance(size, Expr)
        return ArrayType(elem, size)

class ArraySizeBuiltin(Builtin):
    __slots__ = ()

    def builtin_call(self, arr):
        rv = arr.type.size
        assert rv is not None, 'array type incomplete, has no size'
        return rv


def builtins():
    rv = Scope(parent=None, name='<builtins>')

    rv.put('ptr', PointerBuiltin())
    rv.put('array', ArrayBuiltin())
    rv.put('array_size', ArraySizeBuiltin())

    rv.put('void', VoidType())
    rv.put('va_list', VaListType())
    rv.put('null', NullLit(NullType()))
    rv.put('bool', BoolType())
    rv.put('false', BoolLit(rv.get('bool'), False))
    rv.put('true', BoolLit(rv.get('bool'), True))
    rv.put('char', CharType())

    rv.put('string literal', StringLitType())
    rv.put('int literal', IntLitType())

    rv.put('c_schar', IntType('signed char'))
    rv.put('c_uchar', IntType('unsigned char'))
    rv.put('c_sshort', IntType('signed short'))
    rv.put('c_ushort', IntType('unsigned short'))
    rv.put('c_sint', IntType('signed int'))
    rv.put('c_uint', IntType('unsigned int'))
    rv.put('c_slong', IntType('signed long'))
    rv.put('c_ulong', IntType('unsigned long'))
    rv.put('c_sllong', IntType('signed long long'))
    rv.put('c_ullong', IntType('unsigned long long'))
    rv.put('ptrdiff_t', IntType('ptrdiff_t'))
    rv.put('size_t', IntType('size_t'))
    rv.put('sintptr_t', IntType('intptr_t'))
    rv.put('uintptr_t', IntType('uintptr_t'))
    rv.put('sint8_t', IntType('int8_t'))
    rv.put('uint8_t', IntType('uint8_t'))
    rv.put('sint16_t', IntType('int16_t'))
    rv.put('uint16_t', IntType('uint16_t'))
    rv.put('sint32_t', IntType('int32_t'))
    rv.put('uint32_t', IntType('uint32_t'))
    rv.put('sint64_t', IntType('int64_t'))
    rv.put('uint64_t', IntType('uint64_t'))
    rv.put('sintmax_t', IntType('intmax_t'))
    rv.put('uintmax_t', IntType('uintmax_t'))
    rv.put('float', FloatType('float'))
    rv.put('double', FloatType('double'))
    rv.put('long_double', FloatType('long double'))

    return rv


class AbstractVisitor:
    __slots__ = ('_modes',)

    def __init__(self):
        self._modes = ['top']

    def _mode(self, mode):
        self._modes.append(mode)

    def visit(self, node, *, mode=None, **kwargs):
        num_modes = len(self._modes)
        if mode is not None:
            self._mode(mode)
        mfun = 'visit_%s_%s' % (self._modes[-1], type(node).__name__)
        fun = getattr(self, mfun)
        rv = fun(node, **kwargs)
        assert num_modes <= len(self._modes)
        del self._modes[num_modes:]
        return rv

class AstCreator(AbstractVisitor):
    __slots__ = ('_parser',)

    def __init__(self, parser):
        super().__init__()
        self._parser = parser

    def flatten2(self, node):
        # Until + and * get first-class support, flatten manually.
        children = []
        T = type(node)
        while not isinstance(node, self._parser.Nothing):
            node, child = node.children
            children.append(child)
        children.reverse()
        return children

    def flatten3(self, node):
        # Like flatten2, but drop the (probably comma) slave separator.
        children = []
        T = type(node)
        while not isinstance(node, self._parser.Nothing):
            node, sep, child = node.children
            assert isinstance(node, self._parser.Nothing) == isinstance(sep, self._parser.Nothing)
            children.append(child)
        children.reverse()
        return children

    def visit_top_TreeTu(self, node):
        children = self.flatten2(node)
        scope = Scope(parent=builtins(), name='<globals>')
        ret = None
        items = []
        # TODO create implicit forward declarations of structs/functions here.
        for child in children:
            item = self.visit(child, mode='block', scope=scope, ret=ret)
            if item is None:
                continue
            if isinstance(item, (Decl, FunDef, StructType, UnionType, EnumType)):
                scope.put(item.name, item)
            items.append(item)
        return Block(scope, items)

    def visit_top_Nothing(self, node):
        return self.visit_top_TreeTu(node)

    def visit_block_AtomComment(self, node, *, scope, ret):
        return Comment(b2u(node.data[1:].strip()))

    def visit_block_TreeTagStructStmt(self, node, *, scope, ret):
        _, name, body = node.children
        name = b2u(name.data)
        body = self.visit(body, mode='struct', scope=scope, ret=ret)
        return StructType(name, body)

    def visit_struct_SymSemicolon(self, node, scope, ret):
        return None

    def visit_block_TreeTagAnnotatedStmt(self, node, *, scope, ret):
        attr, stmt = node.children
        _, attr, _, args, _ = attr.children
        attr = b2u(attr.data)
        args = self.flatten3(args)
        args = [self.visit(a, scope=scope, ret=ret) for a in args]
        stmt = self.visit(stmt, scope=scope, ret=ret)
        if not hasattr(stmt, 'attr_' + attr):
            raise ValueError('Invalid @%s for %s' % (attr, stmt))
        if getattr(stmt, 'attr_' + attr):
            raise ValueError('Duplicate @%s for %s' % (attr, stmt))
        setattr(stmt, 'attr_' + attr, True)
        return stmt

    def visit_block_TreeTagDeclaration(self, node, *, scope, ret):
        _, typed_id, _ = node.children
        name, _, type = typed_id.children
        name = b2u(name.data)
        type = self.visit(type, mode='expr', scope=scope, ret=ret)

        assert type.valid

        return Decl(name, type, None)

    def visit_expr_TreeTagIndexExpr(self, node, *, scope, ret):
        left, _, indices, _ = node.children
        left = self.visit(left, scope=scope, ret=ret)
        indices = self.flatten3(indices)
        indices = [self.visit(child, scope=scope, ret=ret) for child in indices]

        if isinstance(left, Builtin):
            return left.builtin_index(*indices)
        assert len(indices) <= 1, 'multiple indices not supported yet'
        if indices:
            assert isinstance(indices[0].type, (IntType, IntLitType)), 'index must be integer'
        if isinstance(left.type, (ArrayType, PointerType)):
            type = left.type.elem
        else:
            raise AttributeError('Cannot index %s' % left)
        return IndexExpr(type, left, indices)

    def visit_expr_AtomId(self, node, *, scope, ret):
        id = b2u(node.data)
        decl = scope.get(id)
        if isinstance(decl, (Builtin, Type, Lit)):
            return decl
        return IdRef(decl.type, decl)

    def visit_block_TreeTagFunStmt(self, node, *, scope, ret):
        del ret
        _, name, sig, body = node.children
        name = b2u(name.data)
        if len(sig.children) == 7:
            _, args, _, _, _, _, ret_ty = sig.children
            variadic = True
        else:
            _, args, _, _, ret_ty = sig.children
            variadic = False
        scope = Scope(scope, name='<args of %s>' % name)
        args = self.flatten3(args)
        arg_decls = []
        for arg in args:
            arg = self.visit(arg, mode='arg', scope=scope, ret=None)
            scope.put(arg.name, arg)
            arg_decls.append(arg)
        ret_ty = self.visit(ret_ty, mode='expr', scope=scope, ret=None)
        type = FunctionType([ad.type for ad in arg_decls], variadic, ret_ty)
        body = self.visit(body, mode='fun', scope=scope, ret=ret_ty)
        return FunDef(name, type, arg_decls, body)

    def visit_fun_SymSemicolon(self, node, *, scope, ret):
        return None

    def visit_arg_TreeTypedId(self, node, *, scope, ret):
        assert ret is None
        name, _, type = node.children
        type = self.visit(type, mode='expr', scope=scope, ret=None)
        assert type.valid
        return Arg(b2u(name.data), type, None)

    def visit_expr_TreeTagAttrExpr(self, node, *, scope, ret):
        expr, attr = node.children
        _, attr, _, args, _ = attr.children
        attr = b2u(attr.data)
        expr = self.visit(expr, scope=scope, ret=ret)
        args = self.flatten3(args)
        args = [self.visit(a, scope=scope, ret=ret) for a in args]
        assert attr == 'const' and not args
        return QualifiedType.get_const(expr)

    def visit_fun_TreeCompoundStmt(self, node, *, scope, ret):
        _, node, _ = node.children
        children = self.flatten2(node)
        scope = Scope(scope, '<block of %s>' % scope.name)
        items = []
        # Note, we do *not* want forward declarations at function scope.
        for child in children:
            item = self.visit(child, mode='block', scope=scope, ret=ret)
            if isinstance(item, (Decl, FunDef, StructType, UnionType, EnumType)):
                scope.put(item.name, item)
            items.append(item)
        return Block(scope, items)

    def visit_block_TreeTagExprStmt(self, node, *, scope, ret):
        node, _ = node.children
        return ExprStmt(self.visit(node, mode='expr', scope=scope, ret=ret))

    def visit_block_TreeTagInitializedDeclaration(self, node, *, scope, ret):
        _, typed_id, _, init, _ = node.children
        name, _, type = typed_id.children
        name = b2u(name.data)
        type = self.visit(type, mode='expr', scope=scope, ret=ret)
        init = self.visit(init, mode='expr', scope=scope, ret=ret)
        if isinstance(type, ArrayType) and type.size is None and isinstance(init, ArrayLit):
            size = IntLit(scope.get('size_t'), len(init.elems))
            type = ArrayType(type.elem, size)
        assert type.valid
        return Decl(name, type, init)

    def visit_expr_TreeTagCallExpr(self, node, *, scope, ret):
        left, _, args, _ = node.children
        left = self.visit(left, scope=scope, ret=ret)
        args = self.flatten3(args)
        args = [self.visit(child, scope=scope, ret=ret) for child in args]

        if isinstance(left, Builtin):
            return left.builtin_call(*args)
        if isinstance(left.type, FunctionType):
            type = left.type.ret_type
            formal_types = left.type.arg_types
            if left.type.variadic:
                if len(args) < len(formal_types):
                    raise TypeError('Argument count mismatch!')
            else:
                if len(args) != len(formal_types):
                    raise TypeError('Argument count mismatch!')
            for f, a in zip(formal_types, args):
                if not f.accepts(a):
                    raise TypeError('Value %s of type %s is not compatible with Type %s' % (a, a.type, f))
        else:
            raise AttributeError('Cannot call %s' % left)
        return CallExpr(type, left, args)

    _backslash = {
        b'\\': b'\\',
        b'\'': b'\'',
        b'\"': b'\"',
        b'e': b'\x1b',
        b'a': b'\a',
        b'b': b'\b',
        b'f': b'\f',
        b'n': b'\n',
        b'r': b'\r',
        b't': b'\t',
        b'v': b'\v',
    }

    def visit_expr_AtomStringLiteral(self, node, *, scope, ret):
        data = node.data[1:-1]
        i = 0
        s = []
        while i < len(data):
            j = data.find(b'\\', i)
            if j == -1:
                s.append(data[i:])
                break
            if i != j:
                s.append(data[i:j])
            c = data[j+1:j+2]
            assert len(c)
            if c in self._backslash:
                s.append(self._backslash[c])
                i = j + 2
                continue
            if c == 'x':
                assert ishexb(data[j+2:j+3])
                assert ishexb(data[j+3:j+4])
                b = int(data[j+2:j+4], 16)
                s.append(bytes([b]))
                i = j + 4
                continue
            if c == 'c':
                if data[j + 2] == '?':
                    s.append(b'\x7f')
                    continue
                assert b'@' <= data[j + 2] <= b'_'
                b = ord(data[j + 2]) - 0x40
                s.append(bytes([b]))
                i = j + 3
                continue
            if isoctb(c):
                if isoctb(data[j+2:j+3]):
                    if isoctb(data[j+3:j+4]):
                        b = int(data[j+1:j+4], 8)
                        i = j + 4
                    else:
                        b = int(data[j+1:j+3], 8)
                        i = j + 3
                else:
                    b = int(data[j+1:j+2], 8)
                    i = j + 2
                s.append(bytes([b]))
                continue
            assert False

        s = b2u(b''.join(s))
        return StringLit(scope.get('string literal'), s)

    def visit_expr_AtomBinaryLiteral(self, node, *, scope, ret):
        val = int(b2u(node.data[2:]), 2)
        return IntLit(scope.get('int literal'), val)

    def visit_expr_AtomOctalLiteral(self, node, *, scope, ret):
        val = int(b2u(node.data[1:] or b'0'), 8)
        return IntLit(scope.get('int literal'), val)

    def visit_expr_AtomDecimalLiteral(self, node, *, scope, ret):
        val = int(b2u(node.data), 10)
        return IntLit(scope.get('int literal'), val)

    def visit_expr_AtomHexadecimalLiteral(self, node, *, scope, ret):
        val = int(b2u(node.data[2:]), 16)
        return IntLit(scope.get('int literal'), val)

    def visit_expr_TreeTagArrayLiteral(self, node, *, scope, ret):
        _, elems, _ = node.children
        elems = self.flatten3(elems)
        elems = [self.visit(e, scope=scope, ret=ret) for e in elems]
        ty = ArrayLitType([e.type for e in elems])
        return ArrayLit(ty, elems)

    def visit_expr_TreeTagAddressofExpr(self, node, *, scope, ret):
        _, rhs = node.children
        rhs = self.visit(rhs, scope=scope, ret=ret)
        assert rhs.type.valid
        ty = PointerType(rhs.type)
        return AddressofExpr(ty, rhs)

    def visit_block_TreeTagReturnStmt(self, node, *, scope, ret):
        assert ret is not None
        assert isinstance(ret, Type)
        _, expr, _ = node.children
        if isinstance(ret, VoidType):
            assert isinstance(expr, self._parser.Nothing)
            return ReturnStmt(None, ret)
        assert not isinstance(expr, self._parser.Nothing)
        assert ret.valid
        expr = self.visit(expr, mode='expr', scope=scope, ret=ret)
        assert ret.accepts(expr)
        return ReturnStmt(expr, ret)

    def visit_block_SymSemicolon(self, node, *, scope, ret):
        return None

class AstLowerer(AbstractVisitor):
    __slots__ = ('_b',)

    def __init__(self):
        super().__init__()
        self._b = Builder()

    def _init(self, expr):
        b = self._b
        return b.initializer_expr(expr)

    def _expr(self, expr):
        return expr

    def visit_top_Block(self, node):
        b = self._b
        tops = []
        for header in ['stdbool.h', 'stddef.h', 'stdint.h']:
            tops.append(b.comment('#include <%s>' % header))
        for item in node.items:
            item = self.visit(item)
            tops.append(item)
        return b.tu(tops)

    def visit_top_Comment(self, node):
        b = self._b
        return b.comment('/* %s */' % node.rambling)

    def visit_top_StructType(self, node):
        b = self._b
        # TODO wrap this in a TypeDecl class to avoid the need for modes?
        if node.body is None:
            return b.struct_declaration(node.name)
        assert False, 'NYI'

    def visit_top_Decl(self, node):
        b = self._b
        name = node.name
        ty = self.visit(node.type, mode='type')

        scs = []
        if node.attr_extern:
            scs.append(b.storage_class_extern())
        if node.attr_static:
            scs.append(b.storage_class_static())

        if node.initializer is None:
            return b.declaration_noinit(scs, ty, name)
        else:
            init = self.visit(node.initializer, init=self._init)
            return b.declaration_init(scs, ty, name, init)

    def visit_type_PointerType(self, node):
        b = self._b
        elem = self.visit(node.elem)
        return b.type_pointer(elem)

    def visit_type_StructType(self, node):
        b = self._b
        return b.type_struct(node.name)

    def visit_top_FunDef(self, node):
        b = self._b
        arg_names = [arg.name for arg in node.arg_decls]
        type = self.visit(node.type, mode='type', arg_names=arg_names)

        scs = []
        if node.attr_extern:
            scs.append(b.storage_class_extern())
        if node.attr_static:
            scs.append(b.storage_class_static())

        if node.body is None:
            return b.declaration_noinit(scs, type, node.name)
        else:
            body = self.visit(node.body, mode='fun')
            return b.function_definition(scs, node.name, type, body)

    def visit_type_FunctionType(self, node, *, arg_names=None):
        b = self._b

        if arg_names is not None:
            assert len(arg_names) == len(node.arg_types)

        ret_type = self.visit(node.ret_type)
        arg_types = []
        for i, arg in enumerate(node.arg_types):
            arg = self.visit(arg)
            if arg_names is not None:
                arg = b.param_declaration([], arg, arg_names[i])
            else:
                arg = b.param_declaration_anon([], arg)
            arg_types.append(arg)

        return b.type_function(ret_type, arg_types, node.variadic)

    def visit_type_VoidType(self, node):
        b = self._b
        return b.type_void()

    def visit_type_BoolType(self, node):
        b = self._b
        return b.type_bool()

    def visit_type_CharType(self, node):
        b = self._b
        return b.type_char()

    _int_types = {
        'signed char': Builder.type_signed_char,
        'unsigned char': Builder.type_unsigned_char,
        'signed short': Builder.type_signed_short,
        'unsigned short': Builder.type_unsigned_short,
        'signed int': Builder.type_signed_int,
        'unsigned int': Builder.type_unsigned_int,
        'signed long': Builder.type_signed_long,
        'unsigned long': Builder.type_unsigned_long,
        'signed long long': Builder.type_signed_long_long,
        'unsigned long long': Builder.type_unsigned_long_long,
    }

    def visit_type_IntType(self, node):
        b = self._b
        u = self._int_types.get(node.underlying)
        if u is not None:
            return u(b)
        return b.type_typedef(node.underlying)

    _float_types = {
        'float': Builder.type_float,
        'double': Builder.type_double,
        'long double': Builder.type_long_double,
    }

    def visit_type_FloatType(self, node):
        b = self._b
        u = self._float_types[node.underlying]
        return u(b)

    def visit_type_QualifiedType(self, node):
        b = self._b
        u = self.visit(node.unqualified_type)
        if node.attr_const:
            u = b.type_const(u)
        return u

    def visit_fun_Block(self, node):
        b = self._b
        decls = []
        for item in node.items:
            item = self.visit(item)
            decls.append(item)
        return b.stmt_compound(decls)

    def visit_fun_ExprStmt(self, node):
        b = self._b
        expr = self.visit(node.expr, init=self._expr)
        return b.stmt_expr(expr)

    def visit_fun_CallExpr(self, node, init):
        b = self._b
        left = self.visit(node.left, init=self._expr)
        args = [self.visit(a, init=self._expr) for a in node.args]
        return init(b.expr_call(left, args))

    def visit_fun_IdRef(self, node, init):
        b = self._b
        assert not isinstance(node.decl, Type)
        name = node.decl.name
        return init(b.expr_id(name))

    def visit_fun_StringLit(self, node, init):
        b = self._b
        return init(b.expr_string(node.val))

    # identical to visit_top_Decl
    # (but separate because they're fundamentally distinct)
    def visit_fun_Decl(self, node):
        b = self._b
        name = node.name
        ty = self.visit(node.type, mode='type')

        scs = []
        if node.attr_extern:
            scs.append(b.storage_class_extern())
        if node.attr_static:
            scs.append(b.storage_class_static())

        if node.initializer is None:
            return b.declaration_noinit(scs, ty, name)
        else:
            init = self.visit(node.initializer, init=self._init)
            return b.declaration_init(scs, ty, name, init)

    def visit_fun_IntLit(self, node, init):
        b = self._b
        rv = b.expr_int(node.val)
        if False and not isinstance(node.type, IntLitType):
            ty = self.visit(node.type, mode='type')
            rv = b.expr_cast(ty, rv)
        return init(rv)

    def visit_fun_NullLit(self, node, init):
        b = self._b
        return init(b.expr_id('NULL'))

    def visit_type_ArrayType(self, node):
        b = self._b
        elem = self.visit(node.elem)
        size = self.visit(node.size, mode='fun', init=self._expr)
        return b.type_array(elem, size)

    def visit_fun_ArrayLit(self, node, init):
        b = self._b
        assert init == self._init
        return b.initializer_braced([self.visit(e, init=init) for e in node.elems])

    def visit_fun_AddressofExpr(self, node, init):
        b = self._b
        rhs = self.visit(node.right, init=self._expr)
        return init(b.expr_addressof(rhs))

    def visit_fun_IndexExpr(self, node, init):
        b = self._b
        lhs = self.visit(node.left, init=self._expr)
        rhs, = [self.visit(i, init=self._expr) for i in node.indices]
        return init(b.expr_index(lhs, rhs))

    def visit_fun_BoolLit(self, node, init):
        b = self._b
        assert type(node.val) is bool
        if node.val:
            return init(b.expr_id('true'))
        else:
            return init(b.expr_id('false'))

    def visit_fun_ReturnStmt(self, node):
        b = self._b
        if node.value is None:
            expr = b.expr_none()
        else:
            expr = self.visit(node.value, init=self._expr)
        return b.stmt_return(expr)

def main():
    import sys
    from nicate.grammar import Grammar
    from nicate.core import Parser
    fn = 'gram/nicate-glass.gram'
    with open(fn) as fo:
        g = Grammar(fn, fo)
    p = Parser(g)

    creator = AstCreator(p)
    lowerer = AstLowerer()

    glass_tu = p.parse_file(sys.stdin)
    semantic_tu = creator.visit(glass_tu)
    c_tu = lowerer.visit(semantic_tu)

    c_tu.emit_to_file(sys.stdout)

if __name__ == '__main__':
    main()
