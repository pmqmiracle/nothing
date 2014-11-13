#include "GL/freeglut.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include "include/vecmath/vecmath.h"
using namespace std;

// Globals
// You will need more global variables to implement color and position changes
const int MAX_BUFFER_SIZE = 1024;
static int count = 0;
//#define LEFT_KEY 37
//#define UP_KEY 38
//#define RIGHT_KEY 39
//#define DOWN_KEY 40
//因为这里程序只是callback所以不用static也可以
//static int first_cnt = 0;
//static int second_cnt = 0;
int first_cnt = 0;
int second_cnt = 0;

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned int> > vecf;

// These are convenience functions which allow us to call OpenGL
// methods on Vec3d objects
inline void glVertex(const Vector3f &a)
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a)
{ glNormal3fv(a); }

// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // add code to change color here
        count++;
        count = count%4;
        cout << "Color change to # " << count << endl;
        break;
    default:
        cout << "Unhandled key press " << key << "." << endl;
    }
	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP:
        second_cnt++;
		break;
    case GLUT_KEY_DOWN:
        second_cnt--;
		break;
    case GLUT_KEY_LEFT:
        first_cnt--;
		break;
    case GLUT_KEY_RIGHT:
        first_cnt++;
		break;
    }
	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}

// This function is responsible for displaying the object.
void drawScene(void)
{
    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.
    gluLookAt(0.0, 0.0, 5.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    // Set material properties of object

	// Here are some colors you might use - feel free to add more
    GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
                                 {0.9, 0.5, 0.5, 1.0},
                                 {0.5, 0.9, 0.3, 1.0},
                                 {0.3, 0.8, 0.9, 1.0} };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[count]);

	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    // Light position
    // 需要加上f,否则报错出现double==>float, 不过也可以加上-fpermissive来忽略
	GLfloat Lt0pos[] = {(1.0f+first_cnt*0.5), (1.0f+second_cnt*0.5), 5.0f, 1.0f};
    cout << "Debug counting: " << first_cnt << ", " << second_cnt << endl;
    cout << "Debug Light Posistion" << Lt0pos[0] << ", " << Lt0pos[1] << endl;

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
    // teapot
	// glutSolidTeapot(1.0);

    cout << "======Drawing OBJ model=============\n";
    int a, c, d, f, g, i;
    for(int ii = 0;ii < vecf.size();++ii)
    {
        a = vecf[ii][0];
        c = vecf[ii][1];
        d = vecf[ii][2];
        f = vecf[ii][3];
        g = vecf[ii][4];
        i = vecf[ii][5];
        glBegin(GL_TRIANGLES);
        glNormal3d(vecn[c-1][0],vecn[c-1][1],vecn[c-1][2]);
        glVertex3d(vecv[a-1][0],vecv[a-1][1],vecv[a-1][2]);
        glNormal3d(vecn[f-1][0],vecn[f-1][1],vecn[f-1][2]);
        glVertex3d(vecv[d-1][0],vecv[d-1][1],vecv[d-1][2]);
        glNormal3d(vecn[i-1][0],vecn[i-1][1],vecn[i-1][2]);
        glVertex3d(vecv[g-1][0],vecv[g-1][1],vecv[g-1][2]);
        glEnd();
    }

    // Dump the image to the screen.
    glutSwapBuffers();
}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

void loadInput()
{
	// load the OBJ file here
    char buffer[MAX_BUFFER_SIZE];
    // for face index
    int a,b,c,d,e,f,g,h,i;
    Vector3f v;
    string token;
    string abc, def, ghi;
    string delimiter = "/";
    cout << "Loading OBJ files..." << endl;
    while(cin.getline(buffer,MAX_BUFFER_SIZE)!=0)
    {
        //cin.getline得到一行一行的an array of characters,可以用这个特性去step through each line of the file
        //you can parse it using a stringstream对象
        //创建stringstream对象from an array of characters(buffer) as follows
        stringstream ss(buffer);
        ss >> token;
        if(token == "v")
        {
            ss >> v[0] >> v[1] >> v[2];
            vecv.push_back(v);
        }
        else if(token == "vn")
        {
            ss >> v[0] >> v[1] >> v[2];
            vecn.push_back(v);
        }
        //method #1 to handle face
        /*else if(token == "f")
        {
            string tmp;
            ss >> abc >> def >> ghi;
            //ignore b,e,h
            //for abc
            size_t pos = 0;
            //string STL 找某个子字符串出现的第一个位置posistion
            pos = abc.find(delimiter);
            //string STL 提取出字符串的一个字串
            tmp = abc.substr(0,pos);
            stringstream convert_a(tmp);
            convert_a >> a;

            //string STL 删除字符串的一部分
            abc.erase(0,pos+delimiter.length());
            pos = abc.find(delimiter);
            abc.erase(0,pos+delimiter.length());
            stringstream convert_c(abc);
            convert_c >> c;
            //for def
            pos = def.find(delimiter);
            tmp = def.substr(0,pos);
            stringstream convert_d(tmp);
            convert_d >> d;

            def.erase(0,pos+delimiter.length());
            pos = def.find(delimiter);
            def.erase(0,pos+delimiter.length());
            stringstream convert_f(def);
            convert_f >> f;
            //for ghi
            pos = ghi.find(delimiter);
            tmp = ghi.substr(0,pos);
            stringstream convert_g(tmp);
            convert_g >> g;

            ghi.erase(0,pos+delimiter.length());
            pos = ghi.find(delimiter);
            ghi.erase(0,pos+delimiter.length());
            stringstream convert_i(ghi);
            convert_i >> i;

            vector<unsigned int> face;
            face.push_back(a);
            face.push_back(c);
            face.push_back(d);
            face.push_back(f);
            face.push_back(g);
            face.push_back(i);
            vecf.push_back(face);
        }*/
        //method #2 to handle face
        else if(token == "f")
        {
            vector<unsigned int> vv;
            string parts[3];
            ss >> parts[0] >> parts[1] >> parts[2];
            unsigned int number;
            for(int i = 0;i < 3;++i)
            {
                size_t pos1,pos2;
                pos1 = parts[i].find(delimiter);
                pos2 = parts[i].find_last_of(delimiter);
                //stringstream s1(parts[i].substr(0,pos1));
                //s1 >> number;
                //或者更简单地写成
                stringstream(parts[i].substr(0,pos1)) >> number;
                vv.push_back(number);
                //stringstream s2(parts[i].substr(pos2+1,parts[i].length()));
                //s2 >> number;
                stringstream(parts[i].substr(pos2+1,parts[i].length())) >> number;
                vv.push_back(number);
            }
            vecf.push_back(vv);
        }
        else
            continue;
    }
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    loadInput();

    glutInit(&argc,argv);

    // We're going to animate it, so double buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 600, 600 );
    glutCreateWindow("Assignment 0");

    cout << "Miracle" << endl;
    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

    // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}
