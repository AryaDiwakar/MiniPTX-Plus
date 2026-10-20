# MiniPTX

MiniPTX is an educational C++20 compiler prototype: MiniC -> AST -> IR -> PTX.

## Build
```
make
```

## Usage
```
./miniptx check examples/arithmetic.mc
./miniptx ir examples/arithmetic.mc
./miniptx compile examples/arithmetic.mc -o out.ptx
```

## Status
Core vertical slice: lexer, parser, IR, PTX emitter. Optimizer, linker, tests in progress.
No CUDA tools available in this environment; PTX validated syntactically only.
