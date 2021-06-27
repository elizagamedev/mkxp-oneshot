//				
//				uniform sampler2D texture;
//				
//				varying vec2 v_texCoord;
//				varying vec2 v_blurCoord[2];
//				
//				void main()
//				{
//					lowp vec4 frag = vec4(0, 0, 0, 0);
//				
//					frag += texture2D(texture, v_texCoord);
//					frag += texture2D(texture, v_blurCoord[0]);
//					frag += texture2D(texture, v_blurCoord[1]);
//				
//					gl_FragColor = frag / 3.0;
//				}
//				
uniform sampler2D texture;

varying vec2 v_texCoord;
varying vec2 v_blurCoord[2];

void main () {
    // Previously, you'd have rendered your complete scene into a texture
    // bound to "fullScreenTexture."

    vec4 rValue = texture2D(frag, v_blurCoord[0] - 3);  
    vec4 gValue = texture2D(frag, v_blurCoord[0] - 2);
    vec4 bValue = texture2D(frag, v_blurCoord[0] - 1);  

    // Combine the offset colors.
    gl_FragColor = vec4(rValue.r, gValue.g, bValue.b, 1.0);

}


// void main () {
//     // Previously, you'd have rendered your complete scene into a texture
//     // bound to "fullScreenTexture."
//     vec4 rValue = texture2D(fullscreenTexture, gl_TexCoords[0] - rOffset);  
//     vec4 gValue = texture2D(fullscreenTexture, gl_TexCoords[0] - gOffset);
//     vec4 bValue = texture2D(fullscreenTexture, gl_TexCoords[0] - bOffset);  
// 
//     // Combine the offset colors.
//     gl_FragColor = vec4(rValue.r, gValue.g, bValue.b, 1.0);
// }