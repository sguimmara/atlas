//#define ecef mat4(0,0,1,0,  1,0,0,0,  0,-1,0,0,  0,0,0,1)
//#define ecef mat4(0,0,-1,0,   1,0,0,0,  0,-1,0,0,  0,0,0,1)
// #define ecef mat4(0,1,0,0,   0,0,-1,0,  -1,0,0,0,  0,0,0,1)
#define ecef mat4(1,0,0,0,  0,-1,0,0,  0,0,1,0,  0,0,0,1)

#define GLOBALS 0
#define INSTANCE 1
#define MATERIAL 2

#define POSITION 0
#define NORMAL 1
#define ST 2

layout(set = GLOBALS, binding = GLOBALS) uniform GlobalProperties
{
    mat4 view;
    mat4 proj;
    vec4 sunDirection;
    vec4 sunColorAndIntensity;
    vec4 viewportTime;
} globals;

layout(set = INSTANCE, binding = INSTANCE) uniform InstanceProperties
{
    mat4 model;
} instance;

vec3 transformToViewSpace(vec4 v)
{
    v.y = -v.y;
    return (globals.view * instance.model * v).xyz;
}

vec4 atl_transformPos(vec3 position)
{
    position.y = -position.y;
    return globals.proj * globals.view * instance.model * vec4(position, 1.0);
}