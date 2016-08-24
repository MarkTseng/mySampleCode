#include <iostream>
#include <stdio.h>

using namespace std;

class BaseClass {
  int i;
public:
  void setInt(int n);
  int getInt();
  BaseClass();
};

class DerivedClass : public BaseClass {
  int j;
  int i;
public:
  void setJ(int n);
  //void setInt(int n);
  //int getInt();
  int mul();
};

void BaseClass::setInt(int n)
{
  printf("%s called, line:%d \n",__func__,__LINE__);
  i = n;
}

BaseClass::BaseClass(void)
{
  printf("%s called, line:%d \n",__func__,__LINE__);
}


int BaseClass::getInt()
{
  printf("%s called, line:%d \n",__func__,__LINE__);
  return i;
}

void DerivedClass::setJ(int n)
{
  printf("%s called, line:%d \n",__func__,__LINE__);
  j = n;
}

#if 0
void DerivedClass::setInt(int n)
{
  printf("%s called, line:%d \n",__func__,__LINE__);
  i = n;
}

int DerivedClass::getInt()
{
  printf("%s called, line:%d \n",__func__,__LINE__);
  return i;
}
#endif

int DerivedClass::mul()
{
  printf("%s called, line:%d \n",__func__,__LINE__);
  return j * getInt();
}

int main()
{
  DerivedClass ob;
  BaseClass bc;

  ob.setInt(10);        // load i in BaseClass
  ob.setJ(4);          // load j in DerivedClass

  cout << ob.mul() << '\n';     // displays 40

  return 0;
}

