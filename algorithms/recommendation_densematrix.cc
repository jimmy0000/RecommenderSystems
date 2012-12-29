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
#define _DEBUG 0
#define _dbg(_x) cout<<#_x<<":"<<(_x)
#define dbg(_x) if(_DEBUG)_dbg(_x), cout<<"\t";
#define dbge(_x) if(_DEBUG)_dbg(_x), cout<<"\n";
#define PII pair<int, int>
#define PSD pair<string, double> 
#define PDS pair<double, string>
#define PDI pair<double, int>
#define PDD pair<double, double>
#define PDDD pair<double, PDD >
#define s2d(_str) ({stringstream _sout(_str); double _d; _sout>>_d; _d;})
#define s2i(_str) ({stringstream sin(_str); int _x; sin>>_x; _x;})
#define num2s(_x) ({stringstream sout; sout<<_x; sout.str();})
#define MAE(trueRatingsVector, estimatedRatingsVector) ({double _mae = 0.0; REP(_i, trueRatingsVector.sz)_mae += abs(trueRatingsVector[_i] - estimatedRatingsVector[_i]); _mae /= trueRatingsVector.sz; _mae;})
#define EPS (double)1e-10
string spaces = " \t\r\n";
const int MAX_NUMBER_OF_USERS = 7000, MAX_NUMBER_OF_ITEMS = 7000, MAX_LEN_OF_LINE_IN_FILE = 10000, MAX_NUMBER_OF_ENTITIES = 7000, MAX_NUMBER_OF_FEATURES = 500;
const int ERROR_TYPE_MAE = 1, ERROR_TYPE_RMSE = 2, MAX_REGULARIZEDSVD_ITERATIONS = 10000;
const double CASE_AMPLIFICATION_FACTOR = 2.5;

map<string, int> usernametoidmap;
map<string, int> itemnametoidmap;
string users[MAX_NUMBER_OF_USERS];
string items[MAX_NUMBER_OF_ITEMS];

double useritemrating[MAX_NUMBER_OF_USERS][MAX_NUMBER_OF_ITEMS]; //7000 * 7000 * 8 = 400 MB
bool isuseritemrating[MAX_NUMBER_OF_USERS][MAX_NUMBER_OF_ITEMS]; //7000 * 7000 * 1 = 50 MB
int useritemratingtimestamp[MAX_NUMBER_OF_USERS][MAX_NUMBER_OF_ITEMS]; //7000 * 7000 * 4 = 200 MB
bool isuseritemtrainrating[MAX_NUMBER_OF_USERS][MAX_NUMBER_OF_ITEMS]; //7000 * 7000 * 1 = 50 MB

double tmpuseritemrating[2][MAX_NUMBER_OF_ENTITIES];
bool tmpisuseritemtrainrating[2][MAX_NUMBER_OF_ENTITIES];
double meanuserrating[MAX_NUMBER_OF_USERS], meanitemrating[MAX_NUMBER_OF_ITEMS], meanuserresidualrating[MAX_NUMBER_OF_USERS], meanitemresidualrating[MAX_NUMBER_OF_ITEMS];
int frequencyofusers[MAX_NUMBER_OF_USERS], frequencyofitems[MAX_NUMBER_OF_ITEMS];
bool entitiesAlreadySeen[MAX_NUMBER_OF_ENTITIES];
double weightedScoreOfEntitiesNotYetSeen[MAX_NUMBER_OF_ENTITIES]; 
double similaritySumOfEntitiesNotYetSeen[MAX_NUMBER_OF_ENTITIES];

double usersimilarityscores[MAX_NUMBER_OF_USERS][MAX_NUMBER_OF_USERS]; //7000 * 7000 * 8 = 400 MB
double itemsimilarityscores[MAX_NUMBER_OF_ITEMS][MAX_NUMBER_OF_ITEMS]; //7000 * 7000 * 8 = 400 MB
short int itemsimilarityitems[MAX_NUMBER_OF_ITEMS][MAX_NUMBER_OF_ITEMS]; //7000 * 7000 * 2 = 100 MB

double itemaveragedifferences[MAX_NUMBER_OF_ITEMS][MAX_NUMBER_OF_ITEMS]; //7000 * 7000 * 8 = 400 MB
short int itemitemcounts[MAX_NUMBER_OF_ITEMS][MAX_NUMBER_OF_ITEMS]; //7000 * 7000 * 2 = 100 MB

double userfeaturematrix[MAX_NUMBER_OF_USERS][MAX_NUMBER_OF_FEATURES]; //7000 * 500 * 8 = 30 MB
double userfeaturenumerator[MAX_NUMBER_OF_USERS][MAX_NUMBER_OF_FEATURES]; //7000 * 500 * 8 = 30 MB
double userfeaturedenominator[MAX_NUMBER_OF_USERS][MAX_NUMBER_OF_FEATURES]; //7000 * 500 * 8 = 30 MB
double featureitemmatrix[MAX_NUMBER_OF_FEATURES][MAX_NUMBER_OF_ITEMS]; //7000 * 500 * 8 = 30 MB
double featureitemnumerator[MAX_NUMBER_OF_FEATURES][MAX_NUMBER_OF_ITEMS]; //7000 * 500 * 8 = 30 MB
double featureitemdenominator[MAX_NUMBER_OF_FEATURES][MAX_NUMBER_OF_ITEMS]; //7000 * 500 * 8 = 30 MB
double featurefeaturematrix[MAX_NUMBER_OF_FEATURES][MAX_NUMBER_OF_FEATURES]; //500 * 500 * 8 = 2 MB

int numusers = 0, numitems = 0, numfeatures = 0;

void Initialize()
{
  numusers = 0, numitems = 0, numfeatures = 0;
  
  if(MAX_NUMBER_OF_ENTITIES < max(MAX_NUMBER_OF_ITEMS, MAX_NUMBER_OF_USERS))
  {
    cout<<"Error: MAX_NUMBER_OF_ENTITIES:"<<MAX_NUMBER_OF_ENTITIES<<" is set to a value lesser than Max number of items or users"<<max(MAX_NUMBER_OF_ITEMS, MAX_NUMBER_OF_USERS)<<endl;
    exit(1);
  }

  usernametoidmap.clear(); itemnametoidmap.clear();
  REP(userid, MAX_NUMBER_OF_USERS)users[userid] = "";
  REP(itemid, MAX_NUMBER_OF_ITEMS)items[itemid] = "";
  memset(useritemrating, 0, sizeof(useritemrating));
  memset(isuseritemrating, false, sizeof(isuseritemrating));
  memset(isuseritemtrainrating, false, sizeof(isuseritemtrainrating));
  memset(itemsimilarityscores, 0, sizeof(itemsimilarityscores));
  memset(itemsimilarityitems, -1, sizeof(itemsimilarityitems));
  memset(meanuserrating, 0, sizeof(meanuserrating));
  memset(meanitemrating, 0, sizeof(meanitemrating));
  memset(tmpuseritemrating, 0, sizeof(tmpuseritemrating));
  memset(tmpisuseritemtrainrating, false, sizeof(tmpisuseritemtrainrating));

  memset(itemaveragedifferences, 0, sizeof(itemaveragedifferences));
  memset(itemitemcounts, 0, sizeof(itemitemcounts));
  
  memset(entitiesAlreadySeen, false, sizeof(entitiesAlreadySeen));
  memset(weightedScoreOfEntitiesNotYetSeen, 0, sizeof(weightedScoreOfEntitiesNotYetSeen));  
  memset(similaritySumOfEntitiesNotYetSeen, 0, sizeof(similaritySumOfEntitiesNotYetSeen));
  
  memset(userfeaturematrix, 0, sizeof(userfeaturematrix));
  memset(userfeaturenumerator, 0, sizeof(userfeaturenumerator));
  memset(userfeaturedenominator, 0, sizeof(userfeaturedenominator));
  memset(featureitemmatrix, 0, sizeof(featureitemmatrix));
  memset(featureitemnumerator, 0, sizeof(featureitemnumerator));
  memset(featureitemdenominator, 0, sizeof(featureitemdenominator));
  memset(featurefeaturematrix, 0, sizeof(featurefeaturematrix));
}

void StripSpaces(string &str)
{
  while(str.sz && spaces.find(str[0]) != string::npos)str=str.substr(1);
  while(str.sz && spaces.find(str[str.sz-1]) != string::npos)str=str.substr(0, str.sz-1);
}

void CreateTrainTestSplits(int trainPercentage, bool isTemporalSplit)
{
  REP(userid, numusers)
  {
    vector< PII > v;
    REP(itemid, numitems)if(isuseritemrating[userid][itemid])v.pb( PII(useritemratingtimestamp[userid][itemid], itemid) );
    if(isTemporalSplit)
    {
      sort(v.begin(), v.end());
      int numberOfTrainItems = (int)(((double)trainPercentage * v.sz) / 100.0);
      REP(i, numberOfTrainItems)isuseritemtrainrating[userid][v[i].second] = true;
    }
  }
}

void ComputeFrequenciesAndMeanRatingsForUsersAndItems()
{
  REP(userid, numusers)
  { 
    int numitemsratedbyuser = 0;
    meanuserrating[userid] = 0.0;
    REP(itemid, numitems)if(isuseritemtrainrating[userid][itemid])meanuserrating[userid] += useritemrating[userid][itemid], numitemsratedbyuser++;
    if(numitemsratedbyuser > 0)meanuserrating[userid] /= numitemsratedbyuser;
    frequencyofusers[userid] = numitemsratedbyuser;
  }
  
  REP(itemid, numitems)
  {
    int numusersratedthisitem = 0;
    meanitemrating[itemid] = 0.0;
    REP(userid, numusers)if(isuseritemtrainrating[userid][itemid])meanitemrating[itemid] += useritemrating[userid][itemid], numusersratedthisitem++;
    if(numusersratedthisitem > 0)meanitemrating[itemid] /= numusersratedthisitem;
    frequencyofitems[itemid] = numusersratedthisitem;
  }
  
  REP(userid, numusers)
  { 
    int numitemsratedbyuser = 0;
    meanuserresidualrating[userid] = 0.0;
    REP(itemid, numitems)if(isuseritemtrainrating[userid][itemid])meanuserresidualrating[userid] += (useritemrating[userid][itemid] - meanitemrating[itemid]), numitemsratedbyuser++;
    if(numitemsratedbyuser > 0)meanuserresidualrating[userid] /= numitemsratedbyuser;
  }

  REP(itemid, numitems)
  {
    int numusersratedthisitem = 0;
    meanitemresidualrating[itemid] = 0.0;
    REP(userid, numusers)if(isuseritemtrainrating[userid][itemid])meanitemresidualrating[itemid] += (useritemrating[userid][itemid] - meanuserrating[userid]), numusersratedthisitem++;
    if(numusersratedthisitem > 0)meanitemresidualrating[itemid] /= numusersratedthisitem;
  }

}


