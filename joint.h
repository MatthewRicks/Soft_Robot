//-------------------------------------------------------
// Filename: joint.h
//
// Description:
//
// Creators:  Matthew Ricks & Ryker Haddock
//
// Creation Date: 11/9/2017
//-------------------------------------------------------
#ifndef JOINT_H
#define JOINT_H

#include <osg/ref_ptr>
#include <osg/MatrixTransform>
#include <vector>

class Joint
{
public:
    Joint(int id, double height, double radius);
    int get_id();
    void set_id(int id);
    void get_axis(double &u, double &v);
    void set_axis(double u, double v);
    void set_size(double height, double radius);
    void get_size(double &height, double &radius);
    void set_color(double red, double green, double blue);
    void get_color(double &red, double &green, double &blue);
    int get_sphere_count();
    osg::MatrixTransform* get_T();
    osg::MatrixTransform* get_Ti(int index);

protected:
    double mU;
    double mV;
    double mHeight;
    double mRadius;
    int mId;
    double mColor[3];
    int mSphereCount;
    std::vector<osg::MatrixTransform*> mTi;

    osg::MatrixTransform *mT, *mT0, *mT1, *mT2, *mT3, *mT4;
private:
    void update_T();
    osg::Matrix get_trans(double height);

};

#endif // JOINT_H
