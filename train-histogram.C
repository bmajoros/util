/****************************************************************
 train-histogram.C
 Copyright (C)2014 William H. Majoros (martiandna@gmail.com).
 This is OPEN SOURCE SOFTWARE governed by the Gnu General Public
 License (GPL) version 3, as described at www.opensource.org.
 ****************************************************************/
#include <iostream>
#include <fstream>
#include "BOOM/String.H"
#include "BOOM/CommandLine.H"
#include "BOOM/Histogram.H"
#include "BOOM/Constants.H"
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
  CommandLine cmd(argc,argv,"b:w:p:");
  if(cmd.numArgs()!=2)
    throw String("\n\
train-histogram [options] <infile.txt> <outfile.hist>\n\
    -b num_bins\n\
    -w bin_width\n\
    -p pseudocount\n\
\n");
  const String &infile=cmd.arg(0);
  const String &outfile=cmd.arg(1);
  int numBins=10;
  if(cmd.option('b')) numBins=cmd.optParm('b').asInt();
  float pseudocount=cmd.option('p') ? cmd.optParm('p').asFloat() : 0;

  float min=FLT_MAX, max=FLT_MIN;
  Vector<float> values;
  ifstream is(infile.c_str());
  if(!is.good()) throw infile+" : error opening file";
  while(!is.eof()) {
    String line;
    is>>line;
    line.trimWhitespace();
    if(line.isEmpty()) break;
    float x=line.asFloat();
    values.push_back(x);
    if(x>max) max=x;
    else if(x<min) min=x;
  }
  is.close();

  float range=max-min;
  float binWidth=range/numBins;
  if(cmd.option('w')) {
    binWidth=cmd.optParm('w').asFloat();
    numBins=range/binWidth;
    if(range>numBins*binWidth) ++numBins;
  }
  Histogram<float> hist(min,max,numBins,pseudocount);
  hist.addCounts(values);
  
  hist.save(outfile);

  return 0;
}