void LoadMovieLensData(string datafile, string itemfile, string dataseparator, string itemseparator)
{
  ifstream moviefin(itemfile.cs);
  if(!moviefin){cout<<"Could not open file:"<<itemfile<<endl; exit(0);}
  char buff[MAX_LEN_OF_LINE_IN_FILE];
  while(moviefin)
  {
    if(moviefin.eof())break;
    moviefin.getline(buff, MAX_LEN_OF_LINE_IN_FILE);
    string sbuff = (string)buff; StripSpaces(sbuff);
    if(0 == sbuff.sz || sbuff.find(itemseparator)==string::npos)continue;
    int tabindex = sbuff.find(itemseparator);
    int movieid = s2i(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex+itemseparator.sz);
    tabindex = sbuff.find(itemseparator); string moviename = sbuff.substr(0, tabindex); 
    
    items[movieid] = moviename;
    itemnametoidmap[moviename] = movieid;
    
    if(movieid > numitems)numitems = movieid;
    
    continue;
  }
  
  moviefin.close();


  ifstream fin(datafile.cs);
  if(!fin){cout<<"Could not open file:"<<datafile<<endl; exit(0);}
  int lineno = -1;
  while(fin)
  {
    lineno++;
    if(fin.eof())break;
    fin.getline(buff, MAX_LEN_OF_LINE_IN_FILE);
    string sbuff = (string)buff; StripSpaces(sbuff);
    if(0 == sbuff.sz || sbuff.find(dataseparator)==string::npos)continue;
    int tabindex = sbuff.find(dataseparator);
    string user = sbuff.substr(0, tabindex); int userid = s2i(user); sbuff = sbuff.substr(tabindex+dataseparator.sz);
    tabindex = sbuff.find(dataseparator); int movieid = s2i(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex+dataseparator.sz);
    if(movieid > numitems){cout<<"Error at Lineno:"<<lineno<<" MovieId:"<<movieid<<" is greater than maximum number of movies:"<<numitems<<endl; exit(1);}
    
    tabindex = sbuff.find(dataseparator); int movierating = s2i(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex+dataseparator.sz);
    tabindex = sbuff.find(dataseparator); int timestamp = s2i(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex + dataseparator.sz);
    
    useritemrating[userid][movieid] = movierating;
    isuseritemrating[userid][movieid] = true;
    useritemratingtimestamp[userid][movieid] = timestamp;
    users[userid] = user;
    usernametoidmap[user] = userid;
    if(userid > numusers)numusers = userid;
    continue;
  }
  
  fin.close();
  
}

void TestMovieLensData()
{
  dbge(numusers);
  REP(userid, 10)
  if(users[userid] != "")
  {
    cout<<"User:"<<users[userid]<<endl;
    REP(itemid, numitems)if(isuseritemrating[userid][itemid]){cout<<items[itemid]<<":"<<useritemrating[userid][itemid]; if(itemid != numitems-1)cout<<"\t";}
    cout<<endl;
  }
}


double Similarity_L2Norm(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional)
{
  double ret = 0;
  if(isUserEntity)
  {
    int useridA = entityidA, useridB = entityidB;
    if(!isLowDimensional)
    {
      REP(itemid, numitems)
      if(isuseritemtrainrating[useridA][itemid] && isuseritemtrainrating[useridB][itemid])    
        ret += (useritemrating[useridA][itemid] - useritemrating[useridB][itemid]) * (useritemrating[useridA][itemid] - useritemrating[useridB][itemid]);
    }
    else REP(featureid, numfeatures)ret += (userfeaturematrix[useridA][featureid] - userfeaturematrix[useridB][featureid]) * (userfeaturematrix[useridA][featureid] - userfeaturematrix[useridB][featureid]);
  }
  else
  {
    int itemidA = entityidA, itemidB = entityidB;
    if(!isLowDimensional)
    {
      REP(userid, numusers)
      if(isuseritemtrainrating[userid][itemidA] && isuseritemtrainrating[userid][itemidB])    
        ret += (useritemrating[userid][itemidA] - useritemrating[userid][itemidB]) * (useritemrating[userid][itemidA] - useritemrating[userid][itemidB]);    
    }
    else REP(featureid, numfeatures)ret += (featureitemmatrix[featureid][itemidA] - featureitemmatrix[featureid][itemidB]) * (featureitemmatrix[featureid][itemidA] - featureitemmatrix[featureid][itemidB]);
  }
  ret = 1/(1+sqrt(ret));
  return ret;
}

double Similarity_L2Norm(string entityA, string entityB, bool isUserEntity, bool isLowDimensional)
{
  int entityidA = -1, entityidB = -1;
  if(isUserEntity)entityidA = usernametoidmap[entityA], entityidB = usernametoidmap[entityB];
  else entityidA = itemnametoidmap[entityA], entityidB = itemnametoidmap[entityB];
  return Similarity_L2Norm(entityidA, entityidB, isUserEntity, isLowDimensional);
}

PDDD ComputeCorrelationAndNormsForPearson(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional)
{
  double sum1 = 0, sum2 = 0, sum1sq = 0, sum2sq = 0, psum = 0, norm1 = 0, norm2 = 0;
  int overlaps = 0;
  if(isUserEntity)
  {
    int useridA = entityidA, useridB = entityidB;
    if(!isLowDimensional)
    {
      REP(itemid, numitems)
      {
        if(isuseritemtrainrating[useridA][itemid] && isuseritemtrainrating[useridB][itemid])
        {
          overlaps++;
          sum1 += useritemrating[useridA][itemid];
          sum2 += useritemrating[useridB][itemid];
          sum1sq += useritemrating[useridA][itemid] * useritemrating[useridA][itemid];
          sum2sq += useritemrating[useridB][itemid] * useritemrating[useridB][itemid];
          psum += useritemrating[useridA][itemid] * useritemrating[useridB][itemid];
        }
        
        //NOTE: sum1sq and sum2sq are only for overlapping entries, whereas norm1 and norm2 are for all entities in each vector
        if(isuseritemtrainrating[useridA][itemid])norm1 += useritemrating[useridA][itemid] * useritemrating[useridA][itemid];
        if(isuseritemtrainrating[useridB][itemid])norm2 += useritemrating[useridB][itemid] * useritemrating[useridB][itemid];
      }
    }
    else REP(featureid, numfeatures)
    {
      overlaps++;
      sum1 += userfeaturematrix[useridA][featureid];
      sum2 += userfeaturematrix[useridB][featureid];
      sum1sq += userfeaturematrix[useridA][featureid] * userfeaturematrix[useridA][featureid];
      sum2sq += userfeaturematrix[useridB][featureid] * userfeaturematrix[useridB][featureid];
      psum += userfeaturematrix[useridA][featureid] * userfeaturematrix[useridB][featureid];
      
      norm1 += userfeaturematrix[useridA][featureid] * userfeaturematrix[useridA][featureid];
      norm2 += userfeaturematrix[useridB][featureid] * userfeaturematrix[useridB][featureid];
    }
  }
  else
  {
    int itemidA = entityidA, itemidB = entityidB;
    if(!isLowDimensional)
    {
      REP(userid, numusers)
      {
        if(isuseritemtrainrating[userid][itemidA] && isuseritemtrainrating[userid][itemidB])
        {
          overlaps++;
          sum1 += useritemrating[userid][itemidA];
          sum2 += useritemrating[userid][itemidB];
          sum1sq += useritemrating[userid][itemidA] * useritemrating[userid][itemidA];;
          sum2sq += useritemrating[userid][itemidB] * useritemrating[userid][itemidB];
          psum += useritemrating[userid][itemidA] * useritemrating[userid][itemidB];
        }
        
        if(isuseritemtrainrating[userid][itemidA])norm1 += useritemrating[userid][itemidA] * useritemrating[userid][itemidA];
        if(isuseritemtrainrating[userid][itemidB])norm2 += useritemrating[userid][itemidB] * useritemrating[userid][itemidB];

      }
    }
    else REP(featureid, numfeatures)
    {
      overlaps++;
      sum1 += featureitemmatrix[featureid][itemidA];
      sum2 += featureitemmatrix[featureid][itemidB];
      sum1sq += featureitemmatrix[featureid][itemidA] * featureitemmatrix[featureid][itemidA];
      sum2sq += featureitemmatrix[featureid][itemidB] * featureitemmatrix[featureid][itemidB];
      psum += featureitemmatrix[featureid][itemidA] * featureitemmatrix[featureid][itemidB];
      
      norm1 += featureitemmatrix[featureid][itemidA] * featureitemmatrix[featureid][itemidA];
      norm2 += featureitemmatrix[featureid][itemidB] * featureitemmatrix[featureid][itemidB];    
    }
  }
  
  norm1 = sqrt(norm1); norm2 = sqrt(norm2);
  
  double correlation = -1;
  if(overlaps == 0)correlation = 0;
  else
  {
    double prod1 = (sum1sq - (sum1*sum1)/overlaps), prod2 = (sum2sq - (sum2*sum2)/overlaps);
    double num = psum - (sum1*sum2)/overlaps;
    if(prod1 < 0 || prod2 < 0)correlation = 0;
    else
    {
      double denom = sqrt(prod1 * prod2);
      if(denom < EPS && denom > -1*EPS)correlation = 0;
      else correlation = num/denom;
    }
  }
  
  return PDDD(correlation, PDD(norm1, norm2));
}

double Similarity_Pearson(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional)
{
  PDDD ret = ComputeCorrelationAndNormsForPearson(entityidA, entityidB, isUserEntity, isLowDimensional);
  double correlation = ret.first, norm1 = ret.second.first, norm2 = ret.second.second;
  dbg(correlation); dbg(norm1); dbge(norm2);
  return correlation;
}

