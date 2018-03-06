//-------------------------------------------------------
// Filename: mainwindow.cpp
//
// Description: Functions for the main window
//
// Creators:  Matthew Ricks & Ryker Haddock
//
// Creation Date: 11/9/2017
//-------------------------------------------------------
#include "mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>
#include "ui_mainwindow.h"
#include <QTextStream>
#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //connects the menu button to the function
    connect(ui->actionAbout,SIGNAL(triggered(bool)),SLOT(actionAbout_triggered(bool)));
    connect(ui->actionOpen,SIGNAL(triggered(bool)),SLOT(actionOpen_triggered(bool)));
    connect(ui->actionExit,SIGNAL(triggered(bool)),SLOT(actionExit_triggered(bool)));
    connect(ui->actionSave,SIGNAL(triggered(bool)),SLOT(actionSave_triggered(bool)));
    connect(ui->actionSave_As,SIGNAL(triggered(bool)),SLOT(actionSave_As_triggered(bool)));
    connect(ui->actionView_Floor,SIGNAL(triggered(bool)),SLOT(actionView_Floor_triggered(bool)));
    connect(ui->actionHide_Axis,SIGNAL(triggered(bool)),SLOT(actionHide_Axis_triggered(bool)));
    connect(ui->actionAdd_Shape,SIGNAL(triggered(bool)),SLOT(actionAdd_Shape_triggered(bool)));
    connect(ui->actionRemove_Shape,SIGNAL(triggered(bool)),SLOT(actionRemove_Shape_triggered(bool)));
    connect(ui->actionStarting_Position,SIGNAL(triggered(bool)),SLOT(actionStarting_Position_triggered(bool)));

    ui->JointsList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->JointsList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    //Makes sure we get valid inputs for U, V, and Size
    ui->lineEdit_Size->setValidator(new QDoubleValidator(.5, 100, 10, this));
    ui->lineEdit_U->setValidator(new QDoubleValidator(-1.6,1.6,10,this));
    ui->lineEdit_V->setValidator(new QDoubleValidator(-1.6,1.6,10,this));

    //initialize some parameters
    ui->actionRemove_Shape->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!mSave)
    {
        QMessageBox msgBox;
        int ret = QMessageBox::warning(this, tr("Unsaved Changes"), tr("The document has been modified.\n"
                                                                       "Do you want to save your changes?"),
                                       QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        //Set actions for Save, Discard, and Cancel buttons
        switch (ret)
        {
        case QMessageBox::Save:
            actionSave_triggered(true);
            break;
            //Discard should close the program down gracefully
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
            event->ignore();
            break;
        default:
            break;
        }
    }
}

void MainWindow::actionAbout_triggered(bool)
{
    QMessageBox::about(this, "About", "Press 'h' to return to home view\n\nSoft Robot Visualization Tool\nCreated by: Matthew Ricks & Ryker Haddock");
}

void MainWindow::actionView_Floor_triggered(bool)
{
    if (ui->actionView_Floor->text()=="View Floor")
    {
        ui->graphicsView->view_floor(true);
        ui->actionView_Floor->setText("Hide Floor");
        ui->graphicsView->update();
    }
    else
    {
        ui->actionView_Floor->setText("View Floor");
        ui->graphicsView->view_floor(false);
        ui->graphicsView->update();
    }
}

void MainWindow::actionAdd_Shape_triggered(bool)
{
    mInputWindow = new InputWindow;
    mInputWindow->show();
    connect(mInputWindow,SIGNAL(shape_created(QString, osg::Vec3, osg::Vec3, osg::Vec3, osg::Vec3)),SLOT(shapecreated(QString, osg::Vec3, osg::Vec3, osg::Vec3, osg::Vec3)));
}

void MainWindow::actionRemove_Shape_triggered(bool)
{
    int k{0};
    if (ui->actionView_Floor->text()=="Hide Floor")
        k++;
    ui->graphicsView->removeShape(k);
    mNumShapes--;
    if (mNumShapes == 0)
    {
        ui->actionRemove_Shape->setEnabled(false);
    }
    ui->graphicsView->update();
}

void MainWindow::actionStarting_Position_triggered(bool)
{
    mInputWindow = new InputWindow;
    mInputWindow->set_pose();
    mInputWindow->show();
    connect(mInputWindow,SIGNAL(starting_pose(osg::MatrixTransform*)),SLOT(starting_pose(osg::MatrixTransform*)));
}

