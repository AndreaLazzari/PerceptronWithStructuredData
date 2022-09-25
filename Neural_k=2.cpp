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
#include <utility>
#include <type_traits>

#include "ortools/linear_solver/linear_solver.h"


using namespace std;

//sigma 0.01    0.31    0.68    1.87    12
//rho     1      0.8     0.5     0.2     0.0

  random_device rd{};
  mt19937 gen{rd()};
  normal_distribution<double> distrib {0.0,1.0};
  normal_distribution<double> distrib_dopp {0.0,0.01};
  


  int RBool();
  int RBool(){
      int x = rand();
      
      if (x > (RAND_MAX/2)) { 
          return 1;
      } else {
          return -1;}
  };

  
  template <typename Container>

   //funzione norma
 
 double norm_of(const Container & v){
     return sqrt( accumulate(v.begin(), v.end(), 0., [](double acc, double s){ return acc + s*s;} ) );
 };
  
  
  //funzione normalize
 
 void normalize (vector<double> & v){
     double norm = norm_of(v);
     for (auto & elem:v) { elem /= norm;}
 };

 
// genera un punto sulla sfera n-dimensionale, e lo normalizza
 
 
vector<double> spherepoint(int dimensione) {

    vector<double> point(dimensione);

    for(int i=0; i<dimensione; i++) {
        point[i]= distrib(gen); 
    };

normalize(point);

return point;
};

 
 

 //funzione partner
 
 vector<double> partner(const vector<double> & v ){
     
 vector<double> dopp(v.size());
 
 for (int i=0; i < v.size(); i++){
     
     dopp[i]= v[i] + distrib_dopp(gen);
 };
 
 normalize(dopp);
 
 return dopp;
 
 };


  
  
  
//TRAINING SET
     //int p = 9;
     int n = 10;
     
     const double epsilon = 0.00000000001;
     
  int s=0;
  int ns=0;


 vector <string> name(n);
 //vector <double> training(p);

 
  
  namespace operations_research {
 
      
    
pair <int , double > LinearProgramming(int n_input) {    // <successi, overlap>  pippo.first e pippo.second
    
        
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
    
   double overlap=0;
   double prodScal=0;
   
   vector<vector<double>> vettinp(n_input , vector<double> (n) );
   
   vector<vector<double>> dopp_inp(n_input , vector<double>(n) );
    
   for (int t=0; t < n_input ; t++){
      vettinp[t] = spherepoint(n);
      dopp_inp[t] = partner(vettinp[t]);
   };
  

   
   for (int l=0; l<n_input; l++){

    prodScal += std::inner_product( vettinp[l].begin(), vettinp[l].end(), dopp_inp[l].begin(), 0. );    
    //innerproduct  std numeric o algorithm
    
   };
   
   //
   
   overlap = ( prodScal )/n_input;
   
   // cout << "Overlap " << overlap << endl;

   
   
   vector<int> label(n_input);
   srand(time(0));
      
      
   vector < MPConstraint* > vinc(2*(n_input));
   
   for (int i=0; i<(n_input); i++){
       
       label[i] = RBool();
       vinc[i] = solver.MakeRowConstraint(epsilon, infinity);  //vincoli xi - vettinp
       vinc[i+n_input] = solver.MakeRowConstraint(epsilon, infinity);  //vincoli xi barra - dopp
       
    for (int j=0; j<n; j++){
        
        vinc[i]->SetCoefficient( w[j] , label[i] * vettinp[i][j] );
        vinc[i + n_input]->SetCoefficient( w[j] , label[i] * dopp_inp[i][j] );
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
    
  /*
  cout << "Solution"<< s <<endl;
  
  LOG(INFO) << "Optimal objective value = " << objective->Value();
  
     
  for(int j=0; j<n; j++){
    LOG(INFO) << w[j]->name() << " = " << w[j]->solution_value() << endl;
  };
  
  */
  
    pair <int, double> output;
    output.first = s;
    output.second = overlap;

    return output;

}


}  // namespace operations_research

  
  
  //--------------------------------------------------------------------------------------------------//  
  
  
  
  int main(int argc, char** argv) {

 
   int successi=0;
   double somma_overlap = 0;
   int N_iteraz=1000;
   
   pair < int , double > result;
   
   vector <string> namefile(50);
  
   for (int j=0; j<50 ; j++){
     namefile[j] = "dati_n=" + to_string(n) + "_k=2_" + to_string(j) + ".dat";
   
      ofstream file_out;
      
   file_out.open(namefile[j]);
    
    for (int p=0; p<=30 ; p++){

       for (int l=1; l<=N_iteraz; l++){
       
        result = operations_research::LinearProgramming(p); 
     successi = result.first;
     somma_overlap += result.second;
     
     }

  //  file_out << "probabilità di successo = "<< (successi/N_iteraz)<< endl;
    file_out << /* ( (double) p)/n  << "   " <<  */ ( (double) successi )/N_iteraz << /* "    rho = " << "    " << (somma_overlap )/N_iteraz << */ endl;
    successi=0;
    s=0;
    ns=0;
    somma_overlap=0;
  }
  
 file_out.close();
  
 }
   
  
 //file_out << "soluz trovata  "<<  "  volte e  soluz non trovata   "  << " volte" <<endl;
 
   
   
  return EXIT_SUCCESS;
}
  
