#  Peano

`Peano` is an open source framework for solvers on dynamically adaptive
Cartesian meshes.

Its core is built with C++, but many tools around it are written in Python.
`Peano`  is based upon the fact that spacetrees, a generalisation of the classical octree concept, yield a cascade of adaptive Cartesian grids. Consequently, any spacetree traversal is equivalent to an element-wise traversal of the hierarchy of the adaptive Cartesian grids. The software `Peano`  realises such a grid traversal and storage algorithm, and it provides hook-in points for applications performing per-element, per-vertex, and so forth operations on the grid. It also provides interfaces for dynamic load balancing, sophisticated geometry representations, and other features.

## 1. Supported platforms

We test and maintain `Peano`  for Linux only.
If you prefer Windows or MacOS, it should work as long as you provide the
mandatory tools (see below), but we won't be able to help.

## 2. Dependencies and prerequisites

`Peano`'s core is plain C++17 code. 
We however use a whole set of tools around it:

*  C++17-compatible C++ compiler (required).
*  Python3 (optional; not required if you work only with the C++ baseline).
*  MPI2 (optional). MPI's multithreaded support is required.
*  Intel's Threading  Build Blocks. You can use C++ threads instead, but they seem to come along with a slightly higher overhead, and, obviously,
*  GNU autotools (automake) to set up the system (required). The Intel tool support is not as elaborate.
*  The Visualization Toolkit (VTK) if you want to use the built-in visualisation facilities (optional).
*  Doxygen if you want to create HTML pages of PDFs of the in-code documentation.
*  scons to build the cookbook

## 3. How to build it

`Peano's` build process allows you to build only
the components you actually need for your project.

The build process for three often used configurations 
is described below.

### 3.1. Build the Peano shared library

```shell
libtoolize; aclocal; autoconf; autoheader;
./configure
make -j
make install
```

### 3.2.  Build Peano plus VTK support

> **NOTE:** Assumes a VTK-7.1 installation. How to configure for other VTK versions is explained in the uesr guide.

```shell
libtoolize; aclocal; autoconf; autoheader;
./configure --with-vtk=-7.1 --with-vtk-version=7
make -j
make install
```

### 3.3.  Build Peano plus ExaHyPE2 and VTK support

> **NOTE:** Assumes a VTK-7.1 installation. How to configure for other VTK versions is explained in the uesr guide.

```shell
libtoolize; aclocal; autoconf; autoheader;
./configure --with-exahype --with-vtk=-7.1 --with-vtk-version=7
make -j
make install
```

## 4. User guide

For more details regarding build configuration and features of the
software, take a look at the `Peano` cookbook. You can build a PDF version via:

```shell
cd cookbook
scons 
```