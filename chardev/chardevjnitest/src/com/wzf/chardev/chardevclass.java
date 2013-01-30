package com.wzf.chardev;

public class chardevclass{
    public native int read();
    public native int write(int value);
    static {
        System.loadLibrary("chardev");
    }
}
