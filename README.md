## 3d-constructor

COMPILE:

`$ cc -o 3d 3d.c -lm`

RUN:

`$ ./3d [<width> <height> [<pts-path>]]`

COMMON CONTROLS:

`V` - cycle views

`+`,`-` - closer/farther

`W`,`R` - write to file, read from file

2D CONTROLS:

`S`,`F`,`D`,`E` - move left/right/down/up

`SPACE` - create new edge, manipulate first vertex

`SPACE` - manipulate second vertex

`SPACE` - done

`J`,`L`,`K`,`I` - move the vertex left/right/down/up

3D CONTROLS:

`S`,`F`,`D`,`E` - rotate cw-around-z/ccw-around-z/cw-around-x/ccw-around-x

STEREO MODE:

If path to secondary terminal device is specified, 3D-view frames with slightly different rotation will be output there. Second window is to be positioned to the right of the primary one and both are to be watched simultaneosly with crossed eyes. Looks well, when vertical viewing angle is horizontal. Shall fix soon...
