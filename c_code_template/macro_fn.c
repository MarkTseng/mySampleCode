#include <stdio.h>
#include <stdlib.h>


#define declar_func(_name) \
int _name##_func (int i) \
{                      \
	printf("%s, %d\n", __func__ , i);  \
	return 0; \
} \
 
declar_func(myTest1)
declar_func(myTest2)

int main(int argc, char *argv[] )
{
	myTest1_func(1);
	myTest2_func(10);

	return 0;
}
