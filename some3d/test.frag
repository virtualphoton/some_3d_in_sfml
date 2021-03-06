#version 450
#define MAX_STEPS 200
#define MAX_DISTANCE 100.
#define SURF_DISTANCE .001
#define SHADE_MAX_STEPS 50

uniform float iTime;
uniform vec2 iResolution;
uniform vec3 ro;
uniform vec3 f;
uniform vec3 r;
uniform vec3 u;
uniform float zoom;

float get_dist(vec3 p);
vec3 get_color(vec3 p);
vec3 get_normal(vec3 p);
vec3 get_light(vec3 p, vec3 ro, float iterations);
vec2 ray_march(vec3 ro, vec3 rd);
float clench(float val, float v_min, float v_max, float new_min, float new_max);

void main() {
        vec2 fragCoord = gl_FragCoord.xy;

    vec2 uv = (fragCoord-.5*iResolution.xy)/iResolution.y;
    float t = iTime;

    vec3 center = ro + f*zoom;
    vec3 i = center + uv.x*r + uv.y*u;
    //ray direction = point_on_screen-ro;
    vec3 rd = normalize(i - ro);

    vec2 rm = ray_march(ro, rd);
    float d = rm.x;

    vec3 p = ro+rd*d;
    vec3 col = get_color(p);

    vec3 color = get_light(p, ro, rm.y);

    gl_FragColor =  vec4(color, 1);
}

vec3 get_normal(vec3 p) {
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

float ray_march_light(vec3 ro, vec3 rd, vec3 l_src) {
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

vec3 get_light(vec3 p, vec3 ro, float iterations) {
    vec3 sky = vec3(0, 0, 0.5);
    vec3 light_pos = vec3(0., 5., 6.);
    light_pos.yz *= vec2(2*sin(iTime) + .1, abs(cos(iTime))*2.) + .1;

    vec3 col = get_color(p);

    vec3 l = normalize(light_pos - p);
    vec3 n = get_normal(p);
    float dif = clamp(dot(n, l)*.5+.5, 0, 1);
    float d = ray_march_light(p + n*.2, l, light_pos);
    col *= clench(d, -.2, .01, 0, 1);

    //occlusion

    float occ = iterations / MAX_STEPS*2;
    occ = 1 - occ;
    occ *= occ;
    col *= occ;

    //fog at max distance
    float fog = distance(p, ro);
    fog = (fog/MAX_DISTANCE - .1)*1.12;
    fog = clamp(fog, 0, 1);
    fog *= fog;

    col = col*(1-fog) + sky*fog;
    return vec2(dif, fog);
}

vec2 ray_march(vec3 ro, vec3 rd) {
    //distance from the origin
    float dO = 0.;
    int i;
    for (i = 0; i < MAX_STEPS; i += 1){
        vec3 p = ro + dO*rd;
        float dS = get_dist(p).x;
        dO += dS;
        if (dO < SURF_DISTANCE || dS > MAX_DISTANCE) break;
    }

    return vec2(dO, i);
}

float clench(float val, float v_min, float v_max, float new_min, float new_max) {
    return clamp((val - v_min)/(v_max-v_min)*(new_max-new_min) + new_min, new_min, new_max);
}


uniform vec4 obj3d_0;
uniform vec3 color_0;
uniform vec4 obj3d_1;
uniform vec3 color_1;
float plane_dist(vec3 p, vec4 plane) {  return dot(vec4(p, 1), plane) / length(plane.xyz);}
float sphere_dist(vec3 p, vec4 s) {    return length(p-s.xyz)-s.w;}
vec3 get_color(vec3 p){
float dist, min_dist = 1000000.;
vec3 color;
dist = plane_dist(p, obj3d_0);
if (dist < min_dist){
    min_dist = dist;
    color = color_0;
}
dist = sphere_dist(p, obj3d_1);
if (dist < min_dist){
    min_dist = dist;
    color = color_1;
}
return color;
}
float get_dist(vec3 p){
float dist, min_dist = 1000000.;
dist = plane_dist(p, obj3d_0);
if (dist < min_dist)
    min_dist = dist;
dist = sphere_dist(p, obj3d_1);
if (dist < min_dist)
    min_dist = dist;
return min_dist;
}