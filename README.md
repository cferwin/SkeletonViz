# SkeletonViz
Visualize CT scans, with emphasis on the skeleton and surrounding tissue, in 3D.

## Usage
SkeletonViz is run from the command line with arguments specifying the file path template, first image number, and last image number.

For example, `SkeletonViz.exe "C:\images\foo (%d).tif" 1 100` will load all the files in numerical order from `C:\images\foo (1).tif` to `C:\images\foo (100).tif`.
Use the `%d` to indicate where the numbers should be placed in the file path. Note that leading zeroes are not currently accounted for, such as `foo001.tif` to `foo100.tif`.

Pressing the `c` key will change rendering modes. The first mode is Bone Only. Next is Bone and Tissue, then Both Modes Combined. Pressing the key again
will return to Bone Only mode and so on. Examples of each rendering mode can be seen below in the section Sample Images.

NOTE that it may be necessary to move the camera a little bit after changing modes to force an update. This will be fixed in the future.

Pressing the `a` key will add a clipping plane. This is a visible plane widget which can be manipulated with the mouse to clip and remove parts of the image
from the screen. This allows for closer inspection of components which are otherwise obscured.

Pressing the `d` key will remove a clipping plane. The last clipping plane to be added will be removed first (Last In First Out).

## Dependencies
1. [ITK](https://itk.org/)
2. [VTK](http://www.vtk.org/)
3. [CMake](https://cmake.org/)

## Compilation
1. Install ITK and VTK following the guides on the websites linked above.
2. Run CMake, specifying the main path as the source file, and somewhere else for the build file. Be sure to specify paths for ITK and VTK if needed.

## Sample Images

### Bone Only
![Bone Only](http://i.imgur.com/A49t9Sb.png)

### Bone and Tissue
![Bone and Tissue](http://i.imgur.com/jgbcfQD.png)

### Both Modes Combined
![Combined](http://i.imgur.com/yoiKFKv.png)

## License
MIT

Copyright (c) 2017 Christopher Erwin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
