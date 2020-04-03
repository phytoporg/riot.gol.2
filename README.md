# riot.gol.2 - Game of Life Redux: A three hour tour.

<sub><sup>Though shhh I probably spent closer to four hours on this.</sup></sub>

## High level description

The project is broken into three components: the main program (src/exe), the GOL library (src/lib) and the tests (src/test).

The `inputs` directory contains the sample input file from the problem prompt, and `cmake` some cmake resources for retrieving external dependencies (in this case, dependenc*y*: GTest).

Here's the full break-down:
```
.
├── cmake
│   └── FetchGTest.cmake
├── CMakeLists.txt
├── inputs
│   └── sample.life
├── README.md
└── src
    ├── CMakeLists.txt
    ├── exe
    │   ├── CMakeLists.txt
    │   └── main.cpp
    ├── lib
    │   ├── Cell.h
    │   ├── CellStorage.cpp
    │   ├── CellStorage.h
    │   ├── CMakeLists.txt
    │   ├── GOLGrid.cpp
    │   └── GOLGrid.h
    └── test
        ├── CMakeLists.txt
        └── test_main.cpp
```

My goal with the overall approach to the problem was to try something different from [my last stab at this](https://github.com/phytoporg/riot.gol) and implement a few ideas loosely inspired by chapters 17 and 18 in [Michael Abrash's Graphics Programming Black Book](http://www.jagregory.com/abrash-black-book/). Chiefly: using a lookup table to drive cell state transitions, and only updating cells in each generation which require updating.

The LUT creation and usage can be found in `GOLGrid`. Updating cells is done by only tracking live cells and any dead cells which may neighbor them. During each generation, dead and neighborless cells are retired from storage.

## Building

Grab yourself whichever flavor of [CMake](https://cmake.org/) suits your tastes, or more importantly, works on your respective platform.

### Windows w/Visual Studio 19

VS build tools will have to be in your path. Running this from the [VS developer prompt](https://docs.microsoft.com/en-us/dotnet/framework/tools/developer-command-prompt-for-vs) should give you the right environment.

`mkdir build && cmake -H. -Bbuild -G "Visual Studio 16 2019" -A x64 && cmake --build .\build --config Release -j <num_cores>`

### Linux
`mkdir build && cmake -H. -Bbuild && cmake --build ./build --config Release -j <num_cores>`

## Running Game of Life

In its current state this project doesn't have an installation pass, so just run `gol2` directly from the build artifacts path with ordered parameters pointing to the life file and number of generations, respectively. For example:
`./build/src/exe/gol2 ./inputs/sample.life 10`

## Running tests

`pushd ./build/src/test; ctest`
