#include "OSGWidget.h"

#include <osg/Camera>
#include <osg/DisplaySettings>
#include <osg/Geode>
#include <osg/Material>
#include <osg/Shape>
#include <osg/StateSet>
#include <osgDB/WriteFile>
#include <osgGA/EventQueue>
#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>
#include <osg/MatrixTransform>
#include <osgUtil/SmoothingVisitor>

#include <cassert>
#include <vector>
#include <list>
#include <string>

#include <QKeyEvent>
#include <QPainter>
#include <QWheelEvent>

OSGWidget::OSGWidget(QWidget* parent, Qt::WindowFlags f ):
    QOpenGLWidget( parent,f ),
    mGraphicsWindow{ new osgViewer::GraphicsWindowEmbedded( this->x(),
                                                            this->y(),
                                                            this->width(),
                                                            this->height() ) }
  , mViewer{ new osgViewer::CompositeViewer }
{
    mRoot = new osg::Group;

    float aspectRatio = static_cast<float>( this->width() ) / static_cast<float>( this->height() );
    auto pixelRatio   = this->devicePixelRatio();

    //Set up the camera
    osg::Camera* camera = new osg::Camera;
    camera->setViewport( 0, 0, this->width() * pixelRatio, this->height() * pixelRatio );

    camera->setClearColor( osg::Vec4( 1.f, 1.f, 1.f, 1.f ) );
    camera->setProjectionMatrixAsPerspective( 30.f, aspectRatio, 1.f, 1000.f );
    camera->setGraphicsContext( mGraphicsWindow );

    //Set up the view
    osgViewer::View* view = new osgViewer::View;
    view->setCamera( camera );
    view->setSceneData( mRoot.get() );
    view->addEventHandler( new osgViewer::StatsHandler );

    //Set up the mouse control
    mManipulator = new osgGA::TrackballManipulator;
    mManipulator->setAllowThrow( false );

    view->setCameraManipulator( mManipulator );
    mManipulator->setHomePosition(osg::Vec3d(0,100,0),osg::Vec3d(0,0,0),osg::Vec3d(0,0,100));

    mViewer->addView( view );
    mViewer->setThreadingModel( osgViewer::CompositeViewer::SingleThreaded );
    mViewer->realize();

    // Set the focus policy so that this widget will recieve keyboard events.
    this->setFocusPolicy( Qt::StrongFocus );
    this->setMinimumSize( 100, 100 );

    // This allows this widget to get mouse move events.
    // This is needed with multiple view ports.
    this->setMouseTracking( true );

    //Reset the camera
    go_home();

    drawAxis(true);
}

OSGWidget::~OSGWidget()
{}

void OSGWidget::paintEvent( QPaintEvent* /* paintEvent */ )
{
    this->makeCurrent();

    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );

    this->paintGL();

    painter.end();

    this->doneCurrent();
}

void OSGWidget::paintGL()
{
    mViewer->frame();
}

void OSGWidget::resizeGL( int width, int height )
{
    this->getEventQueue()->windowResize( this->x(), this->y(), width, height );
    mGraphicsWindow->resized( this->x(), this->y(), width, height );

    this->on_resize( width, height );

}

void OSGWidget::keyPressEvent( QKeyEvent* event )
{
    QString keyString   = event->text();
    const char* keyData = keyString.toLocal8Bit().data();

    if( event->key() == Qt::Key_H )
    {
        this->go_home();
        return;
    }

    this->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KeySymbol( *keyData ) );
}

void OSGWidget::keyReleaseEvent( QKeyEvent* event )
{
    QString keyString   = event->text();
    const char* keyData = keyString.toLocal8Bit().data();

    this->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KeySymbol( *keyData ) );
}

void OSGWidget::mouseMoveEvent( QMouseEvent* event )
{
    auto pixelRatio = this->devicePixelRatio();

    this->getEventQueue()->mouseMotion( static_cast<float>( event->x() * pixelRatio ),
                                        static_cast<float>( event->y() * pixelRatio ) );
}

void OSGWidget::mousePressEvent( QMouseEvent* event )
{
    // 1 = left mouse button
    // 2 = middle mouse button
    // 3 = right mouse button

    unsigned int button = 0;

    switch( event->button() )
    {
    case Qt::LeftButton:
        button = 1;
        break;

    case Qt::MiddleButton:
        button = 2;
        break;

    case Qt::RightButton:
        button = 3;
        break;

    default:
        break;
    }

    auto pixelRatio = this->devicePixelRatio();

    this->getEventQueue()->mouseButtonPress( static_cast<float>( event->x() * pixelRatio ),
                                             static_cast<float>( event->y() * pixelRatio ),
                                             button );

}

