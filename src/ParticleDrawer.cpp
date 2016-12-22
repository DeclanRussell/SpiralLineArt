#include "include/ParticleDrawer.h"
#include <glm/gtc/type_ptr.hpp>

ParticleDrawer::ParticleDrawer() : m_numParticles(0)
{
    //Enable point sprites
    glEnable(GL_POINT_SPRITE);
    //Allow the shader to set the size of the point sprite
    glEnable(GL_PROGRAM_POINT_SIZE);

    //set up our particle shader to render depth information to a texture
    m_particleShader = new ShaderProgram();
    //load the source
    Shader vert("shaders/ParticleVert.glsl",GL_VERTEX_SHADER);
    Shader frag("shaders/ParticleFrag.glsl",GL_FRAGMENT_SHADER);
    m_particleShader->attachShader(&vert);
    m_particleShader->attachShader(&frag);
    m_particleShader->bindFragDataLocation(0, "fragout");
    m_particleShader->link();
    m_particleShader->use();

    //Set some uniforms
    m_pointSizeHndl = m_particleShader->getUniformLoc("pointSize");
    m_colourHndl = m_particleShader->getUniformLoc("colour");
    m_MVMatHndl = m_particleShader->getUniformLoc("MV");
    m_PMatHndl = m_particleShader->getUniformLoc("P");
    m_MVPMatHndl = m_particleShader->getUniformLoc("MVP");
    m_screenWidthHndl = m_particleShader->getUniformLoc("screenWidth");

    glUniform1f(m_pointSizeHndl,10.f);
    glUniform3f(m_colourHndl,0.f,1.f,1.f);

    // Create our VAO and position buffer
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // Put our vertices into an OpenGL buffer
    glGenBuffers(1, &m_posVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}
//----------------------------------------------------------------------------------------------------------------------
ParticleDrawer::~ParticleDrawer()
{
    // Delete our buffers
    glDeleteBuffers(1,&m_posVBO);
    glDeleteVertexArrays(1,&m_VAO);
}
//----------------------------------------------------------------------------------------------------------------------
void ParticleDrawer::resizeParticleBuffer(int _n)
{
    //resize our buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*_n, NULL, GL_DYNAMIC_DRAW);
    //update the number of particles we have
    m_numParticles = _n;
}
//----------------------------------------------------------------------------------------------------------------------
void ParticleDrawer::setPositions(std::vector<glm::vec3> &_posData)
{
    //set our buffer data
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*_posData.size(), &_posData[0], GL_DYNAMIC_DRAW);
    //update the number of particles we have
    m_numParticles = _posData.size();
}
//----------------------------------------------------------------------------------------------------------------------
void ParticleDrawer::setScreenWidth(int _width)
{
    m_particleShader->use();
    glUniform1i(m_screenWidthHndl,_width);
}
//----------------------------------------------------------------------------------------------------------------------
void ParticleDrawer::setParticleSize(float _size)
{
    m_particleShader->use();
    glUniform1f(m_pointSizeHndl,_size);
    m_particleSize = _size;
}
//----------------------------------------------------------------------------------------------------------------------
void ParticleDrawer::setColour(float _r, float _g, float _b)
{
    m_particleShader->use();
    glUniform3f(m_colourHndl,_r,_g,_b);
}
//----------------------------------------------------------------------------------------------------------------------
void ParticleDrawer::draw(glm::mat4 _M, glm::mat4 _V, glm::mat4 _P)
{
    glm::mat4 MV = _V*_M;
    glm::mat4 MVP = _P*MV;
    //grab and instance of our shader library
    m_particleShader->use();
    glUniformMatrix4fv(m_PMatHndl, 1, GL_FALSE, glm::value_ptr(_P));
    glUniformMatrix4fv(m_MVMatHndl, 1, GL_FALSE, glm::value_ptr(MV));
    glUniformMatrix4fv(m_MVPMatHndl, 1, GL_FALSE, glm::value_ptr(MVP));

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_POINTS, 0, m_numParticles);

}
//----------------------------------------------------------------------------------------------------------------------
void ParticleDrawer::drawFromVAO(GLuint _VAO, int _n, glm::mat4 _M, glm::mat4 _V, glm::mat4 _P)
{
    glm::mat4 MV = _V*_M;
    glm::mat4 MVP = _P*MV;
    //grab and instance of our shader library
    m_particleShader->use();
    glUniformMatrix4fv(m_PMatHndl, 1, GL_FALSE, glm::value_ptr(_P));
    glUniformMatrix4fv(m_MVMatHndl, 1, GL_FALSE, glm::value_ptr(MV));
    glUniformMatrix4fv(m_MVPMatHndl, 1, GL_FALSE, glm::value_ptr(MVP));

    glBindVertexArray(_VAO);
    glDrawArrays(GL_POINTS, 0, _n);
}
//----------------------------------------------------------------------------------------------------------------------
void *ParticleDrawer::bindPosBufferPtr()
{
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
    return glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}
//----------------------------------------------------------------------------------------------------------------------
void ParticleDrawer::unBindPosBufferPtr()
{
    glUnmapBuffer(GL_ARRAY_BUFFER);
}
//----------------------------------------------------------------------------------------------------------------------
