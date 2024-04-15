#version 460

// uniform vec4 color;
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;
uniform sampler2D tex2;

void main(){
    fragColor = texture(tex, texCoord) * 0.6 + texture(tex2, texCoord) * 0.4;
}