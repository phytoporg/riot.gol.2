# riot.gol.2 - Game of Life Redux: A three hour tour.

<sub><sup>Though shhh I probably spent closer to four hours on this.</sup></sub>

Detailed project description coming soon.

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
