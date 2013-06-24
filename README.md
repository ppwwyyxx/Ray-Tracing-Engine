## Introduction

This is a 3D Rendering Engine written in C++11.

## Features

* Phong Model and Path Tracing Model (Monte Carlo Path Tracing)
* rendering of plane, sphere and mesh
* read mesh from a simple obj file
* soft shadow in Phong model
* depth of field
* custom texture
* fast SAH-Based KDtree.
* mesh smoothing
* mesh simplification
* parallelization
* qt-based gui

## Compile
### Dependencies
* gcc >= 4.8
* OpenCV
* Magick++
* Qt4 (optional)

```
$ make	(command line)
$ make gui   (gui)
```
