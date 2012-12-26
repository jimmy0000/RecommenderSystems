#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <sstream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
using namespace std;
#define LET(_x,_a) typeof(_a) _x(_a)
#define FOR(_i, _a, _n) for(LET(_i, _a); _i < _n; ++_i)
#define REP(_i, _n) FOR(_i, 0, _n)
#define pb push_back
#define sz size()
#define cs c_str()
#define EACH(it, v) FOR(LET(it, v.begin()); it != v.end(); ++it)
#define _DEBUG 1
#define _dbg(_x) cout<<#_x<<":"<<(_x)
#define dbg(_x) if(_DEBUG)_dbg(_x), cout<<"\t";
#define dbge(_x) if(_DEBUG)_dbg(_x), cout<<"\n";
#define VI vector<int>
#define PII pair<int, int>
#define PSD pair<string, float> 
#define PDS pair<float, string>
#define PDI pair<float, int>
#define PID pair<int, float>
#define PDD pair<float, float>
#define PDDD pair<float, PDD >
#define s2d(_str) ({stringstream _sout(_str); float _d; _sout>>_d; _d;})
#define s2i(_str) ({stringstream sin(_str); int _x; sin>>_x; _x;})
#define s2l(_str) ({stringstream sin(_str); LL _x; sin>>_x; _x;})
#define num2s(_x) ({stringstream sout; sout<<_x; sout.str();})
#define MAE(trueRatingsVector, estimatedRatingsVector) ({float _mae = 0.0; REP(_i, trueRatingsVector.sz)_mae += abs(trueRatingsVector[_i] - estimatedRatingsVector[_i]); _mae /= trueRatingsVector.sz; _mae;})
#define EPSILON (double)1e-10
typedef long long LL;
string spaces = " \t\r\n";
const int MAX_NUMBER_OF_USERS = 2400000, MAX_NUMBER_OF_ITEMS = 6500, MAX_LEN_OF_LINE_IN_FILE = 10000;

int *adjacencyList[MAX_NUMBER_OF_USERS];
double pagerankarr[MAX_NUMBER_OF_USERS];
double newpagerankarr[MAX_NUMBER_OF_USERS];
int *danglingnodearr;
int numusers = 0, numdanglingnodes = 0;

void Initialize()
{
  REP(userid, MAX_NUMBER_OF_USERS)
  {
    adjacencyList[userid] = new int[1];
    adjacencyList[userid][0] = 0;
  }
}

void StripSpaces(string &str)
{
  while(str.sz && spaces.find(str[0]) != string::npos)str=str.substr(1);
  while(str.sz && spaces.find(str[str.sz-1]) != string::npos)str=str.substr(0, str.sz-1);
}

void LoadUserMapping(string datafile)
{
  ifstream fin(datafile.cs);
  if(!fin){cout<<"Could not open file:"<<datafile<<endl; exit(0);}
  
  clock_t start = clock();
  while(fin)
  {
    if(fin.eof())break;
    int followerid;
    fin>>followerid;
    if(followerid == -1)continue;
    numusers++;

    if(numusers % 1000000 == 0)cout<<"Time taken to load "<< numusers <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< numusers <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  fin.close();
}

void LoadSocialNetwork(string datafile)
{
  //This procedure took 2 mins to load and 300 MB of memory
  ifstream fin(datafile.cs);
  if(!fin){cout<<"Could not open file:"<<datafile<<endl; exit(0);}
  int lineno = -1;
  char buff[MAX_LEN_OF_LINE_IN_FILE];
  
  clock_t start = clock();
  int prevfollowerid = -1;
  vector<int> v;
  while(fin)
  {
    lineno++;
    if(fin.eof())break;
    int followerid = -1, followeeid = -1, count = -1;
    fin>>followerid>>followeeid>>count;
    if(followerid == -1 || followeeid == -1 || count == -1)continue;
   
    if(prevfollowerid != -1 && prevfollowerid != followerid)
    {
      int N = v.sz;
      adjacencyList[prevfollowerid] = new int[N + 1];
      adjacencyList[prevfollowerid][0] = N;
    
      REP(i,N)adjacencyList[prevfollowerid][i + 1] = v[i];

      v.clear();
    }
    
    prevfollowerid = followerid;
    v.pb(followeeid);
    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
 
  vector<int> danglingnodevec;
  double default_pagerank_value = 1.0 / (double)numusers;
  REP(userid, numusers)
  {
    pagerankarr[userid] = default_pagerank_value;
    if (adjacencyList[userid][0] == 0)danglingnodevec.pb(userid);
  }

  numdanglingnodes = danglingnodevec.sz;
  danglingnodearr = new int[numdanglingnodes];
  REP(i, numdanglingnodes)danglingnodearr[i] = danglingnodevec[i];

  REP(followerid, 2)
  {
    dbg(followerid); dbg(adjacencyList[followerid][0]); dbg(pagerankarr[followerid]);
    REP(i, adjacencyList[followerid][0])cout<<" "<<adjacencyList[followerid][i + 1];
    cout<<endl;
  }
 
  fin.close();
}

void ComputePageRank(double s = 0.85, double tolerance = 0.0001,int MAXIMUM_ITERATIONS = 150)
{
  clock_t start = clock();
  REP(iteration, MAXIMUM_ITERATIONS)
  {
    double innerproduct = 0;
    REP(i, numdanglingnodes)innerproduct += pagerankarr[ danglingnodearr[i] ];

    double additionalPageRankTerm = s * innerproduct / (double)numusers;
    REP(userid, numusers)newpagerankarr[userid] = additionalPageRankTerm;
    REP(userid, numusers)
    {
      newpagerankarr[userid] += (1.0 - s) * pagerankarr[userid];
      REP(i, adjacencyList[userid][0])
      {
        int destuserid = adjacencyList[userid][i+1];
        newpagerankarr[destuserid] += s * pagerankarr[userid] / (double)adjacencyList[userid][0];
      }
    }


    double L1NormDifference = 0;
    REP(userid, numusers)
    {
      L1NormDifference += abs(newpagerankarr[userid] - pagerankarr[userid]);
      pagerankarr[userid] = newpagerankarr[userid];
    }

    cout<<"Iteration:"<<iteration<<" L1NormDifference:"<<L1NormDifference<<" ended at time: "<<(clock() -start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;

    if(L1NormDifference < tolerance - tolerance * EPSILON)break;
  }
}

void OutputPageRank(string outputfile)
{
  ofstream fout(outputfile.cs);
  clock_t start = clock();
  REP(userid, numusers)
  {
    fout<<userid<<" "<<pagerankarr[userid]<<endl;

    if(userid % 1000000 == 0)cout<<"Time taken to load "<< userid <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< numusers <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;

  fout.close();
}


int main()
{
  //Initialize
  Initialize();
  
  LoadUserMapping("/home/arvind/track1/userid_mapping.txt");

  LoadSocialNetwork("/home/arvind/track1/weighteddirgraph_mapped_retweet_sorted_bfsdepth2.txt");
  
  ComputePageRank();

  OutputPageRank("/home/arvind/track1/user_mapped_pagerank_retweet_bfsdepth2_undirected_rooted.txt");

  return 0;  
}