void MainWindow::starting_pose(osg::MatrixTransform *transform)
{
    ui->graphicsView->set_starting_pose(transform);
    if (mList.size()>0)
    {
        ui->graphicsView->change_joint_config(0,mList);
        ui->graphicsView->update();
    }
    show_matrix();
}

void MainWindow::shapecreated(QString shape, osg::Vec3 size, osg::Vec3 translation, osg::Vec3 rotation, osg::Vec3 color)
{
    ui->graphicsView->create_shape(shape,size,translation,rotation,color);
    ui->actionRemove_Shape->setEnabled(true);
    mNumShapes++;
    ui->graphicsView->update();
}

void MainWindow::actionOpen_triggered(bool)
{
    // tr sets the title for the open window, "C://" sets which directory is the default
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), "C://","XML files (*.xml);;All files (*.*)");

    if(!mList.empty())
    {
        QMessageBox msgBox;
        int ret = QMessageBox::warning(this, tr("Open New File"), tr("By opening this file, you will lose any unsaved changes.\n"
                                                                     "Do you want to save your changes?"),
                                       QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        //Set actions for Save, Discard, and Cancel buttons
        switch (ret)
        {
        case QMessageBox::Save:
            actionSave_triggered(true);
            break;
            //Discard should close the program down gracefully
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
            return;
            break;
        default:
            break;
        }
    }

    mList.clear();
    ui->graphicsView->reset();
    mRow_edit = -1;

    if (ui->actionHide_Axis->text() != "Hide Axis")
        ui->actionHide_Axis->setText("Hide Axis");

    if (ui->actionView_Floor->text() != "View Floor")
        ui->actionView_Floor->setText("View Floor");

    std::list<Joint*> linkedlist{};
    QFile file(filename);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QString error{"Cannot Read File"};
        QMessageBox::warning(this, "Error", error);
        return;
    }

    //Reads in the file
    XmlReader joint_reader(linkedlist);

    if (!joint_reader.read(&file))
    {
        QString error{"Parse error in file\n"};
        error += joint_reader.errorString();
        QMessageBox::warning(this, "File Read Error", error);
        return;
    }

    //Updates the list
    for (std::list<Joint*>::iterator it= linkedlist.begin(); it != linkedlist.end(); it++)
    {
        mList.push_back(*it);
    }

    //Updates the GUI
    ui->graphicsView->open_arm(mList);
    mName = filename;
    mSave=true;
    update_list();
}

void MainWindow::actionExit_triggered(bool)
{
    close();
}

void MainWindow::actionSave_triggered(bool)
{
    QFile file(mName);

    //if there is no file to save to, call Save As
    if (mName.isEmpty())
    {
        actionSave_As_triggered(true);
    }

    else if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        QString error{"Cannot Read File"};
        QMessageBox::warning(this, "File Read Error", error);
    }
    //Write to the specified file
    else
    {
        XmlWriter joint_writer(mList);
        joint_writer.write(&file);
        mSave = true;
    }
}

void MainWindow::actionSave_As_triggered(bool)
{
    //Specify a file name and location to save to
    QString name = QFileDialog::getSaveFileName(this, tr("Save As"), "C://", "XML Files (*.xml)");

    //Allows for cancelling during save as process
    if (name.isEmpty())
    {
        return;
    }

    mName = name;
    actionSave_triggered(true);
}

void MainWindow::actionHide_Axis_triggered(bool)
{
    if (ui->actionHide_Axis->text()=="View Axis")
    {
        ui->graphicsView->drawAxis(true);
        ui->actionHide_Axis->setText("Hide Axis");
        ui->graphicsView->update();
    }
    else
    {
        ui->actionHide_Axis->setText("View Axis");
        ui->graphicsView->drawAxis(false);
        ui->graphicsView->update();
    }
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = ui->JointsList->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu myMenu;
    myMenu.addAction("Delete",  this, SLOT(deleteItem()));

    // Show context menu at handling position
    myMenu.exec(globalPos);
}

void MainWindow::update_list()
{
    int i = 1;
    ui->JointsList->clear();
    for (std::list<Joint*>::iterator it= mList.begin(); it != mList.end(); it++)
    {
        QString string{"Joint "};
        string.append(QString::number(i));
        ui->JointsList->addItem(string);
        i++;
    }
    //Colors the row that we are editing
    if (mRow_edit>=0 && mList.size()>0)
        ui->JointsList->item(mRow_edit)->setBackgroundColor(Qt::lightGray);
}

