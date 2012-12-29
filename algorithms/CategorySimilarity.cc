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
const int MAX_NUMBER_OF_LEVELS = 4;

map<string, int> category2indexmap[MAX_NUMBER_OF_LEVELS];
map<int, double> globalcategorypopularity[MAX_NUMBER_OF_LEVELS];
double *usercategoryscores[MAX_NUMBER_OF_USERS][MAX_NUMBER_OF_LEVELS];
double *userweightedcategoryscores[MAX_NUMBER_OF_USERS][MAX_NUMBER_OF_LEVELS];
double *userinverseweightedcategoryscores[MAX_NUMBER_OF_USERS][MAX_NUMBER_OF_LEVELS];
int *usercategoryids[MAX_NUMBER_OF_USERS][MAX_NUMBER_OF_LEVELS];
int itemcategoryids[MAX_NUMBER_OF_ITEMS][MAX_NUMBER_OF_LEVELS];
int numcategories[MAX_NUMBER_OF_LEVELS];

int numusers = 0, numitems = 0;

void Initialize()
{
  numusers = 0, numitems = 0;

  REP(level, MAX_NUMBER_OF_LEVELS)
  {
    category2indexmap[level] = map<string, int>();
    category2indexmap[level].clear();

    globalcategorypopularity[level] = map<int, double>();
    globalcategorypopularity[level].clear();

    numcategories[level] = 0;
  }
    
  REP(userid, MAX_NUMBER_OF_USERS)REP(level, MAX_NUMBER_OF_LEVELS)
  {
    usercategoryids[userid][level] = new int[1];
    usercategoryids[userid][level][0] = 0;
    usercategoryscores[userid][level] = new double[1];
    usercategoryscores[userid][level][0] = 0;
    userweightedcategoryscores[userid][level] = new double[1];
    userweightedcategoryscores[userid][level][0] = 0;
    userinverseweightedcategoryscores[userid][level] = new double[1];
    userinverseweightedcategoryscores[userid][level][0] = 0;
  }
  REP(itemid, MAX_NUMBER_OF_ITEMS)
  {
    REP(level, MAX_NUMBER_OF_LEVELS)itemcategoryids[itemid][level] = 0;
  }
}

void StripSpaces(string &str)
{
  while(str.sz && spaces.find(str[0]) != string::npos)str=str.substr(1);
  while(str.sz && spaces.find(str[str.sz-1]) != string::npos)str=str.substr(0, str.sz-1);
}

