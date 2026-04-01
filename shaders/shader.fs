#version 330 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}

#in vec4 Color;

#out vec4 FragColor;

#void main()
#{
#    FragColor = Color;
#}