double Similarity_Pearson(string entityA, string entityB, bool isUserEntity, bool isLowDimensional)
{
  int entityidA = -1, entityidB = -1;
  if(isUserEntity)entityidA = usernametoidmap[entityA], entityidB = usernametoidmap[entityB];
  else entityidA = itemnametoidmap[entityA], entityidB = itemnametoidmap[entityB];
  return Similarity_Pearson(entityidA, entityidB, isUserEntity, isLowDimensional);
}

double Similarity_Pearson_NormWeighted(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional)
{
  PDDD ret = ComputeCorrelationAndNormsForPearson(entityidA, entityidB, isUserEntity, isLowDimensional);
  double correlation = ret.first, norm1 = ret.second.first, norm2 = ret.second.second;
  dbg(correlation); dbg(norm1); dbge(norm2);
  return correlation * norm1 * norm2;
}

double Similarity_Pearson_NormWeighted(string entityA, string entityB, bool isUserEntity, bool isLowDimensional)
{
  int entityidA = -1, entityidB = -1;
  if(isUserEntity)entityidA = usernametoidmap[entityA], entityidB = usernametoidmap[entityB];
  else entityidA = itemnametoidmap[entityA], entityidB = itemnametoidmap[entityB];
  return Similarity_Pearson_NormWeighted(entityidA, entityidB, isUserEntity, isLowDimensional);
}

PDDD ComputeNumeratorDenominatorsForCosine(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional)
{
  double num = 0, norm1 = 0, norm2 = 0;
  if(isUserEntity)
  {
    int useridA = entityidA, useridB = entityidB;
    if(!isLowDimensional)
    {
      REP(itemid, numitems)
      {
        if(isuseritemtrainrating[useridA][itemid] && isuseritemtrainrating[useridB][itemid])num += useritemrating[useridA][itemid] * useritemrating[useridB][itemid];
        if(isuseritemtrainrating[useridA][itemid])norm1 += useritemrating[useridA][itemid] * useritemrating[useridA][itemid];
        if(isuseritemtrainrating[useridB][itemid])norm2 += useritemrating[useridB][itemid] * useritemrating[useridB][itemid];
      }
    }
    else
    REP(featureid, numfeatures)
    {
      num += userfeaturematrix[useridA][featureid] * userfeaturematrix[useridB][featureid];
      norm1 += userfeaturematrix[useridA][featureid] * userfeaturematrix[useridA][featureid];
      norm2 += userfeaturematrix[useridB][featureid] * userfeaturematrix[useridB][featureid];
    }
  }
  else
  {
    int itemidA = entityidA, itemidB = entityidB;
    if(!isLowDimensional)
    {
      REP(userid, numusers)
      {
        if(isuseritemtrainrating[userid][itemidA] && isuseritemtrainrating[userid][itemidB])    
          num += (useritemrating[userid][itemidA] - useritemrating[userid][itemidB]) * (useritemrating[userid][itemidA] - useritemrating[userid][itemidB]);    
      
        if(isuseritemtrainrating[userid][itemidA])norm1 += useritemrating[userid][itemidA] * useritemrating[userid][itemidA];
        if(isuseritemtrainrating[userid][itemidB])norm2 += useritemrating[userid][itemidB] * useritemrating[userid][itemidB];
      }
    }
    else
    REP(featureid, numfeatures)
    {
      num += featureitemmatrix[featureid][itemidA] * featureitemmatrix[featureid][itemidB];
      norm1 += featureitemmatrix[featureid][itemidA] * featureitemmatrix[featureid][itemidA];
      norm2 += featureitemmatrix[featureid][itemidB] * featureitemmatrix[featureid][itemidB];
    }
  }
  norm1 = sqrt(norm1); norm2 = sqrt(norm2);
  
  return PDDD(num, PDD(norm1, norm2));
}

double Similarity_Cosine(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional)
{
  PDDD ret = ComputeNumeratorDenominatorsForCosine(entityidA, entityidB, isUserEntity, isLowDimensional);
  double num = ret.first, norm1 = ret.second.first, norm2 = ret.second.second;
  //dbg(num); dbg(norm1); dbge(norm2); 
  if(norm1 * norm2 < EPS)return 0;
  return num/(norm1 * norm2);
}

double Similarity_Cosine(string entityA, string entityB, bool isUserEntity, bool isLowDimensional)
{
  int entityidA = -1, entityidB = -1;
  if(isUserEntity)entityidA = usernametoidmap[entityA], entityidB = usernametoidmap[entityB];
  else entityidA = itemnametoidmap[entityA], entityidB = itemnametoidmap[entityB];
  return Similarity_Cosine(entityidA, entityidB, isUserEntity, isLowDimensional);
}

double Similarity_Cosine_Unnormalized(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional)
{
  PDDD ret = ComputeNumeratorDenominatorsForCosine(entityidA, entityidB, isUserEntity, isLowDimensional);
  double num = ret.first, norm1 = ret.second.first, norm2 = ret.second.second;
  //dbg(num); dbg(norm1); dbge(norm2); 
  return num;
}

double Similarity_Cosine_Unnormalized(string entityA, string entityB, bool isUserEntity, bool isLowDimensional)
{
  int entityidA = -1, entityidB = -1;
  if(isUserEntity)entityidA = usernametoidmap[entityA], entityidB = usernametoidmap[entityB];
  else entityidA = itemnametoidmap[entityA], entityidB = itemnametoidmap[entityB];
  return Similarity_Cosine_Unnormalized(entityidA, entityidB, isUserEntity, isLowDimensional);
}

void BackupRatingState(int entityidA, int entityidB, bool isUserEntity)
{
  if(isUserEntity)
  {
    memcpy(tmpuseritemrating[0], useritemrating[entityidA], sizeof(useritemrating[entityidA]));
    memcpy(tmpuseritemrating[1], useritemrating[entityidB], sizeof(useritemrating[entityidB]));
    memcpy(tmpisuseritemtrainrating[0], isuseritemtrainrating[entityidA], sizeof(isuseritemtrainrating[entityidA]));
    memcpy(tmpisuseritemtrainrating[1], isuseritemtrainrating[entityidB], sizeof(isuseritemtrainrating[entityidB]));
  }
  else
  {
    REP(userid, numusers)
    {
      tmpuseritemrating[0][userid] = useritemrating[userid][entityidA];
      tmpuseritemrating[1][userid] = useritemrating[userid][entityidB];
      tmpisuseritemtrainrating[0][userid] = isuseritemtrainrating[userid][entityidA];
      tmpisuseritemtrainrating[1][userid] = isuseritemtrainrating[userid][entityidB];
    }
  }
}

void RestoreRatingState(int entityidA, int entityidB, bool isUserEntity)
{
  if(isUserEntity)
  {
    memcpy(useritemrating[entityidA], tmpuseritemrating[0], sizeof(useritemrating[entityidA]));
    memcpy(useritemrating[entityidB], tmpuseritemrating[1], sizeof(useritemrating[entityidB]));
    memcpy(isuseritemtrainrating[entityidA], tmpisuseritemtrainrating[0], sizeof(isuseritemtrainrating[entityidA]));
    memcpy(isuseritemtrainrating[entityidB], tmpisuseritemtrainrating[1], sizeof(isuseritemtrainrating[entityidB]));  
  }
  else
  {
    REP(userid, numusers)
    {
      useritemrating[userid][entityidA] = tmpuseritemrating[0][userid];
      useritemrating[userid][entityidB] = tmpuseritemrating[1][userid];
      isuseritemtrainrating[userid][entityidA] = tmpisuseritemtrainrating[0][userid];
      isuseritemtrainrating[userid][entityidB] = tmpisuseritemtrainrating[1][userid];
    }  
  }
}

void ModifyRating_DefaultVoting(int entityidA, int entityidB, bool isUserEntity)
{
  if(isUserEntity)
  {
    REP(itemid, numitems)
    {
      if(isuseritemtrainrating[entityidA][itemid] && !isuseritemtrainrating[entityidB][itemid])
        isuseritemtrainrating[entityidB][itemid] = true, useritemrating[entityidB][itemid] = meanuserrating[entityidB];
        
      if(!isuseritemtrainrating[entityidA][itemid] && isuseritemtrainrating[entityidB][itemid])
        isuseritemtrainrating[entityidA][itemid] = true, useritemrating[entityidB][itemid] = meanuserrating[entityidA];
    }
  }
  else
  {
    REP(userid, numusers)
    {
      if(isuseritemtrainrating[userid][entityidA] && !isuseritemtrainrating[userid][entityidB])
        isuseritemtrainrating[userid][entityidB] = true, useritemrating[userid][entityidB] = meanitemrating[entityidB];
        
      if(!isuseritemtrainrating[userid][entityidA] && isuseritemtrainrating[userid][entityidB])
        isuseritemtrainrating[userid][entityidA] = true, useritemrating[userid][entityidA] = meanitemrating[entityidA];
    }  
  }
}

void ModifyRating_InverseFrequency(int entityidA, int entityidB, bool isUserEntity)
{
  if(isUserEntity)
  {
    REP(itemid, numitems)
    {
      if(isuseritemtrainrating[entityidA][itemid])useritemrating[entityidA][itemid] *= log((double)(2 * numusers)/ (double)(1 + frequencyofitems[itemid]));
      if(isuseritemtrainrating[entityidB][itemid])useritemrating[entityidB][itemid] *= log((double)(2 * numusers)/ (double)(1 + frequencyofitems[itemid])); 
    }
  }
  else
  {
    REP(userid, numusers)
    {
      if(isuseritemtrainrating[userid][entityidA])useritemrating[userid][entityidA] *= log((double)(2 * numitems)/ (double)(1 + frequencyofusers[userid]));
      if(isuseritemtrainrating[userid][entityidB])useritemrating[userid][entityidB] *= log((double)(2 * numitems)/ (double)(1 + frequencyofusers[userid]));  
    }
  }
}

void ModifyRating_CaseAmplification(int entityidA, int entityidB, bool isUserEntity)
{
  if(isUserEntity)
  {
    REP(itemid, numitems)
    {
      if(isuseritemtrainrating[entityidA][itemid])useritemrating[entityidA][itemid] = pow(useritemrating[entityidA][itemid], CASE_AMPLIFICATION_FACTOR);
      if(isuseritemtrainrating[entityidB][itemid])useritemrating[entityidB][itemid] = pow(useritemrating[entityidB][itemid], CASE_AMPLIFICATION_FACTOR);
    }
  }
  else
  {
    REP(userid, numusers)
    {
      if(isuseritemtrainrating[userid][entityidA])useritemrating[userid][entityidA] = pow(useritemrating[userid][entityidA], CASE_AMPLIFICATION_FACTOR);
      if(isuseritemtrainrating[userid][entityidB])useritemrating[userid][entityidB] = pow(useritemrating[userid][entityidB], CASE_AMPLIFICATION_FACTOR);
    }
  }
}

