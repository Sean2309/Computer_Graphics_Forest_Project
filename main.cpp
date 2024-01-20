//----------------------------------------------------------------------------------------
/**
 * \file    main.cpp
 * \author  Sean Phay
 * \date    2023
 * \brief   Main file of Rainforest Scene Project
 */
 //----------------------------------------------------------------------------------------

#include <iostream>
#include <glm/glm.hpp>
#include <time.h>
#include <list>
#include "pgr.h"
#include "render.h"
#include "spline.h"
#include "data.h"
#include "INIReader.h"
#include <unordered_map>
#include <string>

 //----------------------------------------------------------------------------------------
// START OF INITIALIZING VARIABLES
extern SCommonShaderProgram shaderProgram;
extern bool useLighting;

typedef std::list<void *> GameObjectsList;

struct GameState {

	int windowWidth;    // set by reshape callback
	int windowHeight;   // set by reshape callback

	int cameraState = 0;        // true;
	float cameraElevationAngle; // in degrees = initially 0.0f

	bool gameOver;              // false;
	bool keyMap[KEYS_COUNT];    // false

	float elapsedTime;
	float missileLaunchTime;
	float ufoMissileLaunchTime;

} gameState;

struct GameObjects {

  TerrainObject *terrain;
  PenguinObject *penguin;
  SparrowObject *sparrow;
  CatObject *cat;
  RockObject *rock;
  StoneObject *stone;
  FernObject *fern1;
  FernObject* fern2;
  FernObject* fern3;
  FernObject* fern4;
  PalmTreeObject *palmTree1; 
  PalmTreeObject* palmTree2;
  PalmTreeObject* palmTree3;
  PalmTreeObject* palmTree4;
  CampfireObject *campfire;
  BlockObject* block;

  GameObjectsList targets;
  GameObjectsList missiles;
  GameObjectsList ufos;
  

  GameObjectsList explosions;
  BannerObject* bannerObject; // NULL;
} gameObjects;


int pointEnable = 0;
glm::vec3 pointLightPos = glm::vec3(0.0f, -0.5f, 0.05f);
glm::vec3 pointLightAmbient = glm::vec3(0.2f);
glm::vec3 pointLightSpecular = glm::vec3(1.0f);

bool fogLinearToggleInput = false;
bool fogExpToggleInput = false;
float fogNearValue = -0.5f;
float fogDensityValue = 5.0f;



// END OF INITIALIZING VARIABLES
//----------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------
// START OF CONFIG PARSING

std::unordered_map<std::string, float> readConfig(const std::string& filename) {
	INIReader reader(filename);
	std::unordered_map<std::string, float> configValues;

	if (reader.ParseError() < 0) {
		std::cout << "Can't load " << filename << std::endl;
		return configValues; // Returns an empty map if there's an error
	}

	// Reading cat configuration
	configValues["Cat.position_x"] = reader.GetReal("Cat", "position_x", 0.15f);
	configValues["Cat.position_y"] = reader.GetReal("Cat", "position_y", 0.65f);
	configValues["Cat.position_z"] = reader.GetReal("Cat", "position_z", 0.12f);
	configValues["Cat.size"] = reader.GetReal("Cat", "size", CAT_SIZE);

	// Reading fern configuration
	configValues["Fern.size"] = reader.GetReal("Fern", "size", FERN_SIZE);

	// Reading campfire configuration
	configValues["Campfire.size"] = reader.GetReal("Campfire", "size", CAMPFIRE_SIZE);

	return configValues;
}

void reloadConfig() {
	// Read the configuration
	auto configValues = readConfig("config.ini");

	// Update game objects based on the new configuration
	// Similar to how you handle it in the restartGame function
	// For example, updating the cat object:
	if (gameObjects.cat != NULL) {
		gameObjects.cat->size = configValues["Cat.size"];
		gameObjects.cat->position = glm::vec3(
			configValues["Cat.position_x"],
			configValues["Cat.position_y"],
			configValues["Cat.position_z"]
		);
	}

	gameObjects.fern1->size = configValues["Fern.size"];
	gameObjects.fern2->size = configValues["Fern.size"];

	gameObjects.campfire->size = configValues["Cat.size"];
}


// END OF CONFIG PARSING
// ---------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// START OF COLLISION HELPER FUNCTIONS
/// Checks whether a given point is inside a sphere or not.
/**
\param[in]  point      Point to be tested.
\param[in]  center     Center of the sphere.
\param[in]  radius     Radius of the sphere.
\return                True if the point lies inside the sphere, otherwise false.
*/
bool pointInSphere(const glm::vec3& point, const glm::vec3& center, float radius) {
	if (glm::distance(point, center) <= radius)
		return true;
	else
	return false;
}

/// Checks if there is intersection between two given spheres or not.
/**
\param[in]  center1    First sphere center.
\param[in]  radius1    First sphere radius.
\param[in]  center2    Second sphere center.
\param[in]  radius2    Second sphere radius.
\return                True if the spheres overlap, otherwise false.
*/
bool spheresIntersection(const glm::vec3& center1, float radius1, const glm::vec3& center2, float radius2) {
	//   check whether given spheres are intersecting each other or not
	if (glm::distance(center1, center2) < radius1 + radius2)
		return true;
	else
		return false;
}
// END OF COLLISION HELPER FUNCTIONS
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// START OF TELEPORT + GENERATE RANDOM POSITION FUNCTIONS


