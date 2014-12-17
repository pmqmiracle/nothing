#include "ClothSystem.h"
#include <cmath>

float ClothSystem::DIST_BETWEEN_POINTS = 0.35f;
float ClothSystem::sqrt2 = 1.414214f;

float ClothSystem::mass = 1.9f;
Vector3f ClothSystem::g = Vector3f(0,-0.3f,0);

float ClothSystem::drag_k = 0.5f;//0.1f;

float ClothSystem::structural_k = 20.f;//0.001f;//100.f;
float ClothSystem::structural_restLength = DIST_BETWEEN_POINTS;

float ClothSystem::shear_k = 12.f;//0.001f;//40.0f;
float ClothSystem::shear_restLength = sqrt2*DIST_BETWEEN_POINTS;

float ClothSystem::flexion_k = 12.f;//0.01f;//40.0f;
float ClothSystem::flexion_restLength = 2*DIST_BETWEEN_POINTS;

float ClothSystem::MAX_WIND = 0.5f;
float ClothSystem::wind_noise = 0.02f;

//TODO: Initialize here
ClothSystem::ClothSystem(int m):ParticleSystem(m)
{
    m_numParticles = m;

    //for rendering
    m_normals.clear();
    for(int i = 0;i < m;++i)
    {
        m_normals.push_back(Vector3f(0,1,0));
    }

    int n = sqrt(m_numParticles);
    vector<Vector3f> vv;
	for (int i = 0; i < n; i++)
    {
        for(int j = 0;j < n;++j)
        {
            // for this system, we care about the position and the velocity
            // 偶数 position
            //vv.push_back(Vector3f(i,4.0f,j));
            // 奇数 velocity
            //vv.push_back(Vector3f(0,0,0));

            //Git
            //y轴的值都一样
            //position
            //vv.push_back(DIST_BETWEEN_POINTS*Vector3f(2.0f,i, j));
            vv.push_back(DIST_BETWEEN_POINTS*Vector3f(i+2,5.0f, j));
            //velocity
            vv.push_back(Vector3f(0, 0, 0));
        }
	}
    setState(vv);
    setTime(0);
}

Vector3f ClothSystem::getVelocity(int particle_index)
{
    return m_vVecState[2*particle_index+1];
}

Vector3f ClothSystem::getPosition(int particle_index)
{
    return m_vVecState[2*particle_index];
}

int ClothSystem::linearIndex(int i, int j)
{
    int rows = sqrt(m_numParticles);
    return rows*i+j;
}

// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state)
{
    vector<Vector3f> vv;
    //miracle: super bug!!!!
    //state.size() = m_numParticles * 2; not equal !!!!!!
    //int n = sqrt(state.size());
    int n = sqrt(m_numParticles);
    for(int i = 0;i < n;++i)
    {
        for(int j = 0;j < n;++j)
        {
            //f(X,t)={v,forces/mass}

            //velocity
            vv.push_back(state[2*linearIndex(i,j)+1]);
            //miracle
            //设置定点"挂"衣服设置不同的可以有不同的很cool的效果=w=
            if((i == 0 && j == 0)||(i == n-1 && j ==0))
            //if((i == n-1 && j == 0)||(i == n-1 && j == n-1))
            //if((i == 0 && j == 0)||(j == n-1 && i ==0))
            //if((i == 0 && j == 0)||(j == n-1 && i == n-1))
            //if((i == 0 && j == 0)||(j == n-1 && i == n-1)||(i == n-1 && j == 0)||(i ==0 && j == n-1))
            //if((i == 0 && j == 0))
            {
                //forces/mass
                vv.push_back(Vector3f(0,0,0));
            }
            else
            {
                //forces/mass
                vv.push_back(evalF(state,i,j));
            }
        }
    }

	return vv;
}

