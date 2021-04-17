uniform float iTime;
uniform vec2 iResolution;
uniform vec3 ro;
uniform vec3 f;
uniform vec3 r;
uniform vec3 u;
uniform float zoom;

#define MAX_STEPS 200
#define MAX_DISTANCE 100.
#define SURF_DISTANCE .001
#define SHADE_MAX_STEPS 100

float get_dist(vec3 p);
vec3 get_normal(vec3 p);
float get_light(vec3 p);
vec2 ray_march(vec3 ro, vec3 rd);


void main(){
	vec2 fragCoord = gl_FragCoord.xy;
    vec2 uv = (fragCoord-.5*iResolution.xy)/iResolution.y;
    float t = iTime;
    
    vec3 col = vec3(0.);

    vec3 center = ro + f*zoom;
    vec3 i = center + uv.x*r + uv.y*u;
    //ray direction = point_on_screen-ro;
    vec3 rd = normalize(i - ro);
    
    vec2 rm = ray_march(ro, rd);
    float d = rm.x;
    vec3 p = ro+rd*d;
    float dif;
    if (d < MAX_DISTANCE)
        dif = get_light(p);
    else
        dif = .3;

    col = vec3(dif);
    
    gl_FragColor =  vec4(col, 1.);
}



float get_dist(vec3 p){
    vec4 s = vec4(0, 2*(sin(iTime)*.5+1), 6, 1);
    float s_dist = length(p-s.xyz)-s.w;
    float plane_dist = p.y - 0.;
    return min(plane_dist, s_dist);
}

vec3 get_normal(vec3 p){
    float d = get_dist(p);
    vec2 e = vec2(.001, 0.);
    //becasuse if we approximate by plane, these will be close to direction cosine of n
    vec3 n = d-vec3(
        get_dist(p-e.xyy),
        get_dist(p-e.yxy),
        get_dist(p-e.yyx)
    );
    return normalize(n);
}

float raymarch_light(vec3 ro, vec3 rd, vec3 l_src){
    float dO = 0;
    float md = 1;
    for (int i = 0; i < SHADE_MAX_STEPS; i++){
        vec3 p = ro + rd * dO;
        float dS = get_dist(p);
        if (dS < md) md = dS;
        dS = abs(dS);
        if (dS < .1)
            dS = .1;
        dO += dS;
        if(dS > 50 || dot(l_src-ro, l_src-p) < 0) break;
    }
    return md;
}

float get_light(vec3 p){
    vec3 light_pos = vec3(0., 5., 6.);
    light_pos.yz *= vec2(abs(2*sin(iTime)), cos(iTime))*2.;
    vec3 l = normalize(light_pos - p);
    vec3 n = get_normal(p);
    float dif = clamp(dot(n, l)*.5+.5, 0, 1);
    float d = raymarch_light(p + n*.2, l, light_pos);
    d = clamp(d+.3, 0, 1);
    d = clamp(d*2, 0, .8);
    dif *= d+.2;
    return dif;
}

vec2 ray_march(vec3 ro, vec3 rd){
    //distance from the origin
    float dO = 0.;
    int i;
    for (i = 0; i < MAX_STEPS; i += 1){
        vec3 p = ro + dO*rd;
        float dS = get_dist(p);
        dO += dS;
        if (dO < SURF_DISTANCE || dS > MAX_DISTANCE) break;
    }
    
    return vec2(dO, i);
}