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
#define s2d(_str) ({stringstream sin(_str); double _x; sin>>_x; _x;})
#define VI vector<int>
#define PDD pair< double, double >
#define PDDD pair< double, PDD >
#define EPS (double)1e-10
typedef long long LL;
string spaces = " \t\r\n";
const int MAX_NUMBER_OF_USERS = 2400000, MAX_NUMBER_OF_ITEMS = 6500, MAX_LEN_OF_LINE_IN_FILE = 10000;

VI user2itemrating[MAX_NUMBER_OF_USERS];
VI item2uservector[MAX_NUMBER_OF_ITEMS];
int *item2userid[MAX_NUMBER_OF_ITEMS];
double *item2userrating[MAX_NUMBER_OF_ITEMS];
VI validationuser2itemrating[MAX_NUMBER_OF_USERS];
VI publictestuser2itemrating[MAX_NUMBER_OF_USERS];
VI privatetestuser2itemrating[MAX_NUMBER_OF_USERS];

double globalbias = -0.812561997812;
double userbias[MAX_NUMBER_OF_USERS], userbiasoffset[MAX_NUMBER_OF_USERS];
double itembias[MAX_NUMBER_OF_ITEMS], itembiasoffset[MAX_NUMBER_OF_ITEMS];
double itemitemcosinesimilarityscores[MAX_NUMBER_OF_ITEMS][MAX_NUMBER_OF_ITEMS];
double itemitempearsonsimilarityscores[MAX_NUMBER_OF_ITEMS][MAX_NUMBER_OF_ITEMS];
int numusers = 0, numitems = MAX_NUMBER_OF_ITEMS;

