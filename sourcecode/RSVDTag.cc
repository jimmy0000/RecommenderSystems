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
#define FOR(_i, _a, _n) for(LET(_i, _a); _i != _n; ++_i)
#define REP(_i, _n) FOR(_i, 0, _n)
#define pb push_back
#define sz size()
#define cs c_str()
#define EACH(_it, _v) FOR(_it, _v.begin(), _v.end())
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
#define EPS (float)1e-6
typedef long long LL;
string spaces = " \t\r\n";
const int MAX_NUMBER_OF_USERS = 2400000, MAX_NUMBER_OF_ITEMS = 6500, MAX_LEN_OF_LINE_IN_FILE = 10000;
const int MAX_REGULARIZEDSVD_ITERATIONS = 100, MAX_NUMBER_OF_FEATURES = 100;
const int MAX_NUMBER_OF_TAGS = 300000;

map<int, int> tag2indexmap;
float *usertagscores[MAX_NUMBER_OF_USERS];
int *usertagids[MAX_NUMBER_OF_USERS];
float *itemtagscores[MAX_NUMBER_OF_ITEMS];
int *itemtagids[MAX_NUMBER_OF_ITEMS];
float userfeaturematrix[MAX_NUMBER_OF_USERS][MAX_NUMBER_OF_FEATURES];
float tagfeaturematrix[MAX_NUMBER_OF_TAGS][MAX_NUMBER_OF_FEATURES];
float itemfeaturematrix[MAX_NUMBER_OF_ITEMS][MAX_NUMBER_OF_FEATURES];

int numusers = 0, numitems = 0, numfeatures = 0, numtags = 0;

void Initialize()
{
  numusers = 0, numitems = 0, numfeatures = 0, numtags = 0;
    
  REP(userid, MAX_NUMBER_OF_USERS)
  {
    usertagids[userid] = new int[1];
    usertagids[userid][0] = 0;
  }
  
  REP(itemid, MAX_NUMBER_OF_ITEMS)
  {
    itemtagids[itemid] = new int[1];
    itemtagids[itemid][0] = 0;
  }
}

void StripSpaces(string &str)
{
  while(str.sz && spaces.find(str[0]) != string::npos)str=str.substr(1);
  while(str.sz && spaces.find(str[str.sz-1]) != string::npos)str=str.substr(0, str.sz-1);
}

void LoadUserTagMapping(string datafile)
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
    int userid = -1; string tagscores = "";
    fin>>userid>>tagscores;
    if(userid == -1 || tagscores == "" || tagscores == "0")continue;
    vector< PID > v;
    string str = "";
    int len = tagscores.sz;
    int curNumberOftags = 0;
    REP(i, len)
    {
      if(tagscores[i] == ';')
      {
        if(str.sz > 0)
        {
          int tagid = s2i(str);
          if(!tag2indexmap.count(tagid))tag2indexmap[tagid] = numtags++;
          tagid = tag2indexmap[tagid];
          
          v.pb(PID(tagid, 1.0f));
          curNumberOftags++;
        }
        str = "";
      }
      else str += tagscores[i];
    }
    if(str.sz > 0)
    {
      int tagid = s2i(str);
      if(!tag2indexmap.count(tagid))tag2indexmap[tagid] = numtags++;
      tagid = tag2indexmap[tagid];
          
      v.pb(PID(tagid, 1.0f));
      curNumberOftags++;
    }
    
    sort(v.begin(), v.end());
    
    usertagids[userid] = new int[curNumberOftags + 1];
    usertagscores[userid] = new float[curNumberOftags + 1];
    usertagids[userid][0] = curNumberOftags;
    usertagscores[userid][0] = (float)curNumberOftags;
    REP(i,  curNumberOftags)usertagids[userid][i + 1] = v[i].first, usertagscores[userid][i + 1] = v[i].second;
    
    if(userid > numusers)numusers = userid;
    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
    if(lineno < 5)
    {
      dbg(lineno); dbg(userid); dbg(curNumberOftags); REP(i, min(5, curNumberOftags))cout<<" "<<usertagids[userid][i + 1]<<","<<usertagscores[userid][i + 1];
      cout<<endl;
    }
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  numusers++;
  dbg(numusers); dbge(numtags);
  fin.close();
}

