# <span style="color:#40E0D0"> Description </span> 

The project was an interesting project to take on. I created a Forest-like scene that features a few different objects, such as ferns, trees, a campfire and a few animals. Several camera positions are available in the scene. The First-Person View is mounted on a Penguin object. 2 other static views are also available. 

In terms of interactive elements, there are 2 main ones. Firstly, if any of the fern objects are left-clicked, an explosive animation will be triggered. Secondly, since a campfire is dangerous, running into it will cause the game to be over. An explosive animation will also be triggered.

The project was a great learning experience. I learned how to use the Assimp library to load in 3D models. I also learned how to use the GLM library to perform matrix transformations. I also learned how to use the stb_image library to load in textures.

--- 
<br>

# <span style="color:#40E0D0"> Controls </span> 

**The keyboard controls are as follows :** <br>
1. **`W`** :
    - Move forward.
2. **`S`** :
    - Move backward.
3. **`A`** :
    - Turn left.
4. **`D`** :
    - Turn right.
5. **`Q`** :
    - Move down (Negative z-direction).
6. **`E`** :
    - Move up (Positive z-direction).
7. **`R`** :
    - Restart Game and all Objects.
8. **`P`** :
    - Toggle Point Light On/Off
9. **`C`** :
    - Change Camera 
8. **`O`** :
    - Reload Config file
11. **`ESC`** :
    - Exit Game

**The mouse controls are as follows :** <br>
1. **`Left Click`** :
    - Click onto objects. Explosion is created if any Fern objects are clicked
2. **`Right Click`** :
    - Open menu

--- 
<br>

# <span style="color:#40E0D0"> Important Terms </span>

**View Matrix**
- Gives the camera view, showing the view of the camera.
- Helps understand where things are located and which direction they're facing.

**Projection Matrix**
- Acts as a special lens or screen of the camera.
- Transforms the 3D world into a 2D world on the screen.
- Determines how wide the angle is, how far things are, and how they appear on the screen.

**Model Matrix**
- Imagined as a set of instructions catered towards a particular object.
- Instructs the computer how to move, rotate, and scale that object in the 3D world.

**Inverse PV Matrix**
- Helps the computer understand where things in the 2D image belong in the 3D space around you.

--- 
<br>

# <span style="color:#40E0D0"> Breakdown of Code </span>

1. **Shader Program Init**:
   - Initializes the shader programs.
   - Includes lighting, textures, banners, explosions, and skybox rendering.

2. **Mesh Loading**:
   - Functions that load mesh data from external input files using the Assimp library.
   - Input files should be in .obj format with a .mtl file, and a .jpg (optional for jpg).
   - Loaded mesh geometries are organized into "MeshGeometry" objects.

3. **Geometry Init**:
   - Sets up geometry (VBO, VAO, EBO, etc.) for specific objects.
   - Examples include initUfoGeometry, initExplosionGeometry.
   - Involves creating and configuring OpenGL buffers and vertex arrays for vertex data management.

4. **Texture Loading**:
   - Textures linked to respective objects are loaded into OpenGL texture objects.

5. **Model Init**:
   - Main calling function for the initialization of individual objects: initializeModels().
   - Loads various models using the loaded mesh data and initializes their geometries.

6. **Cleanup**:
   - Cleanup functions delete allocated OpenGL resources (vertex arrays, buffers, textures) to prevent memory leaks.
   - Examples include cleanupGeometry, cleanupModels.

7. **Vertex Attributes & Rendering Setup**:
   - Functions used in the initialization process to set up vertex attributes for different shaders.
   - Prepares OpenGL for rendering, specifying how these attributes are organized and sent to the shaders.
