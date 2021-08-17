uniform sampler2D maskTex;
uniform sampler2D texture;

varying vec2 v_texCoord;
vec4 colorOut;

void main(){ 

     vec4 color = texture2D(texture,v_texCoord);
     vec4 mask  = texture2D(maskTex,v_texCoord);

     colorOut = vec4(color.rgb, color.a * mask.r); //alpha value can be in any channel, depends on texture format.

     gl_FragColor = colorOut;
}