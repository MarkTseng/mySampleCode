public class JNIFoo {    
    public native String nativeFoo();    

    static {
        System.loadLibrary("foo");
    }        

    public void print () {
    String str = nativeFoo();
    System.out.println(str);
    }
    
    public static void main(String[] args) {
    (new JNIFoo()).print();
    return;
    }
}

