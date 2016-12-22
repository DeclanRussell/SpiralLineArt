//----------------------------------------------------------------------------------------------------------------------
/// @file ParticleFrag.glsl
/// @author Declan Russell
/// @date 17/03/15
/// @version 1.0
/// @namepsace GLSL
/// @class ParticleFrag
/// @brief Fragment shader for drawing point sprites as spheres.
//----------------------------------------------------------------------------------------------------------------------

#version 400

//----------------------------------------------------------------------------------------------------------------------
/// @brief eye space postion from vertex shader
//----------------------------------------------------------------------------------------------------------------------
in vec3 position;
//----------------------------------------------------------------------------------------------------------------------
/// @brief size of our points
//----------------------------------------------------------------------------------------------------------------------
uniform float pointSize;
//----------------------------------------------------------------------------------------------------------------------
/// @brief projection matrix
//----------------------------------------------------------------------------------------------------------------------
uniform mat4 P;
//----------------------------------------------------------------------------------------------------------------------
/// @brief colour of our sphere
//----------------------------------------------------------------------------------------------------------------------
uniform vec3 colour;
//----------------------------------------------------------------------------------------------------------------------
/// @brief position of the light in our scene
//----------------------------------------------------------------------------------------------------------------------
//uniform vec3 lightPos;
//----------------------------------------------------------------------------------------------------------------------
/// @brief output fragment. This will be the depth of our particles
//----------------------------------------------------------------------------------------------------------------------
out vec4 fragout;
in vec3 ogPos;

float sizeFunc(vec3 _p)
{
    return ((abs(_p.y))*0.5f)+0.5f;
}
//----------------------------------------------------------------------------------------------------------------------
/// @brief fragment main. Draws point sprites as spheres, calculates and outputs there depth.
//----------------------------------------------------------------------------------------------------------------------
void  main(){
    //normal of our fragment
    vec3 normal;
    // calculate eye-space sphere normal from texture coordinates
    normal.xy = (gl_PointCoord*2.0f) - 1.0f;
    normal.y*=-1.0;
    float r2 = dot(normal.xy, normal.xy);
    if (r2 > 1.0) discard; // kill pixels outside circle

    fragout = vec4(colour,1.0);

    //----------------------------------------------------------------
    //---------- This stuff is only needed if we are to do some ------
    //---------- phong shading but for this tool shading is not ------
    //------------------------neccesarry------------------------------
    normal.z = sqrt(1.0-r2);

    // calculate depth
    // point radius calculated from inverse projection * 0.5*pointSize
    vec4 pixelPos = vec4(position + (normal * pointSize * 0.5), 1.0);
    //pixelPos.z -= 1.5;
    //vec4 clipSpacePos = P * pixelPos;
    //vec3 depth = vec3(clipSpacePos.z / clipSpacePos.w);

    //fragout = vec4(depth,1.0);
    fragout = vec4(colour,1.0);
//    fragout = vec4(sizeFunc(ogPos),1.0);
//    fragout = vec4(vec3(abs(ogPos.y)),1.0);


}
