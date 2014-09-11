#include <jni.h>
#include <memory.h>

#ifdef _WIN32
#define PATH_SEPARATOR ';'
#else
#define PATH_SEPARATOR ':'
#endif

int main()
{
	JavaVMOption options[1];
	JNIEnv *env;
	JavaVM *jvm;
	JavaVMInitArgs vm_args;
	long status;
	jclass cls;
	jmethodID mid;
	jint square;
	jboolean not;

	options[0].optionString = "-Djava.class.path=.";
	memset(&vm_args, 0, sizeof(vm_args));
	vm_args.version = JNI_VERSION_1_2;
	vm_args.nOptions = 1;
	vm_args.options = options;
	status = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);

	if (status != JNI_ERR)
	{
		cls = env->FindClass("Sample2");
		if(cls !=0)
		{   mid = env->GetStaticMethodID(cls, "intMethod", "(I)I");
			if(mid !=0)
			{	square = env->CallStaticIntMethod(cls, mid, 5);
				printf("Result of intMethod: %d\n", square);
			}

			mid = env->GetStaticMethodID(cls, "booleanMethod", "(Z)Z");
			if(mid !=0)
			{	not = env->CallStaticBooleanMethod(cls, mid, 1);
				printf("Result of booleanMethod: %d\n", not);
			}
		}

		jvm->DestroyJavaVM();
		return 0;
	}
	else
		return -1;
}