/****************************************************************
 txt-to-binwig.C
 william.majoros@duke.edu

 This is open-source software, governed by the ARTISTIC LICENSE 
 (see www.opensource.org).
 ****************************************************************/
#include <iostream>
#include "BOOM/String.H"
#include "BOOM/CommandLine.H"
#include "BOOM/Regex.H"
#include "BOOM/File.H"
using namespace std;
using namespace BOOM;


class Application {
public:
  Application();
  int main(int argc,char *argv[]);
protected:
  Regex number;
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
  : number("\\s*((\\d|\\.|e|E|-)+)\\s*")
{
  // ctor
}



int Application::main(int argc,char *argv[])
{
  // Process command line
  CommandLine cmd(argc,argv,"");
  if(cmd.numArgs()!=2)
    throw String("txt-to-binwig <infile.txt> <outfile.bwig>");
  String infile=cmd.arg(0), outfile=cmd.arg(1);

  File inFile(infile), outFile(outfile,"w");
  String chrom;
  float value=0.0;
  while(!inFile.eof()) {
    String line=inFile.getline();
    if(number.match(line)) {
      float value=number[1].asFloat();
      outFile.write(value);
    }
  }

  return 0;
}

