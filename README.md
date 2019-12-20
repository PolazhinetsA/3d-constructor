## 3d-constructor

COMPILE:

`$ cc -o 3d 3d.c -lm`

RUN:

`$ ./3d [<width> <height> [<num-frames>]]`

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

STEREOSCOPY:

If number of frames specified, several narrow frames are rendered in 3d mode, each rotated by 0.1 more radians around current vertical for watching with crossed eyes.
