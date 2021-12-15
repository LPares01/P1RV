#version 460 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 ourTexCoord;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	FragColor = mix(texture(texture1, ourTexCoord), texture(texture2, vec2(1.0 - ourTexCoord.x, ourTexCoord.y)), 0.2);
}