//----------------------------------------------------------------------------------------
/**
 * \file    data.h
 * \author  Sean Phay
 * \date    2023
 * \brief   Defining of variables & data structures.
 */
//----------------------------------------------------------------------------------------

#ifndef __DATA_H
#define __DATA_H

#define WINDOW_WIDTH   1080
#define WINDOW_HEIGHT  1080
#define WINDOW_TITLE   "Forest Scene"

// keys used in the key map
enum { KEY_LEFT_ARROW, KEY_RIGHT_ARROW, KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_SPACE, KEY_UP_HEIGHT, KEY_DOWN_HEIGHT, KEY_EXPLODE1, KEY_W, KEY_A, KEY_S, KEY_D, KEY_Q, KEY_E, KEY_P, KEY_O, KEYS_COUNT };

// WIP => define the params for the palm trees here
#define PALM_TREE_COUNT_MIN 5
#define PALM_TREE_COUNT_MAX 10
#define SPARROW_COUNT_MIN 1
#define SPARROW_COUNT_MAX 1
#define TARGET_COUNT_MIN 2
#define TARGET_COUNT_MAX 5

// Removed sections on asteroids and ufo
// missles can be used to throw another object
#define PENGUIN_VIEW_ANGLE_DELTA 2.5f
#define PENGUIN_SPEED_INCREMENT  0.1f
#define PENGUIN_LENGTH_INCREMENT  0.05f // TODO : config file
#define PENGUIN_SPEED_MAX        0.4f
#define PENGUIN_LENGTH_MAX        0.1f
#define PENGUIN_LENGTH_MIN       0.25f
#define PENGUIN_HEIGHT_MAX        1.0f
#define PENGUIN_HEIGHT_MIN        0.0f

#define MISSILE_MAX_DISTANCE       1.5f
#define MISSILE_LAUNCH_TIME_DELAY  0.25f // seconds

#define PENGUIN_SIZE        0.1f
#define SPARROW_SIZE     0.2f
#define CAT_SIZE         0.2f
#define TERRAIN_SIZE     1.0f
#define ROCK_SIZE        0.2f
#define TARGET_SIZE      0.1f
#define FERN_SIZE        0.25f
#define CAMPFIRE_SIZE    0.15f
#define BLOCK_SIZE       1.12f

#define PALM_TREE_SIZE   0.3f
#define MISSILE_SIZE     0.0085f
#define BILLBOARD_SIZE   0.1f
#define BANNER_SIZE      1.0f

#define SPARROW_SPEED_MAX          0.05f
#define MISSILE_SPEED              1.5f

#define SPARROW_ROTATION_SPEED_MAX  1.0f

#define SCENE_WIDTH  1.0f
#define SCENE_HEIGHT 1.0f
#define SCENE_DEPTH  1.0f

#define CAMERA_ELEVATION_MAX 45.0f

// default shaders - color per vertex and matrix multiplication
const std::string colorVertexShaderSrc(
    "#version 140\n"
    "uniform mat4 PVMmatrix;\n"
    "in vec3 position;\n"
    "in vec3 color;\n"
    "smooth out vec4 theColor;\n"
    "void main() {\n"
	"  gl_Position = PVMmatrix * vec4(position, 1.0);\n"
	"  theColor = vec4(color, 1.0);\n"
    "}\n"
);

const std::string colorFragmentShaderSrc(
    "#version 140\n"
    "smooth in vec4 theColor;\n"
    "out vec4 outputColor;\n"
    "void main() {\n"
    "  outputColor = theColor;\n"
    "}\n"
);