void LoadItemTagMapping(string datafile)
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
    string tagscores = "";
    fin>>itemid>>tagscores; 
    //Looking at the dataset, it seems that a particular tag can occur multiple times for the same item.
    //But, it does not impact me, because i am working with a low rank approximation and these duplicate entries will anyways be updated in the update equations
    if(itemid == -1 || tagscores == "" || tagscores == "0")continue;
    vector< PID > v;
    string str = "";
    int len = tagscores.sz;
    int curNumberOftags = 0;
    REP(i, len)
    {
      if(tagscores[i] == ';')
      {
        if(str.sz > 0)
        {
          int tagid = s2i(str);
          if(!tag2indexmap.count(tagid))tag2indexmap[tagid] = numtags++;
          tagid = tag2indexmap[tagid];
          
          v.pb(PID(tagid, 1.0f));
          curNumberOftags++;
        }
        str = "";
      }
      else str += tagscores[i];
    }
    if(str.sz > 0)
    {
      int tagid = s2i(str);
      if(!tag2indexmap.count(tagid))tag2indexmap[tagid] = numtags++;
      tagid = tag2indexmap[tagid];
          
      v.pb(PID(tagid, 1.0f));
      curNumberOftags++;
    }
    
    sort(v.begin(), v.end());
    
    itemtagids[itemid] = new int[curNumberOftags + 1];
    itemtagscores[itemid] = new float[curNumberOftags + 1];
    itemtagids[itemid][0] = curNumberOftags;
    itemtagscores[itemid][0] = (float)curNumberOftags;
    REP(i,  curNumberOftags)itemtagids[itemid][i + 1] = v[i].first, itemtagscores[itemid][i + 1] = v[i].second;
    
    if(itemid > numitems)numitems = itemid;
    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
    if(lineno < 5)
    {
      dbg(lineno); dbg(itemid); dbg(curNumberOftags); REP(i, min(5, curNumberOftags))cout<<" "<<itemtagids[itemid][i + 1]<<","<<itemtagscores[itemid][i + 1];
      cout<<endl;
    }
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  numitems++;
  dbg(numitems); dbg(numtags);
  fin.close();
}

float Similarity_Pearson(int userid, int itemid)
{
  float sum1 = 0, sum2 = 0, sum1sq = 0, sum2sq = 0, psum = 0, norm1 = 0, norm2 = 0;
  int overlaps = numfeatures;
  
  REP(featureid, numfeatures)
  {
    sum1 += userfeaturematrix[userid][featureid];
    sum2 += itemfeaturematrix[itemid][featureid];
    sum1sq += userfeaturematrix[userid][featureid] * userfeaturematrix[userid][featureid];
    sum2sq += itemfeaturematrix[itemid][featureid] * itemfeaturematrix[itemid][featureid];
    psum += userfeaturematrix[userid][featureid] * itemfeaturematrix[itemid][featureid];
  }
  
  float correlation = -1, prod1 = 0.0f, prod2 = 0.0f;
  if(overlaps == 0)correlation = 0.0f;
  else
  {
    prod1 = (sum1sq - (sum1*sum1)/overlaps), prod2 = (sum2sq - (sum2*sum2)/overlaps);
    float num = psum - (sum1*sum2)/overlaps;
    if(prod1 * prod2 < 0)correlation = 0.0f;
    else
    {
      float denom = sqrt(prod1 * prod2);
      if(denom < EPS)correlation = 0.0f;
      else correlation = num/denom;
    }
  }
  
  return correlation;
}

float Similarity_Cosine(int userid, int itemid)
{
  float num = 0.0f,  norm1 = 0.0f, norm2 = 0.0f;
  REP(featureid, numfeatures)
  {
    norm1 += userfeaturematrix[userid][featureid] * userfeaturematrix[userid][featureid];
    norm2 += itemfeaturematrix[itemid][featureid] * itemfeaturematrix[itemid][featureid];
    num += userfeaturematrix[userid][featureid] * itemfeaturematrix[itemid][featureid];
  }
  
  norm1 = sqrt(norm1); norm2 = sqrt(norm2);
  
  float prod = norm1 * norm2;
  if(prod < EPS && prod > -1 * EPS)return 0.0f;
  return num / prod;
}

void AppendTagSimilaritiesToDataset(string inputfile, string dataseparator, string outputfile)
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
    
    float pearsonSimilarity = Similarity_Pearson(userid, itemid);
    float cosineSimilarity = Similarity_Cosine(userid, itemid);
    fout<<userid<<" "<<itemid<<" "<<pearsonSimilarity<<" "<<cosineSimilarity<<endl;

    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  fout.close();
  fin.close();
}

