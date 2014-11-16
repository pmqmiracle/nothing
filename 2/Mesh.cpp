#include "Mesh.h"
#include <GL/freeglut.h>
#include <GL/glut.h>

using namespace std;

void Mesh::load( const char* filename )
{
	// 2.1.1. load() should populate bindVertices, currentVertices, and faces
	// Add your code here.
    ifstream infile(filename);
    char buffer[1024];
    while(infile.getline(buffer,1024)!=0)
    {
        //bug!!!必须用buffer来初始化ss才能够读进来啊pmq
        stringstream ss(buffer);

        string token;
        ss >> token;

        if(token == "v")
        {
            float x,y,z;
            ss >> x >> y >> z;
            bindVertices.push_back(Vector3f(x,y,z));
        }
        else if(token == "f")
        {
            int x,y,z;
            ss >> x >> y >> z;
            //OBJ 1-based --> 0-based index
            faces.push_back(Tuple3u(x-1,y-1,z-1));

            //Git
            //faces.push_back(Tuple3u(x,y,z));
        }
    }
	// make a copy of the bind vertices as the current vertices
	currentVertices = bindVertices;
}


void Mesh::draw()
{
	// Since these meshes don't have normals
	// be sure to generate a normal per triangle.
	// Notice that since we have per-triangle normals
	// rather than the analytical normals from
	// assignment 1, the appearance is "faceted".刻面 小面

    int num_faces = faces.size();
    //vector<Vector3f> face_normals;

    glBegin(GL_TRIANGLES);
    for(int k = 0;k < num_faces;++k)
    {
        Vector3f v1_v0 = currentVertices[faces[k][1]]-currentVertices[faces[k][0]];
        Vector3f v2_v0 = currentVertices[faces[k][2]]-currentVertices[faces[k][0]];
        Vector3f triangle_normal = Vector3f::cross(v1_v0,v2_v0).normalized();
        //face_normals.push_back(triangle_normal);

        glNormal3d(triangle_normal[0],triangle_normal[1],triangle_normal[2]);
        glVertex3d(currentVertices[faces[k][0]][0],currentVertices[faces[k][0]][1],currentVertices[faces[k][0]][2]);
        glNormal3d(triangle_normal[0],triangle_normal[1],triangle_normal[2]);
        glVertex3d(currentVertices[faces[k][1]][0],currentVertices[faces[k][1]][1],currentVertices[faces[k][1]][2]);
        glNormal3d(triangle_normal[0],triangle_normal[1],triangle_normal[2]);
        glVertex3d(currentVertices[faces[k][2]][0],currentVertices[faces[k][2]][1],currentVertices[faces[k][2]][2]);

    }
    glEnd();

    //Git
    /*
    glBegin(GL_TRIANGLES);
    for (unsigned i = 0; i < faces.size(); i++) {
        Tuple3u face = faces[i];
        // obj is 1-indexed
        Vector3f faces[3] = {currentVertices[face[0]-1], currentVertices[face[1]-1], currentVertices[face[2]-1]};
        Vector3f normal = Vector3f::cross(faces[1] - faces[0], faces[2] - faces[1]).normalized();
        glNormal3d(normal[0], normal[1], normal[2]);
        glVertex3d(faces[0][0], faces[0][1], faces[0][2]);
        glNormal3d(normal[0], normal[1], normal[2]);
        glVertex3d(faces[1][0], faces[1][1], faces[1][2]);
        glNormal3d(normal[0], normal[1], normal[2]);
        glVertex3d(faces[2][0], faces[2][1], faces[2][2]);
    }
    glEnd();
    */
}

void Mesh::loadAttachments( const char* filename, int numJoints )
{
	// 2.2. Implement this method to load the per-vertex attachment weights
	// this method should update m_mesh.attachments
    ifstream infile(filename);
    char buffer[1024];
    float ww;
    while(infile.getline(buffer,1024)!=0)
    {
        vector<float> weights_per_vertex;
        stringstream ss(buffer);
        for(int i = 0;i < numJoints-1;++i)
        {
            ss >> ww;
            weights_per_vertex.push_back(ww);
        }
        attachments.push_back(weights_per_vertex);
    }
}
