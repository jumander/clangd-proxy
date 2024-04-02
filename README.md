# clangd-proxy
Proxy between editor and clangd for adding additional LSP functionallity


## installation
1. clone this repository and cd into it
2. create a build directory and cd into it
`mkdir build && cd build`
3. generate build files using cmake
`cmake -S ..`
4. build using your set build system
`ninja`
5. run `./clangd-proxy`
