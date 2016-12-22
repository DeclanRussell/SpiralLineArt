#version 400

//----------------------------------------------------------------------------------------------------------------------
/// @brief the input to our geometry shader
//----------------------------------------------------------------------------------------------------------------------
layout(lines) in;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the output of our geometry shader
//----------------------------------------------------------------------------------------------------------------------
layout(triangle_strip, max_vertices = 6) out;

uniform sampler2D sizeImage;

in vec2 vertNormal[];
in float vertT[];

uniform mat4 MVP;

uniform float thickness;

uniform int mode;

out vec2 position;
out float t;

vec2 posToUV(vec2 _pos)
{
    return (_pos+vec2(1,1))/2;
}

float RGBToIntensity(vec4 _RGB)
{
    return 1.0-(_RGB.x*0.2989+_RGB.y*0.5870+_RGB.z*0.1140);
}

vec4 RGBToCMYK(vec4 _RGB)
{
    vec4 cmyk;
    cmyk.w = 1-max(_RGB.x,max(_RGB.y,_RGB.z));
    cmyk.x = (1-_RGB.x-cmyk.w)/(1-cmyk.w);
    cmyk.y = (1-_RGB.y-cmyk.w)/(1-cmyk.w);
    cmyk.z = (1-_RGB.z-cmyk.w)/(1-cmyk.w);
    return cmyk;
}

float RGBToC(vec4 _RGB)
{
    return RGBToCMYK(_RGB).x;
}

float RGBToM(vec4 _RGB)
{
    return RGBToCMYK(_RGB).y;
}

float RGBToY(vec4 _RGB)
{
    return RGBToCMYK(_RGB).z;
}

float RGBToK(vec4 _RGB)
{
    return RGBToCMYK(_RGB).w;
}

void main(void)
{
    float halfThick = thickness/3.0;
    float thickScaler1,thickScaler2;
    vec2 p1 = gl_in[0].gl_Position.xy;
    vec2 p2 = gl_in[1].gl_Position.xy;
    vec2 n1 = vertNormal[0];
    vec2 n2 = vertNormal[1];

    if(mode==0)
    {
        thickScaler1 = RGBToIntensity(texture(sizeImage,posToUV(p1)));
        thickScaler2 = RGBToIntensity(texture(sizeImage,posToUV(p2)));
    }
    else if(mode==1)
    {
        thickScaler1 = RGBToC(texture(sizeImage,posToUV(p1)));
        thickScaler2 = RGBToC(texture(sizeImage,posToUV(p2)));
    }
    else if(mode==2)
    {
        thickScaler1 = RGBToM(texture(sizeImage,posToUV(p1)));
        thickScaler2 = RGBToM(texture(sizeImage,posToUV(p2)));
    }
    else if(mode==3)
    {
        thickScaler1 = RGBToY(texture(sizeImage,posToUV(p1)));
        thickScaler2 = RGBToY(texture(sizeImage,posToUV(p2)));
    }
    else if(mode==4)
    {
        thickScaler1 = RGBToK(texture(sizeImage,posToUV(p1)));
        thickScaler2 = RGBToK(texture(sizeImage,posToUV(p2)));
    }
    else if(mode==5)
    {
        thickScaler1 = texture(sizeImage,posToUV(p1)).r;
        thickScaler2 = texture(sizeImage,posToUV(p2)).r;
    }
    else if(mode==6)
    {
        thickScaler1 = texture(sizeImage,posToUV(p1)).g;
        thickScaler2 = texture(sizeImage,posToUV(p2)).g;
    }
    else if(mode==7)
    {
        thickScaler1 = texture(sizeImage,posToUV(p1)).b;
        thickScaler2 = texture(sizeImage,posToUV(p2)).b;
    }

    position = p1 - vec2(n1 * halfThick * thickScaler1);
    t = vertT[0];
    gl_Position = MVP*vec4(position,0,1);
    EmitVertex();

    position = p1 + vec2(n1 * halfThick * thickScaler1);
    t = vertT[0];
    gl_Position = MVP*vec4(position,0,1);
    EmitVertex();

    position = p2 + vec2(n2 * halfThick * thickScaler2);
    t = vertT[1];
    gl_Position = MVP*vec4(position,0,1);
    EmitVertex();

    EndPrimitive();

    position = p2 + vec2(n2 * halfThick * thickScaler2);
    t = vertT[1];
    gl_Position = MVP*vec4(position,0,1);
    EmitVertex();

    position = p2 - vec2(n2 * halfThick * thickScaler2);
    t = vertT[1];
    gl_Position = MVP*vec4(position,0,1);
    EmitVertex();

    position = p1 - vec2(n1 * halfThick * thickScaler1);
    t = vertT[0];
    gl_Position = MVP*vec4(position,0,1);
    EmitVertex();

    EndPrimitive();

}