void MainWindow::on_Add_Joint_clicked()
{
    static int id{0};
    id++;
    Joint* j = new Joint(id,5,1);
    j->set_color(0,0,0);
    mList.push_back(j);
    update_list();
    mSave = false;
    ui->graphicsView->create_arm(mList);
    ui->graphicsView->update();
}

void MainWindow::deleteItem()
{
    if (mList.size()<=0)
    {
        mList.clear();
        mRow_edit = -1;
    }
    else if (mList.size()==1)
    {
        ui->graphicsView->erase_joint(0,mList);
        mList.clear();
        mRow_edit = -1;
    }
    else
    {
        //Find selected item in the list and deletes it
        std::list<Joint*>::iterator it= mList.begin();
        int del = ui->JointsList->currentRow();
        std::advance(it,del);
        ui->graphicsView->erase_joint(del,mList);
        mList.remove(*it);
        if (del==mRow_edit)
            mRow_edit = -1;
        else if (del<mRow_edit)
            mRow_edit= mRow_edit-1;
    }
    //remove it from the display list
    update_list();
    ui->graphicsView->update();
    if (mList.size()>0)
    {
        int k = mRow_edit;
        mRow_edit = 0;
        on_lineEdit_U_editingFinished();
        mRow_edit = k;
    }
    show_matrix();
}

void MainWindow::on_JointsList_itemDoubleClicked(QListWidgetItem *item)
{
    // Visual on Graphics window of selected joint
    if (mRow_edit!=-1)
        ui->graphicsView->select_joint(mRow_edit,false);
    mRow_edit = ui->JointsList->currentRow();
    ui->graphicsView->select_joint(mRow_edit,true);
    ui->graphicsView->update();

    std::list<Joint*>::iterator it= mList.begin();
    std::advance(it,mRow_edit);
    double h,r,u,v,red,green,blue;
    (*it)->get_size(h,r);
    (*it)->get_axis(u,v);
    (*it)->get_color(red,green,blue);

    // Update all the sliders and text boxes
    ui->lineEdit_Size->setText(QString::number(h));
    ui->lineEdit_U->setText(QString::number(u));
    ui->lineEdit_V->setText(QString::number(v));
    ui->redSlider->setValue(red);
    ui->greenSlider->setValue(green);
    ui->blueSlider->setValue(blue);
    ui->u_slider->setValue(int(u*100));
    ui->v_slider->setValue(int(v*100));
    update_list();
    show_matrix();
}

void MainWindow::show_matrix()
{
    ui->outputWindow->clear();

    if (mRow_edit==-1)
    {
        return;
    }

    osg::MatrixTransform* matrix = ui->graphicsView->output_matrix(mRow_edit,mList);
    osg::Matrix m = matrix->getMatrix();
    QString string;


    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            string.append(QString::number(m(j,i),'f',3));
            string.append("  ");
        }
        string.append("\n\n");
    }
    ui->outputWindow->setText(string);
}

void MainWindow::on_lineEdit_U_editingFinished()
{
    ui->u_slider->setValue(int(ui->lineEdit_U->text().toDouble()*100));
    update_UV();
}

void MainWindow::on_lineEdit_V_editingFinished()
{
    ui->v_slider->setValue(int(ui->lineEdit_V->text().toDouble()*100));
    update_UV();
}

void MainWindow::on_lineEdit_Size_editingFinished()
{
    if (mRow_edit>=0 && mRow_edit<= mList.size())
    {
        std::list<Joint*>::iterator it= mList.begin();
        std::advance(it,mRow_edit);
        double h, r;
        (*it)->get_size(h,r);
        ui->graphicsView->update_joint_size((*it), ui->lineEdit_Size->text().toDouble(),r);
        ui->graphicsView->change_joint_config(mRow_edit,mList);
        ui->graphicsView->update();
        mSave = false;
    }
    show_matrix();
}

void MainWindow::update_color_label()
{
    if (mRow_edit>=0 && mRow_edit<= mList.size())
    {
        mSave = false;
    }
    double r = ui->redSlider->value();
    double g = ui->greenSlider->value();
    double b = ui->blueSlider->value();
    ui->colorLabel->setStyleSheet(QString("background-color: rgb(%1, %2, %3);").arg(r).arg(g).arg(b));
    if (mRow_edit>=0 && mRow_edit<= mList.size())
    {
        std::list<Joint*>::iterator it= mList.begin();
        std::advance(it,mRow_edit);
        (*it)->set_color(r,g,b);
        ui->graphicsView->joint_color( mRow_edit,(*it));
        ui->graphicsView->update();
    }
}

