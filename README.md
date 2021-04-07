# COMP2432_RBM
Room Booking Manager System for COMP2432 group project

## Deployment

To deploy this project, cd to the root project folder and execute `build.sh` script.

```sh
git clone https://github.com/toolsmax0/COMP2432_RBM.git
cd COMP2432_RBM
sh build.sh
```

To execute, run the following command.

```sh
./out/RBM
```

For further information, observe the following directory structure.

## Directory Structure

```
../COMP2432_RBM
.
├── doc                     # documentation
│   └── ...
├── out                     # out files
│   └── ...
├── src                     # C source code
│   ├── lib                 # submodules and libraries
│   │   └── ...
│   ├── main.c              # main source code file
│   └── master.h            # master header file
├── build.sh                # script for compiling
├── RBM.ini                 # component settings
├── README.md               # guide
└── LICENSE
```