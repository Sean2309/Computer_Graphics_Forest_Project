#version 140

// IMPORTANT: !!! lighting is evaluated in camera space !!!

struct Material {      // structure that describes currently used material
  vec3  ambient;       // ambient component
  vec3  diffuse;       // diffuse component
  vec3  specular;      // specular component
  float shininess;     // sharpness of specular reflection

  bool  useTexture;    // defines whether the texture is used or not
};

// warning: sampler inside the Material struct can cause problems -> so its outside
uniform sampler2D texSampler;  // sampler for the texture access
// need to have 1 sampler for each texture

struct Light {         // structure describing light parameters
  vec3  ambient;       // intensity & color of the ambient component
  vec3  diffuse;       // intensity & color of the diffuse component
  vec3  specular;      // intensity & color of the specular component
  vec3  position;      // light position
  vec3  spotDirection; // spotlight direction
  float spotCosCutOff; // cosine of the spotlight's half angle
  float spotExponent;  // distribution of the light energy within the reflector's cone (center->cone's edge)
  vec3 attenuation;    // attenuation
};

in vec3 position;           // vertex position in world space
in vec3 normal;             // vertex normal
in vec2 texCoord;           // incoming texture coordinates

uniform float time;         // time used for simulation of moving lights (such as sun)
uniform Material material;  // current material

uniform float fogDensity;   // density of fog

uniform mat4 PVMmatrix;     // Projection * View * Model  --> model to clip coordinates
uniform mat4 Vmatrix;       // View                       --> world to eye coordinates
uniform mat4 Mmatrix;       // Model                      --> model to world coordinates
uniform mat4 normalMatrix;  // inverse transposed Mmatrix

uniform vec3 reflectorPosition;   // reflector position (world coordinates)
uniform vec3 reflectorDirection;  // reflector direction (world coordinates)
uniform vec3 campfireLoc;

uniform int pointEnable;

smooth out vec2 texCoord_v;  // outgoing texture coordinates
smooth out vec4 color_v;     // outgoing fragment color


vec4 spotLight(Light light, Material material, vec3 vertexPosition, vec3 vertexNormal) {

  vec3 ret = vec3(0.0);
  // TODO : pass a boolean or something to increase number of params
  // use the material and light structures to obtain the surface and light properties
  // the vertexPosition and vertexNormal variables contain transformed surface position and normal
  // store the ambient, diffuse and specular terms to the ret variable
  // for spot lights, light.position contains the light position
  // everything is expressed in the view coordinate system -> eye/camera is in the origin

  vec3 L = normalize(light.position - vertexPosition);
  vec3 R = reflect(-L, vertexNormal);
  vec3 V = normalize(-vertexPosition);

  float NdotL = max(0.0, dot(vertexNormal, L));
  float RdotV = max(0.0, dot(R, V));
  float spotCoef = max(0.0, dot(-L, light.spotDirection));

  ret += material.ambient * light.ambient;
  ret += material.diffuse * light.diffuse * NdotL;
  ret += material.specular * light.specular * pow(RdotV, material.shininess);

  if(spotCoef < light.spotCosCutOff)
    ret *= 0.0;
  else
    ret *= pow(spotCoef, light.spotExponent);

  const vec3 fogColor = vec3(0.45, 0.45, 0.45);
  vec4 viewSpace = Vmatrix * Mmatrix * vec4(position, 1);

  float dist = abs(viewSpace.z);
  float fogFactor = 1.0 / exp(dist * fogDensity);
  
  vec3 finalcolor = (fogFactor * ret) + (1 - fogFactor) * fogColor;
  return vec4(finalcolor, 1.0);
}

vec4 directionalLight(Light light, Material material, vec3 vertexPosition, vec3 vertexNormal) {

  vec3 ret = vec3(0.0);

  // use the material and light structures to obtain the surface and light properties
  // the vertexPosition and vertexNormal variables contain transformed surface position and normal
  // store the ambient, diffuse and specular terms to the ret variable
  // glsl provides some built-in functions, for example: reflect, normalize, pow, dot
  // for directional lights, light.position contains the direction
  // everything is expressed in the view coordinate system -> eye/camera is in the origin

  vec3 L = normalize(light.position);
  vec3 R = reflect(-L, vertexNormal);
  vec3 V = normalize(-vertexPosition);
  float NdotL = max(0.0, dot(vertexNormal, L));
  float RdotV = max(0.0, dot(R, V));

  ret += material.ambient * light.ambient;
  ret += material.diffuse * light.diffuse * NdotL;
  ret += material.specular * light.specular * pow(RdotV, material.shininess);

  const vec3 fogColor = vec3(0.45, 0.45, 0.45);
  vec4 viewSpace = Vmatrix * Mmatrix * vec4(position, 1);

  float dist= abs(viewSpace.z);
  float fogFactor = 1.0 / exp(dist * fogDensity);
  
  vec3 finalcolor = (fogFactor * ret) + (1 - fogFactor) * fogColor;
  return vec4(finalcolor, 1.0);
}

vec4 pointLight(Light light, Material material, vec3 vertexPosition, vec3 vertexNormal) {
	vec3 ret = vec3(0.0f);
	vec3 lightPosCamera = (Vmatrix * vec4(light.position, 1.0f)).xyz;

    vec3 L = normalize(light.position);
    vec3 R = reflect(-L, vertexNormal);
    vec3 V = normalize(-vertexPosition);
	float NdotL = max(0.0, dot(vertexNormal, L));
    float RdotV = max(0.0, dot(R, V));
   
	ret += material.ambient * light.ambient;
    ret += material.diffuse * light.diffuse * NdotL;
    ret += material.specular * light.specular * pow(RdotV, material.shininess);

	float dist = length(light.position - vertexPosition);
	float attenuation = 1.0f / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * pow(dist, 2));

	ret = attenuation * ret;

	return vec4(ret, 1.0f);
}

// hardcoded lights
Light sun;
float sunSpeed = 0.5f;
Light spaceShipReflector;
Light point;

void setupLights() {

  // set up sun parameters
  sun.ambient  = vec3(0.0);
  sun.diffuse  = vec3(0.5, 0.5, 0.5f);
  sun.specular = vec3(1.0);

  sun.position = (Vmatrix * vec4(sin(time * sunSpeed), 0.0, cos(time * sunSpeed), 0.0)).xyz;
  //sun.position = (Vmatrix * vec4(0.0, 0.0, 1.0, 0.0)).xyz;

  // set up reflector parameters
  spaceShipReflector.ambient       = vec3(0.2f);
  spaceShipReflector.diffuse       = vec3(1.0);
  spaceShipReflector.specular      = vec3(1.0);
  spaceShipReflector.spotCosCutOff = 0.95f;
  spaceShipReflector.spotExponent  = 0.0;

  spaceShipReflector.position = (Vmatrix * vec4(reflectorPosition, 1.0)).xyz;
  spaceShipReflector.spotDirection = normalize((Vmatrix * vec4(reflectorDirection, 0.0)).xyz);
  
}

void main() {

  setupLights();

  // eye-coordinates position and normal of vertex
  vec3 vertexPosition = (Vmatrix * Mmatrix * vec4(position, 1.0)).xyz;         // vertex in eye coordinates
  vec3 vertexNormal   = normalize( (Vmatrix * normalMatrix * vec4(normal, 0.0) ).xyz);   // normal in eye coordinates by NormalMatrix

  // initialize the output color with the global ambient term
  vec3 globalAmbientLight = vec3(0.4f);
  vec4 outputColor = vec4(material.ambient * globalAmbientLight, 0.0);

  // accumulate contributions from all lights
  outputColor += directionalLight(sun, material, vertexPosition, vertexNormal);
  outputColor += spotLight(spaceShipReflector, material, vertexPosition, vertexNormal);

  if(pointEnable == 1) {
    //pointlight setup
    point.position = vec3(0.0f, -0.5f, 0.05f ); // 
    point.diffuse = vec3(0.2f, 0.2f, 0.2f);
    point.ambient = vec3(0.2f);
    point.specular = vec3(1.0f);
    point.attenuation = vec3(0.0f, 0.0f, 0.1f);
    outputColor += pointLight(point, material, vertexPosition, vertexNormal);
  }

  // vertex position after the projection (gl_Position is built-in output variable)
  gl_Position = PVMmatrix * vec4(position, 1);   // out:v vertex in clip coordinates

  // outputs entering the fragment shader
  color_v = outputColor;
  texCoord_v = texCoord;
}