// each vertex shader receives screen space coordinates and calculates world direction
const std::string skyboxFarPlaneVertexShaderSrc(
  "#version 140\n"
  "\n"
  "uniform mat4 inversePVmatrix;\n"
  "in vec2 screenCoord;\n"
  "out vec3 texCoord_v;\n"
  "\n"
  "void main() {\n"
  "  vec4 farplaneCoord = vec4(screenCoord, 0.9999, 1.0);\n"
  "  vec4 worldViewCoord = inversePVmatrix * farplaneCoord;\n"
  "  texCoord_v = worldViewCoord.xyz / worldViewCoord.w;\n"
  "  gl_Position = farplaneCoord;\n"
  "}\n"
);

// fragment shader uses interpolated 3D tex coords to sample cube map
const std::string skyboxFarPlaneFragmentShaderSrc(
  "#version 140\n"
  "\n"
  "uniform samplerCube skyboxSampler;\n"
  "in vec3 texCoord_v;\n"
  "out vec4 color_f;\n"
  "\n"
  "void main() {\n"
  "  color_f = texture(skyboxSampler, texCoord_v);\n"
  "}\n"
);


//
// missile geometry definition 
//

const int missileTrianglesCount = 4;
// temp constants used for missileVertices array contents definition
const float invSqrt2 = (float)(1.0 / sqrt(2.0));

// missile is represented as a tetrahedron (polyhedron with 4 faces and 4 vertices)
const float missileVertices[] = {
  // non-interleaved array
  // vertices of tetrahedron, each face is an equilateral triangle, edge length 2.0

  // vertices
  //  1.0f,  0.0f, -invSqrt2,   -> vertex 0
  //  0.0f,  1.0f,  invSqrt2,   -> vertex 1
  // -1.0f,  0.0f, -invSqrt2,   -> vertex 2
  //  0.0f, -1.0f,  invSqrt2    -> vertex 3

  // three vertices per each triangle
   0.0f, -1.0f,  invSqrt2, // 3
   1.0f,  0.0f, -invSqrt2, // 0
   0.0f,  1.0f,  invSqrt2, // 1

  -1.0f,  0.0f, -invSqrt2, // 2
   0.0f, -1.0f,  invSqrt2, // 3
   0.0f,  1.0f,  invSqrt2, // 1

   1.0f,  0.0f, -invSqrt2, // 0
  -1.0f,  0.0f, -invSqrt2, // 2
   0.0f,  1.0f,  invSqrt2, // 1

  -1.0f,  0.0f, -invSqrt2, // 2
   1.0f,  0.0f, -invSqrt2, // 0
   0.0f, -1.0f,  invSqrt2, // 3

  // colors for vertices
  1.0f, 0.0f, 1.0f, // 3
  1.0f, 0.0f, 1.0f, // 0
  0.0f, 1.0f, 0.0f, // 1

  0.0f, 0.0f, 1.0f, // 2
  1.0f, 0.0f, 1.0f, // 3
  0.0f, 1.0f, 0.0f, // 1

  1.0f, 0.0f, 1.0f, // 0
  0.0f, 0.0f, 1.0f, // 2
  0.0f, 1.0f, 0.0f, // 1

  0.0f, 0.0f, 1.0f, // 2
  1.0f, 0.0f, 1.0f, // 0
  1.0f, 0.0f, 1.0f, // 3

  // normals
   1.0f, 0.0f, invSqrt2,
   1.0f, 0.0f, invSqrt2,
   1.0f, 0.0f, invSqrt2,

  -1.0f, 0.0f, invSqrt2,
  -1.0f, 0.0f, invSqrt2,
  -1.0f, 0.0f, invSqrt2,

   0.0f, 1.0f, -invSqrt2,
   0.0f, 1.0f, -invSqrt2,
   0.0f, 1.0f, -invSqrt2,

   0.0f, -1.0f, -invSqrt2,
   0.0f, -1.0f, -invSqrt2,
   0.0f, -1.0f, -invSqrt2,
};

//
// ufo geometry definition 
//

const int ufoTrianglesCount = 6;
// temp constants used for ufoVertices array contents definition
const float ufoH = 0.25f;
const float cos30d = (float)cos(M_PI/6.0);
const float sin30d = (float)sin(M_PI/6.0);