double Similarity_DefaultVoting_Cosine(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional)
{
  BackupRatingState(entityidA, entityidB, isUserEntity);
  ModifyRating_DefaultVoting(entityidA, entityidB, isUserEntity);
  double similarity = Similarity_Cosine(entityidA, entityidB, isUserEntity, isLowDimensional);
  RestoreRatingState(entityidA, entityidB, isUserEntity);
  
  return similarity;
}

double Similarity_DefaultVoting_Cosine(string entityA, string entityB, bool isUserEntity, bool isLowDimensional)
{
  int entityidA = -1, entityidB = -1;
  if(isUserEntity)entityidA = usernametoidmap[entityA], entityidB = usernametoidmap[entityB];
  else entityidA = itemnametoidmap[entityA], entityidB = itemnametoidmap[entityB];
  return Similarity_DefaultVoting_Cosine(entityidA, entityidB, isUserEntity, isLowDimensional);
}

double Similarity_DefaultVoting_Pearson(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional)
{
  BackupRatingState(entityidA, entityidB, isUserEntity);
  ModifyRating_DefaultVoting(entityidA, entityidB, isUserEntity);
  double similarity = Similarity_Pearson(entityidA, entityidB, isUserEntity, isLowDimensional);
  RestoreRatingState(entityidA, entityidB, isUserEntity);
  
  return similarity;
}

double Similarity_DefaultVoting_Pearson(string entityA, string entityB, bool isUserEntity, bool isLowDimensional)
{
  int entityidA = -1, entityidB = -1;
  if(isUserEntity)entityidA = usernametoidmap[entityA], entityidB = usernametoidmap[entityB];
  else entityidA = itemnametoidmap[entityA], entityidB = itemnametoidmap[entityB];
  return Similarity_DefaultVoting_Pearson(entityidA, entityidB, isUserEntity, isLowDimensional);
}

double Similarity_InverseUserFrequency_Cosine(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional)
{
  BackupRatingState(entityidA, entityidB, isUserEntity);
  ModifyRating_InverseFrequency(entityidA, entityidB, isUserEntity);
  double similarity = Similarity_Cosine(entityidA, entityidB, isUserEntity, isLowDimensional);
  RestoreRatingState(entityidA, entityidB, isUserEntity);
  
  return similarity;
}

double Similarity_InverseUserFrequency_Cosine(string entityA, string entityB, bool isUserEntity, bool isLowDimensional)
{
  int entityidA = -1, entityidB = -1;
  if(isUserEntity)entityidA = usernametoidmap[entityA], entityidB = usernametoidmap[entityB];
  else entityidA = itemnametoidmap[entityA], entityidB = itemnametoidmap[entityB];
  return Similarity_InverseUserFrequency_Cosine(entityidA, entityidB, isUserEntity, isLowDimensional);
}

double Similarity_InverseUserFrequency_Pearson(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional)
{
  BackupRatingState(entityidA, entityidB, isUserEntity);
  ModifyRating_InverseFrequency(entityidA, entityidB, isUserEntity);
  double similarity = Similarity_Pearson(entityidA, entityidB, isUserEntity, isLowDimensional);
  RestoreRatingState(entityidA, entityidB, isUserEntity);
  
  return similarity;
}

double Similarity_InverseUserFrequency_Pearson(string entityA, string entityB, bool isUserEntity, bool isLowDimensional)
{
  int entityidA = -1, entityidB = -1;
  if(isUserEntity)entityidA = usernametoidmap[entityA], entityidB = usernametoidmap[entityB];
  else entityidA = itemnametoidmap[entityA], entityidB = itemnametoidmap[entityB];
  return Similarity_InverseUserFrequency_Pearson(entityidA, entityidB, isUserEntity, isLowDimensional);
}

double Similarity_CaseAmplification_Cosine(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional)
{
  BackupRatingState(entityidA, entityidB, isUserEntity);
  ModifyRating_CaseAmplification(entityidA, entityidB, isUserEntity);
  double similarity = Similarity_Cosine(entityidA, entityidB, isUserEntity, isLowDimensional);
  RestoreRatingState(entityidA, entityidB, isUserEntity);
  
  return similarity;
}

double Similarity_CaseAmplification_Cosine(string entityA, string entityB, bool isUserEntity, bool isLowDimensional)
{
  int entityidA = -1, entityidB = -1;
  if(isUserEntity)entityidA = usernametoidmap[entityA], entityidB = usernametoidmap[entityB];
  else entityidA = itemnametoidmap[entityA], entityidB = itemnametoidmap[entityB];
  return Similarity_CaseAmplification_Cosine(entityidA, entityidB, isUserEntity, isLowDimensional);
}

double Similarity_CaseAmplification_Pearson(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional)
{
  BackupRatingState(entityidA, entityidB, isUserEntity);
  ModifyRating_CaseAmplification(entityidA, entityidB, isUserEntity);
  double similarity = Similarity_Pearson(entityidA, entityidB, isUserEntity, isLowDimensional);
  RestoreRatingState(entityidA, entityidB, isUserEntity);
  
  return similarity;
}

double Similarity_CaseAmplification_Pearson(string entityA, string entityB, bool isUserEntity, bool isLowDimensional)
{
  int entityidA = -1, entityidB = -1;
  if(isUserEntity)entityidA = usernametoidmap[entityA], entityidB = usernametoidmap[entityB];
  else entityidA = itemnametoidmap[entityA], entityidB = itemnametoidmap[entityB];
  return Similarity_CaseAmplification_Pearson(entityidA, entityidB, isUserEntity, isLowDimensional);
}

void TestSimilarityFunctionsForMovieLensDataset(string entityA, string entityB)
{
  cout<<Similarity_Cosine(entityA, entityB,false, false)<<endl;
  cout<<Similarity_Cosine_Unnormalized(entityA, entityB,false, false)<<endl;
  cout<<Similarity_Pearson(entityA, entityB,false, false)<<endl;
  cout<<Similarity_Pearson_NormWeighted(entityA, entityB,false, false)<<endl;
  cout<<Similarity_DefaultVoting_Cosine(entityA, entityB,false, false)<<endl;
  cout<<Similarity_DefaultVoting_Pearson(entityA, entityB,false, false)<<endl;
  cout<<Similarity_InverseUserFrequency_Cosine(entityA, entityB,false, false)<<endl;
  cout<<Similarity_InverseUserFrequency_Pearson(entityA, entityB,false, false)<<endl;
  cout<<Similarity_CaseAmplification_Cosine(entityA, entityB,false, false)<<endl;
  cout<<Similarity_CaseAmplification_Pearson(entityA, entityB,false, false)<<endl;
  cout<<endl;
}

void TestSimilarityFunctionsForMovieLensDataset()
{
  TestSimilarityFunctionsForMovieLensDataset("Star Wars: Episode IV - A New Hope (1977)","Amityville: A New Generation (1993)");
  TestSimilarityFunctionsForMovieLensDataset("Star Wars: Episode IV - A New Hope (1977)","Star Wars: Episode VI - Return of the Jedi (1983)");
  TestSimilarityFunctionsForMovieLensDataset("Star Wars: Episode IV - A New Hope (1977)","Star Wars: Episode V - The Empire Strikes Back (1980)");
  TestSimilarityFunctionsForMovieLensDataset("Star Wars: Episode IV - A New Hope (1977)","Substance of Fire, The (1996)");
  TestSimilarityFunctionsForMovieLensDataset("Star Wars: Episode IV - A New Hope (1977)","Liar Liar (1997)");
}

vector< PDI > TopMatches(int entityId, bool isUserEntity, bool isLowDimensional, int numMatches, double (*SimilarityMetric)(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional) = Similarity_Pearson_NormWeighted)
{
  vector< PDI > v;
  if(isUserEntity)
  {
    int givenUserId = entityId;
    REP(userid, numusers)if(userid != givenUserId)v.pb( PDI((*SimilarityMetric)(givenUserId, userid, isUserEntity, isLowDimensional), userid) );
  }
  else
  {
    int givenItemId = entityId;
    REP(itemid, numitems)if(itemid != givenItemId)v.pb( PDI((*SimilarityMetric)(givenItemId, itemid, isUserEntity, isLowDimensional), itemid) );  
  }
  
  sort(v.begin(), v.end());
  while(v.sz > numMatches)v.erase(v.begin());
  reverse(v.begin(),v.end());
  
  return v;
}

vector< PDI > TopMatches(string entity, bool isUserEntity, bool isLowDimensional, int numMatches, double (*SimilarityMetric)(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional) = Similarity_Pearson_NormWeighted)
{
  int entityId = -1;
  if(isUserEntity)entityId = usernametoidmap[entity];
  else entityId = itemnametoidmap[entity];
  
  return TopMatches(entityId, isUserEntity, isLowDimensional, numMatches, SimilarityMetric);
}

