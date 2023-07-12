#version 430 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D cloud;

void main(){
    vec3 texCol = texture(cloud, TexCoords).rgb;      
    FragColor = vec4(texCol, 1.0);
}