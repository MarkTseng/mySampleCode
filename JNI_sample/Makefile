all:
	javac JNIFoo.java
	javah -jni JNIFoo
	gcc -shared -fpic -o libfoo.so -I/usr/lib/jvm/java-6-oracle/include -I/usr/lib/jvm/java-6-oracle/include/linux foo.c

test:
	java -Djava.library.path=. JNIFoo
