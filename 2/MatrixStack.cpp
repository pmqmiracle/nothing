#include "MatrixStack.h"

MatrixStack::MatrixStack()
{
	// Initialize the matrix stack with the identity matrix.
    // 这里炒鸡大bug!!!不能以为是第一个元素
    // m_matrices[0] = Matrix4f::identity();

    //Git
    m_matrices.push_back(Matrix4f::identity());
}

void MatrixStack::clear()
{
	// Revert to just containing the identity matrix.
    m_matrices.clear();
    // 这里炒鸡大bug!!!不能调用默认构造函数来当作一般函数使用
    // MatrixStack();
    m_matrices.push_back(Matrix4f::identity());
}

Matrix4f MatrixStack::top()
{
	// Return the top of the stack
    return m_matrices[m_matrices.size()-1];

    //Git
    //return m_matrices.back();
}

void MatrixStack::push( const Matrix4f& m )
{
	// Push m onto the stack.
	// Your stack should have OpenGL semantics:
	// the new top should be the old top multiplied by m
    Matrix4f newtop = top()*m;
    m_matrices.push_back(newtop);

    //Git
    //m_matrices.push_back(m_matrices.back()*m);
}

void MatrixStack::pop()
{
	// Remove the top element from the stack
    m_matrices.pop_back();
}