vector< PDI > GetOtherEntityRecommendationsBasedOnCurrentEntitySimilarities(int entityId, bool isUserEntity, bool isLowDimensional, double (*SimilarityMetric)(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional) = Similarity_Pearson_NormWeighted)
{
  if(isUserEntity)REP(itemid, numitems)if(isuseritemtrainrating[entityId][itemid])entitiesAlreadySeen[itemid] = true;
  else REP(userid, numusers)if(isuseritemtrainrating[userid][entityId])entitiesAlreadySeen[userid] = true;
  
  if(isUserEntity)
  {
    REP(userid, numusers)if(userid != entityId)
    {
      double similarity = (*SimilarityMetric)(entityId, userid, isUserEntity, isLowDimensional);
      REP(itemid, numitems)if(isuseritemtrainrating[userid][itemid] && !entitiesAlreadySeen[itemid])
      {
        weightedScoreOfEntitiesNotYetSeen[itemid] += similarity * (useritemrating[userid][itemid] - meanuserrating[userid]);
        similaritySumOfEntitiesNotYetSeen[itemid] += abs(similarity);
      }
    }
  }
  else
  {
    REP(itemid, numitems)if(itemid != entityId)
    {
      double similarity = (*SimilarityMetric)(entityId, itemid, isUserEntity, isLowDimensional);
      REP(userid, numusers)if(isuseritemtrainrating[userid][itemid] && !entitiesAlreadySeen[userid])
      {
        weightedScoreOfEntitiesNotYetSeen[userid] += similarity * (useritemrating[userid][itemid] - meanitemrating[itemid]);
        similaritySumOfEntitiesNotYetSeen[userid] += abs(similarity);
      }
    }
  }
  
  vector< PDI > v; 
  int numentities = isUserEntity ? numitems : numusers;
  REP(entityid, numentities)if(!entitiesAlreadySeen[entityid])
  {
    double weightedscore = 0;
    if(isUserEntity)weightedscore = meanuserrating[entityId];
    else weightedscore = meanitemrating[entityId];
    if(similaritySumOfEntitiesNotYetSeen[entityid] > 0)weightedscore += weightedScoreOfEntitiesNotYetSeen[entityid]/similaritySumOfEntitiesNotYetSeen[entityid];
    v.pb(PDI(weightedscore, entityid));
  }
  
  sort(v.begin(), v.end());
  reverse(v.begin(),v.end());
  
  //Perform Clean up operation
  REP(entityid, numentities)if(entitiesAlreadySeen[entityid])entitiesAlreadySeen[entityid] = false;
  
  return v;
}

vector< PDI > GetOtherEntityRecommendationsBasedOnCurrentEntitySimilarities(string entity, bool isUserEntity, bool isLowDimensional, double (*SimilarityMetric)(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional) = Similarity_Pearson_NormWeighted)
{
  int entityId = -1;
  if(isUserEntity)entityId = usernametoidmap[entity];
  else entityId = itemnametoidmap[entity];
  
  return GetOtherEntityRecommendationsBasedOnCurrentEntitySimilarities(entityId, isUserEntity, isLowDimensional, SimilarityMetric);
}

vector< PDI > GetOtherEntityRecommendationsBasedOnOtherEntitySimilarities(int entityId, bool isUserEntity, bool isLowDimensional, double (*SimilarityMetric)(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional) = Similarity_Pearson_NormWeighted)
{
  vector<int> entitiesAlreadySeen;
  if(isUserEntity)REP(itemid, numitems)if(isuseritemtrainrating[entityId][itemid])entitiesAlreadySeen[itemid] = true, entitiesAlreadySeen.pb(itemid);
  else REP(userid, numusers)if(isuseritemtrainrating[userid][entityId])entitiesAlreadySeen[userid] = true, entitiesAlreadySeen.pb(userid);
  
  vector< PDI > v; 
  if(isUserEntity)
  {
    REP(itemid, numitems)if(!entitiesAlreadySeen[itemid])
    {
      double weightedScore = 0.0;
      double similaritySum = 0.0;
      
      REP(seenitemidindex, entitiesAlreadySeen.sz)
      {
        int seenitemid = entitiesAlreadySeen[seenitemidindex];
        double similarity = (*SimilarityMetric)(itemid, seenitemid, !isUserEntity, isLowDimensional);
        weightedScore += similarity * (useritemrating[entityId][seenitemid] - meanitemrating[seenitemid]);
        similaritySum += abs(similarity);
      }
      
      if(similaritySum > 0)weightedScore /= similaritySum;
      weightedScore += meanitemrating[itemid];
      v.pb(PDI(weightedScore, itemid));
    }
  }
  else
  {
    REP(userid, numusers)if(!entitiesAlreadySeen[userid])
    {
      double weightedScore = 0.0;
      double similaritySum = 0.0;
      
      REP(seenuseridindex, entitiesAlreadySeen.sz)
      {
        int seenuserid = entitiesAlreadySeen[seenuseridindex];
        double similarity = (*SimilarityMetric)(userid, seenuserid, !isUserEntity, isLowDimensional);
        weightedScore += similarity * (useritemrating[seenuserid][entityId] - meanuserrating[seenuserid]);
        similaritySum += abs(similarity);
      }
      
      if(similaritySum > 0)weightedScore /= similaritySum;
      weightedScore += meanuserrating[userid];
      v.pb(PDI(weightedScore, userid));
    }
  }
    
  sort(v.begin(), v.end());
  reverse(v.begin(),v.end());
  
  
  return v;
}

vector< PDI > GetOtherEntityRecommendationsBasedOnOtherEntitySimilarities(string entity, bool isUserEntity, bool isLowDimensional, double (*SimilarityMetric)(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional) = Similarity_Pearson_NormWeighted)
{
  int entityId = -1;
  if(isUserEntity)entityId = usernametoidmap[entity];
  else entityId = itemnametoidmap[entity];
  
  return GetOtherEntityRecommendationsBasedOnOtherEntitySimilarities(entityId, isUserEntity, isLowDimensional, SimilarityMetric);
}

