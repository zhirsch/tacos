
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __java_awt_geom_Ellipse2D$Float__
#define __java_awt_geom_Ellipse2D$Float__

#pragma interface

#include <java/awt/geom/Ellipse2D.h>
extern "Java"
{
  namespace java
  {
    namespace awt
    {
      namespace geom
      {
          class Ellipse2D$Float;
          class Rectangle2D;
      }
    }
  }
}

class java::awt::geom::Ellipse2D$Float : public ::java::awt::geom::Ellipse2D
{

public:
  Ellipse2D$Float();
  Ellipse2D$Float(jfloat, jfloat, jfloat, jfloat);
  virtual ::java::awt::geom::Rectangle2D * getBounds2D();
  virtual jdouble getHeight();
  virtual jdouble getWidth();
  virtual jdouble getX();
  virtual jdouble getY();
  virtual jboolean isEmpty();
  virtual void setFrame(jfloat, jfloat, jfloat, jfloat);
  virtual void setFrame(jdouble, jdouble, jdouble, jdouble);
  jfloat __attribute__((aligned(__alignof__( ::java::awt::geom::Ellipse2D)))) height;
  jfloat width;
  jfloat x;
  jfloat y;
  static ::java::lang::Class class$;
};

#endif // __java_awt_geom_Ellipse2D$Float__
