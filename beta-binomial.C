/****************************************************************
 beta-binomial.C
 Copyright (C)2014 William H. Majoros (martiandna@gmail.com).
 This is OPEN SOURCE SOFTWARE governed by the Gnu General Public
 License (GPL) version 3, as described at www.opensource.org.
 ****************************************************************/
#include <iostream>
#include "BOOM/String.H"
#include "BOOM/CommandLine.H"
#include "BOOM/VectorSorter.H"
using namespace std;
using namespace BOOM;

float SUM_THRESHOLD=1e-5;


/****************************************************************
                      class ProductInterval
 ****************************************************************/
struct ProductInterval {
  ProductInterval(int a,int b);
  ProductInterval() {}
  int peekLargest() const;
  int peekSmallest() const;
  void setLargest(int);
  void setSmallest(int);
  int numTerms() const;
  bool isEmpty() const;
  int pop();
  bool operator<(const ProductInterval &p);
  bool operator>(const ProductInterval &p);
  bool operator==(const ProductInterval &p);
  void printOn(ostream &) const;
protected:
  int largest, smallest;
};
ostream &operator<<(ostream &,const ProductInterval &);



/****************************************************************
                         class Fraction
 ****************************************************************/
struct Fraction {
  Vector<ProductInterval> numerator, denominator;
  void sort();
  void sort(Vector<ProductInterval> &);
  void printOn(ostream &) const;
  int pullOutLargest(Vector<ProductInterval> &);
};
ostream &operator<<(ostream &,const Fraction &);



/****************************************************************
                        class Application
 ****************************************************************/
class Application {
public:
  Application();
  int main(int argc,char *argv[]);
protected:
  float compute(int K,int N,int S,int F);
  void initialize(Fraction &,int K,int N,int S,int F);
  float twoSided(int K,int N,int S,int F,float mean);
  float leftTail(int K,int N,int S,int F);
  float rightTail(int K,int N,int S,int F);
  void cancelFactorials(Fraction &);
  float multiplyRatios(Fraction &);
  float multiplyRatios_log(Fraction &);
  float simpleBinomial(int K,int N,float p);
};



/****************************************************************
                             main()
 ****************************************************************/
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



/****************************************************************
                       Application methods
 ****************************************************************/

Application::Application()
{
  // ctor
}



int Application::main(int argc,char *argv[])
{
  // Process command line
  CommandLine cmd(argc,argv,"bte:s:");
  if(cmd.numArgs()!=4)
    throw String("\n\
beta-binomial <k> <n> <s> <s+f>\n\n\
   computes P(k|n,s,f), probability of k successes in n trials, given s+f\n\
                        pseudo-observations (s successes and f failures)\n\
   -b : compute simple binomial with p=s/(s+f)\n\
   -t : sum tail\n\
   -e X : use X as epsilon (threshold for contributing to tail sum)\n\
   -s n : use an n-sided test (1 or 2); default=1\n\
");
  const int K=cmd.arg(0).asInt();
  const int N=cmd.arg(1).asInt();
  const int S=cmd.arg(2).asInt();
  const int M=cmd.arg(3).asInt();
  const bool wantTail=cmd.option('t');
  const int sides=cmd.option('s') ? cmd.optParm('s').asInt() : 1;
  if(sides<1 || sides>2) throw "-s option must be 1 or 2";
  if(S==0 || S==M) throw "s and f must be >0";
  if(cmd.option('e')) SUM_THRESHOLD=cmd.optParm('e').asFloat();
  const F=M-S;

  if(wantTail) {
    const float mean=N*S/float(M);
    float sum=0;
    if(sides==1) {
      if(K<mean)
	for(int k=0 ; k<=K ; ++k) 
	  sum+=cmd.option('b') ? simpleBinomial(k,N,S/float(S+F)) : 
	    compute(k,N,S,F);
      else for(int k=K ; k<=N ; ++k) {
	  float f=cmd.option('b') ? simpleBinomial(k,N,S/float(S+F)) : 
	    compute(k,N,S,F);
	  sum+=f;
	  if(f<SUM_THRESHOLD) break; }
    }
    else { // 2-sided test
      sum=twoSided(K,N,S,F,mean);
    }
    cout<<sum<<endl;
  }
  else {
    if(cmd.option('b')) cout<<simpleBinomial(K,N,S/float(S+F));
    else cout<<compute(K,N,S,F)<<endl;
  }

  return 0;
}



