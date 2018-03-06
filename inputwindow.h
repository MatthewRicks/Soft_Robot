#ifndef INPUTWINDOW_H
#define INPUTWINDOW_H
#include <QWidget>
#include "osgwidget.h"

namespace Ui {
class InputWindow;
}

class InputWindow : public QWidget
{
    Q_OBJECT

public:
    explicit InputWindow(QWidget *parent = 0);
    ~InputWindow();

    void set_pose();

signals:
    void shape_created(QString shape, osg::Vec3 size, osg::Vec3 translation, osg::Vec3 rotation, osg::Vec3 color);
    void starting_pose(osg::MatrixTransform* transform);

private slots:
    void on_OK_clicked();

    void on_Cancel_clicked();

    void on_Box_clicked();

    void on_Cone_clicked();

    void on_Sphere_clicked();

    void update_color_label();

    void on_redSlider_valueChanged(int value);

    void on_greenSlider_valueChanged(int value);

    void on_blueSlider_valueChanged(int value);

    void on_Cylinder_clicked();

private:
    Ui::InputWindow *ui;
    bool mShape{true};
};

#endif // INPUTWINDOW_H