const float ufoVertices[] = {
  // ufo is formed by two parts (top and bottom) joined together
  // each part is drawn as six separate triangles connected together

  // drawArrays() part of data (top part), interleaved array
  // vertices 0..5 are on the border, vertex 6 is in the center
  // colors of the triangles alternate between yellow (1,1,0) and magenta (1,0,1)

  // interleaved array: position/color/normal
  //  x      y        z     r     g     b             nx     ny         nz
  // triangle 5 0 6 -> yellow color
   cos30d, 0.0f, -sin30d,  1.0f, 1.0f, 0.0f,          ufoH, 1.0f,         0.0f, // 5
     0.0f, ufoH,    0.0f,  1.0f, 1.0f, 0.0f,          ufoH, 1.0f,         0.0f, // 6
   cos30d, 0.0f,  sin30d,  1.0f, 1.0f, 0.0f,          ufoH, 1.0f,         0.0f, // 0
  // triangle 1 2 6 -> yellow color
     0.0f, 0.0f,    1.0f,  1.0f, 1.0f, 0.0f,  -ufoH*sin30d, 1.0f,  ufoH*cos30d, // 1
     0.0f, ufoH,    0.0f,  1.0f, 1.0f, 0.0f,  -ufoH*sin30d, 1.0f,  ufoH*cos30d, // 6
  -cos30d, 0.0f,  sin30d,  1.0f, 1.0f, 0.0f,  -ufoH*sin30d, 1.0f,  ufoH*cos30d, // 2
  // triangle 3 4 6 -> yellow color
  -cos30d, 0.0f, -sin30d,  1.0f, 1.0f, 0.0f,  -ufoH*sin30d, 1.0f, -ufoH*cos30d, // 3
     0.0f, ufoH,    0.0f,  1.0f, 1.0f, 0.0f,  -ufoH*sin30d, 1.0f, -ufoH*cos30d, // 6
     0.0f, 0.0f,   -1.0f,  1.0f, 1.0f, 0.0f,  -ufoH*sin30d, 1.0f, -ufoH*cos30d, // 4

  // triangle 0 1 6 -> magenta color
   cos30d, 0.0f,  sin30d,  1.0f, 0.0f, 1.0f,   ufoH*sin30d, 1.0f,  ufoH*cos30d, // 0
     0.0f, ufoH,    0.0f,  1.0f, 0.0f, 1.0f,   ufoH*sin30d, 1.0f,  ufoH*cos30d, // 6
     0.0f, 0.0f,    1.0f,  1.0f, 0.0f, 1.0f,   ufoH*sin30d, 1.0f,  ufoH*cos30d, // 1
  // triangle 2 3 6 -> magenta color
  -cos30d, 0.0f,  sin30d,  1.0f, 0.0f, 1.0f,         -ufoH, 1.0f,         0.0f, // 2
     0.0f, ufoH,    0.0f,  1.0f, 0.0f, 1.0f,         -ufoH, 1.0f,         0.0f, // 6
  -cos30d, 0.0f, -sin30d,  1.0f, 0.0f, 1.0f,         -ufoH, 1.0f,         0.0f, // 3
  // triangle 4 5 6 -> magenta color
     0.0f, 0.0f,   -1.0f,  1.0f, 0.0f, 1.0f,   ufoH*sin30d, 1.0f, -ufoH*cos30d, // 4
     0.0f, ufoH,    0.0f,  1.0f, 0.0f, 1.0f,   ufoH*sin30d, 1.0f, -ufoH*cos30d, // 6
   cos30d, 0.0f, -sin30d,  1.0f, 0.0f, 1.0f,   ufoH*sin30d, 1.0f, -ufoH*cos30d, // 5

  // draw elements data part (bottom part), interleaved array
  // vertices 0..5 are on the border, vertex 6 is in the center
  // vertices on the border have the same color while vertex in the middle differs

  //  x      y        z      r     g     b        nx      ny      nz
   cos30d,  0.0f,  sin30d,  1.0f, 0.0f, 1.0f,   ufoH*cos30d, -1.0f,  ufoH*sin30d, // 0
     0.0f,  0.0f,    1.0f,  1.0f, 0.0f, 1.0f,          0.0f, -1.0f,         1.0f, // 1
  -cos30d,  0.0f,  sin30d,  1.0f, 0.0f, 1.0f,  -ufoH*cos30d, -1.0f,  ufoH*sin30d, // 2
  -cos30d,  0.0f, -sin30d,  1.0f, 0.0f, 1.0f,  -ufoH*cos30d, -1.0f, -ufoH*sin30d, // 3
     0.0f,  0.0f,   -1.0f,  1.0f, 0.0f, 1.0f,          0.0f, -1.0f,        -1.0f, // 4
   cos30d,  0.0f, -sin30d,  1.0f, 0.0f, 1.0f,   ufoH*cos30d, -1.0f, -ufoH*sin30d, // 5
     0.0f, -ufoH,    0.0f,  0.3f, 0.3f, 0.3f,          0.0f, -1.0f,         0.0f, // 6

};

