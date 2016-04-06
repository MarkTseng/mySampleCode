#include <Python.h>


static PyObject* GTOinitial(PyObject* self, PyObject* args)
{
	char *str = "Hello Python Binding";
	char *name;
	PyArg_ParseTuple(args, "z", &name);
	printf("in c name = %s\n",name);
	return Py_BuildValue("sz",str, name);

}

static PyObject* GTOcircle(PyObject* self, PyObject* args)
{
	double x,y;
	PyArg_ParseTuple(args, "dd", &x, &y);
	return Py_BuildValue("d", x * y);
}

static PyMethodDef GTOFunList[] = {
	{"GTOinitial", GTOinitial, METH_VARARGS },
	{"GTOcircle", GTOcircle, METH_VARARGS },
	{NULL,NULL}
};

void initGTO(){
	(void) Py_InitModule("GTO", GTOFunList);
}


