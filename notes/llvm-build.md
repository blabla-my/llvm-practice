## LLVM Project
LLVM can be considered as a complier. It generates universal IR for different architectures (e.g., x86, arm). And its IR provides type infomation to benefit the static analysis.

To clone the project source code:
`git clone https://github.com/llvm/llvm-project.git`

Sometimes a proxy is needed, just use `proxychains`:
`proxychains git clone https://github.com/llvm/llvm-project.git`

## Build LLVM
Makefiles can be generated using cmake. Cmake is using to manage the project building procedure, it automatically generates various types of makefiles (e.g., ninja, make).

Basically, we can use `cmake <path to source code project>` to generate unix makefiles. See `cmake --help` for detailed usage.

For llvm, under cloned `llvm-project` directory: 
```bash
mkdir llvm_build && cd llvm_build
# this will generate makefiles for unix, if you want to use others (e.g., ninja), add -G <type>.
# check cmake -G -h
cmake -DCMAKE_BUILD_TYPE=Release ../llvm
```
Explanation: 
- `-DCMAKE_BUILD_TYPE`: can be "Debug", "Release", etc.. It's better to use "Release" since the size of output will be smaller.
- '../llvm': this specify the source code project, if you want to build clang, use `../clang`

After the makefiles has been generated, use `make` to build.
After the build finishes, use `sudo make install` to add bin & lib to path.

## Build Clang
As well as llvm, firstly generate makefiles using cmake.
```bash
mkdir clang_build && cd clang_build
cmake -DCMAKE_BUILD_TYPE=Release ../clang
```
then build.
```bash
make
sudo make install
```

## Build Compiler-rt
To use some dynamically loaded object(e.g., xray runtimes), we should build compiler-rt.

These commands should be executed after clang & llvm has been built.


```bash
mkdir compiler-rt && cd compiler-rt
CC=clang CXX=clang++ cmake -DCMAKE_INSTALL_PREFIX=/usr/local/lib/clang/17/ ../compiler-rt
```
Explanation: 
- CC: specify the c compiler.
- CXX: specify the cpp compiler. Since gcc-7.5 cannot build this project, we use clang, which has just been build.
- `-DCMAKE_INSTALL_PREFIX`: specify the install prefix, then the object will be installed to $PREFIX/bin and $PREFIX/lib. Clang will find libs under `/usr/local/lib/clang/17/`.

## Using llvm xray.
LLVM xray can do lightweight instrumentations to trace the function calls.

Official documentL: https://llvm.org/docs/XRay.html

However, the official doc is not complete to run a demo. Better reading this article: https://lucisqr.substack.com/p/do-you-know-llvm-xray .

Instrumentation: 
```bash
clang -fxray-instrument -fxray-instruction-threshold=1 test.c -o test
```

Trace the function calls and store them to a log:
```bash
export XRAY_OPTIONS="patch_premain=true xray_mode=xray-basic verbosity=3"
./test
```

Using llvm-xray to view the tracing results:
```bash
llvm-xray stack --instr_map=./test xray-log.test.*
```
output: 
```bash
❯ llvm-xray stack --instr_map=./test xray-log.test.cKwgy0

Unique Stacks: 2
Top 10 Stacks by leaf sum:

Sum: 119609
lvl   function                                                            count              sum
#0    main                                                                    1           146635
#1    hello                                                                   1           119609

Sum: 9396
lvl   function                                                            count              sum
#0    main                                                                    1           146635
#1    hello2                                                                  1             9396


Top 10 Stacks by leaf count:

Count: 1
lvl   function                                                            count              sum
#0    main                                                                    1           146635
#1    hello                                                                   1           119609

Count: 1
lvl   function                                                            count              sum
#0    main                                                                    1           146635
#1    hello2                                                                  1             9396
```

Explanation:
- clang can do the instrumentation, using `-fxray-instrument`.
- `-fxray-instruction-threshold=1`: this threshold filters some small functions, if set to 1, all functions will be traced.
- XRAY_OPTIONS must be set, other wise the log file won't be generated. 
- llvm-xray stack: show the stacked trace, i.e., from root to leaf (e.g., main->hello2, main->hello)

Follow is the demo test.c:
```c
# include <stdio.h>

void hello () {
    printf("Hello,world.\n");
}

void hello2() {
    int x,y=0;
    int c = x+y;
    printf("%d\n",c);
}

int main() {
    hello();
    hello2();
    hello();
    hello2();
}
```


