The following program draws a picture using one of three shading methods: gouraud, phong, or a silhouette shader. The material can be changed and the light source may be moved.

Controls for the program are as follows:
'q' : quit
'm' : choose next material
'M' : choose previous material
'l' : choose next light
'L' : choose previous light
'x' : increase light x coordinate
'X' : decrease light x coordinate
'y' : increase light y coordinate
'Y' : decrease light y coordinate
'z' : increase light z coordinate
'Z' : decrease light z coordinate
'1' : select gouraud shader
'2' : select phong shader
'3' : select silhouette shader
's' : toggle spotlight
'a' : increase angle of spotlight
'A' : decrease angle of spotlight
't' : increase alpha of spotlight
'T' : decrease alpha of spotlight
'r' : reset all lights

The project is built using CMake. Download all files into a project folder, say "project_dir", and designate that folder as the source directory. Then designate the build directory as "project_dir/build".

The folder ExampleOutputs contains samples of what this program produces.
