#!/bin/bash
set -e
./miniptx check examples/arithmetic.mc
./miniptx compile examples/arithmetic.mc -o /tmp/out.ptx
echo "smoke ok"
