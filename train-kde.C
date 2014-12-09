/****************************************************************
 train-kde.C
 Copyright (C)2014 William H. Majoros (martiandna@gmail.com).
 This is OPEN SOURCE SOFTWARE governed by the Gnu General Public
 License (GPL) version 3, as described at www.opensource.org.
 ****************************************************************/
#include <iostream>
#include <fstream>
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



int Application::main(int argc,char *argv[]) {
  // Process command line
  CommandLine cmd(argc,argv,"");
  if(cmd.numArgs()!=3)
    throw String("train-kde <points.txt> <rounding-thresh> <outfile>");
  const String &infile=cmd.arg(0);
  const float tolerance=cmd.arg(1).asFloat();
  const String &outfile=cmd.arg(2);

  Vector<float> points;
  ifstream is(infile.c_str());
  while(!is.eof()) {
    String line;
    is>>line;
    line.trimWhitespace();
    if(line.length()==0) continue;
    float x=line.asFloat();
    points.push_back(x);
  }

  KDE kde(points,0,tolerance);
  kde.save(outfile);

  return 0;
}

