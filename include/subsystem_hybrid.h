// This class represents the physical system.
// Change all parties[0]  to parties[1]?
#ifndef SUBSYSTEM_H
#define SUBSYSTEM_H
#include "fixedPoint.h"
#include "garbled_circuit_user.h"

class subSystem: public garbled_circuit_user {
public:
  // declares the constants for the system.
  // _ne refers to variables in plaintext
  
  
  
  

  int sizeCusum[2];
  fixedPoint **Cusum;
  int sizeNu[2];
  fixedPoint **Nu;
  
  int sizeTau[2];
  fixedPoint **Tau;
  


  int parties[2];
  double **process_noise;
  int size_process_noise[2];
  double **sensor_noise;
  int size_sensor_noise[2];

  subSystem(int *parties, int integer_bits) {
    this->parties[0] = parties[0];
    this->parties[1] = parties[1];
    this->integerBits = integer_bits;
    this->decimalBits = integer_bits;
    this->totalBits = integerBits + decimalBits;
  }

  // Create a representation of the control matrices to use the emp library
  // Assumes that the constants are already computed in plaintext
  void garbleConstants(  ) {
    // Initialize Cloud's secret
    setZero_GC( this->Tau, this->sizeTau, parties[1]);
    setZero_GC( this->Nu,  this->sizeNu, parties[1]);
  }

  // loads all the matrices required to compute the controller
  // and to simulate the system behavior
  void inputData(  ) {
    string data_folder = "Data/";
    // Initializes matrices A, B, C, x0, y0, xr, ur, tau, nu
    this->Tau = init_size_using_file_GC( data_folder + "Tau.txt", this->sizeTau );
    this->Nu  = init_size_using_file_GC( data_folder + "Nu.txt", this->sizeNu );
  }
  
};

#endif