void Initialize()
{
  numusers = 0, numitems = MAX_NUMBER_OF_ITEMS;

  REP(userid, MAX_NUMBER_OF_USERS)
  {
    user2itemrating[userid] = vector<int>();
    validationuser2itemrating[userid] = vector<int>();
    publictestuser2itemrating[userid] = vector<int>();
    privatetestuser2itemrating[userid] = vector<int>();
  }

  REP(itemid, MAX_NUMBER_OF_ITEMS)
  {
    item2uservector[itemid] = vector<int>();

    item2userid[itemid] = new int[1];
    item2userid[itemid][0] = 0;

    item2userrating[itemid] = new double[1];
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
    tabindex = sbuff.find(dataseparator); double bias = s2d(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex+dataseparator.sz);
    
    if(biastype == "userbias")userbias[id] = bias;
    else if(biastype == "itembias" && id < numitems)itembias[id] = bias;
    else if(biastype == "userbiasoffset")userbiasoffset[id] = bias;
    else if(biastype == "itembiasoffset" && id < numitems)itembiasoffset[id] = bias;

    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  
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
  
    user2itemrating[userid].pb(itemid * rating);
    item2uservector[itemid].pb(userid * rating);

    if(userid > numusers)numusers = userid;
    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;

  numusers += 1;
  fin.close();

  REP(itemid, numitems)
  {
    int numUsersRated = item2uservector[itemid].sz;
    if(numUsersRated == 0)continue;
    item2userid[itemid] = new int[numUsersRated + 1];
    item2userid[itemid][0] = numUsersRated;
    item2userrating[itemid] = new double[numUsersRated + 1];
    item2userrating[itemid][0] = numUsersRated;

    REP(index, numUsersRated)
    {
      double rating = 1.0;
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

    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  
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

    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  
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

    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  fin.close();
}

double Similarity_Pearson(int itemidA, int itemidB)
{
  double sum1 = 0, sum2 = 0, sum1sq = 0, sum2sq = 0, psum = 0;
  int overlaps = 0;
  int numUsersRatedItemA = item2userrating[itemidA][0], numUsersRatedItemB = item2userrating[itemidB][0];
  int userIndexA = 0, userIndexB = 0;

  while(userIndexA < numUsersRatedItemA && userIndexB < numUsersRatedItemB)
  {
    int useridA = item2userid[itemidA][userIndexA + 1], useridB = item2userid[itemidB][userIndexB + 1];

    if(useridA != useridB)
    {
      if(useridA < useridB)userIndexA++;
      else userIndexB++;

      continue;
    }

    double ratingA = item2userrating[itemidA][userIndexA + 1], ratingB = item2userrating[itemidB][userIndexB + 1];
    overlaps++;
    sum1 += ratingA;
    sum2 += ratingB;
    sum1sq += ratingA * ratingA;
    sum2sq += ratingB * ratingB;
    psum += ratingA * ratingB;

    userIndexA++;
    userIndexB++;
  }

  double correlation = -1;

  if(overlaps == 0)correlation = 0;
  else
  {
    double prod1 = (sum1sq - (sum1*sum1)/overlaps), prod2 = (sum2sq - (sum2*sum2)/overlaps);
    double num = psum - (sum1*sum2)/overlaps;

    if(prod1 * prod2 < 0)correlation = 0;
    else
    {
      double denom = sqrt(prod1 * prod2);
      if(denom < EPS)correlation = 0;
      else correlation = num/denom;
    }
  }

  return correlation;
}

double Similarity_Cosine(int itemidA, int itemidB)
{
  double num = 0,  norm1 = 0, norm2 = 0;
  int numUsersRatedItemA = item2userid[itemidA][0], numUsersRatedItemB = item2userid[itemidB][0];
  int userIndexA = 0, userIndexB = 0;

  while(userIndexA < numUsersRatedItemA && userIndexB < numUsersRatedItemB)
  {
    int useridA = item2userid[itemidA][userIndexA + 1], useridB = item2userid[itemidB][userIndexB + 1];

    if(useridA != useridB)
    {
      if(useridA < useridB)userIndexA++;
      else userIndexB++;

      continue;
    }

    double ratingA = item2userrating[itemidA][userIndexA + 1], ratingB = item2userrating[itemidB][userIndexB + 1];
    num += ratingA * ratingB;
    norm1 += ratingA * ratingA;
    norm2 += ratingB * ratingB;
    userIndexA++;
    userIndexB++;
  }

  norm1 = sqrt(norm1); norm2 = sqrt(norm2);
  double prod = norm1 * norm2;

  if(prod < EPS && prod > -1 * EPS)return 0;
  return num / prod;
}

//TODO: Uncomment this
void ComputeItemItemSimilarities()//(double (*SimilarityMetric)(int itemidA, int itemidB) = Similarity_Pearson_NormWeighted)
{
  clock_t start = clock();
  REP(itemid, numitems)
  {
    FOR(itemid2, itemid + 1, numitems)
    {
      //TODO: Uncomment this
      //double similarity = Similarity_Cosine(itemid, itemid2); //(*SimilarityMetric)(itemid, itemid2);

      //itemitemcosinesimilarityscores[itemid][itemid2] = itemitemcosinesimilarityscores[itemid2][itemid] = similarity;
      
      double similarity = Similarity_Pearson(itemid, itemid2); //(*SimilarityMetric)(itemid, itemid2);

      itemitempearsonsimilarityscores[itemid][itemid2] = itemitempearsonsimilarityscores[itemid2][itemid] = similarity;

    }

      //cout<<itemid<<" "<<(double)(clock() - start)/(double)CLOCKS_PER_SEC<<endl;
    if(itemid % 1000 == 0)cout<<"Time taken to Compute Item Item Similarities for numItems:"<<itemid<<" is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  }

  cout<<"Time taken to Compute Item Item Similarities is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;

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
    
      //double cosine_similarity = 0, cosine_similarity_sum = 0;
      double pearson_similarity = 0, pearson_similarity_sum = 0;

      int numItemsRatedByUser = user2itemrating[userid].sz;
      REP(itemIndex2, numItemsRatedByUser)
      {
        int itemid2 = user2itemrating[userid][itemIndex2];
        double rating = 1;
        if(itemid2 < 0)itemid2 *= -1, rating = -1;
        rating = rating - itembias[itemid2] - userbiasoffset[userid];

        //cosine_similarity += rating * itemitemcosinesimilarityscores[itemid][itemid2];
        pearson_similarity += rating * itemitempearsonsimilarityscores[itemid][itemid2];
        //cosine_similarity_sum += abs(cosine_similarity);
        pearson_similarity_sum += abs(pearson_similarity);
      }

      //if(cosine_similarity_sum > EPS)cosine_similarity /= cosine_similarity_sum;
      if(pearson_similarity_sum > EPS)pearson_similarity /= pearson_similarity_sum;

      fout<<userid<<" "<<itemid<<" "<<pearson_similarity<<endl;//" "<<cosine_similarity<<endl;
      lineno++;
      
      if(lineno % 1000000 == 0)cout<<"Time taken to output "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
    }
  }

  fout.close();
  cout<<"Time taken to output "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
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

  cout<<"item item similarity"<<endl;
  ComputeItemItemSimilarities();
  cout<<"output"<<endl;
  OutputFeaturesToFile("/home/arvind/track1/feature_itemitempearsonsimilarity_train.txt", user2itemrating);
  OutputFeaturesToFile("/home/arvind/track1/feature_itemitempearsonsimilarity_validation.txt", validationuser2itemrating);
  OutputFeaturesToFile("/home/arvind/track1/feature_itemitempearsonsimilarity_publictest.txt", publictestuser2itemrating);
  OutputFeaturesToFile("/home/arvind/track1/feature_itemitempearsonsimilarity_privatetest.txt", privatetestuser2itemrating);
  
  
  return 0;
}
