## Introduction

This is a 3D Rendering Engine written in C++11.
For the details of this project, please see ``report.pdf``

## Features

* [Phong Model](http://en.wikipedia.org/wiki/Phong_reflection_model) and [Path Tracing](http://en.wikipedia.org/wiki/Path_tracing) Model (Monte Carlo Path Tracing)
* rendering of plane, sphere and mesh
* read mesh from a simple obj file
* fast SAH-Based KDtree in mesh as well as the whole scene
* soft shadow in Phong model
* depth of field
* custom texture
* mesh smoothing
* mesh simplification
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

``./ray-tracing <demo id>``, show demo scene defined in ``main.cc``.

The display is based on OpenCV, allowing keyboard control of the viewpoint:

* Left/Right Arrow - Orbit around the center.
* Up/Down Arrow - Rotate the output picture.
* > <	 - Rotate viewpoint direction.
* = -  - Zoom in/out.
* h j k l - Move viewpoint, keep direction.
* []  -  Move focal plane (only available when DOF mode is on)

GUI:

GUI is only an objviewer with some simple control.
![gui](https://github.com/ppwwyyxx/Ray-Tracing-Engine/raw/master/demo/gui.png)

## Examples
A simple ball with transmission:

![ball](https://github.com/ppwwyyxx/Ray-Tracing-Engine/raw/master/demo/transmission.png)

Obj and ground texture:

![pict](https://github.com/ppwwyyxx/Ray-Tracing-Engine/raw/master/demo/pic_texture.png)

Soft shadow:

![soft](https://github.com/ppwwyyxx/Ray-Tracing-Engine/raw/master/demo/soft.png)

Depth of field (There is also a video in `demo/`):

![dof](https://github.com/ppwwyyxx/Ray-Tracing-Engine/raw/master/demo/dof.png)

Caustic using path tracing:

![caustic](https://github.com/ppwwyyxx/Ray-Tracing-Engine/raw/master/demo/caustic.png)

Large scene using Phong Model(first) and Path Tracing(second):

![allph](https://github.com/ppwwyyxx/Ray-Tracing-Engine/raw/master/demo/all_phong.png)
![allpt](https://github.com/ppwwyyxx/Ray-Tracing-Engine/raw/master/demo/best_light.png)
