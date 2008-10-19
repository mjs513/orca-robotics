#ifndef TEST_ICE
#define TEST_ICE

module test
{

enum MyEnum
{
    Low,
    Medium,
    High
};

struct MyStruct0
{
    bool mBool;
    byte mByte;
    short mShort;
    int mInt;
    long mLong;
    float mFloat;
    double mDouble;
    string mString;
};

sequence<MyStruct0> MyStruct0Seq;
dictionary<string,MyStruct0> MyStruct0Dict;

class MyClass0
{
    int mInt;
};

class MyClass1 extends MyClass0
{
    double mDouble;
};

struct MyStruct1
{   
    MyStruct0 mStruct;
    MyClass0 mClass;
};

interface MyInterface0
{
    MyStruct0 getStruct();
//         throws DataNotExistException, HardwareFailedException;
    MyClass0 getClass();
};

interface MyInterface1 extends MyInterface0
{
    void set( MyInterface0* iface );
};

}; // module

#endif