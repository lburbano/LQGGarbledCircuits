#ifndef SUBSYSTEM_H
#define SUBSYSTEM_H
#include "fixedPoint.h"
#include "garbled_circuit_user.h"
// CHange all parties[0] to parties[1]?
class subSystem: public garbled_circuit_user {
public:
  // declares the constants for the system.
  // unlike the subsystem class, this class does not have
  // plaintexts
  
  
  
  int sizeCusum[2];
  fixedPoint **Cusum;
  int sizeNu[2];
  fixedPoint **Nu;
  int sizeTau[2];
  fixedPoint **Tau;

  
  double **Nu_ne;
  double **Tau_ne;
  int parties[2];
  

  subSystem(int *parties, int integer_bits) {
    this->parties[0] = parties[0];
    this->parties[1] = parties[1];
    this->integerBits = integer_bits;
    this->decimalBits = integer_bits;
    this->totalBits = integerBits + decimalBits;
  }

  

  
  // Creates a representation of the constants related to the controller to be used with the emp toolkit
  // The values are initialized as zero here since the cloud should not learn the plaintexts
  // Nomenclature based on 
  // Encrypted LQG using Labeled Homomorphic Encryption. https://www.georgejpappas.org/papers/Paper264.pdf
  void garbleConstants(  ) {
    
    // Initialize Cloud's secret
    setData_GC( this->Tau, this->Tau_ne, this->sizeTau, parties[1]);
    setData_GC( this->Nu, this->Nu_ne, this->sizeNu, parties[1]);
  }

  

/*-------------------------------------------------------------------------
Functions required for the operations. Not focused on the control system
---------------------------------------------------------------------------*/

  // Creates instances of vectors and matrices to be used by the emp toolkit
  void inputData( ) {
    // The size of matrices are retrieved from the .txt files, however the information is
    // never used in the function. It does not make sense the cloud learns the plaintext of 
    // matrices
    // Vectors and matrices are initialized with values of zero
    /*
    getFileSize("Data/A.txt", this->sizeA);
    this->A = new fixedPoint *[this->sizeA[0]];
    for (int i = 0; i < this->sizeA[0]; i++) {
      this->A[i] = new fixedPoint[this->sizeA[1]];
    }
    */
    string data_folder = "Data/";
    this->Tau_ne = init_size_file( data_folder + "Tau.txt", this->sizeTau);
    this->Nu_ne  = init_size_file( data_folder + "Nu.txt", this->sizeNu);
    // Load data of matrices A, B, C, K, L
    readFile(this->Tau_ne, data_folder + "Tau.txt", this->sizeTau);
    readFile(this->Nu_ne,  data_folder + "Nu.txt", this->sizeNu);
    this->Nu  = initSize_GC( this->sizeNu );
    this->Tau = initSize_GC( this->sizeTau );
    

  }

};

#endif
