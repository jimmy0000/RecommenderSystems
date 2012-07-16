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
#define PSD pair<string, double> 
#define PDS pair<double, string>
#define PDI pair<double, int>
#define PDD pair<double, double>
#define PDDD pair<double, PDD >
#define s2d(_str) ({stringstream _sout(_str); double _d; _sout>>_d; _d;})
#define s2i(_str) ({stringstream sin(_str); int _x; sin>>_x; _x;})
#define s2l(_str) ({stringstream sin(_str); LL _x; sin>>_x; _x;})
#define num2s(_x) ({stringstream sout; sout<<_x; sout.str();})
#define MAE(trueRatingsVector, estimatedRatingsVector) ({double _mae = 0.0; REP(_i, trueRatingsVector.sz)_mae += abs(trueRatingsVector[_i] - estimatedRatingsVector[_i]); _mae /= trueRatingsVector.sz; _mae;})
#define EPS (double)1e-10
typedef long long LL;
string spaces = " \t\r\n";
const int MAX_NUMBER_OF_USERS = 2400000, MAX_NUMBER_OF_ITEMS = 5000, MAX_LEN_OF_LINE_IN_FILE = 10000;
const LL PUBLIC_TIMESTAMP = 1321891200LL;
const int MAX_REGULARIZEDSVD_ITERATIONS = 250, MAX_NUMBER_OF_FEATURES = 75;

VI user2itemrating[MAX_NUMBER_OF_USERS];
VI item2userrating[MAX_NUMBER_OF_ITEMS];
VI validationuser2itemrating[MAX_NUMBER_OF_USERS];
VI testpublicuser2itemrating[MAX_NUMBER_OF_USERS];
VI testevaluser2itemrating[MAX_NUMBER_OF_USERS];
//TODO: Uncomment this
double userfeaturematrix[1][1]; //[MAX_NUMBER_OF_USERS][MAX_NUMBER_OF_FEATURES];
double itemfeaturematrix[1][1]; //[MAX_NUMBER_OF_ITEMS][MAX_NUMBER_OF_FEATURES];

//TODO: Uncomment this
double itemitemsimilarityscores[MAX_NUMBER_OF_ITEMS][MAX_NUMBER_OF_ITEMS]; //5000 * 5000 * 8 = 200 MB

int numusers = 0, numitems = MAX_NUMBER_OF_ITEMS, numfeatures = 0;

void Initialize()
{
  numusers = 0, numitems = MAX_NUMBER_OF_ITEMS, numfeatures = 0;
  
  REP(userid, MAX_NUMBER_OF_USERS)
  {
    user2itemrating[userid] = vector<int>();
    validationuser2itemrating[userid] = vector<int>();
    testpublicuser2itemrating[userid] = vector<int>();
    testevaluser2itemrating[userid] = vector<int>();
  }
  
  REP(itemid, MAX_NUMBER_OF_ITEMS)item2userrating[itemid] = vector<int>();
  
  memset(itemitemsimilarityscores, 0, sizeof(itemitemsimilarityscores));
  
}

void StripSpaces(string &str)
{
  while(str.sz && spaces.find(str[0]) != string::npos)str=str.substr(1);
  while(str.sz && spaces.find(str[str.sz-1]) != string::npos)str=str.substr(0, str.sz-1);
}


void LoadTrainingData(string datafile, string dataseparator)
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
    fin.getline(buff, MAX_LEN_OF_LINE_IN_FILE);
    string sbuff = (string)buff; StripSpaces(sbuff);
    if(0 == sbuff.sz || sbuff.find(dataseparator)==string::npos)continue;
    int tabindex = sbuff.find(dataseparator);
    string user = sbuff.substr(0, tabindex); int userid = s2i(user); sbuff = sbuff.substr(tabindex+dataseparator.sz);
    tabindex = sbuff.find(dataseparator); int itemid = s2i(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex+dataseparator.sz);
    tabindex = sbuff.find(dataseparator); int rating = s2i(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex+dataseparator.sz);
    // if(rating != -1 && rating != 1)
    // {
      // cout<<"Invalid Rating:"<<rating<<" at line:"<<lineno<<" of training file:"<<datafile<<endl;
      // exit(1);
    // }

    user2itemrating[userid].pb(itemid * rating);
    item2userrating[itemid].pb(userid * rating);
    
    if(userid > numusers)numusers = userid;
    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  numusers += 1;
  dbge(numusers);
  fin.close();
}

