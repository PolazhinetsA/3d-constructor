## 3d-constructor

COMPILE:

`$ cc -o 3d 3d.c -lm`

RUN:

`$ ./3d [<width> <height> [<num-frames>]]`

COMMON CONTROLS:

`V` - cycle views

`K`,`I` - closer/farther

`W`,`R` - write to file, read from file

`T` - text rendering

2D CONTROLS:

`S`,`F`,`D`,`E` - move left/right/down/up

`SPACE` - create new edge, manipulate first vertex

`SPACE` - manipulate second vertex

`SPACE` - done

`J`,`L`,`K`,`I` - move the vertex left/right/down/up

3D CONTROLS:

`S`,`F`,`D`,`E` - rotate cw-around-z/ccw-around-z/cw-around-x/ccw-around-x

`+`,`-` - zoom in/out

TEXT RENDERING:

You'll be prompted to enter a word and the vector font file name. The latter is of human readable format. "megafont" is an example with only 'A' and '0' defined. A pair of alphabetically equal letters, one uppercase and one lowercase, corresponds to a particular edge. Note that, unlike rows, only odd cols are significant in terms of resulting X-position of the vertex, so you can have two edges with common vertex by putting the vertex of the second one right next to that of the first one in file, i.e. at even column. Letters not defined in current font will occupy zero space in the result.

STEREOSCOPY:

If number of frames specified, several narrow frames are rendered in 3d mode, each rotated by 0.1 more radians around current vertical for watching with crossed eyes.
