/****************************************************************
 foldchange.C
 william.majoros@duke.edu

 This is open-source software, governed by the ARTISTIC LICENSE 
 (see www.opensource.org).
 ****************************************************************/
#include <iostream>
#include "BOOM/String.H"
#include "BOOM/CommandLine.H"
#include "BOOM/File.H"
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
  CommandLine cmd(argc,argv,"b:e:ao:w:");
  if(cmd.numArgs()!=1)
    throw String("\n\
foldchange [options] <infile.bwig>\n\
   options:\n\
      -a = add abscissa to each line\n\
      -b begin\n\
      -e end\n\
      -o offset = add offset to abscissa (implies -a)\n\
      -w chom = output in wig format\n\
");
  String infile=cmd.arg(0);
  File inFile(infile);
  int begin=cmd.option('b') ? cmd.optParm('b').asInt() : 0;
  int end=cmd.option('e') ? cmd.optParm('e').asInt() : 
    inFile.getSize()/sizeof(float);
  if(begin>0) inFile.seek(begin*sizeof(float));
  int abscissa=0;
  bool wantAbscissa=cmd.option('a');
  if(cmd.option('o')) { abscissa=cmd.optParm('o').asInt(); wantAbscissa=true; }
  String substrate;
  const bool wantWig=cmd.option('w');
  if(wantWig) {
    substrate=cmd.optParm('w');
    wantAbscissa=true;
  }

  // Generate output
  if(wantWig) cout<<"\
track type=wiggle_0 name=fileName description=fileName\n\
variableStep chrom="<<substrate<<"\n";
  for(int pos=begin ; pos<end ; ++pos) {
    if(wantAbscissa) cout<<abscissa++<<"\t";
    cout<<inFile.readFloat()<<endl;
  }
  return 0;
}