void OSGWidget::mouseReleaseEvent(QMouseEvent* event)
{
    // 1 = left mouse button
    // 2 = middle mouse button
    // 3 = right mouse button

    unsigned int button = 0;

    switch( event->button() )
    {
    case Qt::LeftButton:
        button = 1;
        break;

    case Qt::MiddleButton:
        button = 2;
        break;

    case Qt::RightButton:
        button = 3;
        break;

    default:
        break;
    }

    auto pixelRatio = this->devicePixelRatio();

    this->getEventQueue()->mouseButtonRelease( static_cast<float>( pixelRatio * event->x() ),
                                               static_cast<float>( pixelRatio * event->y() ),
                                               button );
}

void OSGWidget::wheelEvent( QWheelEvent* event )
{
    event->accept();
    int delta = event->delta();

    osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ?   osgGA::GUIEventAdapter::SCROLL_UP
                                                                 : osgGA::GUIEventAdapter::SCROLL_DOWN;

    this->getEventQueue()->mouseScroll( motion );
}

bool OSGWidget::event( QEvent* event )
{
    bool handled = QOpenGLWidget::event( event );

    // This ensures that the OSG widget is always going to be
    // repainted after the user performed some interaction.
    // Doing this in the event handler ensures
    // that we don't forget about some event and prevents duplicate code.
    switch( event->type() )
    {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::Wheel:
        this->update();
        break;

    default:
        break;
    }
    return handled;
}

void OSGWidget::view_floor(bool view)
{
    mFloor = view;
    if (view)
    {
        mRoot->insertChild(0, create_box(osg::Vec3(100,100,.5),osg::Vec3(0,0,-.25),osg::Vec3(0,0,0),osg::Vec3(0,100,255)));
        mOffset++;
    }
    else
    {
        mRoot->removeChild(0,1);
        mOffset--;
    }
}

void OSGWidget::select_joint(int i, bool selected)
{
    osg::Geode* node;
    node = mRoot->getChild(i+mOffset)->asTransform()->getChild(0)->asGeode();
    double val;
    if (selected)
        val = 1;
    else
        val = .5;

    for (int i = 0; i < node->getNumChildren()-2; i++)
    {
        dynamic_cast<osg::ShapeDrawable*>(node->getChild(i+2)->asTransform()->getChild(0))->setColor(osg::Vec4( val, val, val, 1.f));
    }
}

void OSGWidget::reset()
{
    mOffset = 0;
    mRoot->removeChild(0,mRoot->getNumChildren());
    this->drawAxis(true);
}

bool OSGWidget::removeShape(int id)
{
    mRoot->removeChild(id,1);
    mOffset--;
}

osg::Geode* OSGWidget::draw_joint(Joint* joint)
{
    double r, g, b;
    double h, rad;
    joint->get_size(h,rad);
    joint->get_color(r,g,b);
    r = r/255.0;
    g = g/255.0;
    b = b/255.0;
    // What Value should we make the height of these cylinders? Maybe rad/4.0?
    osg::Cylinder* cylinder = new osg::Cylinder(osg::Vec3(0.f, 0.f, 0.f), rad, 1);
    osg::ShapeDrawable* sd = new osg::ShapeDrawable(cylinder);
    sd->setColor(osg::Vec4(r, g, b, 1.f));

    osg::ShapeDrawable* sd1 = new osg::ShapeDrawable(cylinder);
    sd1->setColor(osg::Vec4(r, g, b, 1.f));
    sd->setName("Base");
    sd1->setName("End");

    // transform the end effector to the correct frame
    osg::MatrixTransform* transform = joint->get_T();
    transform->addChild(sd1);

    //Create the node to hold the joint
    osg::Geode* joint_geode = new osg::Geode;
    joint_geode->addChild(sd);
    joint_geode->addChild(transform);

    for(int i = 0; i < joint->get_sphere_count(); i++)
    {
        osg::Sphere* sphere = new osg::Sphere( osg::Vec3( 0.f, 0.f, 0.f ), .9*rad );
        osg::ShapeDrawable* sdi = new osg::ShapeDrawable( sphere );
        sdi->setColor( osg::Vec4( .5, .5, .5, 1.f ) );
        sdi->setName(QString("Sphere %1").arg(i).toStdString());
        osg::MatrixTransform* Ti = joint->get_Ti(i);
        Ti->addChild(sdi);

        joint_geode->addChild(Ti);
    }

    // Set material for basic lighting and enable depth tests.
    osg::StateSet* stateSet = joint_geode->getOrCreateStateSet();
    osg::Material* material = new osg::Material;
    material->setColorMode( osg::Material::AMBIENT_AND_DIFFUSE );
    stateSet->setAttributeAndModes( material, osg::StateAttribute::ON );
    stateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );
    mGeodeLookup[joint] = joint_geode;
    return joint_geode;
}

