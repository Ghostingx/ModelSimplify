mesh_simp:mesh_simp.o SimpleObject.o Vec3f.o Simplify.h
	g++ mesh_simp.cpp SimpleObject.o  Vec3f.o -o mesh_simp
show:showModel.cpp SimpleObject.o Vec3f.o Simplify.h loadBMP.h
	g++ showModel.cpp SimpleObject.o Simplify.h loadBMP.h Vec3f.o -lGL -lGLU -lglut -o show
mesh_simp.o:mesh_simp.cpp Simplify.h SimpleObject.h
SimpleObject.o:SimpleObject.h SimpleObject.cpp Vec3f.h
	g++ SimpleObject.cpp SimpleObject.h -c -o SimpleObject.o
Vec3f.o:Vec3f.h Vec3f.cpp
clean:
	rm *.o
	rm *.gch
	
