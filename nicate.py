import cffi

def load():
    import sys
    assert sys.version_info[0] >= 3
    ffi = cffi.FFI()
    ffi.cdef('typedef uint64_t uintmax_t;')
    with open('fwd.h') as f:
        x = [l for l in f if not l.startswith('#')]
        ffi.cdef('\n'.join(x))
    with open('builder.h') as f:
        x = [l for l in f if not l.startswith('#')]
        ffi.cdef('\n'.join(x))
    return ffi

nicate_ffi = load()
nicate_library = nicate_ffi.dlopen('./libnicate.so')

class Builder:
    __slots__ = ('_c_builder',)

    def __init__(self):
        self._c_builder = nicate_library.builder_create()

    def __del__(self):
        nicate_library.builder_destroy(self._c_builder)

class OwnedObjectBase:
    __slots__ = ('_c_object', '_owner')

    def __init__(self, *args):
        raise TypeError('Do not instantiate directly, call methods on Builder')

def make_class(k):
    class Foo(OwnedObjectBase):
        __slots__ = ()
    Foo.__name__ = k
    Foo.__qualname__ = k
    return Foo

def desc_ty(ty):
    # since this is all undocumented, fail a lot
    if type(ty) == cffi.model.PrimitiveType:
        if ty.name == '_Bool':
            return 'bool'
        if ty.name == 'long double':
            return 'float'
        if ty.name in {'char', 'uint64_t', 'unsigned int'}:
            return ty.name
        assert False, ty.name
    if type(ty) == cffi.model.ConstPointerType:
        if type(ty.totype) == cffi.model.PrimitiveType:
            if ty.totype.name == 'char':
                return 'str'
            assert False, ty.name
    if type(ty) == cffi.model.PointerType:
        if type(ty.totype) == cffi.model.PointerType:
            if type(ty.totype.totype) == cffi.model.StructType:
                name = ty.totype.totype.name
                if name.startswith('Build'):
                    if name != 'Builder':
                        name = name[5:]
                else:
                    assert False, name
                return 'list[%s]' % name
        elif type(ty.totype) == cffi.model.StructType:
            name = ty.totype.name
            if name.startswith('Build'):
                if name != 'Builder':
                    name = name[5:]
            else:
                assert False, name
            return name
    assert False, '%s of %s' % (ty, type(ty))

def make_fun(sk, lk, v):
    r = v.result
    a = v.args
    rd = desc_ty(r)
    ad = ['self'] + [desc_ty(i) for i in a[1:] if not (isinstance(i, cffi.model.PrimitiveType) and i.name == 'size_t')]

    needs_slice = lk in {'build_expr_multi_char', 'build_expr_string'}
    if needs_slice:
        lk += '_slice'
    c = getattr(nicate_library, lk)

    def f(b, *args):
        assert isinstance(b, Builder)
        actuals = [b._c_builder]
        for a in args:
            # FIXME arguably this is all wrong and it should be like `ad`.
            if isinstance(a, OwnedObjectBase):
                actuals.append(a._c_object)
            elif isinstance(a, list):
                actuals.append(len(a))
                actuals.append([i._c_object for i in a])
            elif isinstance(a, str):
                s = a.encode('utf-8', errors='surrogateescape')
                actuals.append(s)
            elif isinstance(a, (float, int)): #including bool
                actuals.append(a)
            else:
                raise TypeError(str(type(a)))
        if needs_slice:
            assert isinstance(a, str)
            actuals.append(len(s))
        r = c(*actuals)
        assert r is not None
        assert isinstance(r, nicate_ffi.CData)
        t = nicate_ffi.typeof(r)
        assert t.kind == 'pointer'
        n = t.item.cname
        assert n.isalnum()
        assert n.startswith('Build')
        n = n[5:]
        w = globals()[n]
        assert issubclass(w, OwnedObjectBase)
        x = object.__new__(w)
        x._owner = b
        x._c_object = r
        return x
    f.__name__ = sk
    f.__qualname__ = 'Builder.%s' % sk
    f.__doc__ = '(%s) -> %s' % (', '.join(ad), rd)
    return f

# yay mucking with internals
for k, v in nicate_ffi._parser._declarations.items():
    if k.startswith('typedef'):
        k = k[8:]
        if k == 'Builder' or not k.startswith('Build'):
            continue
        k = k[5:]
        globals()[k] = make_class(k)
    if k.startswith('function'):
        k = k[9:]
        if k.startswith('builder_'):
            getattr(nicate_library, k)
            continue
        if k.endswith('_slice'):
            # actually called from the other things
            continue
        assert k.startswith('build_')
        sk = k[6:]
        setattr(Builder, sk, make_fun(sk, k, v))

def emit_to_file(tu, file):
    nicate_library.builder_emit_tu_to_file(tu._c_object, file)
TranslationUnit.emit_to_file = emit_to_file

def emit_to_string(tu):
    ptr = nicate_library.builder_emit_tu_to_string(tu._c_object)
    rv = nicate_ffi.string(ptr)
    nicate_library.builder_free_string(ptr);
    return rv.decode('utf-8', errors='surrogateescape')
TranslationUnit.emit_to_string = emit_to_string

del k, sk, v
del make_class, make_fun, load
del emit_to_file, emit_to_string
