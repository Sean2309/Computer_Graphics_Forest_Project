//----------------------------------------------------------------------------------------
/**
 * \file    render.h
 * \author  Sean Phay
 * \date    2023
 * \brief   Drawing functions for the model
 */
//----------------------------------------------------------------------------------------

#ifndef __RENDER_H
#define __RENDER_H

#include "data.h"

// defines geometry of object in the scene (space ship, ufo, etc.)
// geometry is shared among all instances of the same object type
typedef struct _MeshGeometry {
  GLuint        vertexBufferObject;   // identifier for the vertex buffer object
  GLuint        elementBufferObject;  // identifier for the element buffer object
  GLuint        vertexArrayObject;    // identifier for the vertex array object
  unsigned int  numTriangles;         // number of triangles in the mesh
  // material
  glm::vec3     ambient;
  glm::vec3     diffuse;
  glm::vec3     specular;
  float         shininess;
  GLuint        texture;

} MeshGeometry;

// parameters of individual objects in the scene (e.g. position, size, speed, etc.)
typedef struct _Object {
  glm::vec3 position;
  glm::vec3 direction;
  float     speed;
  float     size;

  bool destroyed;

  float startTime;
  float currentTime;

} Object;

typedef struct TerrainObject : public Object {

} TerrainObject;

typedef struct PenguinObject : public Object {

	float viewAngle; // in degrees

} PenguinObject;

typedef struct SparrowObject : public Object {

	float rotationSpeed;
	float currentAngle;

} SparrowObject;

typedef struct CatObject : public Object { 

} CatObject;

typedef struct RockObject : public Object { 

} RockObject;

typedef struct FernObject : public Object {

} FernObject;

typedef struct StoneObject : public Object { 

} StoneObject;

typedef struct TargetObject : public Object {
	glm::vec3 initPosition;
	float rotationSpeed;

} TargetObject;

typedef struct PalmTreeObject : public Object { 

} PalmTreeObject;

typedef struct CampfireObject : public Object {

} CampfireObject;

typedef struct BlockObject : public Object {

} BlockObject;

typedef struct MissileObject : public Object {

} MissileObject;

typedef struct UfoObject : public Object {

  float     rotationSpeed;
  glm::vec3 initPosition;

} UfoObject;

typedef struct ExplosionObject : public Object {

  int    textureFrames;
  float  frameDuration;

} ExplosionObject;

typedef struct BannerObject : public Object {

} BannerObject;

typedef struct _commonShaderProgram {
  // identifier for the shader program
  GLuint program;          // = 0;
  // vertex attributes locations
  GLint posLocation;       // = -1;
  GLint colorLocation;     // = -1;
  GLint normalLocation;    // = -1;
  GLint texCoordLocation;  // = -1;
  // uniforms locations
  GLint PVMmatrixLocation;    // = -1;
  GLint VmatrixLocation;      // = -1;  view/camera matrix
  GLint MmatrixLocation;      // = -1;  modeling matrix
  GLint normalMatrixLocation; // = -1;  inverse transposed Mmatrix

  GLint timeLocation;         // = -1; elapsed time in seconds

  //pointlight
  GLint pointLightLoc;
  GLint campfireLocation;
  GLint pointLightPosLoc;
  GLint pointLightAmbientLoc;
  GLint pointLightSpecularLoc;

  // fog
  GLint fogOnLinearLoc;
  GLint fogOnExpLoc;
  GLint fogOnNearLoc;
  GLint fogOnDensityLoc;

  // material 
  GLint diffuseLocation;    // = -1;
  GLint ambientLocation;    // = -1;
  GLint specularLocation;   // = -1;
  GLint shininessLocation;  // = -1;
  // texture
  GLint useTextureLocation; // = -1; 
  GLint texSamplerLocation; // = -1;
  // reflector related uniforms
  GLint reflectorPositionLocation;  // = -1; 
  GLint reflectorDirectionLocation; // = -1;
} SCommonShaderProgram;


glm::vec3 checkBounds(const glm::vec3 & position, float objectSize = 1.0f);
//WIP

void drawTerrain(TerrainObject* terrain, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawPenguin(PenguinObject* penguin, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawSparrow(SparrowObject* sparrow, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawCat(CatObject* cat, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawRock(RockObject* rock, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawFern(FernObject* fern, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawStone(StoneObject* stone, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawTarget(TargetObject* target, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawPalmTree(PalmTreeObject* palmTree, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawCampfire(CampfireObject* campfire, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawBlock(BlockObject* block, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

void drawMissile(MissileObject* missile, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawExplosion(ExplosionObject* explosion, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawBanner(BannerObject* banner, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawSkybox(const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);

void initializeShaderPrograms();
void cleanupShaderPrograms();

void initializeModels();
void cleanupModels();

#endif // __RENDER_H
