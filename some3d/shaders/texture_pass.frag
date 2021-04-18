#version 450
#define MAX_STEPS 200
#define MAX_DISTANCE 100.
#define SURF_DISTANCE .001
#define SHADE_MAX_STEPS 50
#define TYPE_MUL 256.
#define OBJ_PARAMS_MUL 256.

/*in this shader all data about objects is passed via data 'texture', leaving my gpu in tears
code example in .cpp:
    sf::Image im;
    int num_of_obj = 4;
    im.create(3, num_of_obj);
    im.setPixel(0, 0, {0, 0, 0}); //plane y = 0
    im.setPixel(0, 1, {1, 0, 0}); //R
    im.setPixel(0, 2, {1, 0, 0}); //G
    im.setPixel(0, 3, {1, 0, 0}); //B
    im.setPixel(1, 0, {0, 0, 1, 0}); //obj construction data
    im.setPixel(1, 1, {1, 5, 2, 1});
    im.setPixel(1, 2, {1, 7, 2, 1});
    im.setPixel(1, 3, {3, 6, 2, 1});
    im.setPixel(2, 0, {255, 255, 255}); //colors
    im.setPixel(2, 1, {255, 0, 0});
    im.setPixel(2, 2, {0, 255, 0});
    im.setPixel(2, 3, {0, 0, 255});
    sf::Texture obj_data;
    obj_data.loadFromImage(im);
    shader.setUniform("data", obj_data);
    shader.setUniform("num_of_objs", num_of_obj);
*/
uniform float iTime;
uniform vec2 iResolution;
uniform vec3 ro;
uniform vec3 f;
uniform vec3 r;
uniform vec3 u;
uniform float zoom;

uniform int num_of_objs;
/*
x = 0: (type, 0, 0, 0)
    type*256:
        0 - plane
        1 - sphere
x = 1: obj creation data to be passed into dist funcs
x = 2: color
*/
uniform sampler2D data;

vec2 get_dist(vec3 p);
vec3 get_normal(vec3 p);
vec2 get_light(vec3 p, vec3 ro, float iterations);
vec2 ray_march(vec3 ro, vec3 rd);
float clench(float val, float v_min, float v_max, float new_min, float new_max);

float sph_dist(vec4 s);
float pln_dist(vec4 p);

void main() {
	vec2 fragCoord = gl_FragCoord.xy;
    vec3 sky = vec3(0, 0, 0);

    vec2 uv = (fragCoord-.5*iResolution.xy)/iResolution.y;
    float t = iTime;

    vec3 center = ro + f*zoom;
    vec3 i = center + uv.x*r + uv.y*u;
    //ray direction = point_on_screen-ro;
    vec3 rd = normalize(i - ro);
    
    vec2 rm = ray_march(ro, rd);
    float d = rm.x;

    vec3 p = ro+rd*d;
    vec3 col = texelFetch(data, ivec2(2, get_dist(p).y), 0).xyz;
    
    vec2 light_data = get_light(p, ro, rm.y);
    col *= light_data.x;
    col = col*(1-light_data.y) + sky*light_data.y;
    
    gl_FragColor =  vec4(col, 1);
}

float sph_dist(vec3 p, vec4 s) {
    return length(p-s.xyz)-s.w;
}
float pln_dist(vec3 p, vec4 plane) {
    return dot(vec4(p, 1), plane)/length(plane.xyz);
}

vec2 get_dist(vec3 p) {
    /*
    float s_dist = sph_dist(p, vec4(2, 5, 2, 1));
    float plane_dist = pln_dist(p, vec4(0, 0, 1, 0));
    return vec2(min(plane_dist, s_dist), 1);*/
    float dist;
    vec4 obj_data;
    int type;
    float min_dist = 1000000.;
    int min_idx;
    for (int i = 0; i < num_of_objs; i += 1){
        type = int(TYPE_MUL*texelFetch(data, ivec2(0, i), 0).x);
        obj_data = OBJ_PARAMS_MUL*texelFetch(data, ivec2(1, i), 0);

        if (type == 0)
               dist = pln_dist(p, obj_data);
        else if (type == 1)
               dist = sph_dist(p, obj_data);

        if (dist < min_dist){
            min_dist = dist;
            min_idx = i;
        }
    }
    return vec2(min_dist, min_idx);
}
vec3 get_normal(vec3 p) {
    float d = get_dist(p).x;
    vec2 e = vec2(.001, 0.);
    //becasuse if we approximate by plane, these will be close to direction cosine of n
    vec3 n = d-vec3(
        get_dist(p-e.xyy).x,
        get_dist(p-e.yxy).x,
        get_dist(p-e.yyx).x
    );
    return normalize(n);
}

float ray_march_light(vec3 ro, vec3 rd, vec3 l_src) {
    float dO = 0;
    float md = 1;
    for (int i = 0; i < SHADE_MAX_STEPS; i++){
        vec3 p = ro + rd * dO;
        float dS = get_dist(p).x;
        if (dS < md) md = dS;
        dS = abs(dS);
        if (dS < .1)
            dS = .1;
        dO += dS;
        if(dS > 50 || dot(l_src-ro, l_src-p) < 0) break;
    }
    return md;
}

vec2 get_light(vec3 p, vec3 ro, float iterations) {
    vec3 light_pos = vec3(0., 5., 6.);
    light_pos.yz *= vec2(2*sin(iTime) + .1, abs(cos(iTime))*2.) + .1;
    vec3 l = normalize(light_pos - p);
    vec3 n = get_normal(p);
    float dif = clamp(dot(n, l)*.5+.5, 0, 1);
    float d = ray_march_light(p + n*.2, l, light_pos);
    dif *= clench(d, -.2, .01, 0, 1);
    
    //occlusion
    
    float occ = iterations / MAX_STEPS*2;
    occ = 1 - occ;
    occ *= occ;
    dif *= occ;

    //fog at max distance
    float fog = distance(p, ro);
    fog = (fog/MAX_DISTANCE - .1)*1.12;
    fog = clamp(fog, 0, 1);
    fog *= fog;

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