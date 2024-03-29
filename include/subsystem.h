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
  
  
  
  fixedPoint **xr;
  double **xr_ne;
  int sizexr[2];
  fixedPoint **ur;
  double **ur_ne;
  int sizeur[2];
  fixedPoint **xk;
  double **xk_ne;
  int sizexk[2];
  fixedPoint **zk;
  double **zk_ne;
  int sizezk[2];
  fixedPoint **A;
  double **A_ne;
  int sizeA[2];
  fixedPoint **B;
  double **B_ne;
  int sizeB[2];
  fixedPoint **C;
  double **C_ne;
  int sizeC[2];
  double **uk_ne;

  fixedPoint **L;
  double **L_ne;
  int sizeL[2];
  fixedPoint **K;
  double **K_ne;
  int sizeK[2];
  fixedPoint **gamma3;
  double **gamma3_ne;
  int sizegamma3[2];
  fixedPoint **gamma2;
  double **gamma2_ne;
  int sizegamma2[2];
  fixedPoint **gamma1;
  double **gamma1_ne;
  int sizegamma1[2];
  fixedPoint **LC;
  double **LC_ne;
  int sizeLC[2];
  fixedPoint **LCB;
  double **LCB_ne;
  int sizeLCB[2];
  fixedPoint **LCA;
  double **LCA_ne;
  int sizeLCA[2];

  int sizeA_BK[2];
  double **A_BK_ne;
  fixedPoint **A_BK;

  int sizeBug[2];
  double **Bug_ne;
  fixedPoint **Bug;

  int sizeCusum[2];
  fixedPoint **Cusum;
  int sizeNu[2];
  fixedPoint **Nu;
  
  int sizeTau[2];
  fixedPoint **Tau;
  


  fixedPoint **uTilder;
  double **uTilder_ne;
  int sizeuTilder[2];
  fixedPoint **xgamma;
  double **xgamma_ne;
  int sizexgamma[2];
  fixedPoint **xHatk;
  double **xHatk_ne;
  int sizexHatk[2];
  fixedPoint **uk;
  int sizeuk[2];
  fixedPoint **yp;
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

  

  // Simulates one time step of the system given the control action u[k]
  // TODO: change name. This is a simulation of the system evolution, not a measurement
  //       add noise
  float measureState(fixedPoint **uk, int k) {
    auto revealU_init = high_resolution_clock::now();
    for (int i = 0; i < this->sizeur[0]; i++) 
      for (int j = 0; j < this->sizeur[1]; j++) 
        this->uk_ne[i][j] = uk[i][j].reveal<double>( parties[0] );
    auto revealU_end = high_resolution_clock::now();
    auto revealU_time = std::chrono::duration_cast<std::chrono::microseconds>(revealU_end - revealU_init).count();
    

    double *AxkIn = new double[this->sizeA[0]];
    double *Buk = new double[this->sizeB[0]];
    // Compute A*x, B*u 
    matrixVecMulNE(this->A_ne, this->xk_ne, AxkIn, this->sizeA);
    matrixVecMulNE(this->B_ne, this->uk_ne, Buk, this->sizeB);
    // Compute the state A*x+B*u
    for (int i = 0; i < this->sizeA[0]; i++) {
      this->xk_ne[i][0] = AxkIn[i] + Buk[i]  /*+ this->process_noise[k][i]*/;
    }
    delete[] AxkIn;
    delete[] Buk;
    return revealU_time;
  }

  // Computes the measurements y[k] 
  // Updates zk to use it in the emp library for the next iteration
  float computezk(int k) {
    matrixMulNE(this->C_ne, this->xk_ne, this->zk_ne, this->sizeC, this->sizexk);
    for(int i=0; i< this->sizezk[0]; i++){
      this->zk_ne[i][0] = this->zk_ne[i][0] /*+ this->sensor_noise[k][i]*/;
    }
    auto init_new_labels = high_resolution_clock::now();
    // for(int i=0; i<this->sizezk[0]; i++){ // This is used to inject an attack. Delete before push
    //   for(int j=0; j<this->sizezk[1]; j++){
    //     if(k>80 && k<83 && i == 4){
    //       this->zk_ne[i][j] = this->zk_ne[i][j] + 4;
    //       this->zk[i][j] = fixedPoint(this->zk_ne[i][j], integerBits, decimalBits, parties[0]);
    //     }else{
    //       this->zk[i][j] = fixedPoint(this->zk_ne[i][j], this->integerBits, this->decimalBits, this->parties[0]);
    //     }
    //   }
    // }
    setData_GC(this->zk, this->zk_ne, this->sizezk, this->parties[0] );
    
    auto end_new_labels = high_resolution_clock::now();
    auto label_generation = std::chrono::duration_cast<std::chrono::microseconds>(end_new_labels - init_new_labels).count();
    return label_generation;
  }

  // Computes the gamma matrices in plaintext.
  // Nomenclature based on 
  // Encrypted LQG using Labeled Homomorphic Encryption. https://www.georgejpappas.org/papers/Paper264.pdf
  void computeControlConstants() {
    // compute gamma3
    matrixMulNE(this->L_ne, this->C_ne, this->LC_ne, this->sizeL, this->sizeC);
    matrixMulNE(this->LC_ne, this->B_ne, this->LCB_ne, this->sizeLC, this->sizeB);
    for (int i = 0; i < this->sizegamma3[0]; i++) {
      for (int j = 0; j < this->sizegamma3[1]; j++) {
        this->gamma3_ne[i][j] = this->B_ne[i][j] - this->LCB_ne[i][j];
      }
    }

    // compute gamma2
    matrixMulNE(this->gamma3_ne, this->K_ne, this->gamma2_ne, this->sizegamma3,
              this->sizeK);

    // compute gamma1
    matrixMulNE(this->LC_ne, this->A_ne, this->LCA_ne, this->sizeLC, this->sizeA);
    for (int i = 0; i < this->sizegamma1[0]; i++) {
      for (int j = 0; j < this->sizegamma1[1]; j++) {
        this->gamma1_ne[i][j] =
            (this->A_ne[i][j] - this->LCA_ne[i][j]) - this->gamma2_ne[i][j];
      }
    }

    double **BK_ne;
    BK_ne = new double *[this->sizeA[0]];
    for (int i = 0; i < this->sizeA[0]; i++) {
      BK_ne[i] = new double[this->sizeA[1]];
    }

    matrixMulNE(this->B_ne, this->K_ne, BK_ne, this->sizeB, this->sizeK);
    for (int i = 0; i < this->sizeA[0]; i++) {
      for (int j = 0; j < this->sizeA[1]; j++) {
        this->A_BK_ne[i][j] = this->A_ne[i][j] - BK_ne[i][j];
      }
    }   
     
  }

  // Create a representation of the control matrices to use the emp library
  // Assumes that the constants are already computed in plaintext
  void garbleControlConstants(  ) {
    // gamma3
    // Puts data into cloud's secrets
    setZero_GC( this->K,   this->sizeK, parties[1]);
    setZero_GC( this->L,   this->sizeL, parties[1]);
    setZero_GC( this->B,   this->sizeB, parties[1]);
    setZero_GC( this->Tau, this->sizeTau, parties[1]);
    setZero_GC( this->Nu,  this->sizeNu, parties[1]);
    
    
    
    // Puts data into system's secrets
    setData_GC( this->xk,    this->xk_ne,    this->sizezk, parties[0] );
    setData_GC( this->xr,    this->xr_ne,    this->sizexr, parties[0] );
    setData_GC( this->ur,    this->ur_ne,    this->sizeur, parties[0] );
    setData_GC( this->xHatk, this->xHatk_ne, this->sizexHatk, parties[0] );
    setData_GC( this->yp,    this->xHatk_ne, this->sizexHatk, parties[0] );


    setZero_GC( this->gamma1, this->sizegamma1, parties[1]);
    setZero_GC( this->gamma2, this->sizegamma2, parties[1]);
    setZero_GC( this->gamma3, this->sizegamma3, parties[1]);
    setZero_GC( this->A_BK, this->sizeA_BK, parties[1] );
    
    
  }


  // Computes the constants related to the system references.
  // It also computes in plaintext matrices to compute the prediction
  float computeReferenceConstants() {
    this->sizeuTilder[0] = sizeur[0];
    this->sizeuTilder[1] = sizeur[1];
    this->sizexgamma[0] = sizexr[0];
    this->sizexgamma[1] = sizexr[1];

    this->uTilder = initSize_GC( this->sizeuTilder );
    this->xgamma  = initSize_GC( this->sizexgamma );
    fixedPoint **gamma2xr = initSize_GC( this->sizegamma2 );
    fixedPoint **gamma3ur = initSize_GC( this->sizegamma3 );

    fixedPoint **Kxr = new fixedPoint *[this->sizeK[0]];
    for (int i = 0; i < this->sizeK[0]; i++) {
      Kxr[i] = new fixedPoint[this->sizexr[1]];
    } 

    auto init = high_resolution_clock::now();
    matrixMul(this->K, this->xr, Kxr, this->sizeK, this->sizexr);

    

    for (int i = 0; i < this->sizeuTilder[0]; i++) {
      for (int j = 0; j < this->sizeuTilder[1]; j++) {
        this->uTilder[i][j] = this->ur[i][j] + Kxr[i][j];
      }
    }
    matrixMul(this->gamma2, this->xr, gamma2xr, this->sizegamma2, this->sizexr);
    matrixMul(this->gamma3, this->ur, gamma3ur, this->sizegamma3, this->sizeur);
    for (int i = 0; i < this->sizexgamma[0]; i++) {
      for (int j = 0; j < this->sizexgamma[1]; j++) {
        this->xgamma[i][j] = gamma3ur[i][j] + gamma2xr[i][j];
      }
    }
    // compute B u_{\Gamma}
    matrixMul(this->B, this->uTilder, this->Bug, this->sizeB, this->sizeuTilder);
    auto end = high_resolution_clock::now();
    auto reference_constant = std::chrono::duration_cast<std::chrono::microseconds>(end - init).count();
    
    delete[] gamma2xr;
    delete[] gamma3ur;
    delete[] Kxr;
    return reference_constant;
  }

  // Creates a representation of the constants related to references to be used with the emp toolkit
  // Assumes that constants are already computed in plaintext.
  void garbleReferenceConstants(int load_data) {
    this->sizeuTilder[0] = sizeur[0];
    this->sizeuTilder[1] = sizeur[1];
    this->sizexgamma[0] = sizexk[0];
    this->sizexgamma[1] = sizexk[1];

    this->uTilder = initSize_GC( this->sizeuTilder );
    this->xgamma = initSize_GC( this->sizexgamma );

    
    setZero_GC( this->uTilder, this->sizeuTilder, parties[1]);
    setZero_GC( this->xgamma,  this->sizexgamma, parties[1]);

    for (int i = 0; i < this->sizeB[0]; i++) {
      for (int j = 0; j < this->sizexk[1]; j++) {
        this->Bug[i][j] = fixedPoint(0, decimalBits, integerBits, parties[1]);
      }
    }
    
    

  }  
  // loads all the matrices required to compute the controller
  // and to simulate the system behavior
  void inputData(  ) {
    string data_folder = "Data/";
    // Initializes matrices A, B, C, x0, y0, xr, ur, tau, nu
    this->A_ne     = init_size_file( data_folder + "A.txt", this->sizeA);
    this->B_ne     = init_size_file( data_folder + "B.txt", this->sizeB);
    this->C_ne     = init_size_file( data_folder + "C.txt", this->sizeC);    
    this->xk_ne    = init_size_file( data_folder + "x0.txt", this->sizexk);
    this->zk_ne    = init_size_file( data_folder + "x0.txt", this->sizezk);
    this->xHatk_ne = init_size_file( data_folder + "x0.txt", this->sizexHatk);
    this->ur_ne    = init_size_file( data_folder + "ur.txt", this->sizeur);
    this->xr_ne    = init_size_file( data_folder + "xr.txt", this->sizexr);

    
    // To fix this, I have to find a way to produce multivariate random numbers...
    // [k][sensor_number]
    // this->process_noise = init_size_file( data_folder + "process_noise.txt", this->size_process_noise);
    // this->sensor_noise  = init_size_file( data_folder + "sensor_noise.txt",  this->size_sensor_noise);
    // readFile(this->process_noise, data_folder + "process_noise.txt", this->size_process_noise);
    // readFile(this->sensor_noise, data_folder + "sensor_noise.txt", this->size_sensor_noise);

    
    // this->Tau_ne = init_size_file( data_folder + "Tau.txt", this->sizeTau);
    // this->Nu_ne  = init_size_file( data_folder + "Nu.txt", this->sizeNu);

    // Load data of matrices A, B, C, x0, y0, xr, ur, tau, nu
    readFile(this->A_ne, data_folder + "A.txt", this->sizeA);
    readFile(this->B_ne, data_folder + "B.txt", this->sizeB);
    readFile(this->C_ne, data_folder + "C.txt", this->sizeC);
    readFile(this->xk_ne, data_folder + "x0.txt", this->sizexk);
    readFile(this->xHatk_ne, data_folder + "x0.txt", this->sizexHatk);
    readFile(this->zk_ne, data_folder + "x0.txt", this->sizezk);
    readFile(this->ur_ne, data_folder + "ur.txt", this->sizeur);
    readFile(this->xr_ne, data_folder + "xr.txt", this->sizexr);
    

    // this->sizezk[0] = this->sizeC[0];
    // this->sizezk[1] = this->sizexk[1];
    
    // Initializes as fixedPoint System's secrets
    this->ur     = initSize_GC( this->sizeur );
    this->xr     = initSize_GC( this->sizexr );
    this->xk     = initSize_GC( this->sizexk );
    this->zk     = initSize_GC( this->sizezk );
    this->xHatk  = initSize_GC( this->sizexHatk );
    this->yp     = initSize_GC( this->sizexHatk );
    
    // Initializes as fixedPoint cloud's secrets
    this->K   = init_size_using_file_GC( data_folder + "K.txt", this->sizeK );
    this->L   = init_size_using_file_GC( data_folder + "L.txt", this->sizeL );
    this->Tau = init_size_using_file_GC( data_folder + "Tau.txt", this->sizeTau );
    this->Nu  = init_size_using_file_GC( data_folder + "Nu.txt", this->sizeNu );
    this->B   = initSize_GC( this->sizeB );
    
    
    this->sizeuk[0] = this->sizeur[0];
    this->sizeuk[1] = this->sizeur[1];
    
    this->uk_ne  = init_size( this->sizeur );


    this->sizegamma3[0] = this->sizeB[0];
    this->sizegamma3[1] = this->sizeB[1];
    this->sizeLC[0] = this->sizeL[0];
    this->sizeLC[1] = this->sizeC[1];
    this->sizegamma2[0] = this->sizegamma3[0];
    this->sizegamma2[1] = this->sizeK[1];
    this->sizegamma1[0] = this->sizeA[0];
    this->sizegamma1[1] = this->sizeA[1];
    this->sizeA_BK[0] = this->sizeA[0];
    this->sizeA_BK[1] = this->sizeA[1];
    this->sizeBug[0] = this->sizeB[0];
    this->sizeBug[1] = this->sizexk[1];
    this->sizeLCB[0] = this->sizeLC[0];
    this->sizeLCB[1] = this->sizeB[1];

    
    this->gamma1 = initSize_GC( this->sizegamma1 );
    this->gamma2 = initSize_GC( this->sizegamma2 );
    this->gamma3 = initSize_GC( this->sizegamma3 );
    this->Bug    = initSize_GC( this->sizeBug );
    this->A_BK   = initSize_GC( this->sizeA_BK );


  }
  
};

#endif
