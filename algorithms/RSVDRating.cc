#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <queue>
#include <sstream>
#include <cstring>
#include <cmath>
using namespace std;
#define LET(x,a) typeof(a) x(a)
#define FOR(i, a, n) for(LET(i, a); i < n; ++i)
#define REP(i, n) FOR(i, 0, n)
#define pb push_back
#define sz size()
#define cs c_str()
#define EACH(it, v) FOR(LET(it, v.begin()); it != v.end(); ++it)
#define dbg(x) cout<<#x<<":"<<(x)<<"\t";
#define dbge(x) cout<<#x<<":"<<(x)<<"\n";
#define s2i(_str) ({stringstream sin(_str); int _x; sin>>_x; _x;})
#define s2d(_str) ({stringstream sin(_str); float _x; sin>>_x; _x;})
#define VI vector<int>
#define PDD pair< float, float >
#define PDDD pair< float, PDD >
#define EPS (float)1e-6
typedef long long LL;
string spaces = " \t\r\n";
const int MAX_NUMBER_OF_USERS = 2400000, MAX_NUMBER_OF_ITEMS = 6500, MAX_LEN_OF_LINE_IN_FILE = 10000;
const int MAX_REGULARIZEDSVD_ITERATIONS = 50, MAX_NUMBER_OF_FEATURES = 75;

VI user2itemratingvector[MAX_NUMBER_OF_USERS];
VI item2uservector[MAX_NUMBER_OF_ITEMS];
int *item2userid[MAX_NUMBER_OF_ITEMS];
float *item2userrating[MAX_NUMBER_OF_ITEMS];
int *user2itemid[MAX_NUMBER_OF_USERS];
float *user2itemrating[MAX_NUMBER_OF_USERS];
VI validationuser2itemrating[MAX_NUMBER_OF_USERS];
VI publictestuser2itemrating[MAX_NUMBER_OF_USERS];
VI privatetestuser2itemrating[MAX_NUMBER_OF_USERS];
float userfeaturematrix[MAX_NUMBER_OF_USERS][MAX_NUMBER_OF_FEATURES];
float itemfeaturematrix[MAX_NUMBER_OF_ITEMS][MAX_NUMBER_OF_FEATURES];

float globalbias = -0.812561997812;
float userbias[MAX_NUMBER_OF_USERS], userbiasoffset[MAX_NUMBER_OF_USERS];
float itembias[MAX_NUMBER_OF_ITEMS], itembiasoffset[MAX_NUMBER_OF_ITEMS];
float itemitemcosinesimilarityscores[MAX_NUMBER_OF_ITEMS][MAX_NUMBER_OF_ITEMS];
float itemitempearsonsimilarityscores[MAX_NUMBER_OF_ITEMS][MAX_NUMBER_OF_ITEMS];
int numusers = 0, numitems = MAX_NUMBER_OF_ITEMS, numfeatures = 0;

void Initialize()
{
  numusers = 0, numitems = MAX_NUMBER_OF_ITEMS, numfeatures = 0;

  REP(userid, MAX_NUMBER_OF_USERS)
  {
    user2itemratingvector[userid] = vector<int>();
    validationuser2itemrating[userid] = vector<int>();
    publictestuser2itemrating[userid] = vector<int>();
    privatetestuser2itemrating[userid] = vector<int>();

    user2itemid[userid] = new int[1];
    user2itemid[userid][0] = 0;

    user2itemrating[userid] = new float[1];
    user2itemrating[userid][0] = 0;
  }

  REP(itemid, MAX_NUMBER_OF_ITEMS)
  {
    item2uservector[itemid] = vector<int>();

    item2userid[itemid] = new int[1];
    item2userid[itemid][0] = 0;

    item2userrating[itemid] = new float[1];
    item2userrating[itemid][0] = 0;
  }

  memset(itemitemcosinesimilarityscores, 0, sizeof(itemitemcosinesimilarityscores));
  memset(itemitempearsonsimilarityscores, 0, sizeof(itemitempearsonsimilarityscores));
}

void StripSpaces(string &str)
{
  while(str.sz && spaces.find(str[0]) != string::npos)str = str.substr(1);
  while(str.sz && spaces.find(str[str.sz-1]) != string::npos)str = str.substr(0, str.sz-1);
}

