//-------------------------------------------------------
// Filename: xmlwriter.h
//
// Description:  The cpp file for Soft Robot Model
//
// Creators:  Matthew Ricks & Ryker Haddock
//
// Creation Date: 11/9/2017
//-------------------------------------------------------
#ifndef XMLWRITER_H
#define XMLWRITER_H
#include <QIODevice>
#include <QXmlStreamWriter>
#include<list>
#include "xmlreader.h"

class Joint;

class XmlWriter
{
public:
    XmlWriter(std::list<Joint*> &linkedlist);
    void write(QIODevice *device);

protected:
    QXmlStreamWriter mWriter;
    std::list<Joint*> mLinkedList;

    struct Vector3
    {
        double mX{0};
        double mY{0};
        double mZ{0};
    };

    void write_size(double height, double radius);
    void write_joints();
    void write_joint(Joint *joint);
    void write_id(int id);
    void write_xyz(Vector3 &vec);
    void write_color(Vector3 &color);
    void write_axis(double u, double v);
};

#endif // XMLWRITER_H
