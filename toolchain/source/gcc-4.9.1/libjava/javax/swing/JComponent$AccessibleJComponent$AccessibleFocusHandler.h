
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __javax_swing_JComponent$AccessibleJComponent$AccessibleFocusHandler__
#define __javax_swing_JComponent$AccessibleJComponent$AccessibleFocusHandler__

#pragma interface

#include <java/lang/Object.h>
extern "Java"
{
  namespace java
  {
    namespace awt
    {
      namespace event
      {
          class FocusEvent;
      }
    }
  }
  namespace javax
  {
    namespace swing
    {
        class JComponent$AccessibleJComponent;
        class JComponent$AccessibleJComponent$AccessibleFocusHandler;
    }
  }
}

class javax::swing::JComponent$AccessibleJComponent$AccessibleFocusHandler : public ::java::lang::Object
{

public: // actually protected
  JComponent$AccessibleJComponent$AccessibleFocusHandler(::javax::swing::JComponent$AccessibleJComponent *);
public:
  virtual void focusGained(::java::awt::event::FocusEvent *);
  virtual void focusLost(::java::awt::event::FocusEvent *);
public: // actually package-private
  ::javax::swing::JComponent$AccessibleJComponent * __attribute__((aligned(__alignof__( ::java::lang::Object)))) this$1;
public:
  static ::java::lang::Class class$;
};

#endif // __javax_swing_JComponent$AccessibleJComponent$AccessibleFocusHandler__