# Encrypted-Control
## Setup
This project requires the emp-toolkit. Please install the [Semihonest two party computation tool](https://github.com/emp-toolkit/emp-sh2pc).

## Building
To build this project, follow the next steps:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```
## Running 
Terminal 1
```
./bin/system 1 12345
```

Terminal 2
```
./bin/cloud 2 12345
```
