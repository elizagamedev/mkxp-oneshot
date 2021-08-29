vec2 u_resolution = vec2(320, 240);
uniform sampler2D texture;
varying vec2 v_texCoord;

const float SIZE = 0.0075; // Tweakable. Original: 0.0075
const float VERTICAL_FACTOR = 0.75; // Tweakable. Original: 0.86

// Other constants for readability
const float SIZE_HALF = SIZE * 0.5;
const float TO_ONE = 1. / SIZE;
const float VERTICAL_FACTOR_INV = 1. - VERTICAL_FACTOR;

float Distance(vec2 origin, vec2 point) {
    return abs(sqrt(pow(point.x - origin.x, 2.) + pow(point.y - origin.y, 2.)));
}

bool IsOutside(float distance){
    return mod(distance, SIZE) > SIZE_HALF;
}

vec2 GridCoords(vec2 st){
    return vec2(
    	(st.x - mod(st.x, SIZE)) * TO_ONE,
        (st.y - mod(st.y, SIZE)) * TO_ONE
    );
}

// Returns a color to reduce  off the fragment based on its coordinates
vec4 ColorReduction(vec2 st){
    vec2 gridCoords = GridCoords(st);
    
    st.x = st.x + SIZE_HALF * gridCoords.y;
    
    vec2 pointLeft = vec2(
    	st.x - mod(st.x, SIZE),
        st.y - mod(st.y, SIZE) - SIZE_HALF * VERTICAL_FACTOR_INV
    );
    
    vec2 pointRight = vec2(
    	(st.x + SIZE) - mod(st.x, SIZE),
        st.y - mod(st.y, SIZE) - SIZE_HALF * VERTICAL_FACTOR_INV
    );
    
    vec2 pointUp = vec2(
    	st.x - mod(st.x, SIZE) + SIZE_HALF,
        st.y - mod(st.y, SIZE) + SIZE * VERTICAL_FACTOR
    );
    
    float distLeft = Distance(st, pointLeft);
    float distRight = Distance(st, pointRight);
    float distUp = Distance(st, pointUp);
    
    vec2 point;
    bool isOutside = false;
    float off = 0.;
    
    if (distUp <= distLeft && distUp <= distRight){
        point = pointUp;
        isOutside = IsOutside(distUp);
        off = 1.;
    } else if (distRight <= distLeft){
        point = pointRight;
        isOutside = IsOutside(distRight);
    } else {
        point = pointLeft;
        isOutside = IsOutside(distLeft);
    }
    
    if (isOutside)
        return vec4(1);
    
    vec2 colorCoords = GridCoords(point);
    float colorIndex = mod(colorCoords.y + colorCoords.x + off, 3.);
    
    if (colorIndex == 0.)
    	return vec4(0, 1, 1, 0);
    
    if (colorIndex == 1.)
    	return vec4(1, 0, 1, 0);
    
	return vec4(1, 1, 0, 0);
}

void main() {
    vec2 st = gl_FragCoord.xy / min(u_resolution.y, u_resolution.x);
    vec4 color = ColorReduction(st);
    vec4 usedTexture = texture2D(texture, v_texCoord);
    gl_FragColor = usedTexture - color;
}