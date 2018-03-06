//-------------------------------------------------------
// Filename: mainwindow.h
//
// Description:
//
// Creators:  Matthew Ricks & Ryker Haddock
//
// Creation Date: 11/9/2017
//-------------------------------------------------------
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QWidget>
#include <QDialog>
#include <QFile>
#include "xmlreader.h"
#include "xmlwriter.h"
#include "QListWidgetItem"
#include "osgwidget.h"
#include <vector>
#include "inputwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent (QCloseEvent *event);

private slots:
    void actionAbout_triggered(bool);
    void actionOpen_triggered(bool);
    void actionExit_triggered(bool);
    void actionSave_triggered(bool);
    void actionSave_As_triggered(bool);
    void actionHide_Axis_triggered(bool);
    void update_list();
    void showContextMenu(const QPoint &pos);
    void deleteItem();
    void on_Add_Joint_clicked();
    void actionView_Floor_triggered(bool);
    void actionAdd_Shape_triggered(bool);
    void actionRemove_Shape_triggered(bool);
    void actionStarting_Position_triggered(bool);
    void starting_pose(osg::MatrixTransform* transform);

    void on_JointsList_itemDoubleClicked(QListWidgetItem *item);

    void on_lineEdit_U_editingFinished();
    void on_lineEdit_V_editingFinished();
    void on_lineEdit_Size_editingFinished();

    void on_redSlider_valueChanged(int value);
    void on_greenSlider_valueChanged(int value);
    void on_blueSlider_valueChanged(int value);

    void on_u_slider_valueChanged(int value);
    void on_v_slider_valueChanged(int value);

    void on_actionRecord_Macro_triggered(bool checked);

    void on_actionRun_Macro_triggered(bool checked);

    void shapecreated(QString shape, osg::Vec3 size, osg::Vec3 translation, osg::Vec3 rotation, osg::Vec3 color);

    void show_matrix();


protected:
    //stores all joint information
    std::list<Joint*> mList;

    //Determines whether or not the program has been saved
    bool mSave{true};

    //Stores the name of the save file
    QString mName;

    //This is the index for the double clicked item in the list
    //All edits should reference this number, not current_row
    int mRow_edit{-1};

    InputWindow *mInputWindow;

private:
    Ui::MainWindow *ui;
    void update_color_label();
    std::vector<QString> mMacro;
    bool mRecordMacro{false};
    int mNumShapes{0};

    void save_macro();
    void update_UV();

};

#endif // MAINWINDOW_H
