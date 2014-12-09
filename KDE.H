/****************************************************************
 KDE.H
 Copyright (C)2014 William H. Majoros (martiandna@gmail.com).
 This is OPEN SOURCE SOFTWARE governed by the Gnu General Public
 License (GPL) version 3, as described at www.opensource.org.
 ****************************************************************/
#ifndef INCL_KDE_H
#define INCL_KDE_H
#include <iostream>
#include "BOOM/Vector.H"
using namespace std;
using namespace BOOM;

struct KdePoint {
  float x;
  int count;
  KdePoint(float x,int c=1) : x(x), count(c) {}
  KdePoint() {}
};

class KDE {
public:
  KDE(const Vector<float> &points,float sd=0,float tolerance=0);
  KDE(const String &filename);
  bool save(const String &filename);
  float rightTail(float);
  float leftTail(float);
  float density(float);
  void summaryStats(float &mean,float &stddev);
  void changeBandwidth(float);
private:
  Vector<KdePoint> points;
  float sd;
  float estimateSD(const Vector<float> &points);
};

#endif

