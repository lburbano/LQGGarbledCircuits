// This class represents the physical system.
// Change all Alice to BOB?
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
  double **Nu_ne;
  int sizeTau[2];
  fixedPoint **Tau;
  double **Tau_ne;


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


  subSystem() {}

  

  // Simulates one time step of the system given the control action u[k]
  // TODO: change name. This is a simulation of the system evolution, not a measurement
  //       add noise
  void measureState(fixedPoint **uk) {
    for (int i = 0; i < this->sizeur[0]; i++) {
      for (int j = 0; j < this->sizeur[1]; j++) {
        // get the control action
        this->uk_ne[i][j] = uk[i][j].reveal<double>(ALICE);
      }
    }
    double *AxkIn = new double[this->sizeA[0]];
    double *Buk = new double[this->sizeB[0]];
    // Compute A*x, B*u 
    matrixVecMulNE(this->A_ne, this->xk_ne, AxkIn, this->sizeA);
    matrixVecMulNE(this->B_ne, this->uk_ne, Buk, this->sizeB);
    // Compute the state A*x+B*u
    for (int i = 0; i < this->sizeA[0]; i++) {
      this->xk_ne[i][0] = AxkIn[i] + Buk[i] /*+ wk[i][0]*/;
    }
    delete[] AxkIn;
    delete[] Buk;
  }

  // Computes the measurements y[k] 
  // Updates zk to use it in the emp library for the next iteration
  void computezk() {
    matrixMulNE(this->C_ne, this->xk_ne, this->zk_ne, this->sizeC, this->sizexk);
    setData_GC( this->zk, zk_ne, this->sizezk, ALICE);
    
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
  void garbleControlConstants( int load_data ) {
    // gamma3
    if(load_data == 1){
      setData_GC(this->gamma1, this->gamma1_ne, this->sizegamma1, ALICE);
      setData_GC(this->A_BK, this->A_BK_ne, this->sizeA, ALICE);
    } else{
      setZero_GC( this->gamma1, this->sizegamma1, BOB);
      setZero_GC( this->A_BK, this->sizeA_BK, BOB );
    }
    
  }


  // Computes the constants related to the system references.
  // It also computes in plaintext matrices to compute the prediction
  void computeReferenceConstants() {
    this->sizeuTilder[0] = sizeur[0];
    this->sizeuTilder[1] = sizeur[1];
    this->sizexgamma[0] = sizexk[0];
    this->sizexgamma[1] = sizexk[1];

    this->uTilder_ne = init_size( this->sizeuTilder );
    this->xgamma_ne = init_size( this->sizexgamma );
    

    double **Kxr_ne = new double *[this->sizeK[0]];
    for (int i = 0; i < this->sizeK[0]; i++) {
      Kxr_ne[i] = new double[this->sizexr[1]];
    } 
    matrixMulNE(this->K_ne, this->xr_ne, Kxr_ne, this->sizeK, this->sizexr);
    for (int i = 0; i < this->sizeuTilder[0]; i++) {
      for (int j = 0; j < this->sizeuTilder[1]; j++) {
        this->uTilder_ne[i][j] = this->ur_ne[i][j] + Kxr_ne[i][j];
      }
    }
    double **gamma2xr = new double *[this->sizegamma2[0]];
    for (int i = 0; i < this->sizegamma2[0]; i++) {
      gamma2xr[i] = new double[this->sizexr[1]];
    }
    double **gamma3ur = new double *[this->sizegamma3[0]];
    for (int i = 0; i < this->sizegamma3[0]; i++) {
      gamma3ur[i] = new double[this->sizeur[1]];
    }

    matrixMulNE(this->gamma2_ne, this->xr_ne, gamma2xr, this->sizegamma2, this->sizexr);
    matrixMulNE(this->gamma3_ne, this->ur_ne, gamma3ur, this->sizegamma3, this->sizeur);
    for (int i = 0; i < this->sizexgamma[0]; i++) {
      for (int j = 0; j < this->sizexgamma[1]; j++) {
        this->xgamma_ne[i][j] = gamma3ur[i][j] + gamma2xr[i][j];
      }
    }


    // compute B u_{\Gamma}
    matrixMulNE(this->B_ne, this->uTilder_ne, this->Bug_ne, this->sizeB, this->sizeuTilder);
    
    
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

    if(load_data == 1){
      setData_GC( this->uTilder, this->uTilder_ne, this->sizeuTilder, ALICE);
      setData_GC( this->xgamma, this->xgamma_ne, this->sizexgamma, ALICE);

      for (int i = 0; i < this->sizeB[0]; i++) {
        for (int j = 0; j < this->sizexk[1]; j++) {
          this->Bug[i][j] = fixedPoint(this->Bug_ne[i][j], decimalBits, integerBits, ALICE);
        }
      }
    } else{
      setZero_GC( this->uTilder, this->sizeuTilder, BOB);
      setZero_GC( this->xgamma,  this->sizexgamma, BOB);

      for (int i = 0; i < this->sizeB[0]; i++) {
        for (int j = 0; j < this->sizexk[1]; j++) {
          this->Bug[i][j] = fixedPoint(0, decimalBits, integerBits, BOB);
        }
      }
    }
    

  }  
  // loads all the matrices required to compute the controller
  // and to simulate the system behavior
  void inputData(int load_data) {
    string data_folder = "Data/";
    // Load system matrices A, B, C
    this->A_ne  = init_size_file( data_folder + "A.txt", this->sizeA);
    this->B_ne  = init_size_file( data_folder + "B.txt", this->sizeB);
    this->C_ne  = init_size_file( data_folder + "C.txt", this->sizeC);    
    this->xk_ne = init_size_file( data_folder + "x0.txt", this->sizexk);
    this->zk_ne = init_size_file( data_folder + "x0.txt", this->sizezk);

    readFile(this->A_ne, data_folder + "A.txt", this->sizeA);
    readFile(this->B_ne, data_folder + "B.txt", this->sizeB);
    readFile(this->C_ne, data_folder + "C.txt", this->sizeC);
    readFile(this->xk_ne, data_folder + "x0.txt", this->sizexk);
    readFile(this->zk_ne, data_folder + "x0.txt", this->sizezk);

    // this->sizezk[0] = this->sizeC[0];
    // this->sizezk[1] = this->sizexk[1];
    

    this->zk  = initSize_GC( this->sizezk );
    this->xk  = initSize_GC( this->sizexk );
    
    if(load_data == 1){
      // Load matrices sizes
      this->ur_ne  = init_size_file( data_folder + "ur.txt", this->sizeur);
      this->xr_ne  = init_size_file( data_folder + "xr.txt", this->sizexr);
      this->K_ne   = init_size_file( data_folder + "K.txt", this->sizeK);
      this->L_ne   = init_size_file( data_folder + "L.txt", this->sizeL);
      this->Nu_ne  = init_size_file( data_folder + "Nu.txt", this->sizeNu);
      this->Tau_ne = init_size_file( data_folder + "Tau.txt", this->sizeTau);

      // Load matrices info
      readFile(this->ur_ne, data_folder + "ur.txt", this->sizeur);
      readFile(this->xr_ne, data_folder + "xr.txt", this->sizexr);
      readFile(this->K_ne, data_folder + "K.txt", this->sizeK);
      readFile(this->L_ne, data_folder + "L.txt", this->sizeL);
      readFile(this->Nu_ne, data_folder + "Nu.txt", this->sizeNu);
      readFile(this->Tau_ne, data_folder + "Tau.txt", this->sizeTau);

      // Initialize matrices as fixedPoint
      this->ur  = initSize_GC( this->sizeur );
      this->xr  = initSize_GC( this->sizexr );
      this->K   = initSize_GC( this->sizeK );
      this->L   = initSize_GC( this->sizeL );
      this->Nu  = initSize_GC( this->sizeNu );
      this->Tau = initSize_GC( this->sizeTau );
      

      // Set data into matrices type fixedPoint
      for (int i = 0; i < this->sizeK[0]; i++) 
        for (int j = 0; j < this->sizeK[1]; j++) 
          this->K_ne[i][j] = -this->K_ne[i][j];
      setData_GC( this->K, this->K_ne, this->sizeK, ALICE);
      setData_GC( this->L, this->L_ne, this->sizeL, ALICE);
      setData_GC( this->Nu, this->Nu_ne, this->sizeNu, ALICE);
      setData_GC( this->Tau, this->Tau_ne, this->sizeTau, ALICE);
    }else{
      this->K   = init_size_using_file_GC(data_folder + "K.txt", this->sizeK);
      this->L   = init_size_using_file_GC(data_folder + "L.txt", this->sizeL);
      this->Nu  = init_size_using_file_GC(data_folder + "Nu.txt", this->sizeNu);
      this->Tau = init_size_using_file_GC(data_folder + "Tau.txt", this->sizeTau);
      getFileSize( data_folder + "ur.txt", this->sizeur);

      setZero_GC( this->K, this->sizeK, BOB);
      setZero_GC( this->L, this->sizeL, BOB);
      setZero_GC( this->Nu, this->sizeNu, BOB);
      setZero_GC( this->Tau,  this->sizeTau, BOB);
    }
    this->sizeuk[0] = this->sizeur[0];
    this->sizeuk[1] = this->sizeur[1];
    setData_GC( this->zk, this->xk_ne, this->sizezk, ALICE);  
    setData_GC( this->xk, this->xk_ne, this->sizezk, ALICE);
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

    if(load_data == 1){
      this->gamma3_ne = init_size( this->sizegamma3 );
      this->LC_ne = init_size( this->sizeLC );
      this->LCB_ne = init_size( this->sizeLCB );
      this->gamma2_ne = init_size( this->sizegamma2 );
      this->gamma1_ne = init_size( this->sizegamma1 );
      this->LCA_ne = init_size( this->sizeA);
      this->A_BK_ne = init_size( this->sizeA_BK );
      this->Bug_ne = init_size( this->sizeBug );
    } 
    this->gamma1 = initSize_GC( this->sizegamma1 );
    this->Bug    = initSize_GC( this->sizeBug );
    this->A_BK   = initSize_GC( this->sizeA_BK );


  }
  
};

#endif
