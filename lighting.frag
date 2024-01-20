#version 140

struct Material {           // structure that describes currently used material
  vec3  ambient;            // ambient component
  vec3  diffuse;            // diffuse component
  vec3  specular;           // specular component
  float shininess;          // sharpness of specular reflection

  bool  useTexture;         // defines whether the texture is used or not
};

uniform sampler2D texSampler;  // sampler for the texture access

uniform Material material;     // current material

smooth in vec4 color_v;        // incoming fragment color (includes lighting)
smooth in vec2 texCoord_v;     // fragment texture coordinates
out vec4       color_f;        // outgoing fragment color


uniform bool fogOnLinearToggle; //fog
uniform bool fogOnExpToggle;
uniform float fogNearValue;
uniform float fogDensityValue;

smooth in vec3 positionOut;


void main() {

  color_f = color_v;

  // fog parameter 
  float distToCam = -positionOut.z;
  vec4 fogcolor = vec4(0.4, 0.4, 0.4, 1);
  float fogFar = 2.0f;

  // if material has a texture -> apply it
  if(material.useTexture)
    color_f =  color_v * texture(texSampler, texCoord_v);

  if(fogOnLinearToggle){
    float fogAmount = (fogNearValue - distToCam) / (fogNearValue - fogFar);
    fogAmount = clamp(fogAmount, 0.0, 1.0);
    color_f = mix(color_f, fogcolor, fogAmount);
  }

  if (fogOnExpToggle) {
    float fogAmount = 1.0 - exp(-fogDensityValue * distToCam);
    color_f = mix(color_f, fogcolor, fogAmount);
  }

}
