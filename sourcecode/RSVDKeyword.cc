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
const int MAX_NUMBER_OF_KEYWORDS = 300000;

map<int, int> keyword2indexmap;
float *userkeywordscores[MAX_NUMBER_OF_USERS];
int *userkeywordids[MAX_NUMBER_OF_USERS];
float *itemkeywordscores[MAX_NUMBER_OF_ITEMS];
int *itemkeywordids[MAX_NUMBER_OF_ITEMS];
float userfeaturematrix[MAX_NUMBER_OF_USERS][MAX_NUMBER_OF_FEATURES];
float keywordfeaturematrix[MAX_NUMBER_OF_KEYWORDS][MAX_NUMBER_OF_FEATURES];
float itemfeaturematrix[MAX_NUMBER_OF_ITEMS][MAX_NUMBER_OF_FEATURES];

int numusers = 0, numitems = 0, numfeatures = 0, numkeywords = 0;
float minKeywordScore = 1e9, maxKeywordScore = -1e9;

void Initialize()
{
  numusers = 0, numitems = 0, numfeatures = 0, numkeywords = 0;
    
  REP(userid, MAX_NUMBER_OF_USERS)
  {
    userkeywordids[userid] = new int[1];
    userkeywordids[userid][0] = 0;
  }
  
  REP(itemid, MAX_NUMBER_OF_ITEMS)
  {
    itemkeywordids[itemid] = new int[1];
    itemkeywordids[itemid][0] = 0;
  }
}

void StripSpaces(string &str)
{
  while(str.sz && spaces.find(str[0]) != string::npos)str=str.substr(1);
  while(str.sz && spaces.find(str[str.sz-1]) != string::npos)str=str.substr(0, str.sz-1);
}

void LoadUserKeywordMapping(string datafile)
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
    int userid = -1; string keywordscores = "";
    fin>>userid>>keywordscores;
    if(userid == -1 || keywordscores == "" || keywordscores == "0")continue;
    vector< PID > v;
    string str = "";
    int len = keywordscores.sz;
    int curNumberOfkeywords = 0;
    REP(i, len)
    {
      if(keywordscores[i] == ';')
      {
        if(str.sz > 0)
        {
          int index = str.find(':');
          int keywordid = s2i(str.substr(0, index));
          if(!keyword2indexmap.count(keywordid))keyword2indexmap[keywordid] = numkeywords++;
          keywordid = keyword2indexmap[keywordid];
          
          float score = s2d(str.substr(index + 1));
          if(score < minKeywordScore)minKeywordScore = score;
          if(score > maxKeywordScore)maxKeywordScore = score;
          v.pb(PID(keywordid, score));
          curNumberOfkeywords++;
        }
        str = "";
      }
      else str += keywordscores[i];
    }
    if(str.sz > 0)
    {
      int index = str.find(':');
      int keywordid = s2i(str.substr(0, index));
      if(!keyword2indexmap.count(keywordid))keyword2indexmap[keywordid] = numkeywords++;
      keywordid = keyword2indexmap[keywordid];
          
      float score = s2d(str.substr(index + 1));
      if(score < minKeywordScore)minKeywordScore = score;
      if(score > maxKeywordScore)maxKeywordScore = score;
      v.pb(PID(keywordid, score));
      curNumberOfkeywords++;
    }
    
    sort(v.begin(), v.end());
    
    userkeywordids[userid] = new int[curNumberOfkeywords + 1];
    userkeywordscores[userid] = new float[curNumberOfkeywords + 1];
    userkeywordids[userid][0] = curNumberOfkeywords;
    userkeywordscores[userid][0] = (float)curNumberOfkeywords;
    REP(i,  curNumberOfkeywords)userkeywordids[userid][i + 1] = v[i].first, userkeywordscores[userid][i + 1] = v[i].second;
    
    if(userid > numusers)numusers = userid;
    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
    if(lineno < 5)
    {
      dbg(lineno); dbg(userid); dbg(curNumberOfkeywords); REP(i, min(5, curNumberOfkeywords))cout<<" "<<userkeywordids[userid][i + 1]<<","<<userkeywordscores[userid][i + 1];
      cout<<endl;
    }
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  numusers++;
  dbg(numusers); dbg(numkeywords); dbg(minKeywordScore); dbge(maxKeywordScore);
  fin.close();
}

