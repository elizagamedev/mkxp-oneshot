uniform sampler2D texture;
uniform vec4 rgbOffset;

varying vec2 v_texCoord;

void main () {
    // Previously, you'd have rendered your complete scene into a texture
    // bound to "fullScreenTexture."

    float rOffset = rgbOffset.x;
    float gOffset = rgbOffset.y;
    float bOffset = rgbOffset.z;
    vec4 rValue = texture2D(texture, v_texCoord - rOffset);  
    vec4 gValue = texture2D(texture, v_texCoord - gOffset);
    vec4 bValue = texture2D(texture, v_texCoord - bOffset);  

    // Combine the offset colors.
    gl_FragColor = vec4(rValue.r, gValue.g, bValue.b, rValue.a);

}