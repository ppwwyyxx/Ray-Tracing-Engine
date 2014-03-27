## Introduction
![allpttiny](https://github.com/ppwwyyxx/Ray-Tracing-Engine/raw/master/demo/small/best_tiny.jpg)

This is an 3D Rendering Engine written in C++11.
For the details of this project, please see [report.pdf](https://github.com/ppwwyyxx/Ray-Tracing-Engine/raw/master/report.pdf)

## Features

* [Phong Model](http://en.wikipedia.org/wiki/Phong_reflection_model) and Monte Carlo [Path Tracing](http://en.wikipedia.org/wiki/Path_tracing) Model
* rendering of plane, sphere and mesh
* read mesh from a simple obj file
* fast SAH-Based KDtree
* depth of field
* custom texture
* mesh smoothing
* mesh simplification using [polygon reduction](http://dev.gameres.com/program/visual/3d/PolygonReduction.pdf)
* parallel rendering
* qt-based gui

## Usage
#### Dependencies
* gcc >= 4.8
* [OpenCV](http://opencv.org/)
* [Magick++](http://www.imagemagick.org/Magick++/)
* Qt4 (optional)

#### Compile
```
$ make	(command line)
$ make gui   (gui)
```

#### Use

Command line:

``./ray-tracing <demo/small id>``, show demo/small scene defined in ``main.cc``.

The display is based on OpenCV, allowing keyboard control of the viewpoint:

* Left/Right Arrow - Orbit around the center.
* Up/Down Arrow - Rotate the output picture.
* > <	 - Rotate viewpoint direction.
* = -  - Zoom in/out.
* h j k l - Move viewpoint, keep direction.
* [ ]  -  Move focal plane (only available when DOF mode is on)

GUI:

GUI is only an objviewer with some simple control.
![gui](https://github.com/ppwwyyxx/Ray-Tracing-Engine/raw/master/demo/small/gui.jpg)

## Examples
Obj and ground texture:

![pict](https://github.com/ppwwyyxx/Ray-Tracing-Engine/raw/master/demo/small/pic_texture.jpg)

Depth of field (There is also a video in `demo/small/`):

![dof](https://github.com/ppwwyyxx/Ray-Tracing-Engine/raw/master/demo/small/dof.jpg)

Caustic using path tracing:

![caustic](https://github.com/ppwwyyxx/Ray-Tracing-Engine/raw/master/demo/small/caustic.jpg)

Large scene using Phong Model(first) and Path Tracing(second):

![allph](https://github.com/ppwwyyxx/Ray-Tracing-Engine/raw/master/demo/small/phong.jpg)
![allpt](https://github.com/ppwwyyxx/Ray-Tracing-Engine/raw/master/demo/small/best.jpg)