void TestTrainingData()
{
  dbge(numusers);
  REP(userid, 10)
  {
    cout<<userid;
    int numItemsRated = user2itemrating[userid].sz;
    REP(i, numItemsRated)cout<<":"<<user2itemrating[userid][i];
    cout<<endl;
  }
  
  cout<<endl<<endl;
  REP(itemid, 10)
  {
    cout<<itemid;
    int numUsersRated = item2userrating[itemid].sz;
    REP(i, numUsersRated)cout<<":"<<item2userrating[itemid][i];
    cout<<endl;
  }
  cout<<endl;
}

void LoadValidationData(string datafile, string dataseparator)
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
    fin.getline(buff, MAX_LEN_OF_LINE_IN_FILE);
    string sbuff = (string)buff; StripSpaces(sbuff);
    if(0 == sbuff.sz || sbuff.find(dataseparator)==string::npos)continue;
    int tabindex = sbuff.find(dataseparator);
    string user = sbuff.substr(0, tabindex); int userid = s2i(user); sbuff = sbuff.substr(tabindex+dataseparator.sz);
    tabindex = sbuff.find(dataseparator); int itemid = s2i(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex+dataseparator.sz);
    tabindex = sbuff.find(dataseparator); int rating = s2i(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex+dataseparator.sz);

    validationuser2itemrating[userid].pb(itemid * rating);

    if(userid > numusers)numusers = userid;
    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
    
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  numusers += 1;
  dbge(numusers);
  fin.close();
}

void LoadTestingData(string datafile, string dataseparator)
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
    fin.getline(buff, MAX_LEN_OF_LINE_IN_FILE);
    string sbuff = (string)buff; StripSpaces(sbuff);
    if(0 == sbuff.sz || sbuff.find(dataseparator)==string::npos)continue;
    int tabindex = sbuff.find(dataseparator);
    string user = sbuff.substr(0, tabindex); int userid = s2i(user); sbuff = sbuff.substr(tabindex+dataseparator.sz);
    tabindex = sbuff.find(dataseparator); int itemid = s2i(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex+dataseparator.sz);
    tabindex = sbuff.find(dataseparator); int rating = s2i(sbuff.substr(0, tabindex)); sbuff = sbuff.substr(tabindex+dataseparator.sz);
    tabindex = sbuff.find(dataseparator); LL timestamp = s2l(sbuff.substr(0, tabindex)); 

    if(timestamp < PUBLIC_TIMESTAMP)testpublicuser2itemrating[userid].pb(itemid);
    else testevaluser2itemrating[userid].pb(itemid);

    if(userid > numusers)numusers = userid;
    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
    
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  numusers += 1;
  dbge(numusers);
  fin.close();
}

PDDD ComputeCorrelationAndNormsForPearson(int itemidA, int itemidB)
{
  double sum1 = 0, sum2 = 0, sum1sq = 0, sum2sq = 0, psum = 0, norm1 = 0, norm2 = 0;
  int overlaps = 0;
  int numUsersRatedItemA = item2userrating[itemidA].sz, numUsersRatedItemB = item2userrating[itemidB].sz;
  int userIndexA = 0, userIndexB = 0;
  
  while(userIndexA < numUsersRatedItemA && userIndexB < numUsersRatedItemB)
  {
    int useridA = item2userrating[itemidA][userIndexA], useridB = item2userrating[itemidB][userIndexB];
    int ratingA = 1, ratingB = 1;
    if(useridA < 0)useridA *= -1, ratingA = -1;
    if(useridB < 0)useridB *= -1, ratingB = -1;
    if(useridA != useridB)
    {
      if(useridA < useridB)userIndexA++;
      else userIndexB++;
      
      continue;
    }
    
    overlaps++;
    sum1 += ratingA;
    sum2 += ratingB;
    sum1sq += 1;
    sum2sq += 1;
    psum += ratingA * ratingB;
    
    userIndexA++;
    userIndexB++;
  }
    
  norm1 = sqrt(numUsersRatedItemA); norm2 = sqrt(numUsersRatedItemB);
  
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
  
  return PDDD(correlation, PDD(norm1, norm2));
}

