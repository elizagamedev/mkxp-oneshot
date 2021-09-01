uniform sampler2D texture;
uniform vec4 rgbOffsetx;
uniform vec4 rgbOffsety;

varying vec2 v_texCoord;

void main () {
    // Previously, you'd have rendered your complete scene into a texture
    // bound to "fullScreenTexture."

    vec2 rOffset = vec2(rgbOffsetx.x, rgbOffsety.x);
    vec2 gOffset = vec2(rgbOffsetx.y, rgbOffsety.y);
    vec2 bOffset = vec2(rgbOffsetx.z, rgbOffsety.z);
    vec4 rValue = texture2D(texture, v_texCoord - rOffset);  
    vec4 gValue = texture2D(texture, v_texCoord - gOffset);
    vec4 bValue = texture2D(texture, v_texCoord - bOffset);  

    // Combine the offset colors.
    gl_FragColor = vec4(rValue.r, gValue.g, bValue.b, rValue.a);

}