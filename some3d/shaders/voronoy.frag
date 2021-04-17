uniform float iTime;
uniform vec2 iResolution;

float rnd_(vec2 x){
    int n = int(x.x * 40.0 + x.y * 6400.0);
    n = (n << 13) ^ n;
    return 1.0 - float( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
}
vec2 rnd(vec2 x){
    return vec2(rnd_(x), rnd_(-x));
}
vec3 col_grid(vec2 uv){
    float t = iTime;
    uv *= 10.;
    
    vec2 gv = fract(uv) - .5;
    vec2 id = floor(uv);
    float min_dist = 100.;
    for (float x = -1.; x <= 1.; x++){
        for (float y = -1.; y <= 1.; y++){
            vec2 offset = vec2(x, y);
            vec2 r = rnd(id+offset);
            vec2 pos = offset + cos(r*t)*.5;
            
            float d = length(gv-pos);
            if (d < min_dist)
                min_dist = d;
        }
    }
    return vec3(min_dist);
}


void main(){
	vec2 fragCoord = gl_FragCoord.xy;
    vec2 uv = (2.*fragCoord - iResolution.xy)/iResolution.y;
    gl_FragColor = vec4(col_grid(uv), 1.);
}