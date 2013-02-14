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
#include <cstring>
#include <cassert>
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
const double EPSILON = 1e-9;
const double CONVERGENCETHRESHOLD = 1e-4;
const double STEPSIZE = 1e-8;
const double REGULARIZATIONCONSTANT = 1e-3;
const int MODELTYPE_LINEAR_REGRESSION = 1;
const int MODELTYPE_TANH_REGRESSION = 2;
const int MODELTYPE_LOGISTIC_REGRESSION = 3;
const int PHASE_TRAINING = 1;
const int PHASE_TESTING = 2;
const int PHASE_VALIDATION = 3;
const int MAXITERATIONS = 100000000, MINITERATIONS = 80000000;
const int MAX_NUMBER_OF_FEATURES = 250;
double theta[MAX_NUMBER_OF_FEATURES], prevtheta[MAX_NUMBER_OF_FEATURES], features[MAX_NUMBER_OF_FEATURES];
const int MAX_BUFFER_LENGTH = 100000;
char buff[MAX_BUFFER_LENGTH];
const string TRAINING_FILE = "/home/arvind/track1/extraction_train.txt";
const string VALIDATION_FILE = "/home/arvind/track1/extraction_validation.txt";
const string PUBLIC_TESTING_FILE = "/home/arvind/track1/extraction_publictest.txt";
const string FEATURE_OUTPUT_FILE = "/home/arvind/track1/output.txt";
char *ptr = NULL, *ptr2 = NULL;
int modeltype = MODELTYPE_TANH_REGRESSION; 
//set this numfeatures to 0 to compute from training file
int numfeatures = 0; //Adding the 1 for the offset feature
ofstream fout;
int numberOfValidationPasses = 0;

void Test(string file);

void Initialize()
{
  assert(MINITERATIONS < MAXITERATIONS);
  
  //initialize theta
  REP(i, MAX_NUMBER_OF_FEATURES)theta[i] = rand() / RAND_MAX;

  if(numfeatures > 1)cout<<"INFO: NumFeatures was preset to:"<<numfeatures<<endl;
  else
  {
    numfeatures = 1;

    ifstream fin(TRAINING_FILE.cs);
    if(!fin){cout<<"Could not open file:"<<TRAINING_FILE<<endl; exit(0);}
    
    if(fin.eof()){ cout<<"ERROR: training file did not contain any data"<<endl; exit(1); }
      
    fin.getline(buff, MAX_BUFFER_LENGTH); 
    ptr = buff;
    // since the first column is the target, the number of spaces gives
    // the number of features
    while(*ptr){if(*ptr == ' ' || *ptr == '\t')numfeatures++; ptr++;}

    fin.close();
    cout<<"INFO: NumFeatures was computed to be:"<<numfeatures<<" including the implicit offset feature"<<endl;
  }

  dbge(STEPSIZE); dbge(REGULARIZATIONCONSTANT);

  fout.open( FEATURE_OUTPUT_FILE.cs );
  fout<<"#File Iteration RMSE"<<endl;
}

void Train(string file)
{
    cout<<"File:"<<file<<endl;
  
  int iteration = 0, featureindex = 0;
  clock_t start = clock();
  double error = 0.0, target = 0.0;
  bool isConverged = false;
  
  while(iteration < MAXITERATIONS)
  {
    ifstream fin(file.cs);
    if(!fin.is_open()){cout<<"Could not open file:"<<file<<endl; exit(1);}
    
    while(!fin.eof())
    {
      iteration++;
      
      memcpy(prevtheta, theta, sizeof(theta));
      fin.getline(buff, MAX_BUFFER_LENGTH);
      stringstream sin(buff);
      sin>>target;
      features[0] = 1.0;
      FOR(featureindex, 1, numfeatures + 1)sin>>features[featureindex];
     
      //Compute yhat and other required terms for performing gradient descent
      double dotproduct = 0.0, weight = 1.0, fhat = 0.0;
      REP(i, numfeatures) dotproduct += theta[i] * features[i];
        fhat = tanh(dotproduct);
        weight = -1* (1 - fhat * fhat) * (target - fhat);
      
      error += (fhat - target) * (fhat - target);
        //Do gradient descent
        REP(i, numfeatures)theta[i] = prevtheta[i] - STEPSIZE * (weight * features[i] + REGULARIZATIONCONSTANT * prevtheta[i]);
        
        //Check for convergence over either norm of theta < CONVERGENCETHRESHOLD or iteration > MAXITERATIONS
        if(iteration > MINITERATIONS && (NORM(theta, prevtheta) < CONVERGENCETHRESHOLD - CONVERGENCETHRESHOLD * EPSILON || iteration > MAXITERATIONS))
        {
          isConverged = true;
          break;
        }
      
      if(isConverged)break;
      
      if(iteration % 1000000 == 0)
      {
        cout<<"Time taken to process "<< iteration <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
      }  
      
      if(iteration % 5000000 == 0)
      {
        fout<< file << " " << iteration << " " << sqrt(error / 5000000) << endl;
        error = 0.0;
        REP(i, numfeatures)cout<<" "<<theta[i]; cout<<endl;
        
        Test(VALIDATION_FILE);
        cout<<"File:"<<file<<endl;
      }
      
    }
   
    fin.close();
    
    if(isConverged) //evaluate performance on the validation set and do another pass over the training file
    {
        cout<<"Convergence detected after "<<iteration<<" number of iterations over the training set"<<endl;
        cout<<"Feature Weights:";
        REP(i, numfeatures)cout<<" "<<theta[i];
        cout<<endl;
        
        break;
    }
  }
  
}

void Test(string file)
{
    cout<<"File:"<<file<<endl;

  
  int iteration = 0, featureindex = 0;
  clock_t start = clock();
  double error = 0.0, target = 0.0;
  bool isConverged = false;
  
    ifstream fin(file.cs);
    if(!fin.is_open()){cout<<"Could not open file:"<<file<<endl; exit(1);}
    
    while(!fin.eof())
    {
      iteration++;
      
      fin.getline(buff, MAX_BUFFER_LENGTH);
      stringstream sin(buff);
      sin>>target;
      features[0] = 1.0;
      FOR(featureindex, 1, numfeatures + 1)sin>>features[featureindex];
       
      //Compute yhat and other required terms for performing gradient descent
      double dotproduct = 0.0, fhat = 0.0;
      REP(i, numfeatures) dotproduct += theta[i] * features[i];
        fhat = tanh(dotproduct);
      
      error += (fhat - target) * (fhat - target);
      
      if(iteration % 1000000 == 0)
      {
        cout<<"Time taken to process "<< iteration <<" lines is: "<<(double)(clock()-start)/(double)CLOCKS_PER_SEC<<" seconds"<<endl;
      }  
    }
   
    fin.close();
    
    fout<< file << " " << iteration << " " << sqrt(error / iteration) << endl;
}

int main()
{
  Initialize();

  Train(TRAINING_FILE);
  Test(TRAINING_FILE);
  Test(VALIDATION_FILE);
  Test(PUBLIC_TESTING_FILE);
 
  fout.close(); 
  return 0;
}