// indices of the 6 faces used to draw the bottom part of the ufo
const unsigned int ufoIndices[] = {

  // indices are shifted by 18 (18 vertices are stored before the second part vertices)
  18+5, 18+0, 18+6,
  18+0, 18+1, 18+6,
  18+1, 18+2, 18+6,
  18+2, 18+3, 18+6,
  18+3, 18+4, 18+6,
  18+4, 18+5, 18+6

};


//
// explosion billboard geometry definition 
//

const int explosionNumQuadVertices = 4;
const float explosionVertexData[explosionNumQuadVertices * 5] = {

  // x      y     z     u     v
  -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
   1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
  -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
   1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
};

//
// "game over" banner geometry definition 
//

const int bannerNumQuadVertices = 4;
const float bannerVertexData[bannerNumQuadVertices * 5] = {

  // x      y      z     u     v
  -1.0f,  0.15f, 0.0f, 0.0f, 1.0f,
  -1.0f, -0.15f, 0.0f, 0.0f, 0.0f,
   1.0f,  0.15f, 0.0f, 3.0f, 1.0f,
   1.0f, -0.15f, 0.0f, 3.0f, 0.0f
};

// rectangular block geometry definition

const int blockTrianglesCount = 30;

const float blockVertices[] = {
    // Vertices         // Colors        // Normals
    0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.5, 0.01, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.01, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.5, 0.02, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.02, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.5, 0.03, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.03, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.5, 0.04, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.04, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.5, 0.05, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.05, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.5, 0.06, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.06, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.5, 0.07, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.07, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.5, 0.08, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.08, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.5, 0.09, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.09, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.5, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.5, 0.11, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.11, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.5, 0.12, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.12, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.5, 0.13, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.13, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.5, 0.14, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.14, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.5, 0.15, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.15, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
};

const unsigned int blockIndices[] = {

    0, 1, 2,
    1, 3, 2,
    2, 3, 4,
    3, 5, 4,
    4, 5, 6,
    5, 7, 6,
    6, 7, 8,
    7, 9, 8,
    8, 9, 10,
    9, 11, 10,
    10, 11, 12,
    11, 13, 12,
    12, 13, 14,
    13, 15, 14,
    14, 15, 16,
    15, 17, 16,
    16, 17, 18,
    17, 19, 18,
    18, 19, 20,
    19, 21, 20,
    20, 21, 22,
    21, 23, 22,
    22, 23, 24,
    23, 25, 24,
    24, 25, 26,
    25, 27, 26,
    26, 27, 28,
    27, 29, 28,
    28, 29, 30,
    29, 31, 30

};



#endif // __DATA_H
