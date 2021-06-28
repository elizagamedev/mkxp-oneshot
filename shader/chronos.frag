uniform sampler2D texture;

varying vec2 v_texCoord;
varying vec2 v_blurCoord[2];

void main () {

    // The offsets seem to be MASSIVE when it comes to shaders, so do keep in mind they need to be small decimal points
    vec4 rValue = texture2D(texture, v_texCoord - 0.03);  
    vec4 gValue = texture2D(texture, v_texCoord + 0.02);
    vec4 bValue = texture2D(texture, v_texCoord - 0.01);  

    // Combine the offset colors.
    gl_FragColor = vec4(rValue.r, gValue.g, bValue.b, 0.5);

}