void DoRegularizedSVD(float regularizationConstant, int numLatentFactors)
{
  if(numLatentFactors > MAX_NUMBER_OF_FEATURES)
  {
    cout<<"NumLatentFactors:"<<numLatentFactors<<" is greater than MAX_NUMBER_OF_FEATURES:"<<MAX_NUMBER_OF_FEATURES<<endl;
    cout<<"Setting NumLatentFactors to MAX_NUMBER_OF_FEATURES:"<<MAX_NUMBER_OF_FEATURES<<endl;
    numLatentFactors = MAX_NUMBER_OF_FEATURES;
  }
  
  cout<<"#NumLatentFactors:"<<numLatentFactors<<" RegularizationConstant:"<<regularizationConstant<<endl;
  cout<<"#Iteration RMSEerror TimeTakenInSeconds"<<endl;
  numfeatures = numLatentFactors;
 
  srand(time(NULL));
  REP(featureid, numfeatures)
  {
    REP(userid, numusers)userfeaturematrix[userid][featureid] = ((float)rand() / (float)RAND_MAX);
    REP(itemid, numitems)itemfeaturematrix[itemid][featureid] = ((float)rand() / (float)RAND_MAX);
    REP(tagid, numtags)tagfeaturematrix[tagid][featureid] = ((float)rand() / (float)RAND_MAX);
  }
  
  float estimatedRating = 0.0, currentError = 0.0, regularizationterm = 0.0, stepsize = 0.0, tmpuf = 0.0, tmpif = 0.0, tmpkf = 0.0;
  REP(iteration, MAX_REGULARIZEDSVD_ITERATIONS)
  {   
    float rmse = 0.0;
    int numLinesProcessed = 0; 
    clock_t start = clock();
    stepsize = min(0.001f, 1 / (float)(iteration + 1)); //adaptive step size = 1/iteration
    regularizationterm = regularizationConstant * stepsize;
    REP(userid, numusers)
    {
      int curNumberOftags = usertagids[userid][0];
      if(curNumberOftags == 0)continue;
      
      FOR(tagIndex, 1, curNumberOftags)
      {
        numLinesProcessed++;
        int tagid = usertagids[userid][tagIndex];
        float score = usertagscores[userid][tagIndex];
        
        estimatedRating = 0.0f;
        REP(featureid, numfeatures)estimatedRating += userfeaturematrix[userid][featureid] * tagfeaturematrix[tagid][featureid];
    
        //Clip Ratings
        //if(estimatedRating > maxKeywordScore)estimatedRating = maxKeywordScore;
        //else if(estimatedRating < minKeywordScore)estimatedRating = minKeywordScore;

        rmse += (score - estimatedRating) * (score - estimatedRating);
        currentError = (score - estimatedRating) * stepsize; 
        REP(featureid, numfeatures)
        {
          tmpuf = userfeaturematrix[userid][featureid];
          tmpkf = tagfeaturematrix[tagid][featureid];
          userfeaturematrix[userid][featureid] += currentError * tmpkf - regularizationterm * tmpuf;
          tagfeaturematrix[tagid][featureid] += currentError * tmpuf - regularizationterm * tmpkf;
        }      
      }
    }
    cout<<"Iteration:"<<iteration<<" RMSE:"<<sqrt(rmse / numLinesProcessed)<<" Time taken for training users: "<<(float)(clock() - start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
    
    REP(itemid, numitems)
    {
      int curNumberOftags = itemtagids[itemid][0];
      if(curNumberOftags == 0)continue;
      
      FOR(tagIndex, 1, curNumberOftags)
      {
        numLinesProcessed++;
        int tagid = itemtagids[itemid][tagIndex];
        float score = itemtagscores[itemid][tagIndex];
        
        estimatedRating = 0.0f;
        REP(featureid, numfeatures)estimatedRating += itemfeaturematrix[itemid][featureid] * tagfeaturematrix[tagid][featureid];
    
        //Clip Ratings
        //if(estimatedRating > maxKeywordScore)estimatedRating = maxKeywordScore;
        //else if(estimatedRating < minKeywordScore)estimatedRating = minKeywordScore;

        rmse += (score - estimatedRating) * (score - estimatedRating);
        currentError = (score - estimatedRating) * stepsize; 
        REP(featureid, numfeatures)
        {
          tmpif = itemfeaturematrix[itemid][featureid];
          tmpkf = tagfeaturematrix[tagid][featureid];
          itemfeaturematrix[itemid][featureid] += currentError * tmpkf - regularizationterm * tmpif;
          tagfeaturematrix[tagid][featureid] += currentError * tmpif - regularizationterm * tmpkf;
        }      
      }
    }

    rmse = sqrt(rmse / numLinesProcessed);
    cout<<"Iteration:"<<iteration<<" rmse:"<<rmse<<" Time taken for training items: "<<(float)(clock() - start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
    
    REP(i, 5)
    {
      REP(featureid, 5)
      {
        cout<<userfeaturematrix[i][featureid]<<","<<itemfeaturematrix[i][featureid]<<","<<tagfeaturematrix[i][featureid]<<" ";
      }
      cout<<endl;
    }
  }
}


int main()
{
  //Initialize
  Initialize();
  
  LoadUserTagMapping("/home/arvind/track1/user_tag_mapped_sorted.txt");
  LoadItemTagMapping("/home/arvind/track1/item_tag_mapped_sorted.txt");
  
  DoRegularizedSVD(0.0001f, 100);
  
  AppendTagSimilaritiesToDataset("/home/arvind/track1/rec_log_train_sorted_mapped_train.txt", " ", "/home/arvind/track1/feature_rsvdtagsimilarity_train.txt");
  AppendTagSimilaritiesToDataset("/home/arvind/track1/rec_log_train_sorted_mapped_validation.txt", " ", "/home/arvind/track1/feature_rsvdtagsimilarity_validation.txt");
  AppendTagSimilaritiesToDataset("/home/arvind/track1/rec_log_test_private_result_sorted_mapped.txt", " ", "/home/arvind/track1/feature_rsvdtagsimilarity_privatetest.txt");
  AppendTagSimilaritiesToDataset("/home/arvind/track1/rec_log_test_public_result_sorted_mapped.txt", " ", "/home/arvind/track1/feature_rsvdtagsimilarity_publictest.txt");

  return 0;  
}

