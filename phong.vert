#version 330

layout (location = 0) in vec3 Position;    // Vertex position
layout (location = 1) in vec3 Normal;      // Vertex normal

out vec3 Norm;
out vec3 Pos;
out vec3 LightDir;

uniform mat3 NormalMatrix;      // ModelView matrix without the translation component, and inverted
uniform mat4 MVP;               // ModelViewProjection Matrix
uniform mat4 ModelView;         // ModelView matrix
uniform vec3 light_pos;         // Position of the light

void main()
{
    Norm = normalize(NormalMatrix * Normal);
    Pos = Position;
    LightDir = NormalMatrix * (light_pos - Position);

    gl_Position = MVP * vec4(Position, 1.0);
}