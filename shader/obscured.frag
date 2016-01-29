
uniform sampler2D texture;
uniform sampler2D obscured;

varying vec2 v_texCoord;

void main()
{
	vec4 color = texture2D(texture, v_texCoord);
	color.a *= texture2D(obscured, v_texCoord).r;
	gl_FragColor = color;
}
