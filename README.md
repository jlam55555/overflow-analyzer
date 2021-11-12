# overflow-analyzer

A very simple static analyzer designed for buffer overflows in C

Final project for ECE455 by Jonathan Lam and Daniel Tsarev

---

# Overview
TODO: talk about analysis

---

# Build instructions
This requires a standard OCaml installation with `opam`. The `dune` build system is used for running and testing.

### Setup
```bash
$ opam install dune base llvm    # dependencies
$ opam install utop ounit2 odoc  # devdependencies
```

### Building
```bash
$ make build
$ make run                       # build and run
```

##### Building documentation
```bash
$ make docs
$ make show_docs                 # make and open in browser
```

##### Cleanup
Clean up build artifacts
```bash
$ make clean
```

##### Running tests
Build LLVM bytecode test cases:
```bash
$ make build_bc
```

Run tests:
```bash
$ dune test
```

---

# Development
REPL using `utop`:
```dune
$ dune utop .
```
