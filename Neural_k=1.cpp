#include<random>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <fstream>

#include "ortools/linear_solver/linear_solver.h"


using namespace std;


  int RBool();
  int RBool(){
      int x = rand();
      
      if (x > (RAND_MAX/2)) { 
          return 1;
      } else {
          return -1;}
  };

//TRAINING SET
     //int p = 9;
     int n = 10;
     
  int s=0;
  int ns=0;


 vector <string> name(n);
 //vector <double> training(p);


  random_device rd{};
  mt19937 gen{rd()};
  normal_distribution<double> distrib{0.0,1.0};
  
 
  
  namespace operations_research {
 
      
    
int LinearProgramming(int n_input) {
    
        
  MPSolver solver("linear_programming",
                  MPSolver::GLOP_LINEAR_PROGRAMMING);

    const double infinity = solver.infinity();
     
    
    
  //variabili della funz obiettivo: pesi w , 1 vettore n-dimensionale
  
 
    for (int j=0; j<n ; j++){
     name[j] = "w" + to_string(j);
    // cout << name[j] <<endl;
    };
      
 
vector <MPVariable*> w(n);  
     
    for (int j=0; j<n; j++){
        w[j] = solver.MakeNumVar(-infinity, infinity, name[j]);
    };
                          
  
  //LOG(INFO) << "Number of variables = " << solver.NumVariables();
  
  
 
 //--------------------------------------------------------------------------------------------------//  
  
  
 
 //vincoli
  
  //sigma per prodotto scalare (<w,csi>) > 0

  // sigma[j]*( < w[j] , vettinp[i]) > 0
 
   
   vector<int> label(n_input);
   srand(time(0));
   
   vector < MPConstraint* > vinc(n_input);
   
   for (int i=0; i<n_input; i++){
       
       label[i] = RBool();
       vinc[i] = solver.MakeRowConstraint(0.00000000001, infinity);
       
    for (int j=0; j<n; j++){
        
        vinc[i]->SetCoefficient( w[j] , label[i] * distrib(gen) );
    
    }; 
  
   };
   
  //LOG(INFO) << "Number of constraints = " << solver.NumConstraints();
  
  
  //--------------------------------------------------------------------------------------------------//  
  
  
  // Objective function: < 0 , w >
  
  
  MPObjective* const objective = solver.MutableObjective();
      
   for(int j=0; j<n; j++){
    objective->SetCoefficient( w[j] , 0.);
   };
   
   objective->SetMaximization();
   
   
   
   
    //--------------------------------------------------------------------------------------------------//  
   

      
  
  const MPSolver::ResultStatus result_status = solver.Solve();
  // Check that the problem has an optimal solution.
  if (result_status != MPSolver::OPTIMAL) {
      ns++;
     // cout << "ns = " << ns <<endl;
   //cout << "The problem does not have an optimal solution!"<< ns <<endl;
    
  } else{ 
    s++;
    //cout << "s = " << s <<endl;
  }
    
  
  //cout << "Solution"<< s <<endl;
  
  //LOG(INFO) << "Optimal objective value = " << objective->Value();
  
     
  //for(int j=0; j<n; j++){
    //LOG(INFO) << w[j]->name() << " = " << w[j]->solution_value() << endl;
  //};

  return s ;

}


}  // namespace operations_research

  
  
  //--------------------------------------------------------------------------------------------------//  
  
  
  
  int main(int argc, char** argv) {

 
   int successi=0;
   int N_iteraz=1000;
   
   
   vector <string> namefile(50);
   
   for (int j=0; j<50 ; j++){
     namefile[j] = "dati_n=10_basprec" + to_string(j) + ".dat";
   
      ofstream file_out;
      
   file_out.open(namefile[j]);
      
    for (int p=0; p<=20 ; p++){
        
       for (int l=1; l<=N_iteraz; l++){
      
     successi = operations_research::LinearProgramming(p);
  
       }
    
    //file_out << "probabilità di successo = "<< (successi/N_iteraz)<< endl;
    file_out << /* ( (double) p)/n  << "   " << */ ( (double) successi )/N_iteraz << endl;
    successi=0;
    s=0;
    ns=0;
    
  }
  
  file_out.close();
  
   }
   
  
  
 //file_out << "soluz trovata  "<<  "  volte e  soluz non trovata   "  << " volte" <<endl;
 
 
   
   
  return EXIT_SUCCESS;
}