glm::vec3 generateRandomPosition(void) {
	glm::vec3 newPosition;
	bool invalidPosition = false;

	do {

		// position is generated randomly
		// coordinates are in range -1.0f ... 1.0f
		newPosition = glm::vec3(
			(float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
			(float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
			-2.0f
		);
		invalidPosition = pointInSphere(newPosition, gameObjects.penguin->position, 3.0f * PENGUIN_SIZE);

	} while (invalidPosition == true);

	return newPosition;
}

void teleport(void) {
	// generate new space ship position randomly
	gameObjects.penguin->position = glm::vec3(
		(float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
		(float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
		0.0f
	);
}
// END OF TELEPORT + GENERATE RANDOM POSITION FUNCTIONS
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// START OF CREATING OBJECT FUNCTIONS

TargetObject* createTarget(void) {
	TargetObject* newTarget= new TargetObject;

	newTarget->destroyed = false;

	newTarget->startTime = gameState.elapsedTime;
	newTarget->currentTime = newTarget->startTime;

	newTarget->size = TARGET_SIZE;

	newTarget->initPosition = glm::vec3(-0.1f, 0.7f, 0.12f);
	newTarget->position = newTarget->initPosition;

	return newTarget;
}


void createMissile(const glm::vec3& missilePosition, const glm::vec3& missileDirection, float& missileLaunchTime) {

	float currentTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME); // milliseconds => seconds
	if (currentTime - missileLaunchTime < MISSILE_LAUNCH_TIME_DELAY)
		return;

	missileLaunchTime = currentTime;

	MissileObject* newMissile = new MissileObject;

	newMissile->destroyed = false;
	newMissile->startTime = gameState.elapsedTime;
	newMissile->currentTime = newMissile->startTime;
	newMissile->size = MISSILE_SIZE;
	newMissile->speed = MISSILE_SPEED;
	newMissile->position = missilePosition;
	newMissile->direction = glm::normalize(missileDirection);

	gameObjects.missiles.push_back(newMissile);
}

BannerObject* createBanner(void) {
 BannerObject* newBanner = new BannerObject;
 
  newBanner->size = BANNER_SIZE;
  newBanner->position = glm::vec3(0.0f, 0.0f, 0.0f);
  newBanner->direction = glm::vec3(0.0f, 1.0f, 0.0f);
  newBanner->speed = 0.0f;
  newBanner->size = 1.0f;

  newBanner->destroyed = false;

  newBanner->startTime = gameState.elapsedTime;
  newBanner->currentTime = newBanner->startTime;

  return newBanner;
}

// END OF CREATING OBJECT FUNCTIONS
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// START OF CLEAN UP + RESTART GAME FUNCTIONS
void cleanUpObjects(void) {

	// delete explosions
	while (!gameObjects.explosions.empty()) {
		delete gameObjects.explosions.back();
		gameObjects.explosions.pop_back();
	}

	// remove banner
	if (gameObjects.bannerObject != NULL) {
		delete gameObjects.bannerObject;
		gameObjects.bannerObject = NULL;
	}
}

bool checkValiditySize(float size) {
    return size > 0.1f && size < 2.0f;
}

bool checkValidityPosition(const glm::vec3& position) {
    return position.x >= -1.0f && position.x <= 1.0f &&
           position.y >= -1.0f && position.y <= 1.0f &&
           position.z >= -1.0f && position.z <= 1.0f;
}

void restartGame(void) {

	cleanUpObjects();

	// Constant Values
	fogLinearToggleInput = false;
	fogExpToggleInput = false;
	fogNearValue = -0.5f;
	fogDensityValue = 0.1f;

    // Read the configuration
    auto configValues = readConfig("config.ini");

	gameState.elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME); // milliseconds => seconds

	// init terrain
	if (gameObjects.terrain == NULL)
		gameObjects.terrain = new TerrainObject;

	gameObjects.terrain->position = glm::vec3(0.0f, 0.0f, 0.0f);
	gameObjects.terrain->size = TERRAIN_SIZE;

	// initialize penguin
	if(gameObjects.penguin == NULL)
		gameObjects.penguin = new PenguinObject;

	gameObjects.penguin->position = glm::vec3(0.0f, 0.0f, 0.08f);
	gameObjects.penguin->viewAngle = 100.0f; // degrees
	gameObjects.penguin->direction = glm::vec3(cos(glm::radians(gameObjects.penguin->viewAngle )), sin(glm::radians(gameObjects.penguin->viewAngle)), 0.0f);
	gameObjects.penguin->speed = 0.0f;
	gameObjects.penguin->size = PENGUIN_SIZE;
	gameObjects.penguin->destroyed = false;
	gameObjects.penguin->startTime = gameState.elapsedTime;
	gameObjects.penguin->currentTime = gameObjects.penguin->startTime;

	std::cout << "Elapsed Time" << gameState.elapsedTime << std::endl;

	// init sparrow
	if (gameObjects.sparrow == NULL)
		gameObjects.sparrow = new SparrowObject;

	gameObjects.sparrow->position = glm::vec3(0.0f, 0.0f, 0.1f);
	gameObjects.sparrow->currentAngle = 0.0f;
	gameObjects.sparrow->size = SPARROW_SIZE;

	// init cat
	if (gameObjects.cat == NULL)
        gameObjects.cat = new CatObject;
	glm::vec3 tempCatPos = glm::vec3(
		configValues["Cat.position_x"],
		configValues["Cat.position_y"],
		configValues["Cat.position_z"]
	);

    // Using the values from the config
	if (checkValiditySize(configValues["Cat.size"]) && (checkValidityPosition(tempCatPos))) {
		gameObjects.cat->size = configValues["Cat.size"];
		gameObjects.cat->position = tempCatPos;
	}
	else {
		gameObjects.cat->size = CAT_SIZE;
		gameObjects.cat->position = glm::vec3(0.15, 0.65, 0.12);
	}

	// init rock
	if (gameObjects.rock == NULL)
		gameObjects.rock = new RockObject;

	gameObjects.rock->position = glm::vec3(0.5f, 0.0f, 0.1f);
	gameObjects.rock->size = ROCK_SIZE;

	// init stone
	if (gameObjects.stone == NULL)
		gameObjects.stone = new StoneObject;

	gameObjects.stone->position = glm::vec3(-0.45f, 0.6f, 0.15f);
	gameObjects.stone->direction = glm::vec3(-0.1f, 0.7f, 0.12f);
	gameObjects.stone->size = ROCK_SIZE;

	// init palm tree
	if (gameObjects.palmTree1 == NULL)
		gameObjects.palmTree1 = new PalmTreeObject;

	gameObjects.palmTree1->position = glm::vec3(0.45f, 0.3f, 0.26f);
	gameObjects.palmTree1->size = PALM_TREE_SIZE;
	gameObjects.palmTree1->destroyed = false;

	if (gameObjects.palmTree2 == NULL)
		gameObjects.palmTree2 = new PalmTreeObject;

	gameObjects.palmTree2->position = glm::vec3(0.45f, 0.65f, 0.26f);
	gameObjects.palmTree2->size = PALM_TREE_SIZE;
	gameObjects.palmTree2->destroyed = false;

	if (gameObjects.palmTree3 == NULL)
		gameObjects.palmTree3 = new PalmTreeObject;

	gameObjects.palmTree3->position = glm::vec3(-0.65f, 0.3f, 0.26f);
	gameObjects.palmTree3->size = PALM_TREE_SIZE;
	gameObjects.palmTree3->destroyed = false;

	if (gameObjects.palmTree4 == NULL)
		gameObjects.palmTree4 = new PalmTreeObject;

	gameObjects.palmTree4->position = glm::vec3(-0.65f, 0.65f, 0.26f);
	gameObjects.palmTree4->size = PALM_TREE_SIZE;
	gameObjects.palmTree4->destroyed = false;

	// init fern
	if (gameObjects.fern1 == NULL)
		gameObjects.fern1 = new FernObject;

	gameObjects.fern1->position = glm::vec3(0.35f, 0.0f, -0.00005f);

	if (checkValiditySize(configValues["Fern.size"])) {
		gameObjects.fern1->size = configValues["Fern.size"];
	}
	else {
		gameObjects.fern1->size = FERN_SIZE;
	}
	

	if (gameObjects.fern2 == NULL)
		gameObjects.fern2 = new FernObject;

	gameObjects.fern2->position = glm::vec3(0.35f, -0.3f, -0.00005f);

	if (checkValiditySize(configValues["Fern.size"])) {
		gameObjects.fern2->size = configValues["Fern.size"];
	}
	else {
		gameObjects.fern2->size = FERN_SIZE;
	}

	if (gameObjects.fern3 == NULL)
		gameObjects.fern3 = new FernObject;

	gameObjects.fern3->position = glm::vec3(-0.35f, 0.0f, -0.00005f);
	gameObjects.fern3->size = FERN_SIZE;

	if (gameObjects.fern4 == NULL)
		gameObjects.fern4 = new FernObject;

	gameObjects.fern4->position = glm::vec3(-0.35f, -0.3f, -0.00005f);
	gameObjects.fern4->size = FERN_SIZE;

	// init campfire
	if (gameObjects.campfire == NULL)
		gameObjects.campfire = new CampfireObject;

	gameObjects.campfire->position = glm::vec3(0.0f, -0.5f, 0.05f);
	gameObjects.campfire->size = configValues["Campfire.size"];
	gameObjects.campfire->destroyed = false;

	// init block
	if (gameObjects.block == NULL)
		gameObjects.block = new BlockObject;
	
	gameObjects.block->position = glm::vec3(0.69f, -0.45f, 0.05f);
	gameObjects.block->size = BLOCK_SIZE;
	gameObjects.block->direction = glm::vec3(
		0.0f,
		1.0f,
		0.0f
	);
	gameObjects.block->direction = glm::normalize(gameObjects.block->direction);


	// init target
	for (int i = 0; i<TARGET_COUNT_MIN; i++) {
		TargetObject* newTarget = createTarget();

		gameObjects.targets.push_back(newTarget);
	}

	if(gameState.cameraState == true) {
		gameState.cameraState = false;
		glutPassiveMotionFunc(NULL);
	}
	gameState.cameraElevationAngle = 0.0f;

	// reset key map
	for(int i=0; i<KEYS_COUNT; i++)
		gameState.keyMap[i] = false;

	gameState.gameOver = false;
	//   gameState.missileLaunchTime = -MISSILE_LAUNCH_TIME_DELAY;
	//   gameState.ufoMissileLaunchTime = -MISSILE_LAUNCH_TIME_DELAY;
}

// END OF CLEAN UP + RESTART GAME FUNCTIONS
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// START OF INSERT EXPLOSION FUNCTION

void insertExplosion(const glm::vec3 &position) {

	ExplosionObject* newExplosion = new ExplosionObject;

	newExplosion->speed = 0.0f;
	newExplosion->destroyed = false;

	newExplosion->startTime = gameState.elapsedTime;
	newExplosion->currentTime = newExplosion->startTime;

	newExplosion->size = BILLBOARD_SIZE;
	newExplosion->direction = glm::vec3(0.0f, 0.0f, 1.0f);

	newExplosion->frameDuration = 0.1f;
	newExplosion->textureFrames = 16;

	newExplosion->position = position;

	gameObjects.explosions.push_back(newExplosion);
}

// END OF INSERT EXPLOSION FUNCTION
//----------------------------------------------------------------------------------------

//------------------------------------------------------------------cameraState--------------------
// START OF MANIPULATING PENGUIN VIEW (Top Down View)
bool checkTreeCollisions(glm::vec3 tempPos) {
	if (gameObjects.palmTree1->destroyed == false) {
		if (spheresIntersection(tempPos, gameObjects.penguin->size, gameObjects.palmTree1->position, gameObjects.palmTree1->size) == true) {
			//gameObjects.penguin->position -= glm::vec3(0.05, 0.05, 0);
			return true;
		}
	}
	if (gameObjects.palmTree2->destroyed == false) {
		if (spheresIntersection(tempPos, gameObjects.penguin->size, gameObjects.palmTree2->position, gameObjects.palmTree2->size) == true) {

			//gameObjects.penguin->position -= glm::vec3(0.05, 0.05, 0);
			return true;
		}
	}
	if (gameObjects.palmTree3->destroyed == false) {
		if (spheresIntersection(tempPos, gameObjects.penguin->size, gameObjects.palmTree3->position, gameObjects.palmTree3->size) == true) {
			//gameObjects.penguin->position -= glm::vec3(0.05, 0.05, 0);
			return true;
		}
	}
	if (gameObjects.palmTree4->destroyed == false) {
		if (spheresIntersection(tempPos, gameObjects.penguin->size, gameObjects.palmTree4->position, gameObjects.palmTree4->size) == true) {
			//gameObjects.penguin->position -= glm::vec3(0.05, 0.05, 0);
			return true;
		}
	}
}

void increaseBirdSpeed(float deltaSpeed = PENGUIN_SPEED_INCREMENT) {
	gameObjects.penguin->speed = std::min(gameObjects.penguin->speed + deltaSpeed, PENGUIN_SPEED_MAX);
}

void decreaseBirdSpeed(float deltaSpeed = PENGUIN_SPEED_INCREMENT) {

	gameObjects.penguin->speed =
		std::max(gameObjects.penguin->speed - deltaSpeed, 0.0f);
}

void increaseBirdHeight(float deltaLength = PENGUIN_LENGTH_INCREMENT) {
	//gameObjects.penguin->position.z = std::min(gameObjects.penguin->position.z + deltaLength, bird_HEIGHT_MAX);
	gameObjects.penguin->position.z = gameObjects.penguin->position.z + deltaLength;
}

void decreaseBirdHeight(float deltaLength = PENGUIN_LENGTH_INCREMENT) {
	gameObjects.penguin->position.z = std::max(gameObjects.penguin->position.z - deltaLength, PENGUIN_HEIGHT_MIN);
}

void moveBirdForward(float deltaLength = PENGUIN_LENGTH_INCREMENT) {
	glm::vec3 tempPos = gameObjects.penguin->position + deltaLength * gameObjects.penguin->direction;
	if (!gameObjects.penguin->destroyed && !checkTreeCollisions(tempPos)) {
		gameObjects.penguin->position = tempPos;
	} 
}

void moveBirdBackward(float deltaLength = PENGUIN_LENGTH_INCREMENT) {
	glm::vec3 tempPos = gameObjects.penguin->position - deltaLength * gameObjects.penguin->direction;
	if (!gameObjects.penguin->destroyed && !checkTreeCollisions(tempPos)) {
		gameObjects.penguin->position = tempPos;
	}
}

void turnBirdLeft(float deltaAngle) {

	gameObjects.penguin->viewAngle += deltaAngle;

	if (gameObjects.penguin->viewAngle > 360.0f)
		gameObjects.penguin->viewAngle -= 360.0f;

	float angle = glm::radians(gameObjects.penguin->viewAngle);

	gameObjects.penguin->direction.x = cos(angle);
	gameObjects.penguin->direction.y = sin(angle);
}

void turnBirdRight(float deltaAngle) {

	gameObjects.penguin->viewAngle -= deltaAngle;

	if (gameObjects.penguin->viewAngle < 0.0f)
		gameObjects.penguin->viewAngle += 360.0f;

	float angle = glm::radians(gameObjects.penguin->viewAngle);

	gameObjects.penguin->direction.x = cos(angle);
	gameObjects.penguin->direction.y = sin(angle);
}

// START OF MANIPULATING PENGUIN VIEW (Top Down View)
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// START OF DRAWWINDOWCONTENTS FUNCTION

void drawWindowContents() {

	// setup parallel projection
	glm::mat4 orthoProjectionMatrix = glm::ortho(
		-SCENE_WIDTH, SCENE_WIDTH,
		-SCENE_HEIGHT, SCENE_HEIGHT,
		-10.0f*SCENE_DEPTH, 10.0f*SCENE_DEPTH
		);
	// static viewpoint - top view
	glm::mat4 orthoTopViewMatrix = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
		);

	// static viewpoint - side view
	glm::mat4 orthoSideViewSMatrix = glm::lookAt(
		glm::vec3(0.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	// setup camera & projection transform
	glm::mat4 viewMatrix = orthoTopViewMatrix;
	glm::mat4 projectionMatrix = orthoProjectionMatrix;

	// setup camera angles here
	if (gameState.cameraState == 0) { // top camera

		glm::vec3 cameraPosition = glm::vec3(-1.0, 1.0, 1.0);
		glm::vec3 cameraUpVector = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 cameraCenter;

		glm::vec3 cameraViewDirection = glm::vec3(1.0, -1.0, -1.0);

		glm::vec3 rotationAxis = glm::cross(cameraViewDirection, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 cameraTransform = glm::rotate(glm::mat4(1.0f), glm::radians(gameState.cameraElevationAngle), rotationAxis);

		cameraUpVector = glm::vec3(cameraTransform * glm::vec4(cameraUpVector, 0.0f));
		cameraViewDirection = glm::vec3(cameraTransform * glm::vec4(cameraViewDirection, 0.0f));

		cameraCenter = cameraPosition + cameraViewDirection;

		viewMatrix = glm::lookAt(
			cameraPosition,
			cameraCenter,
			cameraUpVector
		);

		projectionMatrix = glm::perspective(glm::radians(60.0f), gameState.windowWidth / (float)gameState.windowHeight, 0.1f, 10.0f);
	}

	if (gameState.cameraState == 1) { // penguin camera

		glm::vec3 cameraPosition = gameObjects.penguin->position;
		glm::vec3 cameraUpVector = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 cameraCenter;

		glm::vec3 cameraViewDirection = gameObjects.penguin->direction;

		glm::vec3 rotationAxis = glm::cross(cameraViewDirection, cameraUpVector); // glm::vec3(0.0f, 0.0f, 1.0f)
		glm::mat4 cameraTransform = glm::rotate(glm::mat4(1.0f), glm::radians(gameState.cameraElevationAngle), rotationAxis);

		cameraUpVector = glm::vec3(cameraTransform * glm::vec4(cameraUpVector, 0.0f));
		cameraViewDirection = glm::vec3(cameraTransform * glm::vec4(cameraViewDirection, 0.0f));

		cameraCenter = cameraPosition + cameraViewDirection;

		viewMatrix = glm::lookAt(
			cameraPosition,
			cameraCenter,
			cameraUpVector
		);

		projectionMatrix = glm::perspective(glm::radians(60.0f), gameState.windowWidth / (float)gameState.windowHeight, 0.1f, 10.0f);
	}

	if (gameState.cameraState == 2) { // side camera

		glm::vec3 cameraPosition = glm::vec3(0.5, 0.5, 1.0);
		glm::vec3 cameraUpVector = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 cameraCenter;

		glm::vec3 cameraViewDirection = glm::vec3(-1.0, 0.0, -1.0);

		glm::vec3 rotationAxis = glm::cross(cameraViewDirection, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 cameraTransform = glm::rotate(glm::mat4(1.0f), glm::radians(gameState.cameraElevationAngle), rotationAxis);

		cameraUpVector = glm::vec3(cameraTransform * glm::vec4(cameraUpVector, 0.0f));
		cameraViewDirection = glm::vec3(cameraTransform * glm::vec4(cameraViewDirection, 0.0f));

		cameraCenter = cameraPosition + cameraViewDirection;

		viewMatrix = glm::lookAt(
			cameraPosition,
			cameraCenter,
			cameraUpVector
		);

		projectionMatrix = glm::perspective(glm::radians(60.0f), gameState.windowWidth / (float)gameState.windowHeight, 0.1f, 10.0f);
	}

	// setting up sun
	glUseProgram(shaderProgram.program);
	//oricode
	glUniform1f(shaderProgram.timeLocation, gameState.elapsedTime);

	glUniform3fv(shaderProgram.reflectorPositionLocation, 1, glm::value_ptr(gameObjects.penguin->position - 0.1f * gameObjects.penguin->direction)); // light pos
	glUniform3fv(shaderProgram.reflectorDirectionLocation, 1, glm::value_ptr(gameObjects.penguin->direction)); // light facing direction

	glUniform1i(shaderProgram.pointLightLoc, pointEnable);
	glUniform3fv(shaderProgram.pointLightPosLoc, 1, glm::value_ptr(pointLightPos));
	glUniform3fv(shaderProgram.pointLightAmbientLoc, 1, glm::value_ptr(pointLightAmbient));
	glUniform3fv(shaderProgram.pointLightSpecularLoc, 1, glm::value_ptr(pointLightSpecular));

	glUniform1f(shaderProgram.fogOnLinearLoc, fogLinearToggleInput);
	glUniform1f(shaderProgram.fogOnExpLoc, fogExpToggleInput);
    glUniform1f(shaderProgram.fogOnNearLoc, fogNearValue);
    glUniform1f(shaderProgram.fogOnDensityLoc, fogDensityValue);

	glUseProgram(0);



	/*glUniform3fv(shaderProgram.reflectorPositionLocation, 1, glm::value_ptr(gameObjects.balloon->position));
	glUniform3fv(shaderProgram.reflectorDirectionLocation, 1, glm::value_ptr(gameObjects.balloon->direction * gameObjects.balloon->enable_spot));
	glUniform1f(shaderProgram.fogDensityLocation, fogDensity * fogEnable);
	glUniform1i(shaderProgram.pointLocation, pointEnable);
	glUseProgram(0);*/


	// drawing object functions here
	drawPenguin(gameObjects.penguin, viewMatrix, projectionMatrix);
	drawTerrain(gameObjects.terrain, viewMatrix, projectionMatrix);
	drawSparrow(gameObjects.sparrow, viewMatrix, projectionMatrix);

	drawCat(gameObjects.cat, viewMatrix, projectionMatrix);
	drawRock(gameObjects.rock, viewMatrix, projectionMatrix);
	drawStone(gameObjects.stone, viewMatrix, projectionMatrix);

	drawPalmTree(gameObjects.palmTree1, viewMatrix, projectionMatrix);
	drawPalmTree(gameObjects.palmTree2, viewMatrix, projectionMatrix);
	drawPalmTree(gameObjects.palmTree3, viewMatrix, projectionMatrix);
	drawPalmTree(gameObjects.palmTree4, viewMatrix, projectionMatrix);

	drawCampfire(gameObjects.campfire, viewMatrix, projectionMatrix);
	drawBlock(gameObjects.block, viewMatrix, projectionMatrix);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	CHECK_GL_ERROR();

	int id = 0;
	for (GameObjectsList::iterator it = gameObjects.targets.begin(); it != gameObjects.targets.end(); ++it) {
		glStencilFunc(GL_ALWAYS, id + 1, -1);
		CHECK_GL_ERROR();

		TargetObject* target = (TargetObject*)(*it);
		drawTarget(target, viewMatrix, projectionMatrix);

		id++;
	}

	// disable stencil test
	glDisable(GL_STENCIL_TEST);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glStencilFunc(GL_ALWAYS, 2, 0xFF);
	drawFern(gameObjects.fern1, viewMatrix, projectionMatrix);
	glDisable(GL_STENCIL_TEST);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 3, 0xFF);
	drawFern(gameObjects.fern2, viewMatrix, projectionMatrix);
	glDisable(GL_STENCIL_TEST);

	glEnable(GL_STENCIL_TEST); 
	glStencilFunc(GL_ALWAYS, 4, 0xFF);
	drawFern(gameObjects.fern3, viewMatrix, projectionMatrix);
	glDisable(GL_STENCIL_TEST);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 5, 0xFF);
	drawFern(gameObjects.fern4, viewMatrix, projectionMatrix);
	glDisable(GL_STENCIL_TEST);

	CHECK_GL_ERROR();

	// draw missiles
	for (GameObjectsList::iterator it = gameObjects.missiles.begin(); it != gameObjects.missiles.end(); ++it) {
		MissileObject* missile = (MissileObject *)(*it);
		drawMissile(missile, viewMatrix, projectionMatrix);
	}

	// draw skybox
	drawSkybox(viewMatrix, projectionMatrix);

	// draw explosions with depth test disabled
	glDisable(GL_DEPTH_TEST);

	for (GameObjectsList::iterator it = gameObjects.explosions.begin(); it != gameObjects.explosions.end(); ++it) {
		ExplosionObject* explosion = (ExplosionObject *)(*it);
		drawExplosion(explosion, viewMatrix, projectionMatrix);
	}
	glEnable(GL_DEPTH_TEST);

	if (gameState.gameOver == true) {
		// draw game over banner
		if (gameObjects.bannerObject != NULL)
			drawBanner(gameObjects.bannerObject, orthoTopViewMatrix, orthoProjectionMatrix);
	}
}

// END OF DRAWWINDOWCONTENTS FUNCTION
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// START OF CALLBACK FUNCTIONS

// Called to update the display. You should call glutSwapBuffers after all of your
// rendering to display what you rendered.
void displayCallback() {
	GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
	mask |= GL_STENCIL_BUFFER_BIT;

	glClear(mask);

	drawWindowContents();

	glutSwapBuffers();
}

// Called whenever the window is resized. The new window size is given, in pixels.
// This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshapeCallback(int newWidth, int newHeight) {

	gameState.windowWidth = newWidth;
	gameState.windowHeight = newHeight;

	glViewport(0, 0, (GLsizei)newWidth, (GLsizei)newHeight);
}

// Test collisons between objects in the scene and insert explosion billboards.
void checkCollisions(void) {

	if (gameObjects.campfire->destroyed == false) {
		if (spheresIntersection(gameObjects.penguin->position, gameObjects.penguin->size, gameObjects.campfire->position, gameObjects.campfire->size) == true) {
			gameObjects.penguin->destroyed = true;
			insertExplosion(gameObjects.campfire->position);
			gameState.gameOver = true;
		}
	}



	//// test collisions between asteroid and penguin
	//GameObjectsList::iterator it;

	//for (it = gameObjects.targets.begin(); it != gameObjects.targets.end(); ++it) {
	//	TargetObject * target = (TargetObject *)(*it);

	//	if (target->destroyed == false) {
	//		// check whether a given target collides with penguin or not
	//		// calls spheresIntersection() function with appropriate parameters
	//		if (spheresIntersection(gameObjects.penguin->position, gameObjects.penguin->size, target->position, target->size) == true) {
	//			target->destroyed = true;	                       // mark asteroid dead
	//			insertExplosion(gameObjects.penguin->position);  // insert explostion billboard
	//			gameState.gameOver = true;                         // -> game over
	//		}
	//	}
	//}
	
}

void updateObjects(float elapsedTime) {

	// update penguin 
	float timeDelta = elapsedTime - gameObjects.penguin->currentTime;
	gameObjects.penguin->currentTime = elapsedTime;

	// check the new position and wrap it if it is necessary
	gameObjects.penguin->position = checkBounds(gameObjects.penguin->position, gameObjects.penguin->size);

	// update missiles
	GameObjectsList::iterator it = gameObjects.missiles.begin();
	while (it != gameObjects.missiles.end()) {
		MissileObject* missile = (MissileObject*)(*it);

		// update missile
		float timeDelta = elapsedTime - missile->currentTime;

		missile->currentTime = elapsedTime;
		missile->position += timeDelta * missile->speed * missile->direction;

		// check the new position and wrap it if it is necessary
		missile->position = checkBounds(missile->position, missile->size);

		if ((missile->currentTime - missile->startTime)*missile->speed > MISSILE_MAX_DISTANCE)
			missile->destroyed = true;

		if (missile->destroyed == true) {
			it = gameObjects.missiles.erase(it);
		}
		else {
			++it;
		}
	}
	// update ufos
	// it = gameObjects.ufos.begin();
	// while (it != gameObjects.ufos.end()) {
	// 	UfoObject* ufo = (UfoObject*)(*it);

	// 	if (ufo->destroyed == true) {
	// 		it = gameObjects.ufos.erase(it);
	// 	}
	// 	else {
	// 		// update ufo
	// 		ufo->currentTime = elapsedTime;

	// 		float curveParamT = ufo->speed * (ufo->currentTime - ufo->startTime);

	// 		ufo->position = ufo->initPosition + evaluateClosedCurve(curve1Data, curve1Size, curveParamT);
	// 		ufo->direction = glm::normalize(evaluateClosedCurve_1stDerivative(curve1Data, curve1Size, curveParamT));

	// 		// check the new position and wrap it if it is necessary
	// 		ufo->position = checkBounds(ufo->position, ufo->size);

	// 		++it;
	// 	}
	// }
	
	// update explosion billboards
	it = gameObjects.explosions.begin();
	while (it != gameObjects.explosions.end()) {
		ExplosionObject* explosion = (ExplosionObject*)(*it);

		// update explosion
		explosion->currentTime = elapsedTime;

		if (explosion->currentTime > explosion->startTime + explosion->textureFrames*explosion->frameDuration)
			explosion->destroyed = true;

		if (explosion->destroyed == true) {
			it = gameObjects.explosions.erase(it);
		}
		else {
			++it;
		}
	}

	
}

// Callback responsible for the scene update
void timerCallback(int) {

	// update scene time
	gameState.elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME); // milliseconds => seconds

	// call appropriate actions according to the currently pressed keys in key map
	// (combinations of keys are supported but not used in this implementation)
	if (gameState.keyMap[KEY_D] == true) 
		turnBirdRight(PENGUIN_VIEW_ANGLE_DELTA);
	
	if (gameState.keyMap[KEY_A] == true) 
		turnBirdLeft(PENGUIN_VIEW_ANGLE_DELTA);
	
	if (gameState.keyMap[KEY_W] == true) {
		 moveBirdForward();
	}
	if (gameState.keyMap[KEY_S] == true) {
		 moveBirdBackward();
	}
	if (gameState.keyMap[KEY_UP_ARROW] == true)
		increaseBirdSpeed();
	if (gameState.keyMap[KEY_DOWN_ARROW] == true)
		decreaseBirdSpeed();
	if (gameState.keyMap[KEY_Q] == true) {
		increaseBirdHeight();
	}
	if (gameState.keyMap[KEY_E] == true)
		decreaseBirdHeight();
	

	// if (gameState.keyMap[KEY_EXPLODE1] == true){
	// 	GameObjectsList::iterator it = gameObjects.aircraft.begin();
	//     std::advance(it, 0); // now it points to the nth asteroids of the list (counting from zero)
	//     AircraftObject* aircraft = (AircraftObject*)(*it);
	// 	aircraft->destroyed = true;	          // remove asteroid
	// 	insertExplosion(aircraft->position);  // insert explosion billboard
	// }
	if ((gameState.gameOver == true) && (gameObjects.bannerObject != NULL)) {
		gameObjects.bannerObject->currentTime = gameState.elapsedTime;
	}

	//update sparrow
	gameObjects.sparrow->currentAngle = fmod((gameObjects.sparrow->currentAngle + 5.0f) , 360.0f);
	float rad_angle = glm::radians(gameObjects.sparrow->currentAngle);
	gameObjects.sparrow->position = glm::vec3(0.5f * sin(rad_angle), 0.5f * cos(rad_angle), 0.5f);
	
	gameObjects.sparrow->currentTime = gameState.elapsedTime;

	// Check the new position and wrap it if necessary
	gameObjects.sparrow->position = checkBounds(gameObjects.sparrow->position, gameObjects.sparrow->size);

	// update objects in the scene
	updateObjects(gameState.elapsedTime);


	// space pressed -> launch missile
	if (gameState.keyMap[KEY_SPACE] == true) {
		// missile position and direction
		glm::vec3 missilePosition = gameObjects.penguin->position;
		glm::vec3 missileDirection = gameObjects.penguin->direction;

		missilePosition += missileDirection*1.5f*MISSILE_SIZE;

		createMissile(missilePosition, missileDirection, gameState.missileLaunchTime);
	}

  // TODO : replace the missile with a rock 

	// test collisions among objects in the scene
	 checkCollisions();

	// generate new ufos randomly
	// if (gameObjects.ufos.size() < UFOS_COUNT_MIN) {
	// 	int howManyUfos = rand() % (UFOS_COUNT_MAX - UFOS_COUNT_MIN + 1);

	// 	for (int i = 0; i<howManyUfos; i++) {
	// 		UfoObject* newUfo = createUfo();

	// 		gameObjects.ufos.push_back(newUfo);
	// 	}
	// }

	// game over? -> create banner with scrolling text "game over"
	if (gameState.gameOver == true) {
		gameState.keyMap[KEY_SPACE] = false;
		if (gameObjects.bannerObject == NULL) {
			// if game over and banner still not created -> create banner
			gameObjects.bannerObject = createBanner();
		}
	}

	// set timeCallback next invocation
	glutTimerFunc(33, timerCallback, 0);

	glutPostRedisplay();
}

// Called when mouse is moving while no mouse buttons are pressed.
void passiveMouseMotionCallback(int mouseX, int mouseY) {

	if (mouseY != gameState.windowHeight / 2) {

		float cameraElevationAngleDelta = 0.5f * (gameState.windowHeight - mouseY - gameState.windowHeight / 2);

		if (fabs(gameState.cameraElevationAngle + cameraElevationAngleDelta) < CAMERA_ELEVATION_MAX)
			gameState.cameraElevationAngle += cameraElevationAngleDelta;

		// set mouse pointer to the window center
		glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);

		glutPostRedisplay();
	}
}

