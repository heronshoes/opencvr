# ropencv2: Ruby binding of OpenCV

## About this library

ropencv2 is a ruby extension library which provides the wrapper of OpenCV.

## Tiny sample

```ruby
#!/usr/bin/env ruby

require 'cv2'

# Read a JPEG file
img = CV2::imread("input.jpg")
# draw a circle of radius: 100 at position (200, 200) with blue color
# (BGR: 255, 0, 0), thickness: 3 and antialiased
CV2::circle(img, [200, 200], 50, [255, 0, 0], thickness: 3, lineType: CV2::LINE_AA)
# Save to out.jpg
CV2::imwrite("out.jpg", img)
```

## How to create binding code

ropencv2 generates binding code with the similar way of python binding, which is officially provided by OpenCV.

To provide ruby API of C/C++ library, we need to write binding code for each API (functions, enums, etc.). OpenCV has many APIs, so it would be an exhausting task if we manually write all binding codes.

To avoid this problem, python binding codes are automatically generated by reading header files of C++ interface. ropencv2 uses similar way and reuse some scripts of python binding generator.

## Supported features

ropencv2 is just a prototype now. It provides only a few functions.

* `cv::imread()`
* `cv::imwrite()`
* `cv::arrowedLine()`
* `cv::circle()`
* `cv::Mat::cols`
* `cv::Mat::rows`
* `cv::Mat::channles()`

## Future plan

* Support more classes, functions, etc.
* Create gem
* Use numo-narray (Does anyone need?)
  * See the below section for the detail

## Matrix library

OpenCV uses `cv::Mat` class as matrix class and image data is represented as an instance of it. However OpenCV python binding provides image data as `ndarray` of numpy library because numpy is widely used in python world. Thus python developers can manipulate image data not only with OpenCV APIs but also with numpy APIs and other python libraries which are based on numpy.

To provide matrix data as numpy, python binding has special process when `cv::Mat` instance is created. For example, when `cv::imread()` is called, python binding does the below things:

* Create a new `cv::Mat` instance with custom generated called `NumPyAllocator`.
  * `NumPyAllocator` allocates memory by using numpy C API (`PyArray_SimpleNew()`) and set the pointer to a member variable of `cv::Mat`.
* Copy the return value of `cv::imread()` to the created `cv::Mat` instance.
* Returns the pointer as the return value. So it can be used as `ndarray`.

In ruby world, numo-narray is a famous matrix library, but ropencv2 does not replace `cv::Mat` with numo-narray. I haven't tried it because:

* I'm not sure such requirement really exists
* I'm not familiary with numo-narray C API and therefore not sure it's possible or not

## How to build

### Prerequisites

To build and use ropencv2, OpenCV and python binding have to be built and installed because ropencv2 uses some files generated in python binding building steps.

This explanation assuemes that you have already built and installed OpenCV by using the following directories.

* `/path1/to1/opencv-X.Y.Z/`: OpenCV (version: X.Y.Z) source directory
* `/path2/to2/build-opencv/`: OpenCV build directory
* `/path3/to3/opencv/`: Install directory

It means that you built OpenCV with the following commands.

```
$ cmake -S /path1/to1/opencv-X.Y.Z \
  -B /path2/to2/build-opencv \
  -DCMAKE_INSTALL_PREFIX=/path3/to3/opencv \
  -Dbuild_opencv_python3=YES \
  # and other options
$ cd /path2/to2/build-opencv
$ make
$ make install # or with sudo
```

### Build ropencv2

Run `gen2rb.py` => `generated` directory is generated and some files are put into it.

```
export PYTHONPATH="/path1/to1/modules/python/src2"
$ ./gen2rb.py /path2/to2/build-opencv/modules/python_bindings_generator/headers.txt
```

Run `extconf.rb` => `Makefile` is genereated.

```
$ export PKG_CONFIG_PATH=/path3/to3/opencv/lib/pkgconfig
$ ruby extconf.rb --with-opt-include=/path3/to3/opencv/include \
  --with-opt-lib=/path3/to3/opencv/lib
```

Build ropencv2 => `cv2.so` is genereated.

```
$ make
```

### Run sample code

```
$ cd samples
$ ./tiny.rb
```