/****************************************************************
 scramble.C
 william.majoros@duke.edu

 This is open-source software, governed by the ARTISTIC LICENSE 
 (see www.opensource.org).
 ****************************************************************/
#include <iostream>
#include "BOOM/String.H"
#include "BOOM/CommandLine.H"
#include "BOOM/FastaReader.H";
#include "BOOM/FastaWriter.H"
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
  if(cmd.numArgs()!=4)
    throw String("\n\
scramble [options] <in.fasta> <out.fasta> <order> <#replicates>\n\
\n\
");
  String infile=cmd.arg(0);
  String outfile=cmd.arg(1);
  int order=cmd.arg(2).asInt();
  int numReps=cmd.arg(3).asInt();

  FastaReader reader(infile);
  BOOM::Vector< pair<BOOM::String,BOOM::String> > &seqs=*reader.readAll();
  int n=seqs.size();
  for(int i=0 ; i<n ; ++i) {
    pair<String,String> &seq=seqs[i];
  }

  return 0;
}

