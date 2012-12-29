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
#include <cstdlib>
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
#define s2i(_str) ({stringstream sin(_str); int _x; sin>>_x; _x;})
#define s2d(_str) ({stringstream sin(_str); double _x; sin>>_x; _x;})
#define s2l(_str) ({stringstream sin(_str); LL _x; sin>>_x; _x;})
#define SAFEEXPONENT(_val) ({double _x = 0.0; if(_val < -50)_x = 0.0; else if(_val > 21)_x = INT_MAX; else _x = exp(_val); _x;})
#define SAFESIGMOID(_val) ({double _x = 0.0; if(_val < -21)_x = 0.0; else if(_val > 50)_x = 1.0; else _x = 1.0/(1.0 + exp(-1*_val)); _x;})
#define NORM(theta, prevtheta) ({double _x = 0.0; REP(_i, numfeatures)_x += (theta[_i] - prevtheta[_i]) * (theta[_i] - prevtheta[_i]); _x = sqrt(_x); _x;})
typedef long long LL;
const int MAX_NUMBER_OF_USERS = 2400000, MAX_NUMBER_OF_ITEMS = 5000;
const int MAX_NUMBER_OF_USERFEATURES = 10, MAX_NUMBER_OF_ITEMFEATURES = 50;
const double EPSILON = 1e-9;
const double CONVERGENCETHRESHOLD = 1e-4;
const double STEPSIZE = 1e-3;
const double REGULARIZATIONCONSTANT = 1e-3;
const int MODELTYPE_LINEAR_REGRESSION = 1;
const int MODELTYPE_TANH_REGRESSION = 2;
const int PHASE_TRAINING = 1;
const int PHASE_TESTING = 2;
const int PHASE_VALIDATION = 3;
const int MAXITERATIONS = 100000000, MINITERATIONS = 70000000;
const int MAX_NUMBER_OF_FEATURES = 100;
double theta[MAX_NUMBER_OF_FEATURES], prevtheta[MAX_NUMBER_OF_FEATURES], features[MAX_NUMBER_OF_FEATURES];
double userfeatures[MAX_NUMBER_OF_USERS][MAX_NUMBER_OF_USERFEATURES];
double itemfeatures[MAX_NUMBER_OF_ITEMS][MAX_NUMBER_OF_ITEMFEATURES];
const int MAX_BUFFER_LENGTH = 100000;
char buff[MAX_BUFFER_LENGTH];
const string TRAINING_FILE = "F:\\track1\\traininguseritemfeatures.txt";
const string VALIDATION_FILE = "F:\\track1\\validationuseritemfeatures.txt";
const string TESTING_FILE = "F:\\track1\\testinguseritemfeatures.txt";
const string OUTPUT_FILE = "F:\\track1\\output.txt";
const string USER_FEATURES_FILE = "F:\\track1\\userfeatures_mapped2.txt";
const string ITEM_FEATURES_FILE = "F:\\track1\\itemfeatures_mapped2.txt";
char *ptr = NULL, *ptr2 = NULL;
int modeltype = MODELTYPE_TANH_REGRESSION; 
int numfeatures = 0, numuserfeatures = 9, numitemfeatures = 0;
ofstream fout;
int numberOfValidationPasses = 0;

void Initialize()
{
  assert(MINITERATIONS < MAXITERATIONS);
  
  memset(userfeatures, 0, sizeof(userfeatures));
  memset(itemfeatures, 0, sizeof(itemfeatures));
  //initialize theta
  REP(i, MAX_NUMBER_OF_FEATURES)theta[i] = rand() / RAND_MAX;
 
  fout.open(OUTPUT_FILE.cs);
}

