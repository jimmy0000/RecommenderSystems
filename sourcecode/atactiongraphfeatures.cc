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
#define EPS (double)1e-10
typedef long long LL;
string spaces = " \t\r\n";
const int MAX_NUMBER_OF_USERS = 2400000, MAX_NUMBER_OF_ITEMS = 6500, MAX_LEN_OF_LINE_IN_FILE = 10000;

int *adjacencyList[MAX_NUMBER_OF_USERS];
double *edgeWeight[MAX_NUMBER_OF_USERS];
double edgeNorm[MAX_NUMBER_OF_USERS];
int mappeditem_mappeduser[MAX_NUMBER_OF_ITEMS];

void Initialize()
{
  REP(userid, MAX_NUMBER_OF_USERS)
  {
    adjacencyList[userid] = new int[1];
    adjacencyList[userid][0] = 0;

    edgeWeight[userid] = new double[1];
    edgeWeight[userid][0] = 0.0;

    edgeNorm[userid] = 0.0;
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
  vector<int> v;
  vector<double> weights;
  double norm = 0.0, sum = 0.0;
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
    
      edgeWeight[prevfollowerid] = new double[N + 1];
      edgeWeight[prevfollowerid][0] = sum;
      
      if(N > 0)REP(i,N){
        adjacencyList[prevfollowerid][i + 1] = v[i];
        edgeWeight[prevfollowerid][i + 1] = weights[i];
      }

      edgeNorm[prevfollowerid] = sqrt(norm);

      v.clear();
      weights.clear();
      sum = 0.0;
      norm = 0.0;
    }
    
    prevfollowerid = followerid;
    v.pb(followeeid);
    weights.pb((double)count);
    sum += (double)count;
    norm += (double)count * (double)count;
    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
 
  REP(followerid, 2)
  {
    dbg(followerid); dbg(adjacencyList[followerid][0]);
    dbg(edgeWeight[followerid][0]); dbge(edgeNorm[followerid]);
    REP(i, adjacencyList[followerid][0])cout<<" "<<adjacencyList[followerid][i + 1]<<":"<<edgeWeight[followerid][i+1];
    cout<<endl;
  }
 
  fin.close();
}