void LoadUserCategoryMapping(string datafile)
{
  //This procedure took 2 mins to load and 300 MB of memory
  ifstream fin(datafile.cs);
  if(!fin){cout<<"Could not open file:"<<datafile<<endl; exit(0);}
  int lineno = -1;
  char buff[MAX_LEN_OF_LINE_IN_FILE];
 
  clock_t start = clock();
  while(fin)
  {
    lineno++;
    if(fin.eof())break;
    int userid = -1; string categoryscores = "";
    fin>>userid>>categoryscores;
    if(userid == -1 || categoryscores == "" || categoryscores == "0")continue;
    vector< PID > v[MAX_NUMBER_OF_LEVELS];
    REP(level, MAX_NUMBER_OF_LEVELS)v[level] = vector< PID >();

    string str = "";
    int len = categoryscores.sz;
    REP(i, len)
    {
      if(categoryscores[i] == ';')
      {
        if(str.sz > 0)
        {
          int index = str.find(':');
          string category = str.substr(0, index);
          double score = s2d(str.substr(index + 1));
          int level = 0;
          string s = "";
          int lencategory = category.sz;
          REP(c, lencategory)
          {
            if(category[c] == '.' && s.sz > 0)
            {
              if(!category2indexmap[level].count(s))category2indexmap[level][s] = numcategories[level]++;
              int categoryid = category2indexmap[level][s];

              v[level].pb( PID(categoryid, score) );
              if(!globalcategorypopularity[level].count(categoryid))globalcategorypopularity[level][categoryid] = 0.0;
              globalcategorypopularity[level][categoryid] += score;
              level++;
            }
            s += category[c];
          }

          if(s.sz > 0)
          {
            if(!category2indexmap[level].count(s))category2indexmap[level][s] = numcategories[level]++;
            int categoryid = category2indexmap[level][s];
   
            v[level].pb( PID(categoryid, score) );
            if(!globalcategorypopularity[level].count(categoryid))globalcategorypopularity[level][categoryid] = 0.0;
            globalcategorypopularity[level][categoryid] += score;
          }
        }
        str = "";
      }
      else str += categoryscores[i];
    }
   
   
    if(str.sz > 0)
    {
      int index = str.find(':');
      string category = str.substr(0, index);
      double score = s2d(str.substr(index + 1));
      int level = 0;
      string s = "";
      int lencategory = category.sz;
      REP(c, lencategory)
      {
        if(category[c] == '.' && s.sz > 0)
        {
          if(!category2indexmap[level].count(s))category2indexmap[level][s] = numcategories[level]++;
          int categoryid = category2indexmap[level][s];

          v[level].pb( PID(categoryid, score) );
          if(!globalcategorypopularity[level].count(categoryid))globalcategorypopularity[level][categoryid] = 0.0;
          globalcategorypopularity[level][categoryid] += score;
          level++;
        }
        s += category[c];
      }

      if(s.sz > 0)
      {
        if(!category2indexmap[level].count(s))category2indexmap[level][s] = numcategories[level]++;
        int categoryid = category2indexmap[level][s];
   
        v[level].pb( PID(categoryid, score) );
        if(!globalcategorypopularity[level].count(categoryid))globalcategorypopularity[level][categoryid] = 0.0;
        globalcategorypopularity[level][categoryid] += score;
      }
    }
  
    REP(level, MAX_NUMBER_OF_LEVELS)
    if(v[level].sz > 0)
    {
      sort(v[level].begin(), v[level].end());
      vector< PID > tmpv;

      tmpv.pb(v[level][0]);
      FOR(i, 1, v[level].sz)
      if(tmpv.back().first == v[level][i].first)tmpv.back().second += v[level][i].second;
      else tmpv.pb(v[level][i]);

      int curNumberOfCategories = tmpv.sz;
      usercategoryids[userid][level] = new int[curNumberOfCategories + 1];
      usercategoryscores[userid][level] = new double[curNumberOfCategories + 1];
      userweightedcategoryscores[userid][level] = new double[curNumberOfCategories + 1];
      userinverseweightedcategoryscores[userid][level] = new double[curNumberOfCategories + 1];
      usercategoryids[userid][level][0] = curNumberOfCategories;
      usercategoryscores[userid][level][0] = userweightedcategoryscores[userid][level][0] = userinverseweightedcategoryscores[userid][level][0] = (double)curNumberOfCategories;
      REP(i, curNumberOfCategories)
      {
        usercategoryids[userid][level][i + 1] = tmpv[i].first;
        usercategoryscores[userid][level][i + 1] = userweightedcategoryscores[userid][level][i + 1] = userinverseweightedcategoryscores[userid][level][i + 1] = tmpv[i].second;
      }
    }
 
    if(userid > numusers)numusers = userid;
    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
    if(lineno < 5)
    {
      dbg(lineno); dbg(userid);
      REP(level, MAX_NUMBER_OF_LEVELS)
      if(usercategoryids[userid][level][0] > 0)
      {
      cout<<"\t";
      REP(i, min(5, usercategoryids[userid][level][0]))
        cout<<" "<<level<<","<<usercategoryids[userid][level][i + 1]<<","<<usercategoryscores[userid][level][i+1];
      }
      cout<<endl;
    }
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  numusers++;
  dbg(numusers);
  fin.close();

  start = clock();
  REP(userid, numusers)
  {
    REP(level, MAX_NUMBER_OF_LEVELS)
    {
      double sum = 0.0, weightedsum = 0.0, inverseweightedsum = 0.0;
      int curNumberOfCategories = usercategoryids[userid][level][0];
      if (curNumberOfCategories == 0)continue;
      REP(categoryIndex, curNumberOfCategories)
      {
        int categoryid = usercategoryids[userid][level][categoryIndex + 1];
        double score = usercategoryscores[userid][level][categoryIndex + 1];
        userweightedcategoryscores[userid][level][categoryIndex + 1] = score * globalcategorypopularity[level][categoryid];
        userinverseweightedcategoryscores[userid][level][categoryIndex + 1] = score /(double)globalcategorypopularity[level][categoryid];

        sum += usercategoryscores[userid][level][categoryIndex + 1];
        weightedsum += userweightedcategoryscores[userid][level][categoryIndex + 1];
        inverseweightedsum += userinverseweightedcategoryscores[userid][level][categoryIndex + 1];
      }

      REP(categoryIndex, curNumberOfCategories)
      {
        usercategoryscores[userid][level][categoryIndex + 1] /= sum;
        userweightedcategoryscores[userid][level][categoryIndex + 1] /= weightedsum;
        userinverseweightedcategoryscores[userid][level][categoryIndex + 1] /= inverseweightedsum;
      }
    }
    
    if(userid < 5)
    {
      dbge(userid);
      REP(level, MAX_NUMBER_OF_LEVELS)
      if(usercategoryids[userid][level][0] > 0)
      {
      cout<<"\t";
      REP(i, min(5, usercategoryids[userid][level][0]))
        cout<<" "<<level<<","<<usercategoryids[userid][level][i+1]<<","<<usercategoryscores[userid][level][i+1]<<","<<userweightedcategoryscores[userid][level][i+1]<<","<<userinverseweightedcategoryscores[userid][level][i+1];
      }
      cout<<endl;
    }
    if((userid+1) % 1000000 == 0)cout<<"Time taken to load "<< (userid+1) <<" users is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  }

  cout<<"Time taken to postprocess "<< numusers <<" number of users is: "<<(double)(clock() - start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
}

void LoadItemCategoryMapping(string datafile)
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
    int itemid = -1; 
    string category = "", keywords = "";
    fin>>itemid>> category >> keywords; 
    if(itemid == -1 || category == "" || category == "0")continue;
    
    string str = "";
    int lencategory = category.sz;
    int level = 0;
    REP(c, lencategory)
    {
      if(category[c] == '.' && str.sz > 0)
      {
        if(!category2indexmap[level].count(str))category2indexmap[level][str] = numcategories[level]++;
        int categoryid = category2indexmap[level][str];
        itemcategoryids[itemid][level] = categoryid;
        level++;
      }
      else str += category[c];
    }
    if(str.sz > 0)
    {
      if(!category2indexmap[level].count(str))category2indexmap[level][str] = numcategories[level]++;
      int categoryid = category2indexmap[level][str];  
      itemcategoryids[itemid][level] = categoryid;
    }
    
    if(itemid > numitems)numitems = itemid;
    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
    if(lineno < 5)
    {
      dbg(lineno); dbg(itemid);
      REP(level, MAX_NUMBER_OF_LEVELS)cout<<" "<<level<<","<<itemcategoryids[itemid][level];
      cout<<endl;
    }
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  numitems++;
  dbg(numitems);
  fin.close();
}

void AppendCategorySimilaritiesToDataset(string inputfile, string dataseparator, string outputfile)
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
    
    double similarity = 0.0, weightedSimilarity = 0.0, inverseWeightedSimilarity = 0.0;
    REP(level, MAX_NUMBER_OF_LEVELS)
    {
      int categoryid = itemcategoryids[itemid][level];
      REP(categoryIndex, usercategoryids[userid][level][0])
      if(usercategoryids[userid][level][categoryIndex + 1] == categoryid)
      {
        similarity = usercategoryscores[userid][level][categoryIndex + 1];
        weightedSimilarity = userweightedcategoryscores[userid][level][categoryIndex + 1];
        inverseWeightedSimilarity = userinverseweightedcategoryscores[userid][level][categoryIndex + 1];

        break;
      }
    }

    fout<<userid<<" "<<itemid<<" "<<similarity<<" "<<weightedSimilarity<<" "<<inverseWeightedSimilarity<<endl;

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
  
  LoadUserCategoryMapping("/home/arvind/track1/useritem_categories_follow_mapped_sorted.txt");
  LoadItemCategoryMapping("/home/arvind/track1/item_mapped_sorted.txt");
  
  AppendCategorySimilaritiesToDataset("/home/arvind/track1/rec_log_train_sorted_mapped_train.txt", " ", "/home/arvind/track1/feature_categorysimilarity_train.txt");
  AppendCategorySimilaritiesToDataset("/home/arvind/track1/rec_log_train_sorted_mapped_validation.txt", " ", "/home/arvind/track1/feature_categorysimilarity_validation.txt");
  AppendCategorySimilaritiesToDataset("/home/arvind/track1/rec_log_test_private_result_sorted_mapped.txt", " ", "/home/arvind/track1/feature_categorysimilarity_privatetest.txt");
  AppendCategorySimilaritiesToDataset("/home/arvind/track1/rec_log_test_public_result_sorted_mapped.txt", " ", "/home/arvind/track1/feature_categorysimilarity_publictest.txt");

  return 0;  
}

