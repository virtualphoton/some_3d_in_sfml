uniform float iTime;
uniform vec2 iResolution;
uniform vec3 ro;
uniform vec3 f;
uniform vec3 r;
uniform vec3 u;
uniform float zoom;

float dist_line(vec3 ro, vec3 rd, vec3 p){
    //area of parallelogram / length of its base = rho(p, line)
    return length(cross(rd, p-ro))/length(rd);
}

float draw_point(vec3 ro, vec3 rd, vec3 p){
    float d = dist_line(ro, rd, p);
    return smoothstep(.06, .04, d);
}

void main(){
	vec2 fragCoord = gl_FragCoord.xy;
    vec2 uv = (fragCoord*2.-iResolution.xy)/iResolution.y;
    
    float t = iTime;
    //ray origin
    
    //forward, right, up
    
    vec3 center = ro + f*zoom;
    
    //intersection point on the screen
    vec3 i = center + uv.x*r + uv.y*u;
    
    //ray direction = point_on_screen-ro;
    vec3 rd = i - ro;
    float d = 0.;
    d += draw_point(ro, rd, vec3(0., 0., 0.));
    d += draw_point(ro, rd, vec3(0., 0., 1.));
    d += draw_point(ro, rd, vec3(0., 1., 0.));
    d += draw_point(ro, rd, vec3(0., 1., 1.));
    d += draw_point(ro, rd, vec3(1., 0., 0.));
    d += draw_point(ro, rd, vec3(1., 0., 1.));
    d += draw_point(ro, rd, vec3(1., 1., 0.));
    d += draw_point(ro, rd, vec3(1., 1., 1.));
    
    vec4 fragColor = vec4(vec3(d), 1.);
    gl_FragColor = fragColor;
}