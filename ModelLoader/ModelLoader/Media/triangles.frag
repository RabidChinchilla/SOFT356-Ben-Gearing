#version 450 core

out vec4 fColor;
in vec4 fragColour;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    //fColor = vec4(0.3, 0.6, 0.8, 9.0);
	//fColor = fragColour;
	fColor = texture(ourTexture, TexCoord) * fragColour + 0.002;
	


}