void TestMovieLensUserItemRecommendationSystem(double (*SimilarityMetric)(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional))
{
  string person = "4";
  vector< PDI > v = TopMatches(person, true, false, 10, SimilarityMetric);
  REP(i, v.sz)cout<<person<<"\t"<<users[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl;
  
  v = GetOtherEntityRecommendationsBasedOnCurrentEntitySimilarities(person, true, false, SimilarityMetric);
  REP(i, min(10,(int)v.sz))cout<<items[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl<<endl;
  
  string item = "Star Wars: Episode IV - A New Hope (1977)";//"Star Wars (1977)";
  v = TopMatches(item, false, false, 10, SimilarityMetric);
  REP(i, v.sz)cout<<item<<"\t"<<items[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl;
  
  string item2 = "Seven (Se7en) (1995)";
  v = TopMatches(item2, false, false, 10, SimilarityMetric);
  REP(i, v.sz)cout<<item2<<"\t"<<items[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl;
  //v = GetOtherEntityRecommendationsBasedOnCurrentEntitySimilarities(item2, false, false, SimilarityMetric);
  //REP(i, min(10, (int)v.sz))cout<<users[v[i].second]<<"\t"<<v[i].first<<endl;
  //cout<<endl<<endl;
  string item3 = "Die Hard: With a Vengeance (1995)";
  v = TopMatches(item3, false, false, 10, SimilarityMetric);
  REP(i, v.sz)cout<<item3<<"\t"<<items[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl;
  
  string item4 = "Raiders of the Lost Ark (1981)";
  v = TopMatches(item4, false, false, 10, SimilarityMetric);
  REP(i, v.sz)cout<<item4<<"\t"<<items[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl;
  
  //string starwars6 = "Return of the Jedi (1983)", starwars5 = "Empire Strikes Back, The (1980)";
  string starwars6 = "Star Wars: Episode VI - Return of the Jedi (1983)", starwars5 = "Star Wars: Episode V - The Empire Strikes Back (1980)";
  cout<<Similarity_Pearson(item, starwars5, false, false)<<" "<<Similarity_L2Norm(item, starwars5, false, false)<<" "<<Similarity_Cosine(item, starwars5, false, false)<<endl;
  cout<<Similarity_Pearson(item, starwars6, false, false)<<" "<<Similarity_L2Norm(item, starwars6, false, false)<<" "<<Similarity_Cosine(item, starwars6, false, false)<<endl;
  string tmp = "Substance of Fire, The (1996)";
  cout<<Similarity_Pearson(item, tmp, false, false)<<" "<<Similarity_L2Norm(item, tmp, false, false)<<" "<<Similarity_Cosine(item, tmp, false, false)<<endl;
  
  int star4 = itemnametoidmap[item], star5 = itemnametoidmap[starwars5], tmpid = itemnametoidmap[tmp];
  int ntmp = 0, nstar45 = 0;
  REP(userid, numusers)
  if(isuseritemtrainrating[userid][star4])
  {
    if(isuseritemtrainrating[userid][star5])nstar45++;
    if(isuseritemtrainrating[userid][tmpid])ntmp++;
  }
  
  cout<<nstar45<<" "<<ntmp<<endl;
  

}

void TestMovieLensUserItemRecommendationSystem()
{
  TestMovieLensUserItemRecommendationSystem(Similarity_Pearson_NormWeighted);
  //TestMovieLensUserItemRecommendationSystem(Similarity_Pearson);
  //TestMovieLensUserItemRecommendationSystem(Similarity_L2Norm);
  //TestMovieLensUserItemRecommendationSystem(Similarity_Cosine);
  //TestMovieLensUserItemRecommendationSystem(Similarity_Cosine_Unnormalized);
}

void ComputeItemItemSimilarities(int topN, bool isLowDimensional, double (*SimilarityMetric)(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional) = Similarity_Pearson_NormWeighted)
{
  REP(itemid, numitems)
  {
    if(topN < 0)
    {
      itemsimilarityscores[itemid][itemid] = 1.0;
      FOR(itemid2, itemid + 1, numitems)
      {
        double similarity = (*SimilarityMetric)(itemid, itemid2, false, isLowDimensional);
        itemsimilarityscores[itemid][itemid2] = itemsimilarityscores[itemid2][itemid] = similarity;
      }
    }
    else
    {
      vector< PDI > v = TopMatches(itemid, false, isLowDimensional, topN, SimilarityMetric);
      REP(i, v.sz)itemsimilarityscores[itemid][i] = v[i].first, itemsimilarityitems[itemid][i] = v[i].second;
    }
  }
}

void TestMovieLensItemItemRecommendationSystem()
{
  ComputeItemItemSimilarities(numitems, false, Similarity_Pearson_NormWeighted);
 
  string item = "Star Wars (1977)"; //"Star Wars: Episode IV - A New Hope (1977)";
  cout<<item<<endl;
  int itemid = itemnametoidmap[item];
  REP(i, 10)if(itemsimilarityitems[itemid][i] > -1)cout<<items[itemsimilarityitems[itemid][i]]<<"\t"<<itemsimilarityscores[itemid][i]<<endl;
  cout<<endl;
  
  string item2 = "GoldenEye (1995)"; 
  cout<<item2<<endl;
  int itemid2 = itemnametoidmap[item2];
  REP(i, 10)if(itemsimilarityitems[itemid2][i] > -1)cout<<items[itemsimilarityitems[itemid2][i]]<<"\t"<<itemsimilarityscores[itemid2][i]<<endl;
  cout<<endl<<endl;

  string item3 = "Seven (Se7en) (1995)"; 
  cout<<item3<<endl;
  int itemid3 = itemnametoidmap[item3];
  REP(i, 10)if(itemsimilarityitems[itemid3][i] > -1)cout<<items[itemsimilarityitems[itemid3][i]]<<"\t"<<itemsimilarityscores[itemid3][i]<<endl;
  cout<<endl<<endl;

  string item4 = "Toy Story (1995)"; 
  cout<<item4<<endl;
  int itemid4 = itemnametoidmap[item4];
  REP(i, 10)if(itemsimilarityitems[itemid4][i] > -1)cout<<items[itemsimilarityitems[itemid4][i]]<<"\t"<<itemsimilarityscores[itemid4][i]<<endl;
  cout<<endl<<endl;
  
}

void ComputeSlopeOneAveragesAndCounts()
{  
  REP(userid, numusers)REP(itemid1, numitems)FOR(itemid2, itemid1+1, numitems)
  if(isuseritemtrainrating[userid][itemid1] && isuseritemtrainrating[userid][itemid2])
  {
    itemaveragedifferences[itemid1][itemid2] += (useritemrating[userid][itemid1] - useritemrating[userid][itemid2]);
    itemaveragedifferences[itemid2][itemid1] += (useritemrating[userid][itemid2] - useritemrating[userid][itemid1]);
    itemitemcounts[itemid1][itemid2]++; 
    itemitemcounts[itemid2][itemid1]++;
  }
  
  REP(itemid1, numitems)FOR(itemid2, itemid1+1, numitems)
  if(itemitemcounts[itemid1][itemid2] > 0)
  {
    itemaveragedifferences[itemid1][itemid2] /= itemitemcounts[itemid1][itemid2];
    itemaveragedifferences[itemid2][itemid1] /= itemitemcounts[itemid2][itemid1];
  }
}

vector< PDI > GetRecommendations_SlopeOne(int userid)
{  
  vector< PDI > v;
  
  REP(itemid1, numitems)if(!isuseritemtrainrating[userid][itemid1])
  {
    int sumofcounts = 0;
    double rating = 0;
    REP(itemid2, numitems)if(isuseritemtrainrating[userid][itemid2] && itemitemcounts[itemid1][itemid2] > 0) //NOTE: itemid2 != itemid1 condition is automatically covered by this
    {
      rating += (useritemrating[userid][itemid2] + itemaveragedifferences[itemid1][itemid2]) * itemitemcounts[itemid1][itemid2];
      sumofcounts += itemitemcounts[itemid1][itemid2];
    }
    dbg(rating); dbge(sumofcounts);
    if(sumofcounts > 0)rating /= sumofcounts;
    v.pb( PDI(rating, itemid1) );
  }
  
  sort(v.begin(), v.end());
  reverse(v.begin(), v.end());
  
  return v;
}

vector< PDI > GetRecommendations_SlopeOne(string username)
{
  int userid = usernametoidmap[username];
  return GetRecommendations_SlopeOne(userid);
}

void LoadSampleDatasetForSlopeOne()
{
  useritemrating[0][0] = 5; useritemrating[0][1] = 3; useritemrating[0][2] = 2; 
  isuseritemrating[0][0] = true; isuseritemrating[0][1] = true; isuseritemrating[0][2] = true;
  useritemrating[1][0] = 3; useritemrating[1][1] = 4; 
  isuseritemrating[1][0] = true; isuseritemrating[1][1] = true; 
                            useritemrating[2][1] = 2; useritemrating[2][2] = 5; 
                            isuseritemrating[2][1] = true; isuseritemrating[2][2] = true;
                            
  numusers = 3; numitems = 3;
  users[0] = "John"; users[1] = "Mark"; users[2] = "Lucy";
  usernametoidmap["John"] = 0; usernametoidmap["Mark"] = 1; usernametoidmap["Lucy"] = 2;
  
  items[0] = "Item1"; items[1] = "Item2"; items[2] = "Item3";
  itemnametoidmap["Item1"] = 0; itemnametoidmap["Item2"] = 1; itemnametoidmap["Item3"] = 2;
}

void TestSlopeOne()
{
  ComputeSlopeOneAveragesAndCounts();
  
  string person = "4";//"Lucy";
  vector< PDI > v = GetRecommendations_SlopeOne(person);
  REP(i, min(10,(int)v.sz))cout<<items[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl<<endl;
  
  person = "5";//"Mark";
  v = GetRecommendations_SlopeOne(person);
  REP(i, min(10,(int)v.sz))cout<<items[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl<<endl;
}


void TestConstantValue(double (*SimilarityMetric)(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional) = Similarity_Pearson_NormWeighted)
{
  //HACK: Modify the original user-item-rating matrix to have a constant value always
  REP(userid, numusers)REP(itemid, numitems)if(isuseritemtrainrating[userid][itemid])useritemrating[userid][itemid] = 1;
  
  string person = "4";
  vector< PDI > v = TopMatches(person, true, false, 10, SimilarityMetric);
  REP(i, v.sz)cout<<person<<"\t"<<users[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl;
  
  v = GetOtherEntityRecommendationsBasedOnCurrentEntitySimilarities(person, true, false, SimilarityMetric);
  REP(i, min(10,(int)v.sz))cout<<items[v[i].second]<<"\t"<<v[i].first<<endl;
  //REP(i,(int)v.sz)cout<<items[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl<<endl;
  
  string item = "Star Wars (1977)";//"Star Wars: Episode IV - A New Hope (1977)";
  string starwars6 = "Return of the Jedi (1983)", starwars5 = "Empire Strikes Back, The (1980)";
  //string starwars6 = "Star Wars: Episode VI - Return of the Jedi (1983)", starwars5 = "Star Wars: Episode V - The Empire Strikes Back (1980)";
  //cout<<Similarity_Pearson(item, starwars5, false, false)<<" "<<Similarity_L2Norm(item, starwars5, false, false)<<" "<<Similarity_Cosine(item, starwars5, false, false);
  //cout<<" "<<Similarity_Pearson_NormWeighted(item, starwars5, false, false)<<endl;
  
}

void TestRandomValue(double (*SimilarityMetric)(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional) = Similarity_Pearson_NormWeighted)
{
  srand(time(NULL));
  //HACK: Modify the original user-item-rating matrix to have the random value
  REP(userid, numusers)REP(itemid, numitems)if(isuseritemtrainrating[userid][itemid])useritemrating[userid][itemid] = 1 + rand()%5;
  
  string person = "4";
  vector< PDI > v = TopMatches(person, true, false, 10, SimilarityMetric);
  REP(i, v.sz)cout<<person<<"\t"<<users[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl;
  
  v = GetOtherEntityRecommendationsBasedOnCurrentEntitySimilarities(person, true, false, SimilarityMetric);
  REP(i, min(10,(int)v.sz))cout<<items[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl<<endl;
}

void TestAverageOfUserRating(double (*SimilarityMetric)(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional) = Similarity_Pearson_NormWeighted)
{
  //HACK: Modify the original user-item-rating matrix to have the average of the user's rating
  REP(userid, numusers)
  {
    double averageratingperuser = 0; int numitemsratedbyuser = 0;
    REP(itemid, numitems)if(isuseritemtrainrating[userid][itemid])averageratingperuser += useritemrating[userid][itemid], numitemsratedbyuser++;
    if(numitemsratedbyuser > 0)averageratingperuser /= numitemsratedbyuser;
    REP(itemid, numitems)if(isuseritemtrainrating[userid][itemid])useritemrating[userid][itemid] = averageratingperuser;
  }
  
  cout<<"4"<<endl;
  REP(itemid, numitems)if(isuseritemtrainrating[4][itemid])cout<<items[itemid]<<":"<<useritemrating[4][itemid]<<" ";
  cout<<endl<<endl;
  
  cout<<"5"<<endl;
  REP(itemid, numitems)if(isuseritemtrainrating[5][itemid])cout<<items[itemid]<<":"<<useritemrating[5][itemid]<<" ";
  cout<<endl<<endl;

  // string person = "4";
  // vector< PDI > v = TopMatches(person, true, false, numusers, SimilarityMetric);
  // REP(i, v.sz)cout<<person<<"\t"<<users[v[i].second]<<"\t"<<v[i].first<<endl;
  // cout<<endl;
  
  // v = GetOtherEntityRecommendationsBasedOnCurrentEntitySimilarities(person, true, false, SimilarityMetric);
  // REP(i, min(10,(int)v.sz))cout<<items[v[i].second]<<"\t"<<v[i].first<<endl;
  // cout<<endl<<endl;

  cout<<Similarity_Pearson_NormWeighted(4, 5, true, false)<<endl;
}

void TestAverageOfItemRating(double (*SimilarityMetric)(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional) = Similarity_Pearson_NormWeighted)
{
  //HACK: Modify the original user-item-rating matrix to have the average of the items's rating
  REP(itemid, numitems)
  {
    double averageratingperitem = 0; int numitemsratedperitem = 0;
    REP(userid, numusers)if(isuseritemtrainrating[userid][itemid])averageratingperitem += useritemrating[userid][itemid], numitemsratedperitem++;
    if(numitemsratedperitem > 0)averageratingperitem /= numitemsratedperitem;
    REP(userid, numusers)if(isuseritemtrainrating[userid][itemid])useritemrating[userid][itemid] = averageratingperitem;
  }
  
  string person = "4";
  vector< PDI > v = TopMatches(person, true, false, 10, SimilarityMetric);
  REP(i, v.sz)cout<<person<<"\t"<<users[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl;
  
  v = GetOtherEntityRecommendationsBasedOnCurrentEntitySimilarities(person, true, false, SimilarityMetric);
  REP(i, min(10,(int)v.sz))cout<<items[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl<<endl;
}

double ComputeFrobeniusNormCost()
{
  double cost = 0;
  REP(userid, numusers)REP(itemid, numitems)if(isuseritemtrainrating[userid][itemid])
  {
    double localcost = 0;
    REP(featureid, numfeatures)localcost += userfeaturematrix[userid][featureid] * featureitemmatrix[featureid][itemid];
    cost += (useritemrating[userid][itemid] - localcost)*(useritemrating[userid][itemid] - localcost);
  }
  return sqrt(cost);
}

void ComputeMatricesForNonNegativeMatrixFactorization(int iNumfeatures, int numIterations)
{
  srand(time(NULL));
  numfeatures = iNumfeatures;
  
  //Initialize the userfeature and featureitem matrices randomly with values in [0,1]
  REP(userid, numusers)REP(featureid, numfeatures)userfeaturematrix[userid][featureid] = rand() / (double)RAND_MAX;
  REP(featureid, numfeatures)REP(itemid, numitems)featureitemmatrix[featureid][itemid] = rand() / (double)RAND_MAX;
  
  // system("pause");
  // REP(userid, numusers)REP(featureid, numfeatures){cout<<userfeaturematrix[userid][featureid]; if(featureid != numfeatures - 1)cout<<" "; else cout<<endl;}
  // cout<<endl<<endl;
  // REP(featureid, numfeatures)REP(itemid, numitems){cout<<featureitemmatrix[featureid][itemid]; if(itemid != numitems - 1)cout<<" "; else cout<<endl;}
  // cout<<endl<<endl;
  
  
  REP(iteration, numIterations)
  {
    cout<<"Iteration:"<<iteration<<endl;
    clock_t iterationstart = clock();
    clock_t start = clock();
    double cost = ComputeFrobeniusNormCost();
    cout<<cost<<endl;
    clock_t end = clock();
    if(_DEBUG)cout<<"Time taken for Computing Frobenius Norm Cost: "<<(double)(end-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
    if(cost < EPS)break;
  
    start = clock();
    //Update feature item matrix
    REP(featureid, numfeatures)REP(itemid, numitems)
    {
      featureitemnumerator[featureid][itemid] = 0;
      REP(userid, numusers)if(isuseritemtrainrating[userid][itemid])featureitemnumerator[featureid][itemid] += userfeaturematrix[userid][featureid] * useritemrating[userid][itemid];
      //cout<<featureitemnumerator[featureid][itemid]; if(itemid != numitems - 1)cout<<" "; else cout<<endl;
    }
    end = clock();
    if(_DEBUG)cout<<"Time taken for Computing Feature Item Numerator: "<<(double)(end-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
    
    start = clock();
    REP(featureid, numfeatures)REP(featureid2, numfeatures)
    { 
      featurefeaturematrix[featureid][featureid2] = 0; 
      REP(userid, numusers)featurefeaturematrix[featureid][featureid2] += userfeaturematrix[userid][featureid] * userfeaturematrix[userid][featureid2];
    }
    end = clock();
    if(_DEBUG)cout<<"Time taken for Computing FeatureFeatureMatrix for Feature Item Denominator: "<<(double)(end-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;

    start = clock();    
    REP(featureid, numfeatures)REP(itemid, numitems)
    {
      featureitemdenominator[featureid][itemid] = 0;
      REP(featureid2, numfeatures)featureitemdenominator[featureid][itemid] += featurefeaturematrix[featureid][featureid2] * featureitemmatrix[featureid2][itemid];
      //cout<<featureitemdenominator[featureid][itemid]; if(itemid != numitems - 1)cout<<" "; else cout<<endl;
    }
    end = clock();
    if(_DEBUG)cout<<"Time taken for Computing Feature Item Denominator: "<<(double)(end-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
      
    start = clock();    
    REP(featureid, numfeatures)REP(itemid, numitems)if(featureitemdenominator[featureid][itemid] > EPS)featureitemmatrix[featureid][itemid] *= (featureitemnumerator[featureid][itemid] / featureitemdenominator[featureid][itemid]);
    end = clock();
    if(_DEBUG)cout<<"Time taken for Updating Feature Item Matrix with Denominator: "<<(double)(end-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;

    // system("pause");
    // cout<<"after update to feature item"<<endl;
    // REP(userid, numusers)REP(featureid, numfeatures){cout<<userfeaturematrix[userid][featureid]; if(featureid != numfeatures-1)cout<<" "; else cout<<endl;}
    // cout<<endl<<endl;
    // REP(featureid, numfeatures)REP(itemid, numitems){cout<<featureitemmatrix[featureid][itemid]; if(itemid != numitems - 1)cout<<" "; else cout<<endl;}
    // cout<<endl<<endl;
    
    start = clock();
    //Update user feature matrix
    REP(userid, numusers)REP(featureid, numfeatures)
    {
      userfeaturenumerator[userid][featureid] = 0;
      REP(itemid, numitems)if(isuseritemtrainrating[userid][itemid])userfeaturenumerator[userid][featureid] += useritemrating[userid][itemid] * featureitemmatrix[featureid][itemid];
    }
    end = clock();
    if(_DEBUG)cout<<"Time taken for Computing User Feature Numerator: "<<(double)(end-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
    
    start = clock();
    REP(featureid2, numfeatures)REP(featureid, numfeatures)
    { 
      featurefeaturematrix[featureid2][featureid] = 0; 
      REP(itemid, numitems)featurefeaturematrix[featureid2][featureid] += featureitemmatrix[featureid2][itemid] * featureitemmatrix[featureid][itemid];
    }
    end = clock();
    if(_DEBUG)cout<<"Time taken for Computing Feature2FeatureMatrix for User Feature Denominator: "<<(double)(end-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
    
    start = clock();    
    REP(userid, numusers)REP(featureid, numfeatures)
    {
      userfeaturedenominator[userid][featureid] = 0;
      REP(featureid2, numfeatures)userfeaturedenominator[userid][featureid] += userfeaturematrix[userid][featureid2] * featurefeaturematrix[featureid2][featureid];
    }
    end = clock();
    if(_DEBUG)cout<<"Time taken for Computing Feature Item Denominator: "<<(double)(end-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;

    start = clock();    
    REP(userid, numusers)REP(featureid, numfeatures)if(userfeaturedenominator[userid][featureid] > EPS)userfeaturematrix[userid][featureid] *= (userfeaturenumerator[userid][featureid] / userfeaturedenominator[userid][featureid]);
    end = clock();
    if(_DEBUG)cout<<"Time taken for Updating User Feature Matrix with Denominator: "<<(double)(end-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
    
    // system("pause");
    // cout<<"after denominator update to user feature"<<endl;
    // REP(userid, numusers)REP(featureid, numfeatures){cout<<userfeaturematrix[userid][featureid]; if(featureid != numfeatures-1)cout<<" "; else cout<<endl;}
    // cout<<endl<<endl;
    // REP(featureid, numfeatures)REP(itemid, numitems){cout<<featureitemmatrix[featureid][itemid]; if(itemid != numitems - 1)cout<<" "; else cout<<endl;}
    // cout<<endl<<endl;
    
    if(_DEBUG)cout<<"Time taken for Iteration: "<<(double)(end-iterationstart)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
}

void LoadSampleDatasetForNMF()
{
  useritemrating[0][0] = 22; useritemrating[0][1] = 28;
  isuseritemrating[0][0] = true; isuseritemrating[0][1] = true;
  useritemrating[1][0] = 49; useritemrating[1][1] = 64; 
  isuseritemrating[1][0] = true; isuseritemrating[1][1] = true; 

  numusers = 2; numitems = 2;
  users[0] = "John"; users[1] = "Mark";
  usernametoidmap["John"] = 0; usernametoidmap["Mark"] = 1;
  
  items[0] = "Item1"; items[1] = "Item2";
  itemnametoidmap["Item1"] = 0; itemnametoidmap["Item2"] = 1;

}

void TestNonNegativeMatrixFactorization(int iNumfeatures, int numIterations, double (*SimilarityMetric)(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional) = Similarity_Pearson_NormWeighted)
{
  ComputeMatricesForNonNegativeMatrixFactorization(iNumfeatures, numIterations);
  
  string person = "4";
  vector< PDI > v = TopMatches(person, true, true, 10, SimilarityMetric);
  REP(i, v.sz)cout<<person<<"\t"<<users[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl;
  
  v = GetOtherEntityRecommendationsBasedOnCurrentEntitySimilarities(person, true, true, SimilarityMetric);
  REP(i, min(10,(int)v.sz))cout<<items[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl<<endl;
  
  string item = "Star Wars (1977)";//"Star Wars: Episode IV - A New Hope (1977)";
  v = TopMatches(item, false, true, 10, SimilarityMetric);
  REP(i, v.sz)cout<<item<<"\t"<<items[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl;
  
  string item2 = "Seven (Se7en) (1995)";
  v = TopMatches(item2, false, true, 10, SimilarityMetric);
  REP(i, v.sz)cout<<item2<<"\t"<<items[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl;
  //v = GetOtherEntityRecommendationsBasedOnCurrentEntitySimilarities(item2, false, true, SimilarityMetric);
  //REP(i, min(10, (int)v.sz))cout<<users[v[i].second]<<"\t"<<v[i].first<<endl;
  //cout<<endl<<endl;
  string item3 = "Die Hard: With a Vengeance (1995)";
  v = TopMatches(item3, false, true, 10, SimilarityMetric);
  REP(i, v.sz)cout<<item3<<"\t"<<items[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl;
  
  string item4 = "Raiders of the Lost Ark (1981)";
  v = TopMatches(item4, false, true, 10, SimilarityMetric);
  REP(i, v.sz)cout<<item4<<"\t"<<items[v[i].second]<<"\t"<<v[i].first<<endl;
  cout<<endl;
  
  string starwars6 = "Return of the Jedi (1983)", starwars5 = "Empire Strikes Back, The (1980)";
  //string starwars6 = "Star Wars: Episode VI - Return of the Jedi (1983)", starwars5 = "Star Wars: Episode V - The Empire Strikes Back (1980)";
  cout<<Similarity_Pearson(item, starwars5, false, true)<<" "<<Similarity_L2Norm(item, starwars5, false, true)<<" "<<Similarity_Cosine(item, starwars5, false, true)<<endl;
  cout<<Similarity_Pearson(item, starwars6, false, true)<<" "<<Similarity_L2Norm(item, starwars6, false, true)<<" "<<Similarity_Cosine(item, starwars6, false, true)<<endl;
}

double GetPrediction(int givenuserid, int itemid, bool isUserSimilarityMethod, bool isLowDimensional, double (*SimilarityMetric)(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional) = Similarity_Pearson_NormWeighted)
{
  //static int count = 0;
  //count++;
  double predictedRating = 0.0;
  double similaritySum = 0.0;
  
  if(isUserSimilarityMethod)
  {
    REP(userid, numusers)if(userid != givenuserid && isuseritemtrainrating[userid][itemid])
    {
      double similarity = usersimilarityscores[givenuserid][userid]; //(*SimilarityMetric)(givenuserid, userid, true, isLowDimensional);
      predictedRating += similarity * (useritemrating[userid][itemid] - meanuserrating[userid]);
      similaritySum += abs(similarity);
    }
    if(similaritySum > 0) predictedRating /= similaritySum;
    predictedRating += meanuserrating[givenuserid];
  }
  else
  {
    REP(itemid2, numitems)if(itemid2 != itemid && isuseritemtrainrating[givenuserid][itemid2])
    {
      double similarity = itemsimilarityscores[itemid][itemid2]; //(*SimilarityMetric)(itemid, itemid2, false, isLowDimensional);
      predictedRating += similarity * (useritemrating[givenuserid][itemid2] - meanitemrating[itemid2]);
      similaritySum += abs(similarity);
    }
    
    if(similaritySum > 0) predictedRating /= similaritySum;
    predictedRating += meanitemrating[itemid];
  }
  
  //if(count < 10)cout<<users[givenuserid]<<":"<<items[itemid]<<": Actual:"<<useritemrating[givenuserid][itemid]<<" Estimate:"<<predictedRating<<endl;
  return predictedRating;
}

double ComputeUserUserSimilarities(bool isLowDimensional, double (*SimilarityMetric)(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional) = Similarity_Pearson_NormWeighted)
{
  // clock_t start = clock();
  REP(userid, numusers)
  {
    usersimilarityscores[userid][userid] = 1.0;
    FOR(userid2, userid+1, numusers)
    {
      double similarity = (*SimilarityMetric)(userid, userid2, true, isLowDimensional);
      usersimilarityscores[userid][userid2] = usersimilarityscores[userid2][userid] = similarity;
    }
    
    // if(userid % 100 == 0)
    // {
      // clock_t end = clock();
      // cout<<"UserId: "<<userid<<" Time taken: "<<(double)(end-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
      // start = end;
    // }
  }
  
}

double GetErrorOnTestSet(int errorType, bool isUserSimilarityMethod, bool isLowDimensional, double (*SimilarityMetric)(int entityidA, int entityidB, bool isUserEntity, bool isLowDimensional) = Similarity_Pearson_NormWeighted)
{
  clock_t start = clock();
  if(!isLowDimensional)
  {
    if(isUserSimilarityMethod)ComputeUserUserSimilarities(isLowDimensional, SimilarityMetric);
    else ComputeItemItemSimilarities(-1, isLowDimensional, SimilarityMetric);
  }
  clock_t end = clock();
  //cout<<" Time taken: "<<(double)(end-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;

  
  int numberOfItemsInTestSet = 0;
  double error = 0, rmseerror = 0;
  REP(userid, numusers)REP(itemid, numitems)if(isuseritemrating[userid][itemid] && !isuseritemtrainrating[userid][itemid])
  {
    double prediction = 0.0, tmpsum = 0.0;
    if(!isLowDimensional)prediction = GetPrediction(userid, itemid, isUserSimilarityMethod, isLowDimensional, SimilarityMetric);
    else
    {
      prediction = meanuserrating[userid] + meanitemresidualrating[itemid]; //meanitemrating[itemid] + meanuserresidualrating[userid];
      REP(featureid, numfeatures)
      {
        tmpsum = userfeaturematrix[userid][featureid] * featureitemmatrix[featureid][itemid];
        //ClipRating(tmpsum);
        prediction +=  tmpsum;
      }
    }
    
    if(errorType == ERROR_TYPE_MAE)error += abs(prediction - useritemrating[userid][itemid]);
    else if(errorType == ERROR_TYPE_RMSE)error += (prediction - useritemrating[userid][itemid]) * (prediction - useritemrating[userid][itemid]);
    rmseerror += (prediction - useritemrating[userid][itemid]) * (prediction - useritemrating[userid][itemid]);
    numberOfItemsInTestSet++;
    
    // if(numberOfItemsInTestSet % 10000 == 0)
    // {
      // end = clock();
      // cout<<"numberOfItemsInTestSet:"<<numberOfItemsInTestSet<<" Time taken: "<<(double)(end-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
      // start = clock();
    // }
  }
  
  if(numberOfItemsInTestSet > 0)error /= numberOfItemsInTestSet;
  if(errorType == ERROR_TYPE_RMSE)error = sqrt(error);
  rmseerror /= numberOfItemsInTestSet;
  rmseerror = sqrt(rmseerror);
  // cout<<"RMSE:"<<rmseerror<<endl;

  end = clock();
  // cout<<"Time taken: "<<(double)(end-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  // if(errorType == ERROR_TYPE_MAE)cout<<"MAE:"<<error<<endl;
  // else if(errorType == ERROR_TYPE_RMSE)cout<<"RMSE:"<<error<<endl;
  
  return error;
}

// static inline void ClipRating(double &rating)
// {
  // bool isnegative = (rating < 0);
  // if(isnegative)rating *= -1;
  // if(rating > 5.0)rating = 5.0;
  // if(rating < 1.0)rating = 1.0;
  // if(isnegative)rating *= -1;
// }


void DoRegularizedSVD(double regularizationConstant, int numLatentFactors)
{
  cout<<"#NumLatentFactors:"<<numLatentFactors<<" RegularizationConstant:"<<regularizationConstant<<endl;
  cout<<"#Iteration RMSEerror TimeTakenInSeconds"<<endl;
  memset(userfeaturematrix, 0, sizeof(userfeaturematrix));
  memset(featureitemmatrix, 0, sizeof(featureitemmatrix));
  numfeatures = numLatentFactors;
 
  srand(time(NULL));
  REP(featureid, numfeatures)
  {
    REP(userid, numusers)userfeaturematrix[userid][featureid] = ((double)rand() / (double)RAND_MAX);
    REP(itemid, numitems)featureitemmatrix[featureid][itemid] = ((double)rand() / (double)RAND_MAX);
  }
  
  double estimatedRating = 0.0, currentError = 0.0, regularizationterm = 0.0, stepsize = 0.0, tmpsum  = 0.0;//, tmpuf = 0.0, tmpfi = 0.0;
  REP(iteration, MAX_REGULARIZEDSVD_ITERATIONS)
  {    
    clock_t start = clock();
    stepsize = min(0.001, 1 / (double)(iteration + 1)); //adaptive step size = 1/iteration
    regularizationterm = regularizationConstant * stepsize;
    REP(userid, numusers)REP(itemid, numitems)if(isuseritemtrainrating[userid][itemid])
    {
      estimatedRating = meanuserrating[userid] + meanitemresidualrating[itemid]; //meanitemrating[itemid] + meanuserresidualrating[userid];
      REP(featureid, numfeatures)
      {
        tmpsum = userfeaturematrix[userid][featureid] * featureitemmatrix[featureid][itemid];
        //ClipRating(tmpsum);
        estimatedRating +=  tmpsum;
      }
  
      //ClipRating(estimatedRating);

      currentError = (useritemrating[userid][itemid] - estimatedRating) * stepsize; 
      REP(featureid, numfeatures)
      {
        const double tmpuf = userfeaturematrix[userid][featureid];
        const double tmpfi = featureitemmatrix[featureid][itemid];
        userfeaturematrix[userid][featureid] += currentError * tmpfi - regularizationterm * tmpuf;
        featureitemmatrix[featureid][itemid] += currentError * tmpuf - regularizationterm * tmpfi;
      }
    }
    
    double error = GetErrorOnTestSet(ERROR_TYPE_RMSE, true, true, Similarity_Pearson_NormWeighted);
    clock_t end = clock();
    //cout<<"Iteration:"<<iteration<<" Time taken: "<<(double)(end-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
    cout<<iteration<<" "<<error<<" "<<(double)(end-start)/(double)CLOCKS_PER_SEC<<endl;
  }  
}

int main()
{
  //Initialize
  Initialize();
    
  //Load the User Item Ratings Input File
  //LoadMovieLensData("C:\\Users\\Arvind\\Desktop\\u100k.data", "C:\\Users\\Arvind\\Desktop\\u100k.item", "\t", "|");
  LoadMovieLensData("C:\\Users\\Arvind\\Desktop\\ratings1M.dat", "C:\\Users\\Arvind\\Desktop\\movies1M.dat", "::", "::");
  //LoadSampleDatasetForSlopeOne();
  //LoadSampleDatasetForNMF();
  
  //TestMovieLensData();
  
  CreateTrainTestSplits(80, true);
  
  ComputeFrequenciesAndMeanRatingsForUsersAndItems();
  
  //TestSimilarityFunctionsForMovieLensDataset();
    
  //TestMovieLensUserItemRecommendationSystem();
  //TestMovieLensItemItemRecommendationSystem();
  
  //TestSlopeOne();
  
  //TestConstantValue();
  //TestRandomValue();
  //TestAverageOfUserRating();
  //TestAverageOfItemRating();
  
  //TestNonNegativeMatrixFactorization(100, 100);
  
  // GetErrorOnTestSet(ERROR_TYPE_RMSE, true, false, Similarity_Cosine);
  // GetErrorOnTestSet(ERROR_TYPE_RMSE, true, false, Similarity_Pearson);
  // GetErrorOnTestSet(ERROR_TYPE_RMSE, true, false, Similarity_Pearson_NormWeighted);
  // GetErrorOnTestSet(ERROR_TYPE_RMSE, true, false, Similarity_DefaultVoting_Cosine);
  // GetErrorOnTestSet(ERROR_TYPE_RMSE, true, false, Similarity_DefaultVoting_Pearson);
  // GetErrorOnTestSet(ERROR_TYPE_RMSE, true, false, Similarity_InverseUserFrequency_Cosine);
  // GetErrorOnTestSet(ERROR_TYPE_RMSE, true, false, Similarity_InverseUserFrequency_Pearson);
  // GetErrorOnTestSet(ERROR_TYPE_RMSE, true, false, Similarity_CaseAmplification_Cosine);
  // GetErrorOnTestSet(ERROR_TYPE_RMSE, true, false, Similarity_CaseAmplification_Pearson);
  
  //GetErrorOnTestSet(ERROR_TYPE_RMSE, true, false, Similarity_Pearson_NormWeighted);
  // GetErrorOnTestSet(ERROR_TYPE_MAE, false, false, Similarity_Pearson_NormWeighted);
  DoRegularizedSVD(0, 25);
  
  return 0;
  
}