void LoadItemKeywordMapping(string datafile)
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
    string keywordscores = "";
    fin>>itemid>>keywordscores; 
    //Looking at the dataset, it seems that a particular keyword can occur multiple times for the same item.
    //But, it does not impact me, because i am working with a low rank approximation and these duplicate entries will anyways be updated in the update equations
    if(itemid == -1 || keywordscores == "" || keywordscores == "0")continue;
    vector< PID > v;
    string str = "";
    int len = keywordscores.sz;
    int curNumberOfkeywords = 0;
    REP(i, len)
    {
      if(keywordscores[i] == ';')
      {
        if(str.sz > 0)
        {
          int index = str.find(':');
          int keywordid = s2i(str.substr(0, index));
          if(!keyword2indexmap.count(keywordid))keyword2indexmap[keywordid] = numkeywords++;
          keywordid = keyword2indexmap[keywordid];
          
          float score = s2d(str.substr(index + 1));
          if(score < minKeywordScore)minKeywordScore = score;
          if(score > maxKeywordScore)maxKeywordScore = score;
          v.pb(PID(keywordid, score));
          curNumberOfkeywords++;
        }
        str = "";
      }
      else str += keywordscores[i];
    }
    if(str.sz > 0)
    {
      int index = str.find(':');
      int keywordid = s2i(str.substr(0, index));
      if(!keyword2indexmap.count(keywordid))keyword2indexmap[keywordid] = numkeywords++;
      keywordid = keyword2indexmap[keywordid];
          
      float score = s2d(str.substr(index + 1));
      if(score < minKeywordScore)minKeywordScore = score;
      if(score > maxKeywordScore)maxKeywordScore = score;
      v.pb(PID(keywordid, score));
      curNumberOfkeywords++;
    }
    
    sort(v.begin(), v.end());
    
    itemkeywordids[itemid] = new int[curNumberOfkeywords + 1];
    itemkeywordscores[itemid] = new float[curNumberOfkeywords + 1];
    itemkeywordids[itemid][0] = curNumberOfkeywords;
    itemkeywordscores[itemid][0] = (float)curNumberOfkeywords;
    REP(i,  curNumberOfkeywords)itemkeywordids[itemid][i + 1] = v[i].first, itemkeywordscores[itemid][i + 1] = v[i].second;
    
    if(itemid > numitems)numitems = itemid;
    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
    if(lineno < 5)
    {
      dbg(lineno); dbg(itemid); dbg(curNumberOfkeywords); REP(i, min(5, curNumberOfkeywords))cout<<" "<<itemkeywordids[itemid][i + 1]<<","<<itemkeywordscores[itemid][i + 1];
      cout<<endl;
    }
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  numitems++;
  dbg(numitems); dbg(numkeywords); dbg(minKeywordScore); dbge(maxKeywordScore);
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

