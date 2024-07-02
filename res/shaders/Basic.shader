#shader vertex
#version 330 core
layout(location = 0) in vec4 position;

void main()
{
    gl_Position = position;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 color;
uniform float u_time;

void main()
{
    color = vec4(sin(u_time * 2) / 2 + 0.5, 0., 0., 1.);
}