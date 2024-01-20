#version 140

uniform mat4 PVMmatrix;     // Projection * View * Model --> model to clip coordinates
uniform float time;         // used for simulation of moving lights (such as sun)

in vec3 position;           // vertex position in world space
in vec2 texCoord;           // incoming texture coordinates

smooth out vec2 texCoord_v; // outgoing texture coordinates

float decay = 0.2; // increasing this value will increase the banner speed

void main() {

  // vertex position after the projection (gl_Position is predefined output variable)
  gl_Position = PVMmatrix * vec4(position, 1.0);   // outgoing vertex in clip coordinates

  float localTime = time * decay;
  // localTime = 0;

  vec2 offset = vec2((floor(localTime) - localTime) * 4 + 1.0, 0.0);

  // outputs entering the fragment shader
  texCoord_v = texCoord + offset;
}
