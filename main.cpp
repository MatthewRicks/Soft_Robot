//-------------------------------------------------------
// Filename: main.cpp
//
// Description:  The cpp file for Soft Robot Model
//
// Creators:  Matthew Ricks & Ryker Haddock
//
// Creation Date: 11/9/2017
//-------------------------------------------------------

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();
    w.show();
    return a.exec();
}
