# Welcome to cpp_pointer

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Documentation Status](https://readthedocs.org/projects/None/badge/)](https://None.readthedocs.io/)


### NOTES


JUST A SIMPLE POINTER ARITHMETIC LIBRARY, WRITTEN FOR exploring deducing this.

As well as other nicer modern features

# Prerequisites

Building cpp_pointer requires the following software installed:

* A C++23-compliant compiler
* CMake `>= 3.23`
* Doxygen (optional, documentation building is skipped if missing)

# Building cpp_pointer

The following sequence of commands builds cpp_pointer.
It assumes that your current working directory is the top-level directory
of the freshly cloned repository:

```
cmake -B build
cmake --build build
```

The build process can be customized with the following CMake variables,
which can be set by adding `-D<var>={ON, OFF}` to the `cmake` call:

* `cpp-pointer_BUILD_TESTING`: Enable building of the test suite (default: `ON`)
* `cpp-pointer_BUILD_DOCS`: Enable building the documentation (default: `ON`)



# Testing cpp_pointer

When built according to the above explanation (with `-Dcpp-pointer_BUILD_TESTING=ON`),
the C++ test suite of `cpp_pointer` can be run using
`ctest` from the build directory:

```
cd build
ctest
```


# Documentation

cpp_pointer provides a Sphinx-based documentation, that can
be browsed [online at readthedocs.org](https://cpp-pointer.readthedocs.io).
To build it locally, first ensure the requirements are installed by running this command from the top-level source directory:

```
pip install -r doc/requirements.txt
```

Then build the sphinx documentation from the top-level directory:

```
cmake --build build --target sphinx-doc
```

The web documentation can then be browsed by opening `build/doc/sphinx/index.html` in your browser.
## Acknowledgments

This repository was set up using the [SSC Cookiecutter for C++ Packages](https://github.com/ssciwr/cookiecutter-cpp-project).
