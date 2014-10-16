
/**

Phong shading for the OpenGL tools exampler.
Author Markus Broecker <mbrckr@gmail.com>

MIT License

Copyright (c) 2010, Markus Broecker <mbrckr@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#version 330

in vec3 Norm;
in vec3 Pos;
in vec3 LightDir;

layout (location = 0) out vec4 FragColor;

uniform mat3 NormalMatrix;
uniform mat4 ModelView;

void main()
{
    vec3 normalDirCameraCoords = normalize(Norm);
    vec3 vertexPosLocalCoords = normalize(Pos);
    vec3 lightDirCameraCoords = normalize(LightDir);

    float dist = max(length(LightDir), 1.0);

    float intensity = max(dot(normalDirCameraCoords, lightDirCameraCoords), 0.0) / pow(dist, 1.001);

    vec3 h = normalize(lightDirCameraCoords - vertexPosLocalCoords);
    float intSpec = max(dot(h, normalDirCameraCoords), 0.0);
    vec4 spec = vec4(0.9, 0.9, 0.9, 1.0) * (pow(intSpec, 100) / pow(dist, 1.2));

    FragColor = max((intensity * vec4(0.7, 0.7, 0.7, 1.0)) + spec, vec4(0.07, 0.07, 0.07, 1.0));
}