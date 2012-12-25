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
#define EPS (double)1e-10
typedef long long LL;
string spaces = " \t\r\n";
const int MAX_NUMBER_OF_USERS = 2400000, MAX_NUMBER_OF_ITEMS = 6500, MAX_LEN_OF_LINE_IN_FILE = 10000;

int *outgoingEdges[MAX_NUMBER_OF_USERS];
int *incomingEdges[MAX_NUMBER_OF_USERS];
int *outgoingEdgeWeight[MAX_NUMBER_OF_USERS];
int *incomingEdgeWeight[MAX_NUMBER_OF_USERS];
int arr[MAX_NUMBER_OF_USERS];
int weights[MAX_NUMBER_OF_USERS];
int indices[MAX_NUMBER_OF_USERS];

bool cmp(int a, int b)
{
  if(arr[a] == arr[b])return a < b ? true: false;
  return arr[a] < arr[b] ? true: false;
}

void Initialize()
{
  REP(userid, MAX_NUMBER_OF_USERS)
  {
    outgoingEdges[userid] = new int[1];
    outgoingEdges[userid][0] = 0;

    outgoingEdgeWeight[userid] = new int[1];
    outgoingEdgeWeight[userid][0] = 0;

    incomingEdges[userid] = new int[1];
    incomingEdges[userid][0] = 0;

    incomingEdgeWeight[userid] = new int[1];
    incomingEdgeWeight[userid][0] = 0;

    indices[userid] = userid;
  }
}

void StripSpaces(string &str)
{
  while(str.sz && spaces.find(str[0]) != string::npos)str=str.substr(1);
  while(str.sz && spaces.find(str[str.sz-1]) != string::npos)str=str.substr(0, str.sz-1);
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
  vector<int> inv, outv;
  vector<int> inweights, outweights;
  while(fin)
  {
    lineno++;
    if(fin.eof())break;
    int followerid = -1, followeeid = -1, count = -1;
    fin>>followerid>>followeeid>>count;
    if(followerid == -1 || followeeid == -1 || count == -1)continue;
   
    if(prevfollowerid != -1 && prevfollowerid != followerid)
    {
      int Nin = inv.sz;
      incomingEdges[prevfollowerid] = new int[Nin + 1];
      incomingEdges[prevfollowerid][0] = Nin;
    
      incomingEdgeWeight[prevfollowerid] = new int[Nin + 1];
      incomingEdgeWeight[prevfollowerid][0] = Nin;
      
      REP(i,Nin){
        incomingEdges[prevfollowerid][i + 1] = inv[i];
        incomingEdgeWeight[prevfollowerid][i + 1] = inweights[i];
      }

      int Nout = outv.sz;
      outgoingEdges[prevfollowerid] = new int[Nout + 1];
      outgoingEdges[prevfollowerid][0] = Nout;
      
      outgoingEdgeWeight[prevfollowerid] = new int[Nout + 1];
      outgoingEdgeWeight[prevfollowerid][0] = Nout;

      REP(i, Nout) {
        outgoingEdges[prevfollowerid][i + 1] = outv[i];
        outgoingEdgeWeight[prevfollowerid][i + 1] = outweights[i];
      }

      inv.clear(); inweights.clear();
      outv.clear(); outweights.clear();
    }
    
    prevfollowerid = followerid;
    if(count > 0) {
      outv.pb(followeeid);
      outweights.pb(count);
    }
    else {
      inv.pb(followeeid);
      inweights.pb(count);
    }
    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
 
  REP(followerid, 2)
  {
    dbge(followerid); 
    dbge(incomingEdges[followerid][0]);
    REP(i, incomingEdges[followerid][0])cout<<" "<<incomingEdges[followerid][i + 1]<<":"<<incomingEdgeWeight[followerid][i+1];
    cout<<endl;

    dbge(outgoingEdges[followerid][0]);
    REP(i, outgoingEdges[followerid][0])cout<<" "<<outgoingEdges[followerid][i + 1]<<":"<<outgoingEdgeWeight[followerid][i+1];
    cout<<endl;
  }
 
  fin.close();
}

void OutputBFSDepth2(string filename)
{
  ofstream fout(filename.cs);
  if(!fout){cout<<"Could not open file:"<<filename<<endl; exit(0);}
  
  clock_t start = clock();
  REP(userid, MAX_NUMBER_OF_USERS)
  {
    int totaldegree = 0;
    REP(i, outgoingEdges[userid][0])
    {
      arr[totaldegree] = outgoingEdges[userid][i+1];
      weights[totaldegree++] = outgoingEdgeWeight[userid][i+1];
    }

    REP(i, incomingEdges[userid][0])
    {
      arr[totaldegree] = incomingEdges[userid][i+1];
      weights[totaldegree++] = incomingEdgeWeight[userid][i+1];
    }
    
    REP(i, outgoingEdges[userid][0])
    {
      int neighbour = outgoingEdges[userid][i + 1];
      int curweight = outgoingEdgeWeight[userid][i + 1];
      REP(j, outgoingEdges[neighbour][0])
      {
        arr[totaldegree] = outgoingEdges[neighbour][j + 1];
        weights[totaldegree++] = min(curweight, outgoingEdgeWeight[neighbour][j + 1]);
      }
    }
  
    sort(indices, indices+totaldegree, cmp);  

    int prevfolloweeid = -1;
    REP(i, totaldegree)
    {
      if(arr[ indices[i] ] != prevfolloweeid)
        fout<< userid << " " << arr[ indices[i] ] << " " << weights[ indices[i] ] << endl;

      prevfolloweeid = arr[ indices[i] ];

      indices[i] = i;
    }

    if(userid % 1000000 == 0)cout<<"Time taken to load "<< userid <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  }

  cout<<"Time taken to load "<< MAX_NUMBER_OF_USERS <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
 
  fout.close(); 
}

int main()
{
  //Initialize
  Initialize();
  
  LoadSocialNetwork("/home/arvind/track1/weighteddirgraph_mapped_retweet_sorted.txt");

  OutputBFSDepth2("/home/arvind/track1/weighteddirgraph_mapped_retweet_sorted_bfsdepth2.txt");

  return 0;  
}