float Application::simpleBinomial(int K,int N,float p)
{
  // First, compute the binominal coefficient, N choose K
  Fraction f;
  f.numerator.push_back(ProductInterval(N,1));
  if(K>0) f.denominator.push_back(ProductInterval(K,1));
  if(N-K>0) f.denominator.push_back(ProductInterval(N-K,1));
  f.sort();
  cancelFactorials(f);
  float coef=multiplyRatios_log(f);

  // Now compute product in log space
  float P=coef+K*log(p)+(N-K)*log(1-p);

  // Take out of log space
  P=exp(P);
  return P;
}



float Application::compute(int K,int N,int S,int F)
{
  // Initialize list of factorials
  Fraction fraction;
  initialize(fraction,K,N,S,F);

  // Form product intervals by canceling one factorial in the numerator
  // against one in the denominator
  cancelFactorials(fraction);

  // Now repeatedly form ratios of individual integers, by pulling out the
  // single largest value from the numerator and the denominator
  float product=multiplyRatios_log(fraction);

  return exp(product);
}



void Application::initialize(Fraction &fraction,int K,int N,int S,int F)
{
  const int M=S+F;
  if(N>0) fraction.numerator.push_back(ProductInterval(N,1));
  if(K+S-1>0) fraction.numerator.push_back(ProductInterval(K+S-1,1));
  if(N-K+F-1>0) fraction.numerator.push_back(ProductInterval(N-K+F-1,1));
  if(M-1>0) fraction.numerator.push_back(ProductInterval(M-1,1));

  if(K>0) fraction.denominator.push_back(ProductInterval(K,1));
  if(N-K>0) fraction.denominator.push_back(ProductInterval(N-K,1));
  if(N+M-1>0) fraction.denominator.push_back(ProductInterval(N+M-1,1));
  if(S-1>0) fraction.denominator.push_back(ProductInterval(S-1,1));
  if(F-1>0) fraction.denominator.push_back(ProductInterval(F-1,1));

  fraction.sort();
}



void Application::cancelFactorials(Fraction &fraction)
{
  Fraction f;
  int nNumer=fraction.numerator.size(), nDenom=fraction.denominator.size();
  int m=min(nNumer,nDenom);
  for(int i=0 ; i<m ; ++i) {
    ProductInterval numer=fraction.numerator[i];
    ProductInterval denom=fraction.denominator[i];
    if(numer>denom) {
      numer.setSmallest(denom.peekLargest()+1);
      f.numerator.push_back(numer);
    }
    else if(denom>numer) {
      denom.setSmallest(numer.peekLargest()+1);
      f.denominator.push_back(denom);
    }
  }
  for(int i=m ; i<nNumer ; ++i) 
    f.numerator.push_back(fraction.numerator[i]);
  for(int i=m ; i<nDenom ; ++i) 
    f.denominator.push_back(fraction.denominator[i]);
  
  fraction=f;
}
 


float Application::multiplyRatios(Fraction &f)
{
  float r=1;
  int nNumer=f.numerator.size(), nDenom=f.denominator.size();
  while(!f.numerator.isEmpty() && !f.denominator.isEmpty()) {
    int numer=f.pullOutLargest(f.numerator);
    int denom=f.pullOutLargest(f.denominator);
    float ratio=numer/float(denom);
    r*=ratio;
  }
  while(!f.numerator.isEmpty()) r*=f.pullOutLargest(f.numerator);
  while(!f.denominator.isEmpty())r*=f.pullOutLargest(f.denominator);
  return r;
}



