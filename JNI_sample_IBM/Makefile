all:
	javac Sample1.java
	javah -jni Sample1
	gcc -shared -fpic -o libSample1.so -I/usr/lib/jvm/java-6-oracle/include -I/usr/lib/jvm/java-6-oracle/include/linux Sample1.c
	javac Sample2.java
	gcc -I/usr/lib/jvm/java-6-oracle/include -I/usr/lib/jvm/java-6-oracle/include/linux -L/usr/bin/java -L/usr/lib/jvm/java-6-oracle/jre/lib/amd64/server Sample2.c -ljvm -o Sample2
test_sample1:
	java -Djava.library.path=. Sample1
