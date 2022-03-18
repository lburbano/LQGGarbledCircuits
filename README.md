# Encrypted-Control
## Setup
This project requires the emp-toolkit. Please install the [Semihonest two party computation tool](https://github.com/emp-toolkit/emp-sh2pc).

## Building
To build this project, follow the next steps:
```
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=release ..
$ make
$ cd ..
```
## Running 
### Scenario 1: System garbler
Terminal 1
```
./bin/system 1 12345 [niter] [nbits]
```

Terminal 2
```
./bin/cloud 2 12345 [niter] [nbits]
```
### Scenario 2: Cloud garbler
Terminal 1
```
./bin/system 2 12345 [niter] [nbits]
```

Terminal 2
```
./bin/cloud 1 12345 [niter] [nbits]
```
