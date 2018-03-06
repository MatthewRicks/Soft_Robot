//-------------------------------------------------------
// Filename: joint.cpp
//
// Description:
//
// Creators:  Matthew Ricks & Ryker Haddock
//
// Creation Date: 11/9/2017
//-------------------------------------------------------

#include "joint.h"
#include <math.h>

Joint::Joint(int id, double height, double radius)
{
    mId = id;
    mHeight = height;
    mRadius = radius;
    mSphereCount = height/radius;
    if(mSphereCount <= 0)
        mSphereCount = 1;
    mU = 0;
    mV = 0;
    mT = new osg::MatrixTransform;
    for(int i = 0; i < mSphereCount; i++)
    {
        mTi.push_back(new osg::MatrixTransform);
    }
    update_T();
}

int Joint::get_id()
{
    return mId;
}

void Joint::set_id(int id)
{
    mId = id;
}

void Joint::get_axis(double &u, double &v)
{
    u = mU;
    v = mV;
}

void Joint::set_axis(double u, double v)
{
    mU = u;
    mV = v;
    update_T();
}

osg::MatrixTransform* Joint::get_T()
{
    return mT;
}
osg::MatrixTransform* Joint:: get_Ti(int index)
{
    if(index >= mSphereCount)
        index = mSphereCount-1;
    return mTi[index];
}
osg::Matrix Joint::get_trans(double height)
{
    double scale_factor = height/mHeight;
    double u = mU*scale_factor;
    double v = mV*scale_factor;
    double w_mag = std::sqrt(u*u + v*v);
    double phi = w_mag;
    osg::Matrix trans;

    if(std::abs(phi) < std::pow(10,-6))
    {
        trans= osg::Matrix::translate(0, 0, height);
    }
    else
    {
        double sp = std::sin(phi);
        double cp = std::cos(phi);
        double sig = cp-1;
        double ub = u/phi;
        double vb = v/phi;
        double x = 1/(std::abs(u)+std::abs(v));

        trans = osg::Matrix::rotate(phi,u*x,x*v,0)*osg::Matrix::translate(-sig*height*vb/phi,sig*height*ub/phi,height*sp/phi);
    }
    return trans;
}

void Joint::update_T()
{
    mT->setMatrix(get_trans(mHeight));
    for(int i = 0; i < mSphereCount; i++)
    {
        mTi[i]->setMatrix(get_trans(mHeight*i/mSphereCount + 1.0/mSphereCount + .25));
    }
}

void Joint::set_size(double height, double radius)
{
    mHeight = height;
    mRadius = radius;

    mSphereCount = height/radius;
    if(mSphereCount <= 0)
        mSphereCount = 1;
    mTi.clear();
    for(int i = 0; i < mSphereCount; i++)
    {
        mTi.push_back(new osg::MatrixTransform);
    }
    update_T();
}

void Joint::get_size(double &height, double &radius)
{
    height = mHeight;
    radius = mRadius;
}

void Joint::set_color(double red, double green, double blue)
{
    mColor[0] = red;
    mColor[1] = green;
    mColor[2] = blue;
}

void Joint::get_color(double &red, double &green, double &blue)
{
    red = mColor[0];
    green = mColor[1];
    blue = mColor[2];
}
int Joint::get_sphere_count()
{
    return mSphereCount;
}
