#version 330 core
    
layout(triangles, invocations = 3) in;
layout(triangle_strip, max_vertices = 3) out;
    
uniform mat4 _lightSpaceMatrix[3];

void main()
{          
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = 
            _lightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}  