//helper
Vector3f ClothSystem::evalF(vector<Vector3f> &state, int index_i, int index_j)
{
    int index = linearIndex(index_i, index_j);
    //miracle: bug, 注意getState()和state[2*index]是不一样的!!!!
    //Vector3f current_pos = getState()[2*index];
    Vector3f current_pos = state[2*index];

    Vector3f f_m(0,0,0);
    Vector3f gravity = mass*g;
    //Vector3f force1 = -1 * drag_k * getVelocity(index);
    Vector3f force1 = -1 * drag_k * state[2*index+1];

    //structural springs
    Vector3f structural_force(0,0,0);
    int x[4] = {0,-1,0,1};
    int y[4] = {1,0,-1,0};
    for(int k = 0;k < 4;++k)
    {
        int tx = index_i+x[k];
        int ty = index_j+y[k];
        if(inGrid(tx,ty))
        {
            //Vector3f pos = getState()[2*linearIndex(tx,ty)];
            Vector3f pos = state[2*linearIndex(tx,ty)];
            structural_force += -1*structural_k*((current_pos-pos).abs()-structural_restLength)*(current_pos-pos).normalized();
        }
    }

    //shear springs
    Vector3f shear_forces(0,0,0);
    int x1[4] = {1,-1,-1,1};
    int y1[4] = {1,1,-1,-1};
    for(int k = 0;k < 4;++k)
    {
        int tx = index_i+x1[k];
        int ty = index_j+y1[k];
        if(inGrid(tx,ty))
        {
            //Vector3f pos = getState()[2*linearIndex(tx,ty)];
            Vector3f pos = state[2*linearIndex(tx,ty)];
            shear_forces += -1*shear_k*((current_pos-pos).abs()-shear_restLength)*(current_pos-pos).normalized();
        }
    }

    //flexion springs
    Vector3f flexion_forces(0,0,0);
    int x2[4] = {0,-2,0,2};
    int y2[4] = {2,0,-2,0};
    for(int k = 0;k < 4;++k)
    {
        int tx = index_i+x2[k];
        int ty = index_j+y2[k];
        if(inGrid(tx,ty))
        {
            //Vector3f pos = getState()[2*linearIndex(tx,ty)];
            Vector3f pos = state[2*linearIndex(tx,ty)];
            flexion_forces += -1*flexion_k*((current_pos-pos).abs()-flexion_restLength)*(current_pos-pos).normalized();
        }
    }

    //wind
    wind = Vector3f(0,0,0);
    wind += Vector3f(0, 0, MAX_WIND*sin(time(NULL))); // sinusoidal wind force
    //wind += Vector3f(0, MAX_WIND*sin(time(NULL)),0); // sinusoidal wind force
    wind += wind_noise * Vector3f(getRandomFloat(), getRandomFloat(), getRandomFloat()); // add some noise

    f_m = (gravity+force1+structural_force+shear_forces+flexion_forces+wind)/mass;
    //f_m = (gravity+force1+structural_force+shear_forces+wind)/mass;
    //f_m = (gravity+force1+structural_force+wind)/mass;
    return f_m;
}

bool ClothSystem::inGrid(int x, int y)
{
    return (x < sqrt(m_numParticles) && x >= 0 && y < sqrt(m_numParticles) && y >= 0);
}

