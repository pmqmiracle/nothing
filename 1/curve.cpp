#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx( const Vector3f& lhs, const Vector3f& rhs )
    {
        const float eps = 1e-8f;
        return ( lhs - rhs ).absSquared() < eps;
    }
    //Matrix4f MatrixBezier(1,0,0,0,-3,3,0,0,3,-6,3,0,-1,3,-3,1);
    Matrix4f MatrixBezier = Matrix4f(1,-3,3,-1,0,3,-6,3,0,0,3,-3,0,0,0,1);
    float ss = 1.0/6;
    Matrix4f MatrixBSpline = Matrix4f(1,-3,3,-1,4,0,-6,3,1,3,3,-3,0,0,0,1)*ss;
    //error: MatrixBSpline does not name a type?
    //MatrixBSpline = MatrixBSpline * ss;
}

Curve evalBezier( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 || P.size() % 3 != 1 )
    {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
        exit( 0 );
    }

    // TODO:
    // You should implement this function so that it returns a Curve
    // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

    // Make sure that this function computes all the appropriate
    // Vector3fs for each CurvePoint: V,T,N,B.
    // [NBT] should be unit and orthogonal.

    // 假设Bezier曲线有G1连续度
    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity.  Otherwise, the TNB will not be
    // be defined at points where this does not hold.

    cerr << "\t>>> evalBezier has been called with the following input:" << endl;
    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;

    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
    cerr << "\t>>> Returning curve." << endl;

    //for each piece segment
    float t = 0.0f;
    float incre = 1.0f/steps;
    //Vector3f B0(0,0,1);
    Vector4f tempResult;
    Vector3f V;//q(t)
    Vector3f T;//q'(t).normalize()
    Vector3f N;//B(i-1).cross(Ti)
    Vector3f B;//B0, or Bi=Ti.cross(Ni)
    CurvePoint currentPoint;
    Curve output = Curve();
    //vector<Vector4f> vv;
    for(int i = 0; i < P.size()-1; i+=3)
    {
        t = 0.0f;
        //vv.clear();
        //for(int k = i;k <= i+3;++k)
        //{
        //    vv.push_back(Vector4f(P[k][0],P[k][1],P[k][2],0.0f));
        //}
        //Matrix4f MatrixCp(vv[0],vv[1],vv[2],vv[3]);
        Matrix4f MatrixCp(Vector4f(P[i],0.0f),Vector4f(P[i+1],0.0f),Vector4f(P[i+2],0.0f),Vector4f(P[i+3],0.0f));
        Matrix4f Control_Bezier = MatrixCp*MatrixBezier;
        while(t <= 1.0f)
        {
            Vector4f PowerBasis(1,t,t*t,t*t*t);
            tempResult = Control_Bezier*PowerBasis;
            V = Vector3f(tempResult[0],tempResult[1],tempResult[2]);

            Vector4f PowerBasis2(0,1,2*t,3*t*t);
            tempResult = Control_Bezier*PowerBasis2;
            T = Vector3f(tempResult[0],tempResult[1],tempResult[2]);
            T.normalize();
            if(t == 0.0f)
            {
                B = Vector3f(0,0,1);
                N = Vector3f::cross(B, T);
                N.normalize();
                B = Vector3f::cross(T,N);
                B.normalize();
            }
            else
            {
                N = Vector3f::cross(B,T);
                N.normalize();
                B = Vector3f::cross(T,N);
                B.normalize();
            }
            currentPoint.N = N;
            currentPoint.B = B;
            currentPoint.T = T;
            currentPoint.V = V;
            output.push_back(currentPoint);

            t += incre;
        }
    }

    // Right now this will just return this empty curve.
    //return Curve();
    return output;
}

Curve evalBspline( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 )
    {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit( 0 );
    }

    // TODO:
    // It is suggested that you implement this function by changing
    // basis from B-spline to Bezier.  That way, you can just call
    // your evalBezier function.

    cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
    cerr << "\t>>> Returning curve." << endl;

    //for each piece segment
    float t = 0.0f;
    float incre = 1.0f/steps;
    //Vector3f B0(0,0,1);
    Vector4f tempResult;
    Vector3f V;//q(t)
    Vector3f T;//q'(t).normalize()
    Vector3f N;//B(i-1).cross(Ti)
    Vector3f B;//B0, or Bi=Ti.cross(Ni)
    CurvePoint currentPoint;
    Curve output = Curve();
    //vector<Vector4f> vv;
    for(int i = 0; i < P.size()-3; ++i)
    {
        t = 0.0f;
        Matrix4f MatrixCp(Vector4f(P[i],0.0f),Vector4f(P[i+1],0.0f),Vector4f(P[i+2],0.0f),Vector4f(P[i+3],0.0f));
        Matrix4f Control_Bezier = MatrixCp*MatrixBSpline;
        while(t <= 1.0f)
        {
            Vector4f PowerBasis(1,t,t*t,t*t*t);
            tempResult = Control_Bezier*PowerBasis;
            V = Vector3f(tempResult[0],tempResult[1],tempResult[2]);

            Vector4f PowerBasis2(0,1,2*t,3*t*t);
            tempResult = Control_Bezier*PowerBasis2;
            T = Vector3f(tempResult[0],tempResult[1],tempResult[2]);
            T.normalize();
            if(t == 0.0f)
            {
                B = Vector3f(0,0,1);
                N = Vector3f::cross(B, T);
                N.normalize();
                B = Vector3f::cross(T,N);
                B.normalize();
            }
            else
            {
                N = Vector3f::cross(B,T);
                N.normalize();
                B = Vector3f::cross(T,N);
                B.normalize();
            }
            currentPoint.N = N;
            currentPoint.B = B;
            currentPoint.T = T;
            currentPoint.V = V;
            output.push_back(currentPoint);

            t += incre;
        }
    }
    return output;
}

Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).

    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f( cos(t), sin(t), 0 );

        // Tangent vector is first derivative
        R[i].T = Vector3f( -sin(t), cos(t), 0 );

        // Normal vector is second derivative
        R[i].N = Vector3f( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = Vector3f( 0, 0, 1 );
    }
    return R;
}

void drawCurve( const Curve& curve, float framesize )
{
    // Save current state of OpenGL
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Setup for line drawing
    glDisable( GL_LIGHTING );
    glColor4f( 1, 1, 1, 1 );
    glLineWidth( 1 );

    // Draw curve
    glBegin( GL_LINE_STRIP );
    for( unsigned i = 0; i < curve.size(); ++i )
    {
        glVertex( curve[ i ].V );
    }
    glEnd();

    glLineWidth( 1 );

    // Draw coordinate frames if framesize nonzero
    if( framesize != 0.0f )
    {
        Matrix4f M;

        for( unsigned i = 0; i < curve.size(); ++i )
        {
            M.setCol( 0, Vector4f( curve[i].N, 0 ) );
            M.setCol( 1, Vector4f( curve[i].B, 0 ) );
            M.setCol( 2, Vector4f( curve[i].T, 0 ) );
            M.setCol( 3, Vector4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf( M );
            glScaled( framesize, framesize, framesize );
            glBegin( GL_LINES );
            //normal==红色
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            //binormal==绿色
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            //tangent==蓝色
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }

    // Pop state
    glPopAttrib();
}

