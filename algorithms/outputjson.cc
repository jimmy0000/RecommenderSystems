#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <queue>
#include <sstream>
#include <fstream>
#include <stack>
#include <cmath>
#include <iomanip>
#include <cstring>
using namespace std;
#define LET(_x,_a) typeof(_a) _x(_a)
#define FOR(_i, _a, _n) for(LET(_i, _a); _i < _n; ++_i)
#define REP(_i, _n) FOR(_i, 0, _n)
#define pb push_back
#define sz size()
#define cs c_str()
#define EACH(_it, _v) for(LET(_it, _v.begin()); _it != _v.end(); ++_it)
#define _DEBUG 1
#define _dbg(_x) cout<<#_x<<":"<<(_x)
#define dbg(_x) if(_DEBUG)_dbg(_x), cout<<"\t";
#define dbge(_x) if(_DEBUG)_dbg(_x), cout<<"\n";
#define i2s(_x) ({stringstream sout; sout<<_x; sout.str();})
#define num2s(_x) ({stringstream sout; sout<<_x; sout.str();})
#define s2i(_str) ({stringstream sin(_str); int _x; sin>>_x; _x;})
#define GI ({int _x; scanf("%d", &_x); _x;})
#define COUNT(f, _x) ({int _=0; f if(_x)_+=1;  _;})
#define EXISTS(f, _x) ({int _=0; f if(_x){_=1;break;} _;})
#define ALL(f, _x) (!EXISTS(f,!(_x)))
#define MIN(f, _x) ({int _=INT_MAX; f _<?=(_x); _;})
#define MAX(f, _x) (-MIN(f, -(_x)))
#define CLEAR(_arr) memset(_arr,0,sizeof(_arr))
#define PII pair<int, int>
#define MAXVERTICES 2000000

int maxVertexId = 0;
int *adj[MAXVERTICES];
bool *isEdgeInOriginalGraph[MAXVERTICES];
bool visit[MAXVERTICES];
int vertexCount[MAXVERTICES], edgeCount[MAXVERTICES];
map<int, int> nodeSequencer;
string oneTab = "    ";
string twoTabs = oneTab + oneTab;
string threeTabs = twoTabs + oneTab;

void initialize()
{
  REP(i, MAXVERTICES)
  {
    adj[i] = new int[1]; 
    adj[i][0] = 0;

    isEdgeInOriginalGraph[i] = new bool[1];
    isEdgeInOriginalGraph[i][0] = false;

    vertexCount[i] = edgeCount[i] = 0;
  }
}

void readInputGraph(string inputGraphFileName)
{
  ifstream fin(inputGraphFileName.cs);
  if(!fin.is_open())
  {
    cout<<"Unable to open "<<inputGraphFileName<<endl;
    exit(1);
  }
  
  int prevfollower = -1, follower = -1, followee = -1, isEdgeInOriginalGraphCount = 0;
  vector<int> v;
  vector<bool> isEdgeInOriginalGraphVector;
  int lineno = 0;
  while(fin.good())
  {
    fin>>follower>>followee>>isEdgeInOriginalGraphCount;
    if(prevfollower != follower) {
      adj[prevfollower] = new int[v.sz + 1];
      adj[prevfollower][0] = v.sz;
      REP(i, v.sz)adj[prevfollower][i+1] = v[i];

      isEdgeInOriginalGraph[prevfollower] = new bool[v.sz + 1];
      isEdgeInOriginalGraph[prevfollower][0] = false;
      REP(i, isEdgeInOriginalGraphVector.sz)
        isEdgeInOriginalGraph[prevfollower][i+1] = isEdgeInOriginalGraphVector[i];

      v.clear(); 
      isEdgeInOriginalGraphVector.clear();
    }
    prevfollower = follower;
    v.pb(followee);
    isEdgeInOriginalGraphVector.pb((bool)(isEdgeInOriginalGraphCount > 0));
    if(prevfollower > maxVertexId)maxVertexId = prevfollower; 

    lineno += 1;
    if(lineno % 1000000 == 0)cout<<"Processed Edges:"<<lineno<<endl;
  }

  if(v.sz > 0) {
    adj[prevfollower] = new int[v.sz + 1];
    adj[prevfollower][0] = v.sz + 1;
    REP(i, v.sz)adj[prevfollower][i+1] = v[i]; 
    
    isEdgeInOriginalGraph[prevfollower] = new bool[v.sz + 1];
    isEdgeInOriginalGraph[prevfollower][0] = false;
    REP(i, isEdgeInOriginalGraphVector.sz)
      isEdgeInOriginalGraph[prevfollower][i+1] = isEdgeInOriginalGraphVector[i];
  }

  fin.close();
}