///TODO: render the system (ie draw the particles)
void ClothSystem::draw()
{
    //for wireframe
    /*
    int size = sqrt(m_numParticles);
    glLineWidth(2);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);

    //for wireframe
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            // draw lines down and right
            Vector3f x = getState()[2*linearIndex(i,j)];
            //down
            if (i < size-1) {
                Vector3f x1 = getState()[2*linearIndex(i+1,j)];
                glVertex3f(x[0], x[1], x[2]);
                glVertex3f(x1[0], x1[1], x1[2]);
            }
            //right
            if (j < size-1) {
                Vector3f x2 = getState()[2*linearIndex(i, j+1)];
                glVertex3f(x[0], x[1], x[2]);
                glVertex3f(x2[0], x2[1], x2[2]);
            }
        }
    }
    glEnd();
    */


    //for rendering mode
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    if (true)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    //for rendering mode
    m_normals.clear();
    int size = sqrt(m_numParticles);
    int dx[4] = {0,-1,0,1};
    int dy[4] = {1,0,-1,0};
    for(int i = 0;i < sqrt(m_numParticles);++i)
    {
        for(int j = 0;j < sqrt(m_numParticles);++j)
        {
            vector<Vector3f> neighbors;
            for(int k = 0;k < 4;++k)
            {
                if(inGrid(i+dx[k],j+dy[k]))
                {
                    neighbors.push_back(m_vVecState[2*linearIndex(i+dx[k],j+dy[k])]);
                }
            }
            vector<Vector3f> vv;
            for(unsigned int m = 0;m < neighbors.size();++m)
            {
                vv.push_back(neighbors[m]-m_vVecState[2*linearIndex(i,j)]);
            }
            Vector3f average(0,0,0);
            for(unsigned int v = 0;v < vv.size()-1;++v)
            {
                average = average + Vector3f::cross(vv[v],vv[v+1]).normalized();
            }
            m_normals.push_back(average);
        }
    }
    //upper triangles
    glBegin(GL_TRIANGLES);
    //@TODO: ADD color??
    //glColor4f(1,1,0,0);
/*
    for(int i = 0;i < size-1;++i)
    {
        for(int j = 0;j < size-1;++j)
        {
            int index = linearIndex(i,j);
            glNormal3f(m_normals[index][0],m_normals[index][1],m_normals[index][2]);
            glVertex3f(m_vVecState[2*index][0],m_vVecState[2*index][1],m_vVecState[2*index][2]);
            int index2 = linearIndex(i,j+1);
            glNormal3f(m_normals[index2][0],m_normals[index2][1],m_normals[index2][2]);
            glVertex3f(m_vVecState[2*index2][0],m_vVecState[2*index2][1],m_vVecState[2*index2][2]);
            int index3 = linearIndex(i+1,j);
            glNormal3f(m_normals[index3][0],m_normals[index3][1],m_normals[index3][2]);
            glVertex3f(m_vVecState[2*index3][0],m_vVecState[2*index3][1],m_vVecState[2*index3][2]);
        }
    }
    //lower triangles
    for(int i = 1;i < size;++i)
    {
        for(int j = 1;j < size;++j)
        {
            int index = linearIndex(i,j);
            glNormal3f(m_normals[index][0],m_normals[index][1],m_normals[index][2]);
            glVertex3f(m_vVecState[2*index][0],m_vVecState[2*index][1],m_vVecState[2*index][2]);
            int index2 = linearIndex(i,j-1);
            glNormal3f(m_normals[index2][0],m_normals[index2][1],m_normals[index2][2]);
            glVertex3f(m_vVecState[2*index2][0],m_vVecState[2*index2][1],m_vVecState[2*index2][2]);
            int index3 = linearIndex(i-1,j);
            glNormal3f(m_normals[index3][0],m_normals[index3][1],m_normals[index3][2]);
            glVertex3f(m_vVecState[2*index3][0],m_vVecState[2*index3][1],m_vVecState[2*index3][2]);
        }
    }
*/
    //back
    //upper
    for(int i = 0;i < size-1;++i)
    {
        for(int j = 0;j < size-1;++j)
        {
            int index = linearIndex(i,j);
            glNormal3f(-m_normals[index][0],-m_normals[index][1],-m_normals[index][2]);
            glVertex3f(m_vVecState[2*index][0],m_vVecState[2*index][1],m_vVecState[2*index][2]);
            int index2 = linearIndex(i,j+1);
            glNormal3f(-m_normals[index2][0],-m_normals[index2][1],-m_normals[index2][2]);
            glVertex3f(m_vVecState[2*index2][0],m_vVecState[2*index2][1],m_vVecState[2*index2][2]);
            int index3 = linearIndex(i+1,j);
            glNormal3f(-m_normals[index3][0],-m_normals[index3][1],-m_normals[index3][2]);
            glVertex3f(m_vVecState[2*index3][0],m_vVecState[2*index3][1],m_vVecState[2*index3][2]);
        }
    }
    //lower triangles
    for(int i = 1;i < size;++i)
    {
        for(int j = 1;j < size;++j)
        {
            int index = linearIndex(i,j);
            glNormal3f(-m_normals[index][0],-m_normals[index][1],-m_normals[index][2]);
            glVertex3f(m_vVecState[2*index][0],m_vVecState[2*index][1],m_vVecState[2*index][2]);
            int index2 = linearIndex(i,j-1);
            glNormal3f(-m_normals[index2][0],-m_normals[index2][1],-m_normals[index2][2]);
            glVertex3f(m_vVecState[2*index2][0],m_vVecState[2*index2][1],m_vVecState[2*index2][2]);
            int index3 = linearIndex(i-1,j);
            glNormal3f(-m_normals[index3][0],-m_normals[index3][1],-m_normals[index3][2]);
            glVertex3f(m_vVecState[2*index3][0],m_vVecState[2*index3][1],m_vVecState[2*index3][2]);
        }
    }

    glEnd();

    glPopAttrib();

    //==================================================================================
    glEnable(GL_LIGHTING);
	for (int i = 0; i < m_numParticles; i++)
    {
		Vector3f pos = getState()[2*i];//  position of particle i. YOUR CODE HERE
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2] );
		glutSolidSphere(0.025f,10.0f,10.0f);
		glPopMatrix();
	}

}