double Similarity_Pearson(int itemidA, int itemidB)
{
  PDDD ret = ComputeCorrelationAndNormsForPearson(itemidA, itemidB);
  double correlation = ret.first, norm1 = ret.second.first, norm2 = ret.second.second;
  //dbg(correlation); dbg(norm1); dbge(norm2);
  return correlation;
}

double Similarity_Pearson_NormWeighted(int itemidA, int itemidB)
{
  PDDD ret = ComputeCorrelationAndNormsForPearson(itemidA, itemidB);
  double correlation = ret.first, norm1 = ret.second.first, norm2 = ret.second.second;
  //dbg(correlation); dbg(norm1); dbge(norm2);
  return correlation * norm1 * norm2;
}

double Similarity_Cosine(int itemidA, int itemidB)
{
  double num = 0,  norm1 = 0, norm2 = 0;
  int numUsersRatedItemA = item2userrating[itemidA].sz, numUsersRatedItemB = item2userrating[itemidB].sz;
  int userIndexA = 0, userIndexB = 0;
  
  while(userIndexA < numUsersRatedItemA && userIndexB < numUsersRatedItemB)
  {
    int useridA = item2userrating[itemidA][userIndexA], useridB = item2userrating[itemidB][userIndexB];
    int ratingA = 1, ratingB = 1;
    if(useridA < 0)useridA *= -1, ratingA = -1;
    if(useridB < 0)useridB *= -1, ratingB = -1;
    if(useridA != useridB)
    {
      if(useridA < useridB)userIndexA++;
      else userIndexB++;
      
      continue;
    }
    
    num += ratingA * ratingB;
    
    userIndexA++;
    userIndexB++;
  }
    
  norm1 = sqrt(numUsersRatedItemA); norm2 = sqrt(numUsersRatedItemB);
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
      double similarity = Similarity_Cosine(itemid, itemid2); //(*SimilarityMetric)(itemid, itemid2);
      itemitemsimilarityscores[itemid][itemid2] = itemitemsimilarityscores[itemid2][itemid] = similarity;
    }
    
    if(itemid % 1000 == 0)cout<<"Time taken to Compute Item Item Similarities for numItems:"<<itemid<<" is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to Compute Item Item Similarities is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
}