void LoadMappedItemsToMappedUsers(string datafile)
{
  ifstream fin(datafile.cs);
  if(!fin){cout<<"Could not open file:"<<datafile<<endl; exit(0);}
  int lineno = -1;
  char buff[MAX_LEN_OF_LINE_IN_FILE];
  
  clock_t start = clock();
  while(fin)
  {
    lineno++;
    if(fin.eof())break;
    int mappeduserid = -1; 
    fin>>mappeduserid;
    if(mappeduserid == -1)continue;
    mappeditem_mappeduser[lineno] =  mappeduserid;
    
    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
    if(lineno < 5)
    {
      cout<<"ItemId:"<<lineno<<" MappedUserId:"<<mappeditem_mappeduser[lineno]<<endl;
    }
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  fin.close();
}

void AppendGraphFeaturesToDataset(string inputfile, string dataseparator, string outputfile)
{
  ifstream fin(inputfile.cs);
  if(!fin){cout<<"Could not open file:"<<inputfile<<endl; exit(0);}
  cout<<"Begin processing: "<<inputfile<<endl;
  
  ofstream fout(outputfile.cs);
  int lineno = -1; 
  char buff[MAX_LEN_OF_LINE_IN_FILE];
  
  clock_t start = clock();
  while(fin)
  {
    lineno++;
    if(fin.eof())break;
    fin.getline(buff, MAX_LEN_OF_LINE_IN_FILE);
    string sbuff = (string)buff; StripSpaces(sbuff);
    if(0 == sbuff.sz || sbuff.find(dataseparator)==string::npos)continue;
    int tabindex = sbuff.find(dataseparator);
    string user = sbuff.substr(0, tabindex); int userid = s2i(user); sbuff = sbuff.substr(tabindex+dataseparator.sz);
    tabindex = sbuff.find(dataseparator); int itemid = s2i(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex+dataseparator.sz);

    double common_neighbours = 0, adar = 0, jaccard = 0, cosine_similarity = 0, preferential_attachment = 0;
    double weighted_common_neighbours = 0, weighted_adar = 0, weighted_jaccard = 0, weighted_cosine_similarity = 0, weighted_preferential_attachment = 0;
    
    int destuserid = mappeditem_mappeduser[itemid];
    int sourcedeg = adjacencyList[userid][0], destdeg = adjacencyList[destuserid][0];
    //dbg(sourcedeg); dbge(destdeg);
    preferential_attachment = sqrt(sourcedeg) * sqrt(destdeg);
    double norm1 = edgeNorm[userid], norm2 = edgeNorm[destuserid];
    weighted_preferential_attachment = norm1 * norm2;
    if(sourcedeg > 0 && destdeg > 0)
    {
      for(int i = 1, j = 1; i <= sourcedeg && j <= destdeg;)
      {
        int u = adjacencyList[userid][i], v = adjacencyList[destuserid][j];
        //dbg(i); dbg(u); dbg(j); dbge(v);
        if(u == v)
        {
          common_neighbours++;
          double product_edgeweights = edgeWeight[userid][i] * edgeWeight[destuserid][j];
          weighted_common_neighbours += product_edgeweights;

          double tmp = 1.0 / log( adjacencyList[ adjacencyList[userid][i] ][0] + 2);
          adar += tmp;
          weighted_adar += tmp * product_edgeweights;
          i++; j++;
        }
        else if(u < v)i++;
        else j++;
        
      }

      if(common_neighbours > 0)
      {
        double total_neighbours = sourcedeg + destdeg - common_neighbours;
        double weighted_total_neighbours = norm1 * norm1 + norm2 * norm2 - weighted_common_neighbours;
        jaccard =  common_neighbours / total_neighbours;
        weighted_jaccard = weighted_common_neighbours / weighted_total_neighbours;
        cosine_similarity = common_neighbours / preferential_attachment;
        weighted_cosine_similarity = weighted_common_neighbours / weighted_preferential_attachment;
      }
    } 
    

    fout<<userid<<" "<<itemid;
    fout<<" "<<common_neighbours<<" "<<adar<<" "<<jaccard<<" "<<cosine_similarity<<" "<<preferential_attachment;
    fout<<" "<<weighted_common_neighbours<<" "<<weighted_adar<<" "<<weighted_jaccard<<" "<<weighted_cosine_similarity<<" "<<weighted_preferential_attachment;
    fout<<endl;

    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;

  fout.close();
  fin.close();
}


int main()
{
  //Initialize
  Initialize();
  
  LoadSocialNetwork("/home/arvind/track1/weighteddirgraph_mapped_ataction_sorted.txt");
  LoadMappedItemsToMappedUsers("/home/arvind/track1/mappeditem_mapperuser.txt");
  
  
  AppendGraphFeaturesToDataset("/home/arvind/track1/rec_log_train_sorted_mapped_train.txt", " ", "/home/arvind/track1/feature_atactiongraph_train.txt");
  //AppendGraphFeaturesToDataset("/home/arvind/track1/rec_log_train_sorted_mapped_validation.txt", " ", "/home/arvind/track1/feature_atactiongraph_validation.txt");
  //AppendGraphFeaturesToDataset("/home/arvind/track1/rec_log_test_private_result_sorted_mapped.txt", " ", "/home/arvind/track1/feature_atactiongraph_privatetest.txt");
  //AppendGraphFeaturesToDataset("/home/arvind/track1/rec_log_test_public_result_sorted_mapped.txt", " ", "/home/arvind/track1/feature_atactiongraph_publictest.txt");

  return 0;  
}

