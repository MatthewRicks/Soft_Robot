//-------------------------------------------------------
// Filename: xmlwriter.cpp
//
// Description:
//
// Creators:  Matthew Ricks & Ryker Haddock
//
// Creation Date: 11/9/2017
//-------------------------------------------------------
#include "xmlwriter.h"
#include<list>

XmlWriter::XmlWriter(std::list<Joint*> &linkedlist):
    mLinkedList{linkedlist}
{

}
void XmlWriter::write(QIODevice *device)
{
    mWriter.setDevice(device);
    mWriter.setAutoFormatting(true);
    mWriter.writeStartDocument();

    write_joints();

    mWriter.writeEndDocument();
}

void XmlWriter::write_joints()
{
    mWriter.writeStartElement("joints");

    for (std::list<Joint*>::iterator it=mLinkedList.begin(); it != mLinkedList.end(); it++)
    {
        write_joint(*it);
    }

    mWriter.writeEndElement(); // joints
}
void XmlWriter::write_joint(Joint *joint)
{
    mWriter.writeStartElement("joint");

    int id{0};
    Vector3 color;
    double size1, size2, axis1, axis2;

    id=joint->get_id();
    joint->get_color(color.mX,color.mY,color.mZ);
    joint->get_size(size1,size2);
    joint->get_axis(axis1,axis2);

    write_id(id);

    mWriter.writeStartElement("color");
    write_color(color);
    mWriter.writeEndElement();

    mWriter.writeStartElement("size");
    write_size(size1,size2);
    mWriter.writeEndElement();

    mWriter.writeStartElement("axis");
    write_axis(axis1,axis2);
    mWriter.writeEndElement();

    mWriter.writeEndElement();//joint
}

void XmlWriter::write_id(int id)
{
    mWriter.writeTextElement("id", QString::number(id));
}

void XmlWriter::write_size(double height, double radius)
{
    mWriter.writeTextElement("height", QString::number(height));
    mWriter.writeTextElement("radius", QString::number(radius));
}

void XmlWriter::write_color(Vector3 &color)
{
    mWriter.writeTextElement("red", QString::number(color.mX));
    mWriter.writeTextElement("green", QString::number(color.mY));
    mWriter.writeTextElement("blue", QString::number(color.mZ));
}

void XmlWriter::write_axis(double u, double v)
{
    mWriter.writeTextElement("u", QString::number(u));
    mWriter.writeTextElement("v", QString::number(v));
}

void XmlWriter::write_xyz(Vector3 &vec)
{
    mWriter.writeTextElement("x", QString::number(vec.mX));
    mWriter.writeTextElement("y", QString::number(vec.mY));
    mWriter.writeTextElement("z", QString::number(vec.mZ));
}
