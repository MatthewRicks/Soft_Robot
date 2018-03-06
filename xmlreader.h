//-------------------------------------------------------
// Filename: xmlreader.h
//
// Description:
//
// Creators:  Matthew Ricks & Ryker Haddock
//
// Creation Date: 11/9/2017
//-------------------------------------------------------
#ifndef XMLREADER_H
#define XMLREADER_H
#include <QIODevice>
#include <QXmlStreamReader>
#include <QString>
#include <list>
#include "joint.h"

class XmlReader
{
public:
    XmlReader(std::list<Joint*> &linkedlist);
    bool read(QIODevice *device);
    QString errorString() const;

protected:
    QXmlStreamReader mReader;
    std::list<Joint*> *mLinkedList;

    struct Vector3
    {
        double mX{0};
        double mY{0};
        double mZ{0};
    };

    void read_joints();
    void read_joint();
    void read_id(int &id);
    bool read_color(Vector3 &color);
    bool read_xyz(Vector3 &vec);
    void read_size(double &size1, double &size2);
    void read_axis(double &axis1, double &axis2);
};

#endif // XMLREADER_H
