readme.txt
Final Project: Ray tracer
Milestone 3

Group Member: Siqi Sun ; Yuding Ai ; Ziyin Qu
Pennkey     : siqisun  ; yudingai  ; ziyinq

OS: Yuding Ai:  Ubuntu 16.04 (running OS)
    Siqi Sun :  Mac OS
    Ziyin Qu :  Windows 10

Date: 2017.04.19 - 2017.04.25

To run the program: (tested on Ubuntu 16.04)
This is a Qt project so to run the program, just modify the main.cpp as there are multiple
scenes/version of raytracers to test on, i.e:
		QImage image = raytracer("../json/cornell_box.json"); //reflection + refraction    
		//QImage image = accraytracer("../json/cornell_box.json"); // reflection only
		...
Then press the run button.

P.S. if there is an error about "'it' does not name a type for(auto it = triangle...)", just add
CONFIG += c++11 into the .pro file.



Contributions:

Part A: Siqi Sun

1. Implement the Cylinder class and the ray-cylinder intersection.
2. Implement the Cone class and the ray-cone intersection.
3. Re-implement the intersect function as geometries' (sphere, triangle, cube, etc.) method function.


Part B: Yuding Ai
1. Implement an Acceleration Structures (Grid.cpp) for primitives and meshes (5 Point) 
	- This optimized approach works well on sparsely distributed scenes. 
	- I have tested it on some example json files (along with obj files, 
		I have attached these obj and json files in /json folder) and here are the running 
		time for the original method and the optimized method:

			Original raytracer:  A.dodechedron.json				takes about 7.39 sec
				 		 		 B.gourd.json 					takes about 141 sec
						 		 c.teapot.json					takes about 460 sec

			Optimized raytracer: A.dodechedron.json				takes about 2.83 sec
						 		 B.gourd.json					takes about 39 sec
						 		 C.teapot.json					takes about 85 sec

	Note: All tests are tested by a Thinkpad P50 laptop with Ubuntu 16.04

2. debug together with my teammates.



Part C: Ziyin Qu

1. Implement refraction function and rewrite the recursive process in reflection.cpp
2. Implement anti-aliasing(implemented in raytrace.cpp, please check this part by running
   "anti_aliasraytracer("../json/cornell_box.json")")



P.S. See sample result: 
	A. cornell_box.json:  "cornell_reflec_refrac.png"
						  "cornell_reflectonly_result.png"
						  "cornell_reflec_refrac_antialias.png"
						  "cornell_lamber_texture_nor.png"
						  P.S. We have add an extra texture on the floor
	B. my_shapes.json  :  "my_shapes.png" (cylinder and cone)
	C. gourd.json      :  "gourd.png"
	D. dodechedron.json:  "dodechedron.png"
	E. teapot.json 	   :  "teapot.png" 	
	F. all_shapes.json :  "all_shapes.png"
	G. texture/normal mapping	 :  "texturemap.png" (texture only)
								 :	"texture_normal_map.png" (texture + normal)
								 :  "normalmap.png" (normal only with white diffuse color)


As always, Thanks very much for your time and please let us know if we could be
any further assistance with running our code! 
Thanks !:)