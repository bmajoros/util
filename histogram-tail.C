/****************************************************************
 histogram-tail.C
 Copyright (C)2014 William H. Majoros (martiandna@gmail.com).
 This is OPEN SOURCE SOFTWARE governed by the Gnu General Public
 License (GPL) version 3, as described at www.opensource.org.
 ****************************************************************/
#include <iostream>
#include "BOOM/String.H"
#include "BOOM/CommandLine.H"
#include "BOOM/Histogram.H"
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
  CommandLine cmd(argc,argv,"");
  if(cmd.numArgs()!=3)
    throw String("histogram-tail <infile.hist> <value> RIGHT|LEFT");
  const String &infile=cmd.arg(0);
  const float value=cmd.arg(1).asFloat();
  String side=cmd.arg(2);
  side.toupper();
  if(side!="LEFT" && side!="RIGHT") throw side+" : should be RIGHT or LEFT";

  Histogram<float> hist(infile);
  hist.normalize();
  const float max=hist.getMax(), min=hist.getMin();;
  const int numBins=hist.getNumBins();
  float sum=0;
  const int firstBin=hist.getBinNum(value);
  if(side=="RIGHT")
    for(int i=firstBin ; i<numBins ; ++i) sum+=hist.getBin(i);
  else
    for(int i=firstBin ; i>=0 ; --i) sum+=hist.getBin(i);
  cout<<sum<<endl;

  return 0;
}

