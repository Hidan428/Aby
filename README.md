# Aby - Virtual reality labyrinth game

[![Build Status](https://travis-ci.org/Ebatsin/Aby.svg)](https://travis-ci.org/Ebatsin/Aby)

## Building the project

The following instructions will guide you through the build of the project.

### Required libraries

- Assimp
- GLFW
- GLM
- OpenCV
- OpenGL (v3.3 core profile required)
- SOIL

### Optional libraries & programs

- Google Test (only used when tests are runned)
- doxygen (only used when generating the documentation)

### Building

This project requires CMake to build the project. The project is tested on linux, using Clang and GCC and on OSX, using clang.

To build it on linux or mac, use the following commands : 

```bash
git clone https://github.com/Ebatsin/Aby.git
mkdir Aby/build
cd Aby/build
cmake ../ # you can build the tests and the doc by enabling the options
make
```

### Start the program

In the build directory, simply run the generated executable

```bash
./Aby
```



## Authors

* **Alexis Ben Miloud--Josselin** - *initial work* - [Fougasse](https://github.com/Fougasse)
* **Aurelia Binetti** - *initial work* - [Aurelia2b](https://github.com/Aurelia2b)
* **Bastien Philip** - *initial work* - [ebatsin](https://github.com/ebatsin)
* **Kevin Wilst** - *initial work* - [Hidan428](https://github.com/Hidan428)
* **Lucile Souche** - *initial work* - [Luciole13](https://github.com/Luciole13)
* **Morgane Alonso** - *initial work* - [Niouby](https://github.com/niouby)
* **Valentin Bissuel** - *initial work* - [valbi](https://github.com/valbi)

## License

This project is licensed under the GPLv3 license. See [LICENSE.md](https://github.com/Ebatsin/Aby/blob/master/LICENSE).