float Application::multiplyRatios_log(Fraction &f)
{
  float r=0;
  int nNumer=f.numerator.size(), nDenom=f.denominator.size();
  while(!f.numerator.isEmpty() && !f.denominator.isEmpty()) {
    int numer=f.pullOutLargest(f.numerator);
    int denom=f.pullOutLargest(f.denominator);
    r+=log(numer)-log(denom);
  }
  while(!f.numerator.isEmpty()) r+=log(f.pullOutLargest(f.numerator));
  while(!f.denominator.isEmpty())r+=log(f.pullOutLargest(f.denominator));
  return r;
}



float Application::leftTail(int K,int N,int S,int F)
{
  float sum=0;
  for(int k=0 ; k<=K ; ++k) 
    sum+=compute(k,N,S,F);
  return sum;
}



float Application::rightTail(int K,int N,int S,int F)
{
  float sum=0;
  for(int k=K ; k<=N ; ++k) {
    float f=compute(k,N,S,F);
    sum+=f;
    if(f<SUM_THRESHOLD) break;
  }
  return sum;
}



float Application::twoSided(int K,int N,int S,int F,float mean)
{
  float sum=0;
  if(K<mean) {
    const float p=compute(K,N,S,F);
    sum=p+leftTail(K-1,N,S,F);
    for(int k=N ; k>K ; --k) {
      const float q=compute(k,N,S,F);
      if(q>p) break;
      sum+=q;
    }
  }
  else {
    const float p=compute(K,N,S,F);
    sum=p+rightTail(K+1,N,S,F);
    for(int k=0 ; k<K ; ++k) {
      const float q=compute(k,N,S,F);
      if(q>p) break;
      sum+=q;
    }
  }
  return sum;
}



/****************************************************************
                     ProductInterval methods
 ****************************************************************/

ProductInterval::ProductInterval(int a,int b) : largest(a), smallest(b) 
{
}



int ProductInterval::peekLargest() const 
{ 
  return largest; 
}



int ProductInterval::peekSmallest() const
{ 
  return smallest; 
}



int ProductInterval::numTerms() const
{ 
  return largest-smallest+1; 
}



int ProductInterval::pop() 
{ 
  return largest--; 
}



bool ProductInterval::operator<(const ProductInterval &p) 
{ 
  return largest<p.largest; 
}



bool ProductInterval::operator>(const ProductInterval &p) 
{ 
  return largest>p.largest; 
}



bool ProductInterval::operator==(const ProductInterval &p) 
{ 
  return largest==p.largest; 
}



void ProductInterval::setLargest(int l)
{
  largest=l;
}



void ProductInterval::setSmallest(int s)
{
  smallest=s;
}



void ProductInterval::printOn(ostream &os) const
{
  os<<"["<<largest<<"..."<<smallest<<"]";
}



ostream &operator<<(ostream &os,const ProductInterval &p)
{
  p.printOn(os);
  return os;
}



bool ProductInterval::isEmpty() const
{
  return largest<smallest;
}


/****************************************************************
                         Fraction methods
 ****************************************************************/

void Fraction::sort()
{
  sort(numerator);
  sort(denominator);
}



void Fraction::sort(Vector<ProductInterval> &v) 
{
  DirectComparator<ProductInterval> cmp;
  VectorSorter<ProductInterval> sorter(v,cmp);
  sorter.sortDescendInPlace();
}



void Fraction::printOn(ostream &os) const
{
  for(Vector<ProductInterval>::const_iterator cur=numerator.begin(),
	end=numerator.end() ; cur!=end ; ++cur) 
    os<<*cur<<" ";
  os<<"/ ";
  for(Vector<ProductInterval>::const_iterator cur=denominator.begin(),
	end=denominator.end() ; cur!=end ; ++cur) 
    os<<*cur<<" ";
}



ostream &operator<<(ostream &os,const Fraction &f)
{
  f.printOn(os);
  return os;
}



int Fraction::pullOutLargest(Vector<ProductInterval> &v)
{
  ProductInterval &p=v[0];
  int x=p.pop();
  if(p.isEmpty()) v.cut(0);
  else if(v.size()>1 && v[1]>p) v.swap(0,1);
  return x;
}