void OutputSubmission(string submissionFile, bool isRSVDComputation)
{
  ofstream fout(submissionFile.cs);
  
  REP(iteration, 2)
  {
    clock_t start = clock();
    int numUsersProcessed = 0;
    
    REP(userid, numusers)
    {
      int numItems = (iteration == 0 ? testpublicuser2itemrating[userid].sz : testevaluser2itemrating[userid].sz);
      if(numItems == 0)continue;
      numUsersProcessed++;
      
      //double score1 = 0.0, score2 = 0.0, score3 = 0.0;
      double score1 = -10000000.0, score2 = -10000000.0, score3 = -10000000.0;
      int itemid1 = -1, itemid2 = -1, itemid3 = -1;
      
      REP(itemIndex, numItems)
      {
        //Compute similarity of itemid
        int itemid = (iteration == 0 ? testpublicuser2itemrating[userid][itemIndex]: testevaluser2itemrating[userid][itemIndex]);
        double score = 0.0, similaritySum = 0.0;
        
        if(!isRSVDComputation)
        {
          int numItemsRatedByUser = user2itemrating[userid].sz;
          REP(itemIndex2, numItemsRatedByUser)
          {
            int itemid2 = user2itemrating[userid][itemIndex2];
            int rating = 1;
            if(itemid2 < 0)itemid2 *= -1, rating = -1;
            
            score += rating * itemitemsimilarityscores[itemid][itemid2];
            similaritySum += abs(itemitemsimilarityscores[itemid][itemid2]);
          }
          if(similaritySum > EPS)score /= similaritySum;
        }
        else
        {
          REP(featureid, numfeatures)score += userfeaturematrix[userid][featureid] * itemfeaturematrix[itemid][featureid];
        }
        
        //Update the top 3 scores
        if(score > score1)
        {
          score3 = score2; itemid3 = itemid2;
          score2 = score1; itemid2 = itemid1;
          score1 = score; itemid1 = itemid;
        }
        else if(score > score2)
        {
          score3 = score2; itemid3 = itemid2;
          score2 = score; itemid2 = itemid;
        }
        else if(score > score3)
        {
          score3 = score; itemid3 = itemid;
        }
      }
      
      //Write the top 3 scores to file
      fout<<userid<<",";
      // if(itemid1 > 0 && score1 > EPS)fout<<itemid1;
      // if(itemid2 > 0 && score2 > EPS)fout<<" "<<itemid2;
      // if(itemid3 > 0 && score3 > EPS)fout<<" "<<itemid3;
      if(itemid1 > 0)fout<<itemid1;
      if(itemid2 > 0)fout<<" "<<itemid2;
      if(itemid3 > 0)fout<<" "<<itemid3;
      
      fout<<endl;
      
      if(userid % 1000000 == 0)
        cout<<"Time taken for processing numUsers: "<<userid<<" in testing file for "<< (iteration == 0 ? "PublicLeaderBoard" : "Evaluation") <<" is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
    }
    dbge(numUsersProcessed);
    cout<<"Time taken for processing: "<< (iteration == 0 ? "PublicLeaderBoard" : "Evaluation") <<" is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  }

  fout.close();
}

void WriteItemItemSimilaritesToFile(string file)
{
  ofstream fout(file.cs);
  
  REP(itemid, numitems)
  {
    FOR(itemid2, itemid + 1, numitems)
    {
      fout<<itemitemsimilarityscores[itemid][itemid2]<<endl;
    }
  }
  
  fout.close();
}

void LoadItemItemSimilaritesFromFile(string file)
{
  ifstream fin(file.cs);
  
  REP(itemid, numitems)
  {
    FOR(itemid2, itemid + 1, numitems)
    {
      fin>>itemitemsimilarityscores[itemid][itemid2];
    }
  }
  
  fin.close();
}

double GetErrorOnValidationSet(int iteration)
{
  double error = 0.0, estimatedRating = 0.0;
  int numberOfItemsInTestSet = 0;
  REP(userid, numusers)
  {
    int numItems = validationuser2itemrating[userid].sz;
    if(numItems == 0)continue;
    
    REP(itemIndex, numItems)
    {
      int itemid = user2itemrating[userid][itemIndex], rating = 1;
      if(itemid < 0)itemid *= -1, rating = -1;

      estimatedRating = 0.0;
      REP(featureid, numfeatures)estimatedRating += userfeaturematrix[userid][featureid] * itemfeaturematrix[itemid][featureid];
  
      //Clip Ratings
      if(estimatedRating > 1.0)estimatedRating = 1.0;
      else if(estimatedRating < -1.0)estimatedRating = -1.0;
      
      error += (estimatedRating - rating) * (estimatedRating - rating);
      numberOfItemsInTestSet++;
    }
  }
  
  if(numberOfItemsInTestSet > 0)error /= numberOfItemsInTestSet;
  error = sqrt(error);
  
  if(iteration == 0)dbge(numberOfItemsInTestSet);
  
  return error;
}

void DoRegularizedSVD(double regularizationConstant, int numLatentFactors)
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
    REP(userid, numusers)userfeaturematrix[userid][featureid] = ((double)rand() / (double)RAND_MAX);
    REP(itemid, numitems)itemfeaturematrix[itemid][featureid] = ((double)rand() / (double)RAND_MAX);
  }
  
  double estimatedRating = 0.0, currentError = 0.0, regularizationterm = 0.0, stepsize = 0.0, tmpuf = 0.0, tmpif = 0.0;
  REP(iteration, MAX_REGULARIZEDSVD_ITERATIONS)
  {    
    clock_t start = clock();
    stepsize = min(0.001, 1 / (double)(iteration + 1)); //adaptive step size = 1/iteration
    regularizationterm = regularizationConstant * stepsize;
    REP(userid, numusers)
    {
      int numItems = user2itemrating[userid].sz;
      if(numItems == 0)continue;
      
      REP(itemIndex, numItems)
      {
        int itemid = user2itemrating[userid][itemIndex], rating = 1;
        if(itemid < 0)itemid *= -1, rating = -1;
        
        estimatedRating = 0.0;
        REP(featureid, numfeatures)estimatedRating += userfeaturematrix[userid][featureid] * itemfeaturematrix[itemid][featureid];
    
        //Clip Ratings
        if(estimatedRating > 1.0)estimatedRating = 1.0;
        else if(estimatedRating < -1.0)estimatedRating = -1.0;

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
    cout<<"Iteration:"<<iteration<<" Time taken for training: "<<(double)(clock() - start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
    // if(iteration == 0)cout<<"Iteration:"<<iteration<<" Time taken for training: "<<(double)(clock() - start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
    
    // //Get Error on Validation Set
    // clock_t validationstart = clock();
    // double error = GetErrorOnValidationSet(iteration);
    // if(iteration == 0)cout<<"Iteration:"<<iteration<<" RMSE Error:"<<error<<" Time taken for validation: "<<(double)(clock() - validationstart)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
        
    // clock_t end = clock();
    // if(iteration == 0)cout<<"Iteration:"<<iteration<<" RMSE Error:"<<error<<" Time taken for complete iteration: "<<(double)(end - start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
    // cout<<iteration<<" "<<error<<" "<<(double)(end-start)/(double)CLOCKS_PER_SEC<<endl;
  }
}

void AppendRSVDValuesToDataset(string datafile, string dataseparator)
{
  cout<<"Processing:"<<datafile<<endl;
  ifstream fin(datafile.cs);
  if(!fin){cout<<"Could not open file:"<<datafile<<endl; exit(0);}
  int lineno = -1;
  char buff[MAX_LEN_OF_LINE_IN_FILE];
  
  string outputfile = datafile + "RSVD.txt";
  ofstream fout(outputfile.cs);
  
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
    
    double score = 0.0;
    REP(featureid, numfeatures)score += userfeaturematrix[userid][featureid] * itemfeaturematrix[itemid][featureid];
    
    double clippedscore = score;
    if(clippedscore > 1.0)clippedscore = 1.0;
    else if(clippedscore < -1.0)clippedscore = -1.0;
    
    double tanhscore = tanh(score);
    
    fout<<clippedscore<<" "<<tanhscore<<endl;
    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  
  fout.close();
  fin.close();
}

int main()
{
  //Initialize
  Initialize();
    
  LoadTrainingData("F:\\track1\\rec_log_train_deduped_mapped_train_correct.txt", "\t");
  //TestTrainingData();
  LoadValidationData("F:\\track1\\rec_log_train_deduped_mapped_validation_correct.txt", "\t");
  LoadTestingData("F:\\track1\\rec_log_test_deduped_mapped.txt", "\t");
  //LoadTrainingData("F:\\track1\\trainhead.txt", "\t");
  
  ComputeItemItemSimilarities();//(Similarity_Cosine);
  // WriteItemItemSimilaritesToFile("F:\\track1\\cosinesimilarity.txt");
  //LoadItemItemSimilaritesFromFile("F:\\track1\\cosinesimilarity.txt");
  OutputSubmission("F:\\track1\\attempt1_cosine_reversemapped.txt", false);
  
  // ComputeItemItemSimilarities(Similarity_Pearson);
  // WriteItemItemSimilaritesToFile("F:\\track1\\pearsonsimilarity.txt");
  // LoadItemItemSimilaritesFromFile("F:\\track1\\pearsonsimilarity.txt");
  // OutputSubmission("F:\\track1\\attempt1_pearson_reversemapped.txt", false);
  
  //ComputeItemItemSimilarities(Similarity_Pearson_NormWeighted);
  // WriteItemItemSimilaritesToFile("F:\\track1\\normweightedpearsonsimilarity.txt");
  // LoadItemItemSimilaritesFromFile("F:\\track1\\normweightedpearsonsimilarity.txt");
  //OutputSubmission("F:\\track1\\attempt1_pearson_normweighted_reversemapped.txt", false);

  //DoRegularizedSVD(0.001, 75);
  //OutputSubmission("F:\\track1\\attempt2_rsvd_75_0dot001_reversemapped.txt", true);
  // AppendRSVDValuesToDataset("F:\\track1\\rec_log_train_deduped_mapped_train.txt", "\t");
  // AppendRSVDValuesToDataset("F:\\track1\\rec_log_train_deduped_mapped_validation.txt", "\t");
  // AppendRSVDValuesToDataset("F:\\track1\\rec_log_test_deduped_mapped.txt", "\t");
  
  return 0;
  
}

