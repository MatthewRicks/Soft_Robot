//-------------------------------------------------------
// Filename: xmlreader.cpp
//
// Description:
//
// Creators:  Matthew Ricks & Ryker Haddock
//
// Creation Date: 11/9/2017
//-------------------------------------------------------
#include "xmlreader.h"
#include <QString>
#include <list>

XmlReader::XmlReader(std::list<Joint*> &linkedlist):
    mLinkedList{&linkedlist}
{}

QString XmlReader::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
            .arg(mReader.errorString())
            .arg(mReader.lineNumber())
            .arg(mReader.columnNumber());
}

bool XmlReader::read(QIODevice *device)
{
    mReader.setDevice(device);

    if (mReader.readNextStartElement()) {
        if (mReader.name() == "joints")
            read_joints();
        else
            mReader.raiseError(QObject::tr("Not a Joints File"));
    }
    return !mReader.error();
}

void XmlReader::read_joints()
{
    bool joint{false};
    while(mReader.readNextStartElement())
    {
        if(mReader.name() == "joint")
        {
            read_joint();
            joint = true;
        }
        else
            mReader.skipCurrentElement();
    }
    if (!joint)
        mReader.raiseError("Missing Parameter");
}

void XmlReader::read_joint()
{
    int id{0};
    Vector3 color;
    double size1, size2, axis1, axis2;

    while (mReader.readNextStartElement())
    {
        if (mReader.name() == "id")
            read_id(id);
        else if (mReader.name() == "color")
            read_color(color);
        else if (mReader.name() == "size")
            read_size(size1,size2);
        else if (mReader.name() == "axis")
            read_axis(axis1,axis2);
        else
            mReader.skipCurrentElement();
    }
    Joint* joint = new Joint(id,size1,size2);
    joint->set_color(color.mX,color.mY,color.mZ);
    joint->set_axis(axis1,axis2);
    mLinkedList->push_back(joint);
}

void XmlReader::read_id(int &id)
{
    bool ok{false};
    if (mReader.name() == "id")
    {
        id = mReader.readElementText().toInt(&ok);

        if (!ok)
            mReader.raiseError("Missing joint ID");
    }
}

bool XmlReader::read_color(Vector3 &color)
{
    color.mX=0;
    color.mY=0;
    color.mZ=0;
    //Determines if all information is present
    bool r{false};
    bool g{false};
    bool b{false};

    while(mReader.readNextStartElement())
    {
        if(mReader.name() == "red")
            color.mX = mReader.readElementText().toDouble(&r);
        else if(mReader.name() == "green")
            color.mY = mReader.readElementText().toDouble(&g);
        else if(mReader.name() == "blue")
            color.mZ = mReader.readElementText().toDouble(&b);
        else
            mReader.skipCurrentElement();
    }
    if (r && g && b)
        return true;
    else
    {
        mReader.raiseError("Missing Color Parameter");
        return false;
    }
}
bool XmlReader::read_xyz(Vector3 &vec)
{
    vec.mX=0;
    vec.mY=0;
    vec.mZ=0;
    //Determines if all information is present
    bool x{false};
    bool y{false};
    bool z{false};

    while(mReader.readNextStartElement())
    {
        if(mReader.name() == "x")
            vec.mX = mReader.readElementText().toDouble(&x);
        else if(mReader.name() == "y")
            vec.mY = mReader.readElementText().toDouble(&y);
        else if(mReader.name() == "z")
            vec.mZ = mReader.readElementText().toDouble(&z);
        else
            mReader.skipCurrentElement();
    }
    if(x && y && z)
        return true;

    else
    {
        mReader.raiseError("Missing parameter");
        return false;
    }
}

void XmlReader::read_size(double &size1, double &size2)
{
    size1 = 0;
    size2 = 0;
    bool x{false};
    bool y{false};
    while(mReader.readNextStartElement())
    {
        if(mReader.name() == "height")
            size1 = mReader.readElementText().toDouble(&x);
        else if(mReader.name() == "radius")
            size2 = mReader.readElementText().toDouble(&y);
        else
            mReader.skipCurrentElement();
    }
    if(x && y)
        return;
    else
        mReader.raiseError("Missing Size Paramter");
}

void XmlReader::read_axis(double &axis1, double &axis2)
{
    axis1 = 0;
    axis2 = 0;
    bool x{false};
    bool y{false};
    while(mReader.readNextStartElement())
    {
        if(mReader.name() == "u")
            axis1 = mReader.readElementText().toDouble(&x);
        else if(mReader.name() == "v")
            axis2 = mReader.readElementText().toDouble(&y);
        else
            mReader.skipCurrentElement();
    }
    if(x && y)
        return;
    else
        mReader.raiseError("Missing Axis Paramter");
}
