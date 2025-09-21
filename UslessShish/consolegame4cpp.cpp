#pragma once
#include <iostream>
#include <vector>
using namespace std;
class Window{
  vector<vector<char>> window;
  int width;
  int height;
  Window (int w, int h){
    width=w;
    height=h;
    fill('.');
  }
  void fill(char color){
    for (int i=0;i<window.size();i++){
      for (int j=0;i<window[i].size();j++){
        window[i][j]=color;
      }
    }
  }
  void render(){
    for (int i=0;i<window.size();i++){
      for (int j=0;i<window[i].size();j++){
        cout<<window[i][j]<<" ";
      }
      cout<<endl;
    }
  }
}