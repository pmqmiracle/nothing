#include "SkeletalModel.h"

#include <FL/Fl.H>

using namespace std;
#define MAX_BUFFER 1024

void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)
	m_matrixStack.clear();
	m_matrixStack.push(cameraMatrix);
	if( skeletonVisible )
	{
		drawJoints();
		drawSkeleton();
	}
	else//有人物model可以看到
	{
		// Clear out any weird matrix we may have been using for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());
		// Tell the mesh to draw itself.
		m_mesh.draw();
	}
}

void SkeletalModel::loadSkeleton( const char* filename )
{
    //Git
    /*
    ifstream infile(filename);
    char buffer[1000];
    vector<int> jointParents;
    while (infile.getline(buffer, 1000)) {
        stringstream ss(buffer);
        float x, y, z;
        ss >> x >> y >> z;
        Matrix4f translationMatrix = Matrix4f::translation(x,y,z);
        int parent;
        ss >> parent;
        jointParents.push_back(parent);
        Joint* joint = new Joint;
        joint->transform = translationMatrix;
        m_joints.push_back(joint);
        if (parent == -1)
            m_rootJoint = joint;
    }
    for (unsigned i = 0; i < jointParents.size(); i++) {
        if (jointParents[i] == -1) {
            continue;
        }
        Joint* parent = m_joints[jointParents[i]];
        parent->children.push_back(m_joints[i]);
    }
    */

	// Load the skeleton from file here.
    // .skel文件
    // 18个joints
    // 没一行有四个数字,前3个是float, 是当前joint相对于parent的translation,第四个是int,是当前joint的parent的index
    // root的index是-1
    // 其他的joint[0,17]
    ifstream infile(filename);
    char buffer[MAX_BUFFER];
    float x,y,z;
    int parent_index;
    while(infile.getline(buffer,MAX_BUFFER) != 0)
    {
        stringstream ss(buffer);
        ss >> x >> y >> z;

        Joint *jay = new Joint;
        jay->transform = Matrix4f::translation(x,y,z);
        m_joints.push_back(jay);

        int parent_index;
        ss >> parent_index;
        if(parent_index == -1)
            m_rootJoint = jay;
        else
            m_joints[parent_index]->children.push_back(jay);
    }
}

//在火柴人模式下:在每个joint点上画一个小球,递归DFS
void SkeletalModel::drawJointsHelper(Joint *jay)
{
    m_matrixStack.push(jay->transform);
    glLoadMatrixf(m_matrixStack.top());
    glutSolidSphere(0.025f,12,12);
    for(int i = 0;i < jay->children.size();++i)
    {
        drawJointsHelper(jay->children[i]);
    }
    m_matrixStack.pop();
}

void SkeletalModel::drawJoints( )
{
	// Draw a sphere at each joint. You will need to add a recursive helper function to traverse the joint hierarchy.
	//
	// We recommend using glutSolidSphere( 0.025f, 12, 12 )
	// to draw a sphere of reasonable size.
	//
	// You are *not* permitted to use the OpenGL matrix stack commands
	// (glPushMatrix, glPopMatrix, glMultMatrix).
	// You should use your MatrixStack class
	// and use glLoadMatrix() before your drawing call.
    SkeletalModel::drawJointsHelper(m_rootJoint);
}

//在火柴人模式下,给joints之间画肌肉=w= 就是box
void SkeletalModel::drawSkeletonHelper(Joint *jay)
{
    m_matrixStack.push(jay->transform);

    Matrix4f translate = Matrix4f::translation(0,0,0.5);
    for(int i = 0;i < jay->children.size();++i)
    {
        Joint *jolin = jay->children[i];

        Vector3f z = jolin->transform.getCol(3).xyz();
        float length = z.abs();

        Matrix4f scale = Matrix4f::scaling(0.05,0.05,length);
        //注意是normalized() 而不是返回void的normalize()
        Vector3f y = Vector3f::cross(z,Vector3f(0,0,1)).normalized();
        Vector3f x = Vector3f::cross(y,z).normalized();
        Matrix4f rotate = Matrix4f(Vector4f(x,0),Vector4f(y,0),Vector4f(z.normalized(),0),Vector4f(0,0,0,1));

        //顺序有没有关系?
        //炒鸡有关系 不然出现很奇怪的model
        //先平移 后伸缩 再旋转
        //push的顺序恰好要相反
        m_matrixStack.push(rotate);
        m_matrixStack.push(scale);
        m_matrixStack.push(translate);
        glLoadMatrixf(m_matrixStack.top());

        glutSolidCube(1.0f);

        m_matrixStack.pop();
        m_matrixStack.pop();
        m_matrixStack.pop();

        drawSkeletonHelper(jolin);
    }
    m_matrixStack.pop();
}