void OSGWidget::update_joint_size(Joint* joint, double h, double rad)
{
    osg::Geode* joint_geode = mGeodeLookup[joint];
    for(int i = 0; i < joint->get_sphere_count(); i++)
    {
        joint_geode->removeChild(joint->get_Ti(i));
    }
    joint->set_size(h,rad);
    for(int i = 0; i < joint->get_sphere_count(); i++)
    {
        osg::Sphere* sphere = new osg::Sphere( osg::Vec3( 0.f, 0.f, 0.f ), .9*rad );
        osg::ShapeDrawable* sdi = new osg::ShapeDrawable( sphere );
        sdi->setColor( osg::Vec4( 1.f, 1.f, 1.f, 1.f ) );
        sdi->setName(QString("Sphere %1").arg(i).toStdString());
        osg::MatrixTransform* Ti = joint->get_Ti(i);
        Ti->addChild(sdi);
        joint_geode->addChild(Ti);
    }
    update();
}

void OSGWidget::create_shape(QString shape, osg::Vec3 size, osg::Vec3 translation, osg::Vec3 rotation, osg::Vec3 color)
{
    //declare variables
    osg::ShapeDrawable* sd;
    int j{0};

    //establishes where to insert the shape in the root
    if (mFloor)
        j++;

    if (shape == "box")
    {
        osg::Box* box = new osg::Box( osg::Vec3( 0.f, 0.f, size.z()/2), size.x(),size.y(),size.z() );
        sd = new osg::ShapeDrawable( box );
    }
    else if (shape == "cone")
    {
        osg::Cone* cone = new osg::Cone( osg::Vec3( 0.f, 0.f, size.x()/4 ), size.y(), size.x());
        sd = new osg::ShapeDrawable( cone );
    }
    else if (shape == "cylinder")
    {
        osg::Cylinder* cylinder = new osg::Cylinder( osg::Vec3(0.f,0.f,size.x()/2), size.y(), size.x());
        sd = new osg::ShapeDrawable(cylinder);
    }
    else if (shape == "sphere")
    {
        osg::Sphere* sphere = new osg::Sphere( osg::Vec3( 0.f, 0.f, size.x() ), size.x());
        sd = new osg::ShapeDrawable( sphere );
    }

    //Finishes defining the shape, then inserts it into the graphics view
    mRoot->insertChild(j, shape_setup(sd, translation, rotation, color));

    mOffset++;
}

void OSGWidget::set_starting_pose(osg::MatrixTransform* transform)
{
    mStarting_pose->setMatrix(transform->getMatrix());
    if (mRoot->getNumChildren()>mOffset)
        mRoot->getChild(mOffset)->asTransform()->asMatrixTransform()->setMatrix(mStarting_pose->getMatrix());
}

osg::MatrixTransform *OSGWidget::output_matrix(int i, std::list<Joint *> &list)
{
    osg::MatrixTransform* m = new osg::MatrixTransform;

    //Make it point to the correct object in the list
    std::list<Joint*>::iterator it= list.begin();
    std::advance(it,i);

    if (list.size() == 1)
    {
        m->setMatrix(((*it)->get_T())->getMatrix()*mStarting_pose->getMatrix());
    }
    else
    {
        m->setMatrix(((*it)->get_T())->getMatrix()* mRoot->getChild(i+mOffset)->asTransform()->asMatrixTransform()->getMatrix());
    }
    return m;
}

void OSGWidget::create_arm(std::list<Joint *> &list)
{
    osg::MatrixTransform* prev_m = new osg::MatrixTransform;

    //Make it point to the last object in the list
    std::list<Joint*>::iterator it=list.end();
    it--;

    prev_m->addChild(draw_joint(*it));

    if (list.size() == 1)
    {
        //SET ROBOT STARTING POSITION HERE
        prev_m->setMatrix(mStarting_pose->getMatrix());
    }
    else
    {
        it--;
        //ORDER OF MULTIPLICATION: First, offset. Second, Previous Joint Matrix. Third, Previous Translation matrix.
        prev_m->setMatrix( osg::Matrix::translate(0,0,1)*((*it)->get_T())->getMatrix()* mRoot->getChild(list.size()+(mOffset-2))->asTransform()->asMatrixTransform()->getMatrix() );
    }
    //Multiply the joint by all the transformations
    mRoot->addChild(prev_m);
}

