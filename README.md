# Encrypted-Control

## Setup

---
**NOTE**

If you are using MacOS make sure you have `wget`first. If you don't have it, run the following commands

```
$ /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
$ brew install wget
```
---


Run the `setup.sh` script to install all library dependencies
```
$ ./setup.sh
```

## Building
To build this project, follow the next steps:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```
## Running 

To run the code, open two terminals and run the following commands:

Terminal 1
```
./bin/system 1 12345
```

Terminal 2
```
./bin/cloud 2 12345
```
