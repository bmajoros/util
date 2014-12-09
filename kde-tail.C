/****************************************************************
 kde-tail.C
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



int Application::main(int argc,char *argv[])
{
  // Process command line
  CommandLine cmd(argc,argv,"v:f:");
  if(cmd.numArgs()!=2)
    throw String("kde-tail <*.kde> RIGHT|LEFT [-f <file-of-values>] [-v value]");
  const String &modelFile=cmd.arg(0);
  String side=cmd.arg(1); side.toupper();
  if(side!="RIGHT" && side!="LEFT") throw "specificy RIGHT or LEFT tail";
  bool right=side=="RIGHT";
  
  KDE kde(modelFile);
  if(cmd.option('v')) {
    float x=cmd.optParm('v').asFloat();
    float P=right ? kde.rightTail(x) : kde.leftTail(x);
    cout<<P<<endl;
  }
  else if(cmd.option('f')) {
    ifstream is(cmd.optParm('f').c_str());
    if(!is.good()) throw "cannot open file";
    while(!is.eof()) {
      String line;
      is>>line;
      line.trimWhitespace();
      if(line.length()==0) continue;
      float x=line.asFloat();
      float P=right ? kde.rightTail(x) : kde.leftTail(x);
      cout<<P<<endl;
    }
  }
  
  return 0;
}