void MainWindow::on_redSlider_valueChanged(int value)
{
    update_color_label();
}

void MainWindow::on_greenSlider_valueChanged(int value)
{
    update_color_label();
}

void MainWindow::on_blueSlider_valueChanged(int value)
{
    update_color_label();
}

void MainWindow::update_UV()
{
    if (mRow_edit>=0 && mRow_edit<=mList.size())
    {
        std::list<Joint*>::iterator it= mList.begin();
        std::advance(it,mRow_edit);
        double u = ui->lineEdit_U->text().toDouble();
        double v = ui->lineEdit_V->text().toDouble();
        (*it)->set_axis(u,v);
        ui->graphicsView->change_joint_config(mRow_edit,mList);
        ui->graphicsView->update();
        show_matrix();
        mSave = false;
        if(mRecordMacro == true)
        {
            //store new U and V in mMacro
            mMacro.push_back(QString("%1 %2 %3").arg(mRow_edit).arg(u).arg(v));
        }
    }
}

void MainWindow::on_u_slider_valueChanged(int value)
{
    double new_value = value/100.0;
    ui->lineEdit_U->setText(QString("%1").arg(new_value));
    update_UV();
}

void MainWindow::on_v_slider_valueChanged(int value)
{
    double new_value = value/100.0;
    ui->lineEdit_V->setText(QString("%1").arg(new_value));
    update_UV();
}

void MainWindow::on_actionRecord_Macro_triggered(bool checked)
{
    mRecordMacro = checked;
    if(mRecordMacro == false && mMacro.size() != mList.size())
    {
        QMessageBox msgBox;
        int ret = QMessageBox::warning(this, tr("Unsaved Macro"), tr("Finished recording macro.\n"
                                                                       "Do you want to save? \n"),
                                       QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        //Set actions for Yes, and No buttons
        if(ret == QMessageBox::Yes)
        {
            save_macro();
        }

    }
    else
    {
        int i = 0;
        for(auto it = mList.begin(); it != mList.end(); it++)
        {
            double u, v;
            (*it)->get_axis(u,v);
            mMacro.push_back(QString("%1 %2 %3").arg(i).arg(u).arg(v));
            i++;
        }

    }
}

void MainWindow::save_macro()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Save As"), "C://", "Text Files (*.txt)");

    //Allows for cancelling during save as process
    if (name.isEmpty())
    {
        return;
    }
    QFile file(name);
    if ( file.open(QIODevice::WriteOnly | QIODevice::Text) )
    {
        QTextStream stream( &file );
        for(int i = 0; i < mMacro.size(); i++)
        {
            stream << mMacro[i] << endl;
        }
        file.close();
        mMacro.clear();
    }
}

void MainWindow::on_actionRun_Macro_triggered(bool checked)
{
    int line_count = 0;
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), "C://","Text files (*.txt);");
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QStringList line = in.readLine().split(" ");
          if(line.size() != 3)
          {
              ui->outputWindow->setText(QString("Invalid Macro Line: %1").arg(line_count));
              return;
          }
          int joint = (line[0]).toInt();
          double u = (line[1]).toDouble();
          double v = (line[2]).toDouble();
          auto it = mList.begin();
          std::advance(it,joint);
          (*it)->set_axis(u, v);
          ui->graphicsView->change_joint_config(joint,mList);
          ui->graphicsView->update();
          if(mRecordMacro == true)
          {
              //store new U and V in mMacro
              mMacro.push_back(QString("%1 %2 %3").arg(mRow_edit).arg(u).arg(v));
          }
          if(line_count >= mList.size())
          {
              //delay
              QTime dieTime= QTime::currentTime().addMSecs(50);
              while (QTime::currentTime() < dieTime)
              {
                  QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
              }
          }
          line_count++;
       }
       inputFile.close();
    }
    if (mRow_edit!=-1)
    {
        double u,v;
        auto it = mList.begin();
        std::advance(it,mRow_edit);
        (*it)->get_axis(u,v);
        ui->lineEdit_V->setText(QString("%1").arg(v));
        ui->lineEdit_U->setText(QString("%1").arg(u));
        on_lineEdit_U_editingFinished();
        on_lineEdit_V_editingFinished();
    }
}
