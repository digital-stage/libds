# api-client
Native cpp based client for the API


## Building

First use conan to install all dependencies:

```
conan install -if cmake-build-debug --build=missing .
```

Then configure cmake:

```
cmake -B cmake-build-debug .
```

And build the test application:

```
cmake --build cmake-build-debug --parallel
```

## Build and run tests

To build and run test, you've to enable the BUILD_TESTS flag inside cmake:

```
conan install -if cmake-build-debug --build=missing .
cmake -DBUILD_TESTS=On -B cmake-build-debug .
cmake --build cmake-build-debug --target DigitalStage-test --parallel
```
Then you are able to run the test executable DigitalStage-test inside the cmake-build-debug folder:
```
./cmake-build-debug/DigitalStage-test
```