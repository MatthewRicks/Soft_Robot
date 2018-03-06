#include "inputwindow.h"
#include "ui_inputwindow.h"
#include <QMessageBox>
#include <QCloseEvent>

InputWindow::InputWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputWindow)
{
    ui->setupUi(this);

    ui->transx->setValidator(new QDoubleValidator(this));
    ui->transy->setValidator(new QDoubleValidator(this));
    ui->transz->setValidator(new QDoubleValidator(this));

    ui->sizx->setValidator(new QDoubleValidator(.001, 100, 10, this));
    ui->sizy->setValidator(new QDoubleValidator(.001, 100, 10, this));
    ui->sizz->setValidator(new QDoubleValidator(.001, 100, 10, this));

    ui->rotx->setValidator(new QDoubleValidator(-360, 360, 10, this));
    ui->roty->setValidator(new QDoubleValidator(-360, 360, 10, this));
    ui->rotz->setValidator(new QDoubleValidator(-360, 360, 10, this));
    ui->Box->toggle();
}

InputWindow::~InputWindow()
{
    delete ui;
}

void InputWindow::set_pose()
{
    mShape = false;
    ui->sizx->hide();
    ui->sizy->hide();
    ui->sizz->hide();

    ui->redSlider->hide();
    ui->greenSlider->hide();
    ui->blueSlider->hide();

    ui->colorLabel->hide();

    ui->Box->hide();
    ui->Sphere->hide();
    ui->Cone->hide();
    ui->Cylinder->hide();

    ui->Size1->hide();
    ui->Size2->hide();
    ui->Size3->hide();
}

void InputWindow::on_OK_clicked()
{

    osg::Vec3 color, translation, rotation, size;
    QString shape;

    if( ui->transx->text().isEmpty() )
        ui->transx->setText("0");
    if( ui->transy->text().isEmpty() )
        ui->transy->setText("0");
    if( ui->transz->text().isEmpty() )
        ui->transz->setText("0");

    if (ui->sizx->text().isEmpty())
        ui->sizx->setText("1");
    if (ui->sizy->text().isEmpty())
        ui->sizy->setText("1");
    if (ui->sizz->text().isEmpty())
        ui->sizz->setText("1");

    if (ui->rotx->text().isEmpty())
        ui->rotx->setText("0");
    if (ui->roty->text().isEmpty())
        ui->roty->setText("0");
    if (ui->rotz->text().isEmpty())
        ui->rotz->setText("0");

    color.x() = ui->redSlider->value();
    color.y() = ui->greenSlider->value();
    color.z() = ui->blueSlider->value();

    translation.x() = ui->transx->text().toDouble();
    translation.y() = ui->transy->text().toDouble();
    translation.z() = ui->transz->text().toDouble();

    rotation.x() = ui->rotx->text().toDouble();
    rotation.y() = ui->roty->text().toDouble();
    rotation.z() = ui->rotz->text().toDouble();

    size.x() = ui->sizx->text().toDouble();
    size.y() = ui->sizy->text().toDouble();
    size.z() = ui->sizz->text().toDouble();
    if (!mShape)
    {
        osg::MatrixTransform* transform= new osg::MatrixTransform;
        osg::Matrix mt = osg::Matrix::translate(translation.x(), translation.y(), translation.z());
        osg::Matrix mrx = osg::Matrix::rotate(osg::DegreesToRadians((float)rotation.x()),1,0,0);
        osg::Matrix mry = osg::Matrix::rotate(osg::DegreesToRadians((float)rotation.y()),0,1,0);
        osg::Matrix mrz = osg::Matrix::rotate(osg::DegreesToRadians((float)rotation.z()),0,0,1);
        osg::Matrix m = mrx*mry*mrz*mt;
        transform->setMatrix(m);
        emit(starting_pose(transform));
        close();
        return;
    }

    if (ui->Box->isChecked())
        shape = "box";
    else if (ui->Sphere->isChecked())
        shape = "sphere";
    else if (ui->Cylinder->isChecked())
        shape = "cylinder";
    else
        shape = "cone";

    emit(shape_created(shape, size, translation, rotation, color));
    close();
}

void InputWindow::on_Cancel_clicked()
{
    close();
}

void InputWindow::on_Box_clicked()
{
    ui->Size2->show();
    ui->sizy->show();
    ui->Size3->show();
    ui->sizz->show();

    if (ui->Cone->isChecked())
        ui->Cone->toggle();

    if (ui->Sphere->isChecked())
        ui->Sphere->toggle();

    if (ui->Cylinder->isChecked())
        ui->Cylinder->toggle();

    if (ui->Box->isChecked())
    {
        ui->Size1->setText("Size: X");
        ui->Size2->setText("Size: Y");
        ui->Size3->setText("Size: Z");
    }
}

void InputWindow::on_Cone_clicked()
{
    ui->Size2->show();
    ui->sizy->show();
    ui->Size3->hide();
    ui->sizz->hide();

    if (ui->Sphere->isChecked())
        ui->Sphere->toggle();

    if (ui->Cylinder->isChecked())
        ui->Cylinder->toggle();

    if (ui->Box->isChecked())
        ui->Box->toggle();

     if (ui->Cone->isChecked())
     {
         ui->Size1->setText("Height:");
         ui->Size2->setText("Radius");
     }
}

void InputWindow::on_Sphere_clicked()
{
    ui->Size2->hide();
    ui->sizy->hide();
    ui->Size3->hide();
    ui->sizz->hide();

    if (ui->Cone->isChecked())
        ui->Cone->toggle();

    if (ui->Cylinder->isChecked())
        ui->Cylinder->toggle();

    if (ui->Box->isChecked())
        ui->Box->toggle();

     if (ui->Sphere->isChecked())
     {
         ui->Size1->setText("Radius");
     }
}

void InputWindow::on_Cylinder_clicked()
{
    ui->Size2->show();
    ui->sizy->show();
    ui->Size3->hide();
    ui->sizz->hide();

    if (ui->Sphere->isChecked())
        ui->Sphere->toggle();

    if (ui->Cone->isChecked())
        ui->Cone->toggle();

    if (ui->Box->isChecked())
        ui->Box->toggle();

     if (ui->Cylinder->isChecked())
     {
         ui->Size1->setText("Height:");
         ui->Size2->setText("Radius");
     }
}

void InputWindow::update_color_label()
{
    double r = ui->redSlider->value();
    double g = ui->greenSlider->value();
    double b = ui->blueSlider->value();
    ui->colorLabel->setStyleSheet(QString("background-color: rgb(%1, %2, %3);").arg(r).arg(g).arg(b));
}

void InputWindow::on_redSlider_valueChanged(int value)
{
    update_color_label();
}

void InputWindow::on_greenSlider_valueChanged(int value)
{
    update_color_label();
}

void InputWindow::on_blueSlider_valueChanged(int value)
{
    update_color_label();
}


