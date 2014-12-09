/****************************************************************
 KDE.C
 Copyright (C)2014 William H. Majoros (martiandna@gmail.com).
 This is OPEN SOURCE SOFTWARE governed by the Gnu General Public
 License (GPL) version 3, as described at www.opensource.org.
 ****************************************************************/
#include <iostream>
#include <fstream>
#include <math.h>
#include "KDE.H"
#include "BOOM/SummaryStats.H"
#include "BOOM/VectorSorter.H"
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
using namespace std;
using namespace BOOM;

KDE::KDE(const Vector<float> &points,float sd,float tolerance)
{
  if(sd<=0) sd=estimateSD(points);
  this->sd=sd;

  // Deduplicate and tabulate counts
  Vector<float> rounded;
  for(Vector<float>::const_iterator cur=points.begin(), end=points.end() ;
      cur!=end ; ++cur) {
    float r=tolerance==0 ? *cur : int(*cur/tolerance+5/9.0)*tolerance;
    rounded.push_back(r);
  }
  DirectComparator<float> cmp;
  VectorSorter<float> sorter(rounded,cmp);
  sorter.sortAscendInPlace();
  int n=rounded.size();
  for(int i=0 ; i<n ; ) {
    float x=rounded[i];
    int j=i+1;
    for(; j<n ; ++j) if(rounded[j]!=x) break;
    this->points.push_back(KdePoint(x,j-i));
    i=j;
  }
}



KDE::KDE(const String &filename)
{
  ifstream is(filename.c_str());
  is>>sd;
  int n, count;
  is>>n;
  float x;
  for(int i=0 ; i<n ; ++i) {
    is>>x>>count;
    points.push_back(KdePoint(x,count));
  }
}



bool KDE::save(const String &filename)
{
  ofstream os(filename.c_str());
  os<<sd<<endl;
  os<<points.size()<<endl;
  for(Vector<KdePoint>::iterator cur=points.begin(), end=points.end() ; 
      cur!=end ; ++cur)
    os<<(*cur).x<<"\t"<<(*cur).count<<endl;
}


float KDE::estimateSD(const Vector<float> &points)
{
  SummaryStats stats(points);
  const float sampleSD=stats.getStdDev();
  const int n=stats.getN();
  float sd=1.06*sampleSD*pow(n,-0.2); // Silverman's Rule of Thumb
  
  //###
  //sd/=100;
  //###

  return sd;
}



float KDE::rightTail(float x)
{
  float sum=0;
  int N=0;
  for(Vector<KdePoint>::iterator cur=points.begin(), end=points.end() ;
      cur!=end ; ++cur) {
    const KdePoint &pt=*cur;
    sum+=pt.count*gsl_cdf_gaussian_Q(x-pt.x,sd);
    //sum/=sd; // ###
    //sum+=pt.count*gsl_cdf_gaussian_Q((x-pt.x)/sd,sd); sum/=sd;
    N+=pt.count;
  }
  return sum/N;
}



float KDE::leftTail(float x)
{
  float sum=0;
  int N=0;
  for(Vector<KdePoint>::iterator cur=points.begin(), end=points.end() ;
      cur!=end ; ++cur) {
    const KdePoint &pt=*cur;
    sum+=pt.count*gsl_cdf_gaussian_P(x-pt.x,sd);
    //sum/=sd; // ###
    //sum+=pt.count*gsl_cdf_gaussian_P((x-pt.x)/sd,sd); sum/=sd;
    N+=pt.count;
  }
  return sum/N;
}



float KDE::density(float x)
{
  float sum=0;
  int N=0;
  for(Vector<KdePoint>::iterator cur=points.begin(), end=points.end() ;
      cur!=end ; ++cur) {
    const KdePoint &pt=*cur;
    sum+=pt.count*gsl_ran_gaussian_pdf(x-pt.x,sd);
    //sum/=sd; // ###
    //sum+=pt.count*gsl_cdf_gaussian_P((x-pt.x)/sd,sd); sum/=sd;
    N+=pt.count;
  }
  return sum/N;
}



void KDE::summaryStats(float &mean,float &stddev)
{
  Vector<float> values;
  for(Vector<KdePoint>::iterator cur=points.begin(), end=points.end() ;
      cur!=end ; ++cur) {
    const KdePoint &pt=*cur;
    for(int i=0 ; i<pt.count ; ++i)
      values.push_back(pt.x);
  }
  SummaryStats stats(values);
  mean=stats.getMean();
  stddev=stats.getStdDev();
}



void KDE::changeBandwidth(float f)
{
  sd=f;
}


