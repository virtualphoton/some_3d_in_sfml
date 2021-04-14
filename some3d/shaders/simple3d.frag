uniform float iTime;
uniform vec2 iResolution;

#define MAX_STEPS 100
#define MAX_DISTANCE 100.
#define SURF_DISTANCE .01

float get_dist(vec3 p);
vec3 get_normal(vec3 p);
float get_light(vec3 p);
float ray_march(vec3 ro, vec3 rd);


void main(){
	vec2 fragCoord = gl_FragCoord.xy;
    vec2 uv = (fragCoord-.5*iResolution.xy)/iResolution.y;
    float t = iTime;
    
    vec3 col = vec3(0.);
    
    vec3 ro = vec3(0., 1., 0.);
    vec3 rd = normalize(vec3(uv.x, uv.y, 1.));
    
    float d = ray_march(ro, rd);
    vec3 p = ro+rd*d;
    float dif = get_light(p);
    col = vec3(dif);
    
    gl_FragColor =  vec4(col, 1.);
}



float get_dist(vec3 p){
    vec4 s = vec4(0, 1, 6, 1);
    float s_dist = length(p-s.xyz)-s.w;
    float plane_dist = p.y - 0.;
    return min(plane_dist, s_dist);
}

vec3 get_normal(vec3 p){
    float d = get_dist(p);
    vec2 e = vec2(.01, 0.);
    //becasuse if we approximate by plane, these will be close to direction cosine of n
    vec3 n = d-vec3(
        get_dist(p-e.xyy),
        get_dist(p-e.yxy),
        get_dist(p-e.yyx)
    );
    return normalize(n);
}

float get_light(vec3 p){
    vec3 light_pos = vec3(0., 5., 6.);
    light_pos.xz += vec2(sin(iTime), cos(iTime))*2.;
    vec3 l = normalize(light_pos - p);
    vec3 n = get_normal(p);
    float dif = clamp(dot(n, l), 0., 1.);
    float d = ray_march(p + n*2.*SURF_DISTANCE, l);
    if (d < length(light_pos - p)) dif *= .1;
    return dif;
}

float ray_march(vec3 ro, vec3 rd){
    //distance from the origin
    float dO = 0.;
    for (int i = 0; i < MAX_STEPS; i += 1){
        vec3 p = ro + dO*rd;
        float dS = get_dist(p);
        dO += dS;
        if (dO < SURF_DISTANCE || dS > MAX_DISTANCE) break;
    }
    
    return dO;
}