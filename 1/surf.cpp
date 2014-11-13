#include "surf.h"
#include "extra.h"
using namespace std;

namespace
{
    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;

        return true;
    }

    const float PI = 3.1415926;
    const float Degree2Radians = PI/180;
}

// Sweep a profile curve that lies flat on the xy-plane around the
// y-axis.  The number of divisions is given by steps.
Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;
    Matrix3f rotation;
    Matrix3f normalRotation;
    Vector3f newV;
    Vector3f newN;

    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.
    for(int degree = 0;degree <= 360;degree += 360/steps)
    {
        rotation = Matrix3f::rotateY(degree*Degree2Radians);
        bool singular;
        //if we transform a vertex by a M, then its normal should be transformed by top 3x3 M's inverse transpose
        normalRotation = rotation.inverse(&singular,0.001f).transposed();
        for(int i = 0;i < profile.size();++i)
        {
            newV = rotation*profile[i].V;
            newN = normalRotation*profile[i].N;

            //miracle : why???
            //for normal orientation
            //OpenGL 让每个顶点的normal的方向指向外面,指向我们
            newN = -newN;

            if(degree==0 || degree==360)
            {
                surface.VV.push_back(profile[i].V);
                surface.VN.push_back(-1*profile[i].N);
            }
            else
            {
                surface.VN.push_back(newN);
                surface.VV.push_back(newV);
            }
        }
    }

    Tup3u upperTriangle, lowerTriangle;
    for(int i = 1;i <= steps;++i)
    {
        for(int k = profile.size()*i;k < profile.size()*(i+1);++k)
        {
            //exclude the bottom point, as there's no extra points to form triangles
            if((k+1)%profile.size()==0)
                continue;
            upperTriangle[0] = k;
            upperTriangle[1] = k - profile.size();
            upperTriangle[2] = upperTriangle[1]+1;
            //美腻的bug, 必须按照逆时针的方向指定triangle的顶点,否则会出现光照计算错误或者无法显示,但是这个bug有时会很美腻=v=
            /*upperTriangle[0] = k;
            upperTriangle[2] = k - profile.size();
            upperTriangle[1] = k - profile.size() + 1;*/

            lowerTriangle[0] = k;
            lowerTriangle[1] = upperTriangle[2];
            lowerTriangle[2] = k+1;

            /*lowerTriangle[0] = k;
            lowerTriangle[1] = k - profile.size() + 1;
            lowerTriangle[2] = k+1;*/

            surface.VF.push_back(upperTriangle);
            surface.VF.push_back(lowerTriangle);
        }
    }

    cerr << "\t>>> makeSurfRev called. Return surface " << endl;

    return surface;
}

//好像还有点小问题=.=
Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.
    for(int i = 0;i <sweep.size();++i)
    {
        CurvePoint cp = sweep[i];
        Matrix4f M = Matrix4f(Vector4f(cp.N,0),Vector4f(cp.B,0),Vector4f(cp.T,0),Vector4f(cp.V,1));
        for(int k = 0;k < profile.size();++k)
        {
            //[N B T V]
            //[0 0 0 1]
            Vector4f newv = M * Vector4f(profile[k].V,1);
            Vector4f newn = M * Vector4f(profile[k].N,0);
            surface.VV.push_back(newv.xyz());
            //newn = -newn;
            surface.VN.push_back(-1*newn.xyz());
        }
    }
    // triangle
    Tup3u upperTriangle;
    Tup3u lowerTriangle;
    for(int j = profile.size();j < surface.VV.size();++j)
    {
        //exclude the bottom points
        if((j+1)%profile.size()==0)
            continue;
        upperTriangle[0] = j;
        upperTriangle[1] = j - profile.size();
        upperTriangle[2] = upperTriangle[1] + 1;

        lowerTriangle[0] = j;
        lowerTriangle[1] = upperTriangle[2];
        lowerTriangle[2] = j + 1;

        surface.VF.push_back(upperTriangle);
        surface.VF.push_back(lowerTriangle);
    }
    /*
    int curveSize = profile.size();
    for (unsigned i=0; i < surface.VV.size() - curveSize; i++) {
        if ((i+1)%curveSize != 0) { // not bottommost point
            unsigned top[3] = {i, i+1, i+curveSize};
            surface.VF.push_back(Tup3u(top));
            unsigned bottom[3] = {i+curveSize, i+1, i+1+curveSize};
            surface.VF.push_back(Tup3u(bottom));
        }
    }*/

    cerr << "\t>>> makeGenCyl called. Returning surface." <<endl;
    return surface;
}

void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
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
    else
    {
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glColor4f(0.4f,0.4f,0.4f,1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{
    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;

    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
