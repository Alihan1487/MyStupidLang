#pragma once
#include <iostream>
#include <vector>
#include <functional>
using namespace std;
typedef struct Basic{
  void* data;
  virtual void print(ostream& os) =0 ;
} BasicObject;
class Dynamic{
  public:
    Basic* data;
    Dynamic(){data=nullptr;}
    Dynamic(Basic* val){
      data=val;
    }
    void operator=(Basic* val){
      data=val;
    }
    friend ostream& operator<<(ostream& os,const Dynamic& obj){
      if (obj.data){
        obj.data->print(os);
      }
      else{
        os<<"NULL";
      }
      return os;
    }
};
typedef struct Function:Basic{
function<Dynamic(vector<Dynamic>)> func;
  Dynamic call(vector<Dynamic>& args){
    return func(args);
  }
  void print(ostream& os){
    os<<"Thats function bruuuuh";
    }
function<Dynamic(vector<Dynamic>)> get(){return func;}
} FunctionObject;
typedef struct Int:Basic{
  int data;
  Int(int val){
    data=val;
  }
  void print(ostream& os){os<<data;};
  int get(){
    return data;
  }
} IntObject;
typedef struct String:Basic{
  string data;
  String(string val){
    data=val;
  }
  void print(ostream& os){os<<data;};
  string get(){
    return data;
  }
} StringObject;
typedef struct Null:Basic{
  void print(ostream& os){os<<"NULL";}
} NullObject;
typedef struct Bool:Basic{
  bool data;
  Bool(bool val){data=val;}
  void print(ostream& os){os<<data;}
  bool get(){return data;}
} BoolObject;
