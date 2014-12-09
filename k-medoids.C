/****************************************************************
 k-medoids.C
 Copyright (C)2014 William H. Majoros (martiandna@gmail.com).
 This is OPEN SOURCE SOFTWARE governed by the Gnu General Public
 License (GPL) version 3, as described at www.opensource.org.
 ****************************************************************/
#include <iostream>
#include <fstream>
#include "BOOM/String.H"
#include "BOOM/CommandLine.H"
#include "BOOM/TriangMatrix.H"
#include "BOOM/Array1D.H"
#include "BOOM/Random.H"
#include "BOOM/Constants.H"
using namespace std;
using namespace BOOM;


typedef int ObjectId;
typedef Set<ObjectId> Cluster;
typedef Array1D<Cluster> ClusterSet;

struct Clustering {
  ClusterSet clusters;
  Array1D<ObjectId> medoids;
};

class Application {
public:
  Application();
  int main(int argc,char *argv[]);
private:
  int K;
  int numIter, numRuns;
  TriangMatrix<float> D;
  Vector<String> names;
  Map<String,int> nameToId;
  Clustering clustering;
  Array1D<int> clusterAssignments;
  int numObjects;
  void loadLabels(const String &filename);
  void loadDistances(const String &filename);
  void cluster(int numIter);
  void output(Clustering &clustering);
  float scoreClustering(Clustering &);
  void initClusters();
  void clear(Clustering &);
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
  randomize();
}



int Application::main(int argc,char *argv[])
{
  // Process command line
  CommandLine cmd(argc,argv,"");
  if(cmd.numArgs()!=5)
    throw String("\n\
k-medoids <distances.txt> <labels.txt> <k> <#runs> <#iter-per-run>\n\
");
  String distFile=cmd.arg(0);
  String labelFile=cmd.arg(1);
  K=cmd.arg(2).asInt();
  numRuns=cmd.arg(3).asInt();
  numIter=cmd.arg(4).asInt();

  // Load data
  loadLabels(labelFile);
  loadDistances(distFile);

  // Perform clustering
  Clustering bestClustering;
  float bestScore=LARGEST_FLOAT;
  for(int i=0 ; i<numRuns ; ++i) {
  clear(clustering);
    initClusters();
    cluster(numIter);
    float score=scoreClustering(clustering);
    if(score<bestScore) { bestScore=score; bestClustering=clustering; }
  }

  // Output results
  output(bestClustering);
  cerr<<bestScore<<endl;

  return 0;
}



void Application::loadLabels(const String &filename)
{
  ifstream is(filename.c_str());
  while(!is.eof()) {
    String line;
    line.getline(is);
    line.trimWhitespace();
    if(line.length()>0) {
      int id=names.size();
      names.push_back(line);
      nameToId[line]=id;
    }
  }
  numObjects=names.size();
  if(numObjects<K) throw "Number of objects is smaller than K";
  clustering.medoids.resize(K);
  D.resize(numObjects);
  D.setAllTo(LARGEST_FLOAT);
  for(int i=0 ; i<numObjects ; ++i) D.safeIndex(i,i)=0;
  clusterAssignments.resize(numObjects);
  clustering.clusters.resize(K);
}



void Application::loadDistances(const String &filename)
{
  ifstream is(filename.c_str());
  while(!is.eof()) {
    String line;
    line.getline(is);
    BOOM::Vector<BOOM::String> &fields=*line.getFields();
    int numFields=fields.size();
    if(numFields>=3)
      for(int i=0 ; i<numFields ; ++i) {
	float dist=fields[0].asFloat();
	int id1=nameToId[fields[1]];
	int id2=nameToId[fields[2]];
	D.safeIndex(id1,id2)=dist;
      }
    delete &fields;
  }
}



void Application::initClusters()
{
  // Randomly initialize medoids
  Array1D<bool> used(numObjects); used.setAllTo(false);
  for(int i=0 ; i<K ; ++i) {
    int index=RandomNumber(numObjects);
    while(used[index]) index=(index+1)%numObjects;
    used[index]=true;
    clustering.medoids[i]=index;
  }
}



void Application::cluster(int numIter)
{
  // Iterate
  for(int i=0 ; i<numIter ; ++i) {
    // Assign objects to clusters
    ClusterSet clusterMembers(K);
    for(ObjectId j=0 ; j<numObjects ; ++j) {
      int bestCluster;
      float bestDist=LARGEST_FLOAT;
      for(int k=0 ; k<K ; ++k) {
	int medoid=clustering.medoids[k];
	//if(j==medoid) continue;
	float dist=D.safeIndex(j,medoid);
	if(dist<bestDist) {
	  bestCluster=k;
	  bestDist=dist;
	}
      }
      clusterAssignments[j]=bestCluster;
      clusterMembers[bestCluster].insert(j);
    }

    // Recompute medoids
    for(int k=0 ; k<K ; ++k) {
      Cluster &members=clusterMembers[k];
      ObjectId bestMedoid;
      float bestDist=LARGEST_FLOAT;
      for(Cluster::iterator cur=members.begin(), end=members.end() ;
	  cur!=end ; ++cur) {
	ObjectId id=*cur;
	float sum=0;
	for(Cluster::iterator cur=members.begin(), end=members.end() ;
	    cur!=end ; ++cur) {
	  ObjectId other=*cur;
	  if(other!=id) sum+=D.safeIndex(id,other);
	}
	if(sum<bestDist) {
	  bestDist=sum;
	  bestMedoid=id;
	}
      }
      clustering.medoids[k]=bestMedoid;
    }
  }

  for(ObjectId j=0 ; j<numObjects ; ++j) {
    int bestCluster;
    float bestDist=LARGEST_FLOAT;
    for(int k=0 ; k<K ; ++k) {
      int medoid=clustering.medoids[k];
      float dist=D.safeIndex(j,medoid);
      if(dist<bestDist) {
	bestCluster=k;
	bestDist=dist;
      }
    }
    clusterAssignments[j]=bestCluster;
    clustering.clusters[bestCluster].insert(j);
  }
}



void Application::output(Clustering &clustering)
{
  for(int k=0 ; k<K ; ++k) {
    Cluster &members=clustering.clusters[k];
    ObjectId medoid=clustering.medoids[k];
    int kPlus1=k+1;
    cout<<"Cluster #"<<kPlus1<<":\t";
    cout<<names[medoid]<<"\t";
    for(Cluster::iterator cur=members.begin(), end=members.end() ;
	cur!=end ; ++cur) {
      if(*cur!=medoid) cout<<names[*cur]<<"\t";
    }
    cout<<endl;
  }
}



float Application::scoreClustering(Clustering &clustering)
{
  float sum=0;
  int numClusters=clustering.clusters.size();
  for(int i=0 ; i<numClusters ; ++i) {
    Cluster &cluster=clustering.clusters[i];
    ObjectId medoid=clustering.medoids[i];
    for(Cluster::iterator cur=cluster.begin(), end=cluster.end() ; 
	cur!=end ; ++cur) {
      sum+=D.safeIndex(*cur,medoid);
      }
  }
  return sum;
}



void Application::clear(Clustering &c)
{
  c.medoids.resize(K);
  c.clusters.resize(K);
  for(int i=0 ; i<K ; ++i) c.clusters[i].clear();
}