void doBFS(int depth, int start)
{
  CLEAR(visit);
  clock_t start_clock = clock();

  queue< PII > q;
  if(adj[start][0] == 0)return;
  int top = 0, next = 0, dist = 0, numNodes = 0, numEdges = 0;
  q.push(PII(start, 0));
  visit[start] = true;
  while(!q.empty())
  {   
    top = q.front().first; 
    dist = q.front().second;
    q.pop();

    numNodes++;
    if(dist >= depth)continue;
    FOR(j, 1, adj[top][0] + 1)
    {   
      numEdges++;
      next = adj[top][j];
      while(!visit[next])
      {   
        visit[next] = true;
        q.push(PII(next, dist+1));
      }   
    }   
  }

  vertexCount[start] = numNodes;
  edgeCount[start] = numEdges;
  if(start % 100000 == 0)cout<<"Processed:"<<start<<" vertices"<<endl;
  cout<<"Time Taken For BFS:"<<(double)(clock() - start_clock) /(double)CLOCKS_PER_SEC<<" seconds"<<endl;

   
}


void outputJSON(int start,  string outputFileName)
{
  nodeSequencer.clear();
  ofstream fout(outputFileName.cs);
  if(!fout.is_open())
  {
    cout<<"Unable to open "<<outputFileName<<endl;
    exit(1);
  }
  fout<<"{"<<endl; 
  fout<<oneTab<<"\"nodes\": ["<<endl;

  int lineno = 0, prevnode = -1, defaultHighlightIndex = 0;
  REP(vertex, maxVertexId + 1)if(visit[vertex])
  {
    nodeSequencer[vertex] = lineno++;
    if (prevnode == start)defaultHighlightIndex = lineno - 2;
    if (prevnode != -1)
    {
      fout<<twoTabs<<"{"<<endl;
      fout<<threeTabs<<"\"name\": \""<<prevnode<<"\""<<endl;
      fout<<twoTabs<<"},"<<endl;
    }
    prevnode = vertex;
  }
  
  if (prevnode != -1)
  {
    if (prevnode == start)defaultHighlightIndex = lineno - 2;
    fout<<twoTabs<<"{"<<endl;
    fout<<threeTabs<<"\"name\": \""<<prevnode<<"\""<<endl;
    fout<<twoTabs<<"}"<<endl;
  }
  fout<<oneTab<<"],"<<endl;
  fout<<oneTab<<"\"links\": ["<<endl;
  
  int nodesProcessed = 0, prevstart = -1, prevdest = -1;
  EACH(it, nodeSequencer)
  {
    int vertex = it->first;
    int sequencedVertex = it->second;

    FOR(i, 1, adj[vertex][0] + 1)if(visit[adj[vertex][i]] && isEdgeInOriginalGraph[vertex][i])
    {
      int dest = adj[vertex][i];
      int sequencedDest = nodeSequencer[dest];

      if (prevstart != -1 && prevdest != -1)
      {
        fout<<twoTabs<<"{"<<endl;    
        fout<<threeTabs<<"\"source\": "<<prevstart<<","<<endl;
        fout<<threeTabs<<"\"target\": "<<prevdest<<endl;
        fout<<twoTabs<<"},"<<endl;
      }
      prevstart = sequencedVertex;
      prevdest = sequencedDest;
    }
  }
  
  if (prevstart != -1 && prevdest != -1)
  {
    fout<<twoTabs<<"{"<<endl;    
    fout<<threeTabs<<"\"source\": "<<prevstart<<","<<endl;
    fout<<threeTabs<<"\"target\": "<<prevdest<<endl;
    fout<<twoTabs<<"}"<<endl;
  }

  fout<<oneTab<<"],"<<endl;
  fout<<oneTab<<"\"highlightIndex\": "<<defaultHighlightIndex<<endl;
  fout<<"}"<<endl;
  
  fout.close();
}

int main(int argc, char *argv[1])
{
  if (argc < 3)
  {
    cout<<"Usage: outputjson.exe <graphfilepath> <vertexid>"<<endl;
    exit(1);
  }
  initialize();

  int vertexid = atoi(argv[2]);
  string INPUT_GRAPH_FILENAME = string(argv[1]);
  readInputGraph(INPUT_GRAPH_FILENAME);

  int verticesToCheck[1];
  verticesToCheck[0] = vertexid; 
  //1000001, 1675341, 1378857, 497591, 1540390, 62837, 1000008, 514449, 1378857};
  int numVerticesToCheck = sizeof(verticesToCheck) / sizeof(verticesToCheck[0]);
  int maxDepth = 1;

  REP(i, numVerticesToCheck) {
    doBFS(maxDepth, verticesToCheck[i]);
    outputJSON(verticesToCheck[i], "/home/arvind/track1/output_bfs_depth2_" + i2s(verticesToCheck[i]) + ".json");
  }

  return 0;
}
