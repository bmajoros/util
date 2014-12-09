/****************************************************************
 plot-kde.C
 Copyright (C)2014 William H. Majoros (martiandna@gmail.com).
 This is OPEN SOURCE SOFTWARE governed by the Gnu General Public
 License (GPL) version 3, as described at www.opensource.org.
 ****************************************************************/
#include <iostream>
#include "BOOM/String.H"
#include "BOOM/CommandLine.H"
#include "KDE.H"
using namespace std;
using namespace BOOM;


class Application
{
public:
  Application();
  int main(int argc,char *argv[]);
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
  CommandLine cmd(argc,argv,"b:");
  if(cmd.numArgs()!=2)
    throw String("plot-kde <distr.kde> <num-points> [-b bandwidth]");
  const String &filename=cmd.arg(0);
  const int numPoints=cmd.arg(1).asInt();

  // Load KDE file
  KDE kde(filename);
  if(cmd.option('b')) kde.changeBandwidth(cmd.optParm('b').asFloat());
  
  // Determine bin sizes
  float mean, sd;
  kde.summaryStats(mean,sd);
  float min=mean-4*sd;
  float max=mean+4*sd;
  float range=max-min;

  // Plot points
  float nMinus1=numPoints-1;
  for(int i=0 ; i<numPoints ; ++i) {
    float x=min+i/nMinus1*range;
    float p=kde.density(x);
    cout<<x<<"\t"<<p<<endl;
  }

  return 0;
}

