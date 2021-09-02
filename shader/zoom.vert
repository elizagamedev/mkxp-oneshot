
uniform mat4 projMat;

uniform vec2 texSizeInv;
uniform vec2 translation;
uniform vec2 zoom;

attribute vec2 position;
attribute vec2 texCoord;

varying vec2 v_texCoord;

void main()
{
	gl_Position = projMat * vec4(position + translation, 0, 1);

	v_texCoord = texCoord * texSizeInv;
    v_texCoord *= zoom;
}
