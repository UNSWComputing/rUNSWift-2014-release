#include "PointCloud.hpp"
#include <math.h>

using namespace qglviewer;
using namespace std;

static void drawPoints(std::vector<std::pair<qglviewer::Vec, qglviewer::Vec> > &points)
{
   glBegin(GL_POINTS);
   for (unsigned int i = 0; i < points.size(); ++i) {
      glColor3fv(points[i].first);
      glVertex3fv(points[i].second);
   }
   glEnd();
}

void PointCloud::init()
{
  // Swap the CAMERA and FRAME state keys (NoButton and Control)
  // Save CAMERA binding first. See setHandlerKeyboardModifiers() documentation.
#if QT_VERSION < 0x040000
  setHandlerKeyboardModifiers(QGLViewer::CAMERA, Qt::AltButton);
  setHandlerKeyboardModifiers(QGLViewer::FRAME,  Qt::NoButton);
  setHandlerKeyboardModifiers(QGLViewer::CAMERA, Qt::ControlButton);
#else
  setHandlerKeyboardModifiers(QGLViewer::CAMERA, Qt::AltModifier);
  setHandlerKeyboardModifiers(QGLViewer::FRAME,  Qt::NoModifier);
  setHandlerKeyboardModifiers(QGLViewer::CAMERA, Qt::ControlModifier);
#endif

#ifdef GL_RESCALE_NORMAL  // OpenGL 1.2 Only...
  glEnable(GL_RESCALE_NORMAL);
#endif

  // Add a manipulated frame to the viewer.
  // If you are not "using namespace qglqglviewer", you need
  // to specify: new qglviewer::ManipulatedFrame().
  setManipulatedFrame(new ManipulatedFrame());

   // Light setup
  glDisable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

   // Light default parameters
  const GLfloat light_ambient[4]  = {1.0, 1.0, 1.0, 1.0};
  //const GLfloat light_specular[4] = {1.0, 1.0, 1.0, 1.0};
  //const GLfloat light_diffuse[4]  = {1.0, 1.0, 1.0, 1.0};
  const GLfloat light_specular[4] = {0.0, 0.0, 0.0, 0.0};
  const GLfloat light_diffuse[4]  = {0.0, 0.0, 0.0, 0.0};

  glLightf( GL_LIGHT1, GL_SPOT_EXPONENT, 3.0);
  glLightf( GL_LIGHT1, GL_SPOT_CUTOFF,   100.0);
  glLightf( GL_LIGHT1, GL_CONSTANT_ATTENUATION,  0.1f);
  glLightf( GL_LIGHT1, GL_LINEAR_ATTENUATION,    0.3f);
  glLightf( GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.3f);
  glLightfv(GL_LIGHT1, GL_AMBIENT,  light_ambient);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT1, GL_DIFFUSE,  light_diffuse);

  //help();
  //restoreStateFromFile();

  // Make world axis visible
  //setAxisIsDrawn();

  // Change the near clipping plane
  camera()->setZClippingCoefficient(5.0);
  //camera()->setType(Camera::ORTHOGRAPHIC);
  setSceneBoundingBox(Vec(0.0, 0.0, 0.0), Vec(1.0, 1.0, 1.0));
  camera()->showEntireScene();
}

void PointCloud::draw()
{
  // Here we are in the world coordinate system.
  // Draw your scene here.

  // Save the current model view matrix (not needed here in fact)
  glPushMatrix();

  // Multiply matrix to get in the frame coordinate system.
  glMultMatrixd(manipulatedFrame()->matrix());

  // Scale down the drawings
  //glScalef(0.3f, 0.3f, 0.3f);

   // Place light at camera position
  const Vec cameraPos = camera()->position();
  const GLfloat pos[4] = {cameraPos[0], cameraPos[1], cameraPos[2], 0.0};
  glLightfv(GL_LIGHT1, GL_POSITION, pos);

   // Orientate light along view direction
  glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, camera()->viewDirection());

  // Draw an axis using the QGLViewer static function
  drawAxis();

  // Draws a frame-related spiral.
  //drawSpiral();
  drawPoints(points);

  // Restore the original (world) coordinate system
  glPopMatrix();
}

QString PointCloud::helpString() const
{
  QString text("<h2>M a n i p u l a t e d F r a m e</h2>");
  text += "A <i>ManipulatedFrame</i> converts mouse gestures into <i>Frame</i> displacements. ";
  text += "In this example, such an object defines the position of the spiral that can hence be manipulated.<br><br>";
  text += "Adding two lines of code will then allow you to move the objects of ";
  text += "your scene using the mouse. The button bindings of the <i>ManipulatedFrame</i> ";
  text += "are the same than for the camera. Spinning is possible.<br><br>";
  text += "Default key bindings have been changed in this example : press <b>Control</b> ";
  text += "while moving the mouse to move the camera instead of the ManipulatedFrame.";
  return text;
}
