uniform sampler2D u_texture;
uniform vec2 u_resolution;

vec4 fxaa(vec2 uv) {
    vec2 px = 1.0 / u_resolution;

    vec3 rgbNW = texture2D(u_texture, uv + vec2(-px.x, -px.y)).rgb;
    vec3 rgbNE = texture2D(u_texture, uv + vec2( px.x, -px.y)).rgb;
    vec3 rgbSW = texture2D(u_texture, uv + vec2(-px.x,  px.y)).rgb;
    vec3 rgbSE = texture2D(u_texture, uv + vec2( px.x,  px.y)).rgb;
    vec3 rgbM  = texture2D(u_texture, uv).rgb;

    vec3 luma = vec3(0.299, 0.587, 0.114);

    float lumaMin = min(dot(rgbM,luma),
        min(min(dot(rgbNW,luma), dot(rgbNE,luma)),
            min(dot(rgbSW,luma), dot(rgbSE,luma))));

    float lumaMax = max(dot(rgbM,luma),
        max(max(dot(rgbNW,luma), dot(rgbNE,luma)),
            max(dot(rgbSW,luma), dot(rgbSE,luma))));

    vec2 dir;
    dir.x = -((dot(rgbNW,luma)+dot(rgbNE,luma))-(dot(rgbSW,luma)+dot(rgbSE,luma)));
    dir.y =  ((dot(rgbNW,luma)+dot(rgbSW,luma))-(dot(rgbNE,luma)+dot(rgbSE,luma)));

    dir = clamp(dir, -8.0, 8.0) * px;

    vec3 result =
        0.5 * (texture2D(u_texture, uv + dir*0.33).rgb +
               texture2D(u_texture, uv + dir*0.66).rgb);

    return vec4(result, 1.0);
}

void main() {
    gl_FragColor = fxaa(gl_TexCoord[0].xy);
}