// Called whenever a key on the keyboard was pressed. The key is given by the "keyPressed"
// parameter, which is in ASCII. It's often a good idea to have the escape key (ASCII value 27)
// to call glutLeaveMainLoop() to exit the program.
void keyboardCallback(unsigned char keyPressed, int mouseX, int mouseY) {

	switch (keyPressed) {
	case 'w':
		gameState.keyMap[KEY_W] = true;
		break;
	case 'a':
		gameState.keyMap[KEY_A] = true;
		break;
	case 's':
		gameState.keyMap[KEY_S] = true;
		break;
	case 'd':
		gameState.keyMap[KEY_D] = true;
		break;
	case 'q':
		gameState.keyMap[KEY_Q] = true;
		break;
	case 'e':
		gameState.keyMap[KEY_E] = true;
		break;
	case 27: // escape
		glutLeaveMainLoop();
		break;
	case 'r': // restart game
		restartGame();
		break;
	//case ' ': // launch missile
	//	if (gameState.gameOver != true)
	//		gameState.keyMap[KEY_SPACE] = true;
	//	break;
	case 't': // teleport space ship
		if (gameState.gameOver != true)
			teleport();
		break;
	case 'c': // switch camera
		if (gameState.cameraState == 0)
			gameState.cameraState = 1;
		else if (gameState.cameraState == 1)
			gameState.cameraState = 2;
		else
			gameState.cameraState = 0;
		if (gameState.cameraState == 1) {
			glutPassiveMotionFunc(passiveMouseMotionCallback);
			glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
		}
		else {
			glutPassiveMotionFunc(NULL);
		}
		break;
	case 'b': { // insert explosion randomly
		glm::vec3 explosionPosition = glm::vec3(
			(float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
			(float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
			0.0f
			);
		insertExplosion(explosionPosition);
	}
		break;

	case 'p': {
		printf("point light enabled");
		gameState.keyMap[KEY_P] = true;
		if (pointEnable == 0) { pointEnable = 1; }
		else { pointEnable = 0; }
		std::cout << "point enable : " << pointEnable << std::endl;
		break;
	}
	case 'o': {
		std::cout << "Reloading config file" << std::endl;
		gameState.keyMap[KEY_O] = true;
		reloadConfig();
		break;
	}

	default:
		; // printf("Unrecognized key pressed\n");
	}
}

// Called whenever a key on the keyboard was released. The key is given by
// the "keyReleased" parameter, which is in ASCII. 
void keyboardUpCallback(unsigned char keyReleased, int mouseX, int mouseY) {

	switch (keyReleased) {
	case ' ':
		gameState.keyMap[KEY_SPACE] = false;
		break;
	case 'w':
		gameState.keyMap[KEY_W] = false;
		break;
	case 'a':
		gameState.keyMap[KEY_A] = false;
		break;
	case 's':
		gameState.keyMap[KEY_S] = false;
		break;
	case 'd':
		gameState.keyMap[KEY_D] = false;
		break;
	case 'q':
		gameState.keyMap[KEY_Q] = false;
		break;
	case 'e':
		gameState.keyMap[KEY_E] = false;
		break;
	case 'p': {
		gameState.keyMap[KEY_P] = false;
		break;
	}
	case 'o': {
		gameState.keyMap[KEY_O] = false;
		break;
	}

	default:
		; // printf("Unrecognized key released\n");
	}
}

// The special keyboard callback is triggered when keyboard function or directional
// keys are pressed.
void specialKeyboardCallback(int specKeyPressed, int mouseX, int mouseY) {

	if (gameState.gameOver == true)
		return;

	switch (specKeyPressed) {
	case GLUT_KEY_RIGHT:
		gameState.keyMap[KEY_RIGHT_ARROW] = true;
		break;
	case GLUT_KEY_LEFT:
		gameState.keyMap[KEY_LEFT_ARROW] = true;
		break;
	case GLUT_KEY_UP:
		gameState.keyMap[KEY_UP_ARROW] = true;
		break;
	case GLUT_KEY_DOWN:
		gameState.keyMap[KEY_DOWN_ARROW] = true;
		break;
	case GLUT_KEY_F2:
		gameState.keyMap[KEY_UP_HEIGHT] = true;
		break;
	case GLUT_KEY_F1:
		gameState.keyMap[KEY_DOWN_HEIGHT] = true;
		break;
	case GLUT_KEY_F7:
		gameState.keyMap[KEY_EXPLODE1] = true;
		break;
	default:
		; // printf("Unrecognized special key pressed\n");
	}
}

// The special keyboard callback is triggered when keyboard function or directional
// keys are released.
void specialKeyboardUpCallback(int specKeyReleased, int mouseX, int mouseY) {

	if (gameState.gameOver == true)
		return;

	switch (specKeyReleased) {
	case GLUT_KEY_RIGHT:
		gameState.keyMap[KEY_RIGHT_ARROW] = false;
		break;
	case GLUT_KEY_LEFT:
		gameState.keyMap[KEY_LEFT_ARROW] = false;
		break;
	case GLUT_KEY_UP:
		gameState.keyMap[KEY_UP_ARROW] = false;
		break;
	case GLUT_KEY_DOWN:
		gameState.keyMap[KEY_DOWN_ARROW] = false;
		break;
	case GLUT_KEY_F2:
		gameState.keyMap[KEY_UP_HEIGHT] = false;
		break;
	case GLUT_KEY_F1:
		gameState.keyMap[KEY_DOWN_HEIGHT] = false;
		break;
	case GLUT_KEY_F7:
		gameState.keyMap[KEY_EXPLODE1] = false;
		break;
	default:
		; // printf("Unrecognized special key released\n");
	}
}

// When a user presses and releases mouse buttons in the window, each press
// and each release generates a mouse callback.
void mouseCallback(int buttonPressed, int buttonState, int mouseX, int mouseY) {

	if ((buttonPressed == GLUT_LEFT_BUTTON) && (buttonState == GLUT_DOWN)) {

		// stores value from the stencil buffer (byte)
		unsigned char id = 0;
		glReadPixels(mouseX, WINDOW_HEIGHT - mouseY, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &id);

		

		// the buffer was initially cleared to zeros
		if (id == 0) {
			// background was clicked
			printf("Clicked on background\n");
		}
		else if (id == 1) {
			printf("Target object was clicked");
		}
		else if (id == 2) {
			printf("Fern 1 object was clicked");
			insertExplosion(gameObjects.fern1->position);
		}
		else if (id == 3) {
			printf("Fern 2 object was clicked");
			insertExplosion(gameObjects.fern2->position);
		}
		else if (id == 4) {
			printf("Fern 3 object was clicked");
			insertExplosion(gameObjects.fern3->position);
		}
		else if (id == 5) {
			printf("Fern 4 object was clicked");
			insertExplosion(gameObjects.fern4->position);
		}

	}
}

// END OF CALLBACK FUNCTIONS
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// START OF MAIN APPLICATION FUNCTIONS

// Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void initializeApplication() {

	// initialize random seed
	srand((unsigned int)time(NULL));

	// initialize OpenGL
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClearStencil(0);  // this is the default value
	glEnable(GL_DEPTH_TEST);

	useLighting = true;

	// initialize shaders
	initializeShaderPrograms();
	// create geometry for all models used
	initializeModels();

	gameObjects.penguin = NULL;
	gameObjects.bannerObject = NULL;

	// test whether the curve segment is correctly computed (tasks 1 and 2)
	testCurve(evaluateCurveSegment, evaluateCurveSegment_1stDerivative);

	restartGame();
}

void finalizeApplication(void) {

	cleanUpObjects();

	delete gameObjects.penguin;
	gameObjects.penguin = NULL;

	// delete buffers - space ship, sparrow, missile, ufo, banner, and explosion
	cleanupModels();

	// delete shaders
	cleanupShaderPrograms();
}

// ---------------------------------------
// START OF MENU FUNCTIONS
void menuCamera(int menuItemID) {

	switch (menuItemID) {
	case 1: // Top View
		gameState.cameraState = 0;
		break;
	case 2: // First Person View
		gameState.cameraState = 1;
		break;
	case 3: // Side View
		gameState.cameraState = 2;
		break;
	}
	glutPostRedisplay();
}

void menuFog(int menuItemID) {

	switch (menuItemID) {
	case 1:
		//fog linear on
		fogLinearToggleInput = true;
		fogExpToggleInput = false;
		break;
	case 2:
		//fog exp on
		fogLinearToggleInput = false;
		fogExpToggleInput = true;
		printf("exp fog toggled on");
		break;
	case 3:
		//fog off
		fogLinearToggleInput = false;
		fogExpToggleInput = false;
		break;
	}
	glutPostRedisplay();
}

void menuLight(int menuItemID) { //movement of plane

	switch (menuItemID) {
	case 0:
		pointEnable = 1;
		break;
	case 1:
		pointEnable = 0;
		break;
	}
	glutPostRedisplay();
}

void menu(int menuItemID) {

	switch (menuItemID) {

	case 0:
		gameState.gameOver = true;
		break;
	case 1:
		restartGame();
		break;
	}
}
// END OF MENU FUNCTIONS
// ---------------------------------------


int main(int argc, char** argv) {

  // initialize windowing system
  glutInit(&argc, argv);

#ifndef __APPLE__
  glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
  glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
#else
  glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
#endif

  // initial window size
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutCreateWindow(WINDOW_TITLE);

  glutDisplayFunc(displayCallback);
  // register callback for change of window size
  glutReshapeFunc(reshapeCallback);
  // register callbacks for keyboard
  glutKeyboardFunc(keyboardCallback);
  glutKeyboardUpFunc(keyboardUpCallback);
  glutSpecialFunc(specialKeyboardCallback);     // key pressed
  glutSpecialUpFunc(specialKeyboardUpCallback); // key released


  glutMouseFunc(mouseCallback);

  glutTimerFunc(33, timerCallback, 0); // Timer callback repeats every 33 ms. In this callback, we update all objects in the scene (their pos etc)

  // initialize PGR framework (GL, DevIl, etc.)
  if(!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
    pgr::dieWithError("pgr init failed, required OpenGL not supported?");

  // -------------------------------------------------
  // START OF MENU IMPLEMENTATION

  /* Create menu camera. */
  int idCamera = glutCreateMenu(menuCamera);
  glutAddMenuEntry("Top View", 1);
  glutAddMenuEntry("First Person View", 2);
  glutAddMenuEntry("Side View", 3);

  /* Create menu fog */
  int idFog = glutCreateMenu(menuFog);
  glutAddMenuEntry("Fog Linear", 1);
  glutAddMenuEntry("Fog Exponential", 2);
  glutAddMenuEntry("Fog Off", 3);

  int idLight = glutCreateMenu(menuLight);
  glutAddMenuEntry("Point Light : On", 0);
  glutAddMenuEntry("Point Light : Off", 1);

  /*Create main menu*/
  glutCreateMenu(menu);
  glutAddSubMenu("Camera View", idCamera);
  glutAddSubMenu("Fog Toggle", idFog);
  glutAddSubMenu("Point light Toggle", idLight);
  glutAddMenuEntry("GameOver", 0);
  glutAddMenuEntry("Restart", 1);

  /* Menu will be invoked by the right button click. */
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  // END OF MENU IMPLEMENTATION
  // -------------------------------------------------


  initializeApplication(); // All shaders will be laoded here


#ifndef __APPLE__
  glutCloseFunc(finalizeApplication);
#else
  glutWMCloseFunc(finalizeApplication);
#endif

  glutMainLoop(); // starts the main loop of the app => endless loop. 
  // When the glut library enters it, the app will wait for user input (e.g mouseclick) => the corresponding callback function is called

  return 0;
}

// END OF MAIN APPLICATION FUNCTIONS
//----------------------------------------------------------------------------------------

