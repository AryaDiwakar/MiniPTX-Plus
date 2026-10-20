# MiniPTX-Plus

Educational C++20 compiler prototype: MiniC -> AST -> IR -> optimization passes -> PTX, plus an educational object format/linker.

## Features
- Lexer, recursive-descent parser, AST
- IR with basic control flow
- Optimization passes: constant folding, strength reduction, CSE, DCE
- PTX emitter (sm_50)
- Educational object file format with symbols/relocations
- CLI: check, ast, ir, compile (--no-opt, --dump-ir)
- CI via GitHub Actions

## Build
```
make
```

## Usage
```
./miniptx check examples/arithmetic.mc
./miniptx ir examples/arithmetic.mc
./miniptx compile examples/arithmetic.mc -o out.ptx
./miniptx compile examples/arithmetic.mc --no-opt -o out_unopt.ptx
```

No CUDA tools available in this environment; PTX validated syntactically only.
