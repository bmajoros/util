/****************************************************************
 gff-intersect.C
 Copyright (C)2014 William H. Majoros (martiandna@gmail.com).
 This is OPEN SOURCE SOFTWARE governed by the Gnu General Public
 License (GPL) version 3, as described at www.opensource.org.
 ****************************************************************/
#include <iostream>
#include "BOOM/String.H"
#include "BOOM/CommandLine.H"
#include "BOOM/GffReader.H"
#include "BOOM/Exceptions.H"
using namespace std;
using namespace BOOM;


class Application {
public:
  Application();
  int main(int argc,char *argv[]);
private:
  float intersect(Vector<GffFeature*> &,Vector<GffFeature*> &);
};


int main(int argc,char *argv[])
  {
    try
      {
	Application app;
	return app.main(argc,argv);
      }
    catch(const char *p)
      {
	cerr << p << endl;
      }
    catch(const string &msg)
      {
	cerr << msg.c_str() << endl;
      }
    catch(const exception &e)
      {
	cerr << "STL exception caught in main:\n" << e.what() << endl;
      }
    catch(...)
      {
	cerr << "Unknown exception caught in main" << endl;
      }
    return -1;
  }



Application::Application()
{
  // ctor
}



int Application::main(int argc,char *argv[])
{
  // Process command line
  CommandLine cmd(argc,argv,"d");
  if(cmd.numArgs()!=2)
    throw String("\n\
gff-intersect [options] <1.gff> <2.gff>\n\
   where:   -g = emit intersected intervals [NOT YET IMPLEMENTED]\n\
            -d = compute distance based on sharing\n\
");
  bool wantDist=cmd.option('d');
  String file1=cmd.arg(0), file2=cmd.arg(1);
  
  // Load files
  GffReader reader1(file1), reader2(file2);
  Vector<GffFeature*> *gff1=reader1.loadFeatures(); // sorted by begin coord
  Vector<GffFeature*> *gff2=reader2.loadFeatures(); // sorted by begin coord

  float score=intersect(*gff1,*gff2);
  if(wantDist) cout<<score<<endl;

  return 0;
}



float Application::intersect(Vector<GffFeature*> &v1,Vector<GffFeature*> &v2)
{
  int n1=v1.size(), n2=v2.size(); // numbers of intervals in each set
  for(int i=0 ; i<n1 ; ++i) v1[i]->setFrame(0);
  for(int i=0 ; i<n2 ; ++i) v2[i]->setFrame(0);
  if(n1==0 || n2==0) return 0;
  int size1=v1[0]->length(), size2=v2[0]->length();
  int minOverlap1=max(size1/2,3), minOverlap2=max(size2/2,3);
  int i1=0, i2=0; // indices
  //int overlap1=0, overlap2=0; // numbers of overlapping intervals in each set
  while(i1<n1 && i2<n2) {
    int begin=v2[i2]->getBegin();
    while(i1<n1 && v1[i1]->getEnd()<begin) ++i1;
    if(i1>=n1) break;
    begin=v1[i1]->getBegin();
    while(i2<n2 && v2[i2]->getEnd()<begin) ++i2;
    if(i2>=n2) break;
    int overlap=v1[i1]->overlapBases(*v2[i2]);
    if(overlap>=minOverlap1) v1[i1]->setFrame(1); //++overlap1;
    if(overlap>=minOverlap2) v2[i2]->setFrame(1); //++overlap2;
    if(v1[i1]->getEnd()<v2[i2]->getEnd()) ++i1;
    else ++i2;
  }
  int overlap1=0, overlap2=0;
  for(int i=0 ; i<n1 ; ++i) overlap1+=v1[i]->getFrame();
  for(int i=0 ; i<n2 ; ++i) overlap2+=v2[i]->getFrame();
  float percent1=float(overlap1)/n1, percent2=float(overlap2)/n2;
  //float percent=float(overlap1+overlap2)/(n1+n2);
  //return percent;
  return percent1<percent2 ? percent1 : percent2;
}