void LoadBias(string file, string dataseparator, string biastype)
{
  ifstream fin(file.cs);
  if(!fin){ cout<<"Could not open file:"<<file<<endl; exit(0); }
  int lineno = -1;
  char buff[MAX_LEN_OF_LINE_IN_FILE];
  
  clock_t start = clock();
  while(fin)
  {
    lineno++;
    if(fin.eof())break;
    fin.getline(buff, MAX_LEN_OF_LINE_IN_FILE);
    if(lineno == 0)continue;
    
    string sbuff = (string)buff; StripSpaces(sbuff);
    if(0 == sbuff.sz || sbuff.find(dataseparator)==string::npos)continue;
    int tabindex = sbuff.find(dataseparator);
    string user = sbuff.substr(0, tabindex); int id = s2i(user); sbuff = sbuff.substr(tabindex+dataseparator.sz);
    tabindex = sbuff.find(dataseparator); int cnt = s2i(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex+dataseparator.sz);
    tabindex = sbuff.find(dataseparator); float bias = s2d(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex+dataseparator.sz);
    
    if(biastype == "userbias")userbias[id] = bias;
    else if(biastype == "itembias" && id < numitems)itembias[id] = bias;
    else if(biastype == "userbiasoffset")userbiasoffset[id] = bias;
    else if(biastype == "itembiasoffset" && id < numitems)itembiasoffset[id] = bias;

    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  fin.close();
}

void LoadTrainingData(string file, string dataseparator)
{
  ifstream fin(file.cs);
  if(!fin){ cout<<"Could not open file:"<<file<<endl; exit(0); }
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
    tabindex = sbuff.find(dataseparator); int rating = s2i(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex+dataseparator.sz);
  
    user2itemratingvector[userid].pb(itemid * rating);
    item2uservector[itemid].pb(userid * rating);

    if(userid > numusers)numusers = userid;
    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;

  numusers += 1;
  fin.close();

  REP(userid, numusers)
  {
    int numItemsRated = user2itemratingvector[userid].sz;
    if(numItemsRated == 0)continue;
    user2itemid[userid] = new int[numItemsRated + 1];
    user2itemid[userid][0] = numItemsRated;
    user2itemrating[userid] = new float[numItemsRated + 1];
    user2itemrating[userid][0] = numItemsRated;

    REP(index, numItemsRated)
    {
      float rating = 1.0;
      int itemid = user2itemratingvector[userid][index];
      if(itemid < 0)itemid *= -1, rating = -1;
      rating = rating - itembias[itemid] - userbiasoffset[userid];

      user2itemid[userid][index + 1] = itemid;
      user2itemrating[userid][index + 1] = rating;
    }
  }

  REP(itemid, numitems)
  {
    int numUsersRated = item2uservector[itemid].sz;
    if(numUsersRated == 0)continue;
    item2userid[itemid] = new int[numUsersRated + 1];
    item2userid[itemid][0] = numUsersRated;
    item2userrating[itemid] = new float[numUsersRated + 1];
    item2userrating[itemid][0] = numUsersRated;

    REP(index, numUsersRated)
    {
      float rating = 1.0;
      int userid = item2uservector[itemid][index];
      if(userid < 0)userid *= -1, rating = -1;
      rating = rating - itembias[itemid] - userbiasoffset[userid];

      item2userid[itemid][index + 1] = userid;
      item2userrating[itemid][index + 1] = rating;
    }
  }
}


void LoadValidationData(string file, string dataseparator)
{
  ifstream fin(file.cs);
  if(!fin){ cout<<"Could not open file:"<<file<<endl; exit(0); }
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
    tabindex = sbuff.find(dataseparator); int rating = s2i(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex+dataseparator.sz);
  
    validationuser2itemrating[userid].pb(itemid * rating);

    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  fin.close();
}


void LoadPublicTestData(string file, string dataseparator)
{
  ifstream fin(file.cs);
  if(!fin){ cout<<"Could not open file:"<<file<<endl; exit(0); }
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
    tabindex = sbuff.find(dataseparator); int rating = s2i(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex+dataseparator.sz);
  
    publictestuser2itemrating[userid].pb(itemid * rating);

    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  fin.close();
}

void LoadPrivateTestData(string file, string dataseparator)
{
  ifstream fin(file.cs);
  if(!fin){ cout<<"Could not open file:"<<file<<endl; exit(0); }
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
    tabindex = sbuff.find(dataseparator); int rating = s2i(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex+dataseparator.sz);
  
    privatetestuser2itemrating[userid].pb(itemid * rating);

    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  fin.close();
}

float Similarity_Pearson(int userid, int itemid)
{
  float sum1 = 0, sum2 = 0, sum1sq = 0, sum2sq = 0, psum = 0;
  int overlaps = 0;
  REP(featureid, numfeatures)
  {
    overlaps++;
    sum1 += userfeaturematrix[userid][featureid];
    sum2 += itemfeaturematrix[itemid][featureid];
    sum1sq += userfeaturematrix[userid][featureid] * userfeaturematrix[userid][featureid];
    sum2sq += itemfeaturematrix[itemid][featureid] * itemfeaturematrix[itemid][featureid];
    psum += userfeaturematrix[userid][featureid] * itemfeaturematrix[itemid][featureid];

  }

  float correlation = -1;

  if(overlaps == 0)correlation = 0;
  else
  {
    float prod1 = (sum1sq - (sum1*sum1)/overlaps), prod2 = (sum2sq - (sum2*sum2)/overlaps);
    float num = psum - (sum1*sum2)/overlaps;

    if(prod1 * prod2 < 0)correlation = 0;
    else
    {
      float denom = sqrt(prod1 * prod2);
      if(denom < EPS)correlation = 0;
      else correlation = num/denom;
    }
  }

  return correlation;
}

float Similarity_Cosine(int userid, int itemid)
{
  float num = 0,  norm1 = 0, norm2 = 0;

  REP(featureid, numfeatures)
  {
    num += userfeaturematrix[userid][featureid] * itemfeaturematrix[itemid][featureid];
    norm1 += userfeaturematrix[userid][featureid] * userfeaturematrix[userid][featureid];
    norm2 += itemfeaturematrix[itemid][featureid] * itemfeaturematrix[itemid][featureid];
  }

  norm1 = sqrt(norm1); norm2 = sqrt(norm2);
  float prod = norm1 * norm2;

  if(prod < EPS && prod > -1 * EPS)return 0;
  return num / prod;
}

void OutputFeaturesToFile(string file, VI *v)
{
  cout<<"Processing file:"<<file<<endl;
  ofstream fout(file.cs);
  clock_t start = clock();
  int numUsersProcessed = 0;

  int lineno = 0;
  REP(userid, numusers)
  {
    int numItemsForUser = v[userid].sz;
    if(numItemsForUser == 0)continue;
    numUsersProcessed++;
    
    REP(itemIndex, numItemsForUser)
    {
      int itemid = v[userid][itemIndex];
      if(itemid < 0)itemid *= -1;

      float estimatedRating = 0.0f;
      REP(featureid, numfeatures)estimatedRating += userfeaturematrix[userid][featureid] * itemfeaturematrix[itemid][featureid];

      float cosineSimilarity = Similarity_Cosine(userid, itemid), pearsonSimilarity = Similarity_Pearson(userid, itemid);
      fout<<userid<<" "<<itemid<<" "<<estimatedRating<<" "<<cosineSimilarity<<" "<<pearsonSimilarity<<endl;
      lineno++;
      
      if(lineno % 1000000 == 0)cout<<"Time taken to output "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
    }
  }

  fout.close();
  cout<<"Time taken to output "<< lineno <<" lines is: "<<(float)(clock()-start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
}

float GetErrorOnValidationSet(int iteration)
{
  float error = 0.0f, estimatedRating = 0.0f;
  int numberOfItemsInTestSet = 0;
  REP(userid, numusers)
  {
    int numItemsRated = validationuser2itemrating[userid].sz;
    if(numItemsRated == 0)continue;

    REP(itemIndex, numItemsRated)
    {
      int itemid = validationuser2itemrating[userid][itemIndex];
      float rating = 1.0f;
      if(itemid < 0)itemid *= -1, rating = -1.0f;

      estimatedRating = itembias[itemid] + userbiasoffset[userid];
      REP(featureid, numfeatures)estimatedRating += userfeaturematrix[userid][featureid] * itemfeaturematrix[itemid][featureid];

      error += (estimatedRating - rating) * (estimatedRating - rating);
      numberOfItemsInTestSet++;
    }
  }

  if(numberOfItemsInTestSet > 0)error /= numberOfItemsInTestSet;
  error = sqrt(error);

  if(iteration == 0)dbge(numberOfItemsInTestSet);
  return error;
}

void DoRegularizedSVD(float regularizationConstant, int numLatentFactors)
{
  cout<<"DoRegularizedSVD invoked with regularizationConstant:"<<regularizationConstant<<" numLatentFactors:"<<numLatentFactors<<endl;
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
  }

  float estimatedRating = 0.0, currentError = 0.0, regularizationterm = 0.0, stepsize = 0.0, tmpuf = 0.0, tmpif = 0.0;
  
  REP(iteration, MAX_REGULARIZEDSVD_ITERATIONS)
  {    
    clock_t start = clock();
    float rmse = 0.0f;
    int numLinesProcessed = 0;
    stepsize = min(0.001f, 1 / (float)(iteration + 1)); //adaptive step size = 1/iteration
    regularizationterm = regularizationConstant * stepsize;

    REP(userid, numusers)
    {
      int numItemsRated = user2itemid[userid][0];
      if(numItemsRated == 0)continue;

      REP(itemIndex, numItemsRated)
      {
        numLinesProcessed++;
        float rating = user2itemrating[userid][itemIndex + 1];
        int itemid = user2itemid[userid][itemIndex + 1];

        estimatedRating = 0.0;

        REP(featureid, numfeatures)estimatedRating += userfeaturematrix[userid][featureid] * itemfeaturematrix[itemid][featureid];

        //Clip Ratings
        // if(estimatedRating > 1.0)estimatedRating = 1.0;
        // else if(estimatedRating < -1.0)estimatedRating = -1.0;

        rmse += (rating - estimatedRating) * (rating - estimatedRating);
        currentError = (rating - estimatedRating) * stepsize; 

        REP(featureid, numfeatures)
        {
          tmpuf = userfeaturematrix[userid][featureid];
          tmpif = itemfeaturematrix[itemid][featureid];

          userfeaturematrix[userid][featureid] += currentError * tmpif - regularizationterm * tmpuf;
          itemfeaturematrix[itemid][featureid] += currentError * tmpuf - regularizationterm * tmpif;
        }      
      }
    }
    
    if (numLinesProcessed > 0)rmse /= numLinesProcessed;
    rmse = sqrt(rmse);

    cout<<"Iteration:"<<iteration<<" RMSEerror:"<<rmse<<" Time taken for training: "<<(float)(clock() - start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;

    REP(i, 5)
    {
      REP(featureid, 5)
      {
        cout<<userfeaturematrix[i][featureid]<<","<<itemfeaturematrix[i][featureid]<<" ";
      }
      cout<<endl;
    }

    //if(iteration == 0)cout<<"Iteration:"<<iteration<<" Time taken for training: "<<(float)(clock() - start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;

    //Get Error on Validation Set
    clock_t validationstart = clock();
    float error = GetErrorOnValidationSet(iteration);
    cout<<"Iteration:"<<iteration<<" ValidationRMSEerror:"<<error<<" Time taken for validation: "<<(float)(clock() - validationstart)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;

    // clock_t end = clock();
    // if(iteration == 0)cout<<"Iteration:"<<iteration<<" RMSE Error:"<<error<<" Time taken for complete iteration: "<<(float)(end - start)/(float)CLOCKS_PER_SEC<<" seconds"<<endl;
    // cout<<iteration<<" "<<error<<" "<<(float)(end-start)/(float)CLOCKS_PER_SEC<<endl;

  }
}

int main()
{
  Initialize();
  
  LoadBias("/home/arvind/track1/user_bias.txt", " ", "userbias");
  LoadBias("/home/arvind/track1/item_bias.txt", " ", "itembias");
  LoadBias("/home/arvind/track1/user_bias_offset.txt", " ", "userbiasoffset");
  LoadBias("/home/arvind/track1/item_bias_offset.txt", " ", "itembiasoffset");

  LoadTrainingData("/home/arvind/track1/rec_log_train_sorted_mapped_train.txt", " ");
  LoadValidationData("/home/arvind/track1/rec_log_train_sorted_mapped_validation.txt", " ");
  LoadPublicTestData("/home/arvind/track1/rec_log_test_private_result_sorted_mapped.txt", " ");
  LoadPrivateTestData("/home/arvind/track1/rec_log_test_public_result_sorted_mapped.txt", " ");

  DoRegularizedSVD(0.001, 75);

  OutputFeaturesToFile("/home/arvind/track1/feature_rsvdrating_train.txt", user2itemratingvector);
  OutputFeaturesToFile("/home/arvind/track1/feature_rsvdrating_validation.txt", validationuser2itemrating);
  OutputFeaturesToFile("/home/arvind/track1/feature_rsvdrating_publictest.txt", publictestuser2itemrating);
  OutputFeaturesToFile("/home/arvind/track1/feature_rsvdrating_privatetest.txt", privatetestuser2itemrating);
  
  
  return 0;
}