void LoadUserFeatures(string file)
{
  ifstream fin(file.cs);
  if(!fin){cout<<"Could not open file:"<<file<<endl; exit(0);}
  cout<<"Begin processing:"<<file<<endl;
  int lineno = -1;
  
  int userid = -1, agebucket = -1, gender = -1;
  double numtweets = 0.0;
  
  clock_t start = clock();
  while(fin)
  {
    lineno++;
    if(fin.eof())break;
    fin >> userid >> agebucket >> gender >> numtweets;
    if(userid == -1)continue;
    userfeatures[userid][agebucket] = 1.0;
    userfeatures[userid][5 + gender] = 1.0;
    userfeatures[userid][8] = numtweets;
    
    if(lineno % 1000000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
    if(lineno < 5)
    {
      dbg(userid);
      REP(i, numuserfeatures)cout<<" "<<userfeatures[userid][i];
      cout<<endl;
    }
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  dbge(numuserfeatures);
  fin.close();
}

void LoadItemFeatures(string file)
{
  ifstream fin(file.cs);
  if(!fin){cout<<"Could not open file:"<<file<<endl; exit(0);}
  cout<<"Begin processing:"<<file<<endl;
  int lineno = -1;
    
  bool isFirstLine = true;
  
  clock_t start = clock();
  int itemid = -1;
  while(fin)
  {
    lineno++;
    if(fin.eof())break;
    
    fin.getline(buff, MAX_BUFFER_LENGTH);
    
    //Get number of features in the current file
    if(isFirstLine)
    {
      ptr = buff;
      while(*ptr){if(*ptr == ' ' || *ptr == '\t')numitemfeatures++; ptr++;} //numitemfeatures is given by numspaces
      isFirstLine = false;
      assert(numitemfeatures <= MAX_NUMBER_OF_ITEMFEATURES);
    }
    
    //Load itemid and features corresponding to current line
    ptr = ptr2 = buff;
    itemid = -1;
    int columnno = 0;
    while(*ptr)
    {
      if(*ptr == ' ' || *ptr == '\t')
      {
        *ptr = '\0';
        if(columnno == 0)
        {
          itemid = s2i(ptr2);
          if(itemid == -1)break;
        }
        else itemfeatures[itemid][columnno - 1] = s2d(ptr2);
        
        ptr2 = ptr + 1;
        columnno++;
      }
      ptr++; 
    }
    if(NULL != ptr2 && *ptr2 && itemid != -1)itemfeatures[itemid][columnno - 1]  = s2d(ptr2);
    if(itemid != -1)assert(columnno == numitemfeatures);
    
    
    if(lineno % 1000 == 0)cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
    if(lineno < 5)
    {
      dbg(itemid);
      REP(i, numitemfeatures)cout<<" "<<itemfeatures[itemid][i];
      cout<<endl;
    }
  }
  cout<<"Time taken to load "<< lineno <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
  fin.close();
  dbge(numitemfeatures);
}

void DoRegression(string file, int phase, int curiteration)
{
  if(phase != PHASE_VALIDATION || numberOfValidationPasses == 0)
    cout<<"File:"<<file<<endl;

  if(phase == PHASE_VALIDATION)numberOfValidationPasses++;
  
  bool isFirstLine = true;
  int iteration = -1;
  double error = 0.0;
  if(phase != PHASE_TRAINING)isFirstLine = false;
  clock_t start = clock();
  
  assert(numuserfeatures > 0 && numitemfeatures > 0);
  int userid = -1, itemid = -1, rating = -1, featureoffset = 1 + numuserfeatures + numitemfeatures; //featureoffset of 1 for just the bias term
  LL timestamp = 0;
  bool isConverged = false;
  
  while(iteration < MAXITERATIONS)
  {
    ifstream fin(file.cs);
    if(!fin.is_open()){cout<<"Could not open file:"<<file<<endl; exit(1);}
    
    while(!fin.eof())
    {
      iteration++;
      
      if(phase == PHASE_TRAINING)memcpy(prevtheta, theta, sizeof(theta));
      fin.getline(buff, MAX_BUFFER_LENGTH);
      
      //Get number of features in the current file
      if(isFirstLine)
      {
        ptr = buff;
        while(*ptr){if(*ptr == ' ' || *ptr == '\t')numfeatures++; ptr++;} //numfeatures is given by numspaces - 3
        isFirstLine = false;
        numfeatures -= 3; //this is because we wish to ignore the spaces between (userid, itemid), (itemid, rating), (rating, timestamp)
        numfeatures += featureoffset; //adding featureoffset term consisting of bias, user features, item features
        assert(numfeatures <= MAX_NUMBER_OF_FEATURES);
        dbge(numfeatures);
      }
      
      //Load userid, itemid, rating, timestamp and features corresponding to current line
      ptr = ptr2 = buff;
      userid = itemid = -1;
      int columnno = 0, clicks = 0, impressions = 0;
      while(*ptr)
      {
        if(*ptr == ' ' || *ptr == '\t')
        {
          *ptr = '\0';
          if(columnno == 0)
          {
            userid = s2i(ptr2);
            if(userid == -1)break;
          }
          else if(columnno == 1)
          {
            itemid = s2i(ptr2);
            if(itemid == -1)break;
          }
          else if(columnno == 2) rating = s2i(ptr2);
          else if(columnno == 3) timestamp = s2l(ptr2);
          else features[featureoffset + columnno - 4] = s2d(ptr2); // we start with 0-based index from feature offset onwards. absolute position of featureid starts from featureoffset
          
          ptr2 = ptr + 1;
          columnno++;
        }
        ptr++; 
      }
      if(userid == -1 || itemid == -1)continue;
      if(NULL != ptr2 && *ptr2)features[featureoffset + columnno - 4] = s2d(ptr2);
      if(iteration == 0 && (phase != PHASE_VALIDATION  || (phase == PHASE_VALIDATION && numberOfValidationPasses == 0))){ dbg(columnno); dbge(numfeatures); }
      features[0] = 1.0;
      REP(featureid, numuserfeatures)features[1 + featureid] = userfeatures[userid][featureid];
      REP(featureid, numitemfeatures)features[1 + numuserfeatures + featureid] = itemfeatures[itemid][featureid];
      
      //Compute yhat and other required terms for performing gradient descent
      double dotproduct = 0.0, weight = 1.0, fhat = 0.0;
      REP(i, numfeatures) dotproduct += theta[i] * features[i];
      if(modeltype == MODELTYPE_TANH_REGRESSION)
      {
        fhat = tanh(dotproduct);
        weight = 1 - fhat * fhat; //use this only for training phase. avoiding an extra if statement here
      }
      else if(modeltype == MODELTYPE_LINEAR_REGRESSION)
      {
        fhat = dotproduct;
      }
      
      if(phase == PHASE_TRAINING)
      {
        weight *= -1*(rating - fhat);
        //Do gradient descent
        REP(i, numfeatures)theta[i] = prevtheta[i] - STEPSIZE * (weight * features[i] + REGULARIZATIONCONSTANT * prevtheta[i]);
        
        //Check for convergence over either norm of theta < CONVERGENCETHRESHOLD or iteration > MAXITERATIONS
        if(iteration > MINITERATIONS && (NORM(theta, prevtheta) < CONVERGENCETHRESHOLD - CONVERGENCETHRESHOLD * EPSILON || iteration > MAXITERATIONS))
        {
          isConverged = true;
          break;
        }
      }
      else if(phase == PHASE_VALIDATION)error += (fhat - rating) * (fhat - rating);
      else if(phase == PHASE_TESTING)
      {
        fout<<userid<<" "<<itemid<<" "<<fhat<<" "<<timestamp<<endl;
      }
      
      if(isConverged)break;
      
      //print the first five iterations in case of training and testing files because they will be entered only once
      //print the first five iterations on validation set if this is the first validation phase
      if(iteration < 5 && (phase != PHASE_VALIDATION || numberOfValidationPasses == 0))
      {
        dbg(iteration); dbg(userid); dbg(itemid); dbge(rating);
        cout<<"Feature Values:";
        REP(i, numfeatures)cout<<" "<<features[i];
        cout<<endl;
        cout<<"Feature Weights:";
        REP(i, numfeatures)cout<<" "<<theta[i];
        cout<<endl;
        
      }
      
      if(phase == PHASE_TRAINING && iteration % 1000000 == 0)
        cout<<"Time taken to train "<< iteration <<" iterations is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
    }
    
    fin.close();
    
    if(phase != PHASE_TRAINING)break;
    else  //evaluate performance on the validation set and do another pass over the training file
    {
      if(!isConverged)DoRegression(VALIDATION_FILE, PHASE_VALIDATION, iteration);
      else 
      {
        cout<<"Convergence detected after "<<iteration<<" number of iterations over the training set"<<endl;
        cout<<"Feature Weights:";
        REP(i, numfeatures)cout<<" "<<theta[i];
        cout<<endl;
        break;
      }
    }
  }
  
  //output error on validation set
  if(phase == PHASE_VALIDATION && numberOfValidationPasses % 1000 == 0)
    cout<<"file:"<<file<<" curiteration: "<<curiteration<<" error:"<<(iteration == 0? 0.0 : sqrt(error / iteration))<<endl;
}

int main()
{
  Initialize();

  LoadUserFeatures(USER_FEATURES_FILE);
  LoadItemFeatures(ITEM_FEATURES_FILE);
  
  DoRegression(TRAINING_FILE, PHASE_TRAINING, 0);
  DoRegression(TESTING_FILE, PHASE_TESTING, 0);
  
  return 0;
}
