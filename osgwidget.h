//  ***********************************
//  This class is a used to display 
//  Openscenegraph in a Qt window.
//  
//  This is based upon the work of
//  https://github.com/Submanifold/QtOSG.git
//
//  The was created for
//  MEEN / CEEN 570 - Brigham Young University
//  by Prof. Corey McBride
//  ***********************************


#ifndef MEEN_570_OSGWIDGET
#define MEEN_570_OSGWIDGET

#include <QOpenGLWidget>
#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/CompositeViewer>
#include <osgGA/TrackballManipulator>
#include <osgText/Text>
#include <map>
#include <osg/Geode>
#include "joint.h"
#include <osg/ShapeDrawable>


class OSGWidget : public QOpenGLWidget
{
  Q_OBJECT

public:
  OSGWidget(QWidget* parent = 0,
             Qt::WindowFlags f = 0 );

  virtual ~OSGWidget();
  osg::MatrixTransform* create_box(osg::Vec3 size, osg::Vec3 translation, osg::Vec3 rotation, osg::Vec3 color);
  osg::MatrixTransform* create_ellipsoid(osg::Vec3 size, osg::Vec3 translation, osg::Vec3 rotation, osg::Vec3 color);
  osg::MatrixTransform* create_cone(osg::Vec3 size, osg::Vec3 translation, osg::Vec3 rotation, osg::Vec3 color);
  osg::MatrixTransform* shape_setup(osg::ShapeDrawable* sd, osg::Vec3 translation, osg::Vec3 rotation, osg::Vec3 color);
  bool removeShape(int id);
  osg::Geode* draw_joint (Joint *joint);
  void create_arm (std::list<Joint *> &list);
  void joint_color (int i, Joint* joint);
  void open_arm (std::list<Joint *> &list);
  void erase_joint(int i, std::list<Joint *> &list);
  void change_joint_config(int i, std::list<Joint *> &list);
  void view_floor(bool view);
  void select_joint(int i,bool selected);
  void reset();
  void drawAxis(bool show);
  void update_joint_size(Joint* joint, double h, double rad);
  void create_shape(QString shape, osg::Vec3 size, osg::Vec3 translation, osg::Vec3 rotation, osg::Vec3 color);
  void set_starting_pose(osg::MatrixTransform *transform);
  osg::MatrixTransform* output_matrix(int i, std::list<Joint*> &list);

protected:

  virtual void paintEvent( QPaintEvent* paintEvent );
  virtual void paintGL();
  virtual void resizeGL( int width, int height );

  virtual void keyPressEvent( QKeyEvent* event );
  virtual void keyReleaseEvent( QKeyEvent* event );

  virtual void mouseMoveEvent( QMouseEvent* event );
  virtual void mousePressEvent( QMouseEvent* event );
  virtual void mouseReleaseEvent( QMouseEvent* event );
  virtual void wheelEvent( QWheelEvent* event );
  virtual bool event( QEvent* event );

private:
  virtual void go_home();
  virtual void on_resize( int width, int height );
  int currentID{0};
  int mOffset{0};
  bool mFloor{false};
  osg::MatrixTransform* mStarting_pose = new osg::MatrixTransform;

  osgGA::EventQueue* getEventQueue() const;

  osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> mGraphicsWindow;
  osg::ref_ptr<osgViewer::CompositeViewer> mViewer;
  osg::ref_ptr<osg::Group> mRoot;
  osg::ref_ptr<osgGA::TrackballManipulator> mManipulator;
  std::map<int, osg::MatrixTransform*> mShapeLookup;
  std::map<Joint*, osg::Geode*> mGeodeLookup;
};

#endif