void SkeletalModel::drawSkeleton( )
{
	// Draw boxes between the joints. You will need to add a recursive helper function to traverse the joint hierarchy.
    SkeletalModel::drawSkeletonHelper(m_rootJoint);
}

//在FLTK的UI模式下可以设置rotate参数
void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	 // Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.
     Joint* joint = m_joints[jointIndex];
     Matrix4f rotation = Matrix4f::rotateX(rX) * Matrix4f::rotateY(rY) * Matrix4f::rotateZ(rZ);
     joint->transform.setSubmatrix3x3(0, 0, rotation.getSubmatrix3x3(0, 0));
}

void SkeletalModel::computeBindWorldToJointTransformsHelper(Joint *jay)
{
    bindWorld2JointTransform.push(jay->transform);
    jay->bindWorldToJointTransform = bindWorld2JointTransform.top().inverse();
    for(int i = 0;i < jay->children.size();++i)
    {
        computeBindWorldToJointTransformsHelper(jay->children[i]);
    }
    bindWorld2JointTransform.pop();
}

void SkeletalModel::computeBindWorldToJointTransforms()
{
	// 2.3.1. Implement this method to compute a per-joint transform from
	// world-space to joint space in the BIND POSE.
	//
	// Note that this needs to be computed only once since there is only
	// a single bind pose.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.

    SkeletalModel::computeBindWorldToJointTransformsHelper(m_rootJoint);
}

void SkeletalModel::updateCurrentJointToWorldTransformsHelper(Joint *jay)
{
    currentJoint2WorldTransform.push(jay->transform);
    jay->currentJointToWorldTransform = currentJoint2WorldTransform.top();
    for(int i = 0;i < jay->children.size();++i)
    {
        updateCurrentJointToWorldTransformsHelper(jay->children[i]);
    }
    currentJoint2WorldTransform.pop();
}

void SkeletalModel::updateCurrentJointToWorldTransforms()
{
	// 2.3.2. Implement this method to compute a per-joint transform from
	// joint space to world space in the CURRENT POSE.
	//
	// The current pose is defined by the rotations you've applied to the
	// joints and hence needs to be *updated* every time the joint angles change.
	//
	// This method should update each joint's bindWorldToJointTransform.? wrong?
	// You will need to add a recursive helper function to traverse the joint hierarchy.

    SkeletalModel::updateCurrentJointToWorldTransformsHelper(m_rootJoint);
}

void SkeletalModel::updateMesh()
{
	// 2.3.2. This is the core of SSD.
	// Implement this method to update the vertices of the mesh
	// given the current state of the skeleton.
	// You will need both the bind pose world --> joint transforms.
	// and the current joint --> world transforms.

    //need ??
    //m_matrixStack.clear();
    //在void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
    //里面已经clear过恢复到camera
    for(int i = 0;i < m_mesh.bindVertices.size();++i)
    {
        vector<float> weights_per_vertex = m_mesh.attachments[i];
        //Vector4f sum(0,0,0,0);
        Vector3f sum(0,0,0);
        //每个网格的顶点,都有记录它与17(与root都为0)个joint的链接权重weights
        for(int j = 1;j < m_joints.size();++j)
        {
            if(weights_per_vertex[j-1] == 0)
                continue;
            //miracle: linear algerbra
            Vector3f trans =  (m_joints[j]->currentJointToWorldTransform
                   * m_joints[j]->bindWorldToJointTransform * Vector4f(m_mesh.bindVertices[i],1)).homogenized().xyz();
            sum = sum + weights_per_vertex[j-1] * trans;
        }
        m_mesh.currentVertices[i] = sum;
    }

    //Git
    /*
    m_matrixStack.clear();
    for (unsigned i = 0; i < m_mesh.bindVertices.size(); i++) {
        Vector4f bind = Vector4f(m_mesh.bindVertices[i], 1);
        vector<float> weights = m_mesh.attachments[i];
        Vector3f sum = Vector3f(0, 0, 0);
        for (unsigned j = 0; j < m_joints.size() - 1; j++) {
            float weight = weights[j];
            if (weight == 0) {
                continue;
            }
            Joint* joint = m_joints[j+1];
            Vector3f transformed = (joint->currentJointToWorldTransform * (joint->bindWorldToJointTransform * bind)).homogenized().xyz();
            sum = sum + weight * transformed;
        }
        m_mesh.currentVertices[i] = sum;
    }
    */
}

