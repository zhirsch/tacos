
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __gnu_classpath_jdwp_VMIdManager$ReferenceKey__
#define __gnu_classpath_jdwp_VMIdManager$ReferenceKey__

#pragma interface

#include <java/lang/ref/SoftReference.h>
extern "Java"
{
  namespace gnu
  {
    namespace classpath
    {
      namespace jdwp
      {
          class VMIdManager;
          class VMIdManager$ReferenceKey;
      }
    }
  }
}

class gnu::classpath::jdwp::VMIdManager$ReferenceKey : public ::java::lang::ref::SoftReference
{

public:
  VMIdManager$ReferenceKey(::gnu::classpath::jdwp::VMIdManager *, ::java::lang::Object *);
  VMIdManager$ReferenceKey(::gnu::classpath::jdwp::VMIdManager *, ::java::lang::Object *, ::java::lang::ref::ReferenceQueue *);
  virtual jint hashCode();
  virtual jboolean equals(::java::lang::Object *);
private:
  jint __attribute__((aligned(__alignof__( ::java::lang::ref::SoftReference)))) _hash;
public: // actually package-private
  ::gnu::classpath::jdwp::VMIdManager * this$0;
public:
  static ::java::lang::Class class$;
};

#endif // __gnu_classpath_jdwp_VMIdManager$ReferenceKey__
