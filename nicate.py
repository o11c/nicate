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

def make_fun(sk, lk, v):
    r = v.result
    a = v.args

    c = getattr(nicate_library, lk)

    def f(b, *args):
        assert isinstance(b, Builder)
        actuals = [b._c_builder]
        for a in args:
            if isinstance(a, OwnedObjectBase):
                actuals.append(a._c_object)
            elif isinstance(a, list):
                actuals.append(len(a))
                actuals.append([i._c_object for i in a])
            elif isinstance(a, str):
                actuals.append(a.encode('utf-8', errors='surrogateescape'))
            elif isinstance(a, int): #including bool
                actuals.append(a)
            else:
                raise TypeError(str(type(a)))
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
        assert k.startswith('build_')
        sk = k[6:]
        setattr(Builder, sk, make_fun(sk, k, v))

def emit(tu, file):
    nicate_library.builder_emit_tu(tu._c_object, file)
TranslationUnit.emit = emit

del k, sk, v
del emit, make_class, make_fun, load
