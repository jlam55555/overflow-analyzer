# overflow-analyzer

A very simple static analyzer designed for buffer overflows in C

Final project for ECE455 by Jonathan Lam and Daniel Tsarev

[Report][report] (work in progress) | [Slides][slides]

---

### Build instructions
```bash
$ # debug target includes much more analysis information
$ make -j12 debug
```

---

### Run instructions
Analyze a LLVM bitcode file.
```bash
$ target/overflow_analyzer [LLVM_BC]
```

See sample bitcode files in `res/llvm` (these files are built from the files in `res/c` when running the `build` or `debug` target).

[report]: http://files.lambdalambda.ninja/reports/21-22_fall/ece455_report_overflow_analyzer.lam_tsarev.pdf
[slides]: http://files.lambdalambda.ninja/reports/21-22_fall/ece455_presentation_overflow_analyzer.lam_tsarev.pdf
