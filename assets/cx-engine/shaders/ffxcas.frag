#version 330 compatibility

uniform sampler2D u_texture;
uniform vec2 u_textureSize;

uniform float SHARPENING;      // 0.0 – 1.0
uniform int SOURCE_TRC;        // 0=None,1=709,2=PQ,3=sRGB,4=BT1886,5=HLG,6=Custom
uniform int TARGET_TRC;        // j.w.
uniform float CUSTOM_GAMMA;

in vec2 texCoords;
out vec4 fragColor;

// ------------------------------------------------------------
// Gamma conversion
// ------------------------------------------------------------

float From709(float x){
    return max(min(x/4.5,0.081), pow((x+0.099)/1.099,1.0/0.45));
}
float FromSrgb(float x){
    return max(min(x/12.92,0.04045), pow((x+0.055)/1.055,2.4));
}
float To709(float x){
    return max(min(x*4.5,0.018), 1.099*pow(x,0.45)-0.099);
}
float ToSrgb(float x){
    return max(min(x*12.92,0.0031308), 1.055*pow(x,0.41666)-0.055);
}

float ToLinear(float x){
    if(SOURCE_TRC==1) return From709(x);
    if(SOURCE_TRC==3) return FromSrgb(x);
    if(SOURCE_TRC==4) return pow(x,2.4);
    if(SOURCE_TRC==6) return pow(x,CUSTOM_GAMMA);
    return x;
}

float ToGamma(float x){
    if(TARGET_TRC==1) return To709(x);
    if(TARGET_TRC==3) return ToSrgb(x);
    if(TARGET_TRC==4) return pow(x,1.0/2.4);
    if(TARGET_TRC==6) return pow(x,1.0/CUSTOM_GAMMA);
    return x;
}

// ------------------------------------------------------------
// CAS core
// ------------------------------------------------------------

float fetchLuma(ivec2 p){
    ivec2 clamped_p = clamp(p, ivec2(0,0), ivec2(u_textureSize) - ivec2(1,1));
    vec3 rgb = texelFetch(u_texture, clamped_p, 0).rgb;
    float l = dot(rgb, vec3(0.299,0.587,0.114));
    return ToLinear(l);
}

void main()
{
    vec2 pixel = texCoords * u_textureSize - 0.5;
    ivec2 sp = ivec2(floor(pixel));
    vec2 f = fract(pixel);

    float b = fetchLuma(sp + ivec2( 0,-1));
    float e = fetchLuma(sp + ivec2(-1, 0));
    float f0= fetchLuma(sp);
    float c = fetchLuma(sp + ivec2( 1,-1));
    float g = fetchLuma(sp + ivec2( 1, 0));
    float h = fetchLuma(sp + ivec2( 2, 0));
    float i = fetchLuma(sp + ivec2(-1, 1));
    float j = fetchLuma(sp + ivec2( 0, 1));
    float n = fetchLuma(sp + ivec2( 0, 2));
    float k = fetchLuma(sp + ivec2( 1, 1));
    float l = fetchLuma(sp + ivec2( 2, 1));
    float o = fetchLuma(sp + ivec2( 1, 2));

    float mn = min(min(b,e),min(f0,g));
    float mx = max(max(b,e),max(f0,g));

    float amp = clamp(min(mn,1.0-mx)/mx,0.0,1.0);
    amp = sqrt(amp);

    float peak = -(mix(8.0,5.0,clamp(SHARPENING,0.0,1.0)));
    float w = amp/peak;

    float s = (1.0-f.x)*(1.0-f.y);
    float t = f.x*(1.0-f.y);
    float u = (1.0-f.x)*f.y;
    float v = f.x*f.y;

    float W = s+t+u+v;

    float result =
        f0*s +
        g*t +
        j*u +
        k*v +
        w*(b+e+c+h+i+n+l+o);

    result /= (W + 8.0*w);
    result = clamp(result,0.0,1.0);

    result = ToGamma(result);

    vec3 orig = texture(u_texture,texCoords).rgb;
    float origL = dot(orig, vec3(0.299,0.587,0.114));
    float diff = result - origL;

    vec3 finalColor = orig + diff;

    fragColor = vec4(finalColor,1.0);
}
