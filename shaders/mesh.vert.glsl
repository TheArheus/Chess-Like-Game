#version 430


struct vertex_data
{
    vec2 Vertex;
    vec3 Color;
};

vec2 UVs[] = 
{
    vec2( 0,  0),
    vec2( 1,  0),
    vec2( 1,  1),
    vec2( 0,  1)
};

layout(set = 0, binding = 0) buffer Vertices
{
    vec2 VertexBuffer[];
};

layout(location = 0) out vec2 OutUV;


void main()
{
    vec2 Pos = VertexBuffer[gl_VertexIndex];    
    gl_Position = vec4(Pos, 0, 1.0);
    
    OutUV = UVs[gl_VertexIndex];
}
