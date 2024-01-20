*Important Terms*

View Matrix : 
	- Gives the camera view => Shows you the view of the camera
	- Helps you understand where things are located and which direction they're facing.

Projection Matrix : 
	- Special lens or screen of the camera 
	- Transforms the 3D world into a 2D world on the screen
	- Helps decide how wide the angle is, how far things are, and how they look on your screen

Model Matrix : 
	- Imagine it as a set of instructions catered towards a particular object. 
	- Tells the computer how to move, rotate, and scale that object in the 3D world.

Inverse PV Matrix : 
	- Helps the computer understand where things in the 2D image (like the photo) belong in the 3D space around you





### *Flow of Code* :
1. main.cpp :
	a. main() 
	b. 




*Breakdown of Code*
1. Shader Program Init:
	- Inits the shader programs 
	- E.g lighting, textures, banners, explosions & skybox rendering

2. Mesh Loading 
	- Includes functions that load mesh data from external input files using Assimp library
	- Input files should be in .obj format with a .mtl file, a .jpg(optional for jpg) accompanying
	- Loaded mesh geometries are then organised into data structures => "MeshGeometry" objects

3. Geometry Init
	- Used to set up geometry (VBO, VAO, EBO etc) for specific objects
	- E.g initUfoGeometry, initExplosionGeometry etc
	- It involves creating + configuring OpenGL buffers and vertex arrays to store and manage vertex data

4. Texture Loading
	- Textures linked to the respective objects are loaded into OpenGL texture objects

5. Model init
	- Main calling function for the init of the individual objects => initializeModels()
	- This loads the various models using the loaded mesh data and initialises their geometries

6. Cleanup 
	- Cleanup functions which delete the allocated OpenGL resources (vertex arrays, buffers, textures). This is to prevent memory leaks
	- E.g cleanupGeometry, cleanupModels

7. Vertex Attributes & Rendering Setup
	- Functions used in the init process to set up vertex attributes for different shaders 
		=> i.e positions, colors, normals, texture coordinates
	- Also prepares OpenGL for rendering by specifying how these attributes are organised and sent to the shaders