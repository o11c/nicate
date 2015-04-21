Nicate is a library to make languages that can communicate with each other and C.

It has provides APIs at many levels, each suitable for different purposes.

C APIs:

  - grammar-based c89 output
  - logical (but typeless) c89 construction and output
  - (planned) type-aware ast construction and output
  - (planned) parallel regex lexer stepping library
  - (planned) push-based parser library
  - (planned) bnf-like parser so you can output something else instead of c89
  - (possible) other output modes (gccjit & llvm C APIs, via dlopen)
  - (possible) API emulation for gccjit & llvm C APIs

Python3 APIs:
  - logical c89 construction and output

See hello.c and hello2.py for example client code.

For support, join #o11c on irc.freenode.net
but you might also find like-minded people on #proglangdesign, ##parsers,
and #compilers.
