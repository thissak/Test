#version 460

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 bColor;
layout(location = 2) in vec2 cTexCoord;

out vec4 vertexColor;
out vec2 texCoord;

void main(){
    gl_Position = vec4(aPos, 1.0);
    vertexColor = vec4(bColor, 1.0);
    texCoord = cTexCoord;
}