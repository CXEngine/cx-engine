#version 330 core

in vec2 position;
in vec2 texCoord;

out vec2 texCoords;

void main()
{
    texCoords = texCoord;
}
