#version 460 core
out vec4 FragColor;

in vec4 gl_FragCoord;

// texture samplers
uniform sampler2D texture1;
uniform float screenWidth;
uniform float screenHeight;

void main()
{
	float s = gl_FragCoord.x/screenWidth;
	float t = gl_FragCoord.y/screenHeight;
	FragColor = texture(texture1, vec2(s, t));
}