// overloading class constructors
#include <iostream>
#include <stdio.h>

using namespace std;

class CRectangle {
  protected:
    int width, height;
  public:
    CRectangle ();
    ~CRectangle ();
    CRectangle (int,int);
    int area (void) {return (width*height);}
};

CRectangle::CRectangle ():width(5),height(5) {
  printf("fun %s called, line:%d\n",__func__,__LINE__);
  //width = 5;
  //height = 5;
  printf("width:%p, height:%p\n",&width,&height);
}

CRectangle::~CRectangle () {
  printf("fun %s called, line:%d\n",__func__,__LINE__);
}


CRectangle::CRectangle (int a, int b) {
  printf("fun %s called, line:%d\n",__func__,__LINE__);
  width = a;
  height = b;
}

class CRectangle2 : public CRectangle{
    int width, height;
  public:
    CRectangle2 ();
    ~CRectangle2 ();
    CRectangle2 (int,int);
    int area (void) {return (width*height);}
};

CRectangle2::CRectangle2 () {
  printf("fun %s called, line:%d\n",__func__,__LINE__);
   width = 10;
  height = 10;
  printf("width:%p, height:%p\n",&width,&height);
}

CRectangle2::~CRectangle2 () {
  printf("fun %s called, line:%d\n",__func__,__LINE__);
}

CRectangle2::CRectangle2 (int a, int b) {
  printf("fun %s called, line:%d\n",__func__,__LINE__);
  width = a;
  height = b;
}


int main () {
  //CRectangle rect (3,4);
  CRectangle rectb;
  //CRectangle2 rect2(10,8);
  CRectangle2 rectb2;
  //cout << "rect area: " << rect.area() << endl;
  cout << "rectb area: " << rectb.area() << endl;
  //cout << "rect2 area: " << rect2.area() << endl;
  cout << "rectb2 area: " << rectb2.area() << endl;
  return 0;
}

