/****************************************************************
 connected-components.C
 Copyright (C)2014 William H. Majoros (martiandna@gmail.com).
 This is OPEN SOURCE SOFTWARE governed by the Gnu General Public
 License (GPL) version 3, as described at www.opensource.org.
 ****************************************************************/
#include <iostream>
#include "BOOM/String.H"
#include "BOOM/CommandLine.H"
#include "BOOM/SparseGraph.H"
#include "BOOM/Array1D.H"
#include "BOOM/Stack.H"
using namespace std;
using namespace BOOM;


class Application {
public:
  Application();
  int main(int argc,char *argv[]);
private:
  void connectedComponents(SparseGraph &,
			   Vector<Neighborhood> &components);
  void dfs(SparseGraph &,VertexId,Array1D<bool> &mark,Neighborhood &component);
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
  if(cmd.numArgs()!=1)
    throw String("\n\
connected-components [options] <in.tgf>\n\
");
  String filename=cmd.arg(0);

  SparseGraph G(filename);
  Vector<Neighborhood> components;
  connectedComponents(G,components);


  return 0;
}




void Application::connectedComponents(SparseGraph &G,
				      Vector<Neighborhood> &components)
{
  int N=G.getNumVertices(), componentId=1;
  Array1D<bool> mark(N);
  mark.setAllTo(false);
  for(VertexId i=0 ; i<N ; ++i) {
    if(mark[i]) continue;
    Neighborhood component;
    dfs(G,i,mark,component);
    components.push_back(component);
    int size=component.size();
    cout<<"Component #"<<componentId++<<" "<<size<<" vertices:"<<endl;
    for(Neighborhood::iterator cur=component.begin(), end=component.end() ; 
	cur!=end ; ++cur) {
      VertexId id=*cur;
      cout<<G.getLabel(id)<<"\t";
    }
    cout<<endl;
  }
}



void Application::dfs(SparseGraph &G,VertexId v,Array1D<bool> &mark,
		      Neighborhood &component)
{
  Stack<VertexId> S;
  S.push(v);
  mark[v]=true;
  while(!S.isEmpty()) {
    VertexId v=S.pop();
    component.insert(v);
    bool shouldDelete;
    Neighborhood &children=*G.getNeighborsOf(v,shouldDelete);
    for(Neighborhood::iterator cur=children.begin(), end=children.end() ; 
	cur!=end ; ++cur) {
      VertexId child=*cur;
      if(mark[child]) continue;
      S.push(child);
      mark[child]=true;
      //component.insert(child);
    }
  }
}



