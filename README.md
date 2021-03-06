Nicate is a library to make languages that can communicate with each other and C.

It provides APIs at many levels, each suitable for different purposes.

C APIs:

  - grammar-based C construction and output
  - logical (but typeless) C construction and output
  - (planned) type-aware ast construction and output
  - parallel regex lexer stepping library
  - (low-level) push-based parser library
  - (limited) bnf-like parser so you can output something else instead of c89
  - (possible) other output modes (gccjit & llvm C APIs, via dlopen)
  - (possible) API emulation for gccjit & llvm C APIs

Python3 APIs:
  - logical C construction and output
  - limited bnf-like parser so you can output something else instead of c89
  - parallel regex lexer stepping library
  - low-level push-token -based parser library
  - high-level push-character -based parser/lexer combo

See hello.c and hello2.py for example client code.

For support, join #o11c on irc.freenode.net
but you might also find like-minded people on #proglangdesign, ##parsers,
and #compilers.