void OSGWidget::open_arm(std::list<Joint *> &list)
{
    osg::MatrixTransform* prev_m = new osg::MatrixTransform;

    // SET ROBOT STARTING POINT HERE
    prev_m->setMatrix(osg::Matrix::translate(0,0,.5));

    std::list<Joint*>::iterator it=list.begin();
    for(it; it!=list.end();it++)
    {
        osg::MatrixTransform* m = new osg::MatrixTransform;
        if (it == list.begin())
        {
            prev_m->setMatrix(mStarting_pose->getMatrix());
        }
        else
        {
            it--;
            prev_m->setMatrix( (osg::Matrix::translate(0, 0, 1)*((*it)->get_T())->getMatrix())*prev_m->getMatrix() );
            it++;
        }
        if (it!= list.end())
        {
            m->addChild(draw_joint(*it));
            m->setMatrix(prev_m->getMatrix());
            mRoot->addChild(m);
        }
    }
}

void OSGWidget::erase_joint(int i, std::list<Joint *> &list)
{
    std::list<Joint*>::iterator it= list.begin();
    std::advance(it,i);

    //This will adjust the matrix transforms for all affected joints when delete joint is called
    for (int k = (mRoot->getNumChildren()); k > i+mOffset+1; k--)
    {
        mRoot->getChild(k-1)->asTransform()->asMatrixTransform()->setMatrix( mRoot->getChild(k-2)->asTransform()->asMatrixTransform()->getMatrix() );
    }

    mRoot->removeChild(i+mOffset,1);
}

void OSGWidget::change_joint_config(int i, std::list<Joint *> &list)
{
    std::list<Joint*>::iterator it= list.begin();
    std::advance(it,i);

    // The new matrix for our joint configuration
    osg::MatrixTransform* matrix = new osg::MatrixTransform;
    matrix = (*it)->get_T();
    osg::MatrixTransform* prev_m = new osg::MatrixTransform;

    //set j to the last index in the mRoot list
    int j = mRoot->getNumChildren();
    j--;

    int k{0};

    it=list.begin();
    for(it; it!=list.end();it++)
    {
        if (it == list.begin())
        {
            prev_m->setMatrix(mStarting_pose->getMatrix());
        }
        else
        {
            it--;
            prev_m->setMatrix( osg::Matrix::translate(0, 0, 1)*((*it)->get_T())->getMatrix()*(prev_m->getMatrix()) ); //
            it++;
        }
        if (k>i) //Checks to see if the joint is affected by the changed T matrix
        {
            osg::MatrixTransform* mat = new osg::MatrixTransform;
            mat->setMatrix(prev_m->getMatrix());
            i++;
            if (i+mOffset<=j) //Makes sure it doesnt step past the index of mRoot
            {
                mRoot->getChild(i+mOffset)->asTransform()->asMatrixTransform()->setMatrix( mat->getMatrix() );
            }
        }
        k++;
    }
}

void OSGWidget::joint_color(int i, Joint *joint)
{
    osg::Geode* node = new osg::Geode;
    double r, g, b;
    joint->get_color(r,g,b);
    r = r/255.0;
    g = g/255.0;
    b = b/255.0;
    node = mRoot->getChild(i+mOffset)->asTransform()->getChild(0)->asGeode();
    // Start and End of Joint
    dynamic_cast<osg::ShapeDrawable*>(node->getChild(0))->setColor(osg::Vec4(r, g, b, 1.f));
    dynamic_cast<osg::ShapeDrawable*>(node->getChild(1)->asTransform()->getChild(0))->setColor(osg::Vec4(r, g, b, 1.f));
}

void OSGWidget::drawAxis(bool show)
{
    if (show)
    {
        osg::Geode* geode = new osg::Geode;
        //x axis, red
        geode->addChild(create_box(osg::Vec3(25,.5,.5), osg::Vec3(12.5,0,0), osg::Vec3(0,0,0), osg::Vec3(255,0,0)));
        //y axis, green
        geode->addChild(create_box(osg::Vec3(.5,25,.5), osg::Vec3(0,12.5,0), osg::Vec3(0,0,0), osg::Vec3(0,255,0)));
        //z axis, blue
        geode->addChild(create_box(osg::Vec3(.5,.5,25), osg::Vec3(0,0,12.5), osg::Vec3(0,0,0), osg::Vec3(0,0,255)));

        mRoot->insertChild(mOffset,geode);
        mOffset++;
    }
    else
    {
        mRoot->removeChild(mOffset-1,1);
        mOffset--;
    }
}