void AppendKeywordSimilaritiesToDataset(string inputfile, string dataseparator, string outputfile)
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
    REP(keywordid, numkeywords)keywordfeaturematrix[keywordid][featureid] = ((float)rand() / (float)RAND_MAX);
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
      int curNumberOfkeywords = userkeywordids[userid][0];
      if(curNumberOfkeywords == 0)continue;
      
      FOR(keywordIndex, 1, curNumberOfkeywords)
      {
        numLinesProcessed++;
        int keywordid = userkeywordids[userid][keywordIndex];
        float score = userkeywordscores[userid][keywordIndex];
        
        estimatedRating = 0.0f;
        REP(featureid, numfeatures)estimatedRating += userfeaturematrix[userid][featureid] * keywordfeaturematrix[keywordid][featureid];
    
        //Clip Ratings
        //if(estimatedRating > maxKeywordScore)estimatedRating = maxKeywordScore;
        //else if(estimatedRating < minKeywordScore)estimatedRating = minKeywordScore;

        rmse += (score - estimatedRating) * (score - estimatedRating);
        currentError = (score - estimatedRating) * stepsize; 
        REP(featureid, numfeatures)
        {
          tmpuf = userfeaturematrix[userid][featureid];
          tmpkf = keywordfeaturematrix[keywordid][featureid];
          userfeaturematrix[userid][featureid] += currentError * tmpkf - regularizationterm * tmpuf;
          keywordfeaturematrix[keywordid][featureid] += currentError * tmpuf - regularizationterm * tmpkf;
        }      
      }
    }
    cout<<"Iteration:"<<iteration<<" RMSE:"<<sqrt(rmse / numLinesProcessed)<<" Time taken for training users: "<<(float)(clock() - start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
    
    REP(itemid, numitems)
    {
      int curNumberOfkeywords = itemkeywordids[itemid][0];
      if(curNumberOfkeywords == 0)continue;
      
      FOR(keywordIndex, 1, curNumberOfkeywords)
      {
        numLinesProcessed++;
        int keywordid = itemkeywordids[itemid][keywordIndex];
        float score = itemkeywordscores[itemid][keywordIndex];
        
        estimatedRating = 0.0f;
        REP(featureid, numfeatures)estimatedRating += itemfeaturematrix[itemid][featureid] * keywordfeaturematrix[keywordid][featureid];
    
        //Clip Ratings
        //if(estimatedRating > maxKeywordScore)estimatedRating = maxKeywordScore;
        //else if(estimatedRating < minKeywordScore)estimatedRating = minKeywordScore;

        rmse += (score - estimatedRating) * (score - estimatedRating);
        currentError = (score - estimatedRating) * stepsize; 
        REP(featureid, numfeatures)
        {
          tmpif = itemfeaturematrix[itemid][featureid];
          tmpkf = keywordfeaturematrix[keywordid][featureid];
          itemfeaturematrix[itemid][featureid] += currentError * tmpkf - regularizationterm * tmpif;
          keywordfeaturematrix[keywordid][featureid] += currentError * tmpif - regularizationterm * tmpkf;
        }      
      }
    }

    rmse = sqrt(rmse / numLinesProcessed);
    cout<<"Iteration:"<<iteration<<" rmse:"<<rmse<<" Time taken for training items: "<<(float)(clock() - start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
    
    REP(i, 5)
    {
      REP(featureid, 5)
      {
        cout<<userfeaturematrix[i][featureid]<<","<<itemfeaturematrix[i][featureid]<<","<<keywordfeaturematrix[i][featureid]<<" ";
      }
      cout<<endl;
    }
  }
}


int main()
{
  //Initialize
  Initialize();
  
  LoadUserKeywordMapping("/home/arvind/track1/user_keyword_sorted_mapped.txt");
  LoadItemKeywordMapping("/home/arvind/track1/item_keyword_sorted_mapped_deduped.txt");
  
  DoRegularizedSVD(0.1f, 100);
  

  AppendKeywordSimilaritiesToDataset("/home/arvind/track1/rec_log_train_sorted_mapped_train.txt", " ", "/home/arvind/track1/feature_rsvdkeywordsimilarity_train.txt");
  AppendKeywordSimilaritiesToDataset("/home/arvind/track1/rec_log_train_sorted_mapped_validation.txt", " ", "/home/arvind/track1/feature_rsvdkeywordsimilarity_validation.txt");
  AppendKeywordSimilaritiesToDataset("/home/arvind/track1/rec_log_test_private_result_sorted_mapped.txt", " ", "/home/arvind/track1/feature_rsvdkeywordsimilarity_privatetest.txt");
  AppendKeywordSimilaritiesToDataset("/home/arvind/track1/rec_log_test_public_result_sorted_mapped.txt", " ", "/home/arvind/track1/feature_rsvdkeywordsimilarity_publictest.txt");

  return 0;  
}