osg::MatrixTransform* OSGWidget::create_box(osg::Vec3 size, osg::Vec3 translation, osg::Vec3 rotation, osg::Vec3 color)
{
    double R, G, B;
    R = color.x()/255.0;
    G = color.y()/255.0;
    B = color.z()/255.0;
    currentID++;

    osg::Box* box = new osg::Box( osg::Vec3( 0.f, 0.f, 0.f ), size.x(),size.y(),size.z() );
    osg::ShapeDrawable* sd = new osg::ShapeDrawable( box );
    sd->setColor( osg::Vec4( R, G, B, 1.f ) );
    sd->setName( (QString("%1").arg(currentID)).toStdString() );
    osg::Geode* geode = new osg::Geode;
    geode->addDrawable( sd );
    osg::StateSet* stateSet = geode->getOrCreateStateSet();
    osg::Material* material = new osg::Material;

    material->setColorMode( osg::Material::AMBIENT_AND_DIFFUSE );

    stateSet->setAttributeAndModes( material, osg::StateAttribute::ON );
    stateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    //Set up transform parent node.
    osg::MatrixTransform* transform= new osg::MatrixTransform;
    osg::Matrix mt = osg::Matrix::translate(translation.x(), translation.y(), translation.z());
    osg::Matrix mrx = osg::Matrix::rotate(osg::DegreesToRadians((float)rotation.x()),1,0,0);
    osg::Matrix mry = osg::Matrix::rotate(osg::DegreesToRadians((float)rotation.y()),0,1,0);
    osg::Matrix mrz = osg::Matrix::rotate(osg::DegreesToRadians((float)rotation.z()),0,0,1);
    osg::Matrix m = mrx*mry*mrz*mt;

    transform->setMatrix(m);

    //Add shape to parent
    transform->addChild(geode);

    //Add transform to root
    mShapeLookup[currentID] = transform;
    return transform;
}

osg::MatrixTransform *OSGWidget::shape_setup(osg::ShapeDrawable *sd, osg::Vec3 translation, osg::Vec3 rotation, osg::Vec3 color)
{
    double R, G, B;
    R = color.x()/255.0;
    G = color.y()/255.0;
    B = color.z()/255.0;
    currentID++;

    sd->setColor( osg::Vec4( R, G, B, 1.f ) );
    sd->setName( (QString("%1").arg(currentID)).toStdString() );
    osg::Geode* geode = new osg::Geode;
    geode->addDrawable( sd );
    osg::StateSet* stateSet = geode->getOrCreateStateSet();
    osg::Material* material = new osg::Material;

    material->setColorMode( osg::Material::AMBIENT_AND_DIFFUSE );

    stateSet->setAttributeAndModes( material, osg::StateAttribute::ON );
    stateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    //Set up transform parent node.
    osg::MatrixTransform* transform= new osg::MatrixTransform;
    //Establish the translation and rotation for the shape
    osg::Matrix mt = osg::Matrix::translate(translation.x(), translation.y(), translation.z());
    osg::Matrix mrx = osg::Matrix::rotate(osg::DegreesToRadians((float)rotation.x()),1,0,0);
    osg::Matrix mry = osg::Matrix::rotate(osg::DegreesToRadians((float)rotation.y()),0,1,0);
    osg::Matrix mrz = osg::Matrix::rotate(osg::DegreesToRadians((float)rotation.z()),0,0,1);
    osg::Matrix m = mrx*mry*mrz*mt;

    transform->setMatrix(m);

    //Add shape to parent
    transform->addChild(geode);

    mShapeLookup[currentID] = transform;
    return transform;
}

void OSGWidget::go_home()
{
    osgViewer::ViewerBase::Views views;
    mViewer->getViews( views );

    for(osgViewer::View* view:views)
    {
        view->home();
    }
}

void OSGWidget::on_resize( int width, int height )
{
    std::vector<osg::Camera*> cameras;
    mViewer->getCameras( cameras );

    auto pixelRatio = this->devicePixelRatio();

    cameras[0]->setViewport( 0, 0, width * pixelRatio, height * pixelRatio );
}

osgGA::EventQueue* OSGWidget::getEventQueue() const
{
    osgGA::EventQueue* eventQueue = mGraphicsWindow->getEventQueue();

    if( eventQueue )
        return eventQueue;
    else
        throw std::runtime_error( "Unable to obtain valid event queue");
}


