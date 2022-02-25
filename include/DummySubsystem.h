#ifndef SUBSYSTEM_H
#define SUBSYSTEM_H
#include "fixedPoint.h"
#include "garbled_circuit_user.h"
// CHange all Alice to BOB?
class subSystem: public garbled_circuit_user {
public:
  // declares the constants for the system.
  // unlike the subsystem class, this class does not have
  // plaintexts
  
  
  fixedPoint **xr;
  int sizexr[2];
  fixedPoint **ur;
  int sizeur[2];
  fixedPoint **xk;
  int sizexk[2];
  fixedPoint **zk;
  int sizezk[2];
  fixedPoint **A;
  int sizeA[2];
  fixedPoint **B;
  int sizeB[2];
  fixedPoint **C;
  int sizeC[2];

  fixedPoint **L;
  int sizeL[2];
  fixedPoint **K;
  int sizeK[2];
  fixedPoint **gamma3;
  int sizegamma3[2];
  fixedPoint **gamma2;
  int sizegamma2[2];
  fixedPoint **gamma1;
  int sizegamma1[2];
  fixedPoint **LC;
  int sizeLC[2];
  fixedPoint **LCB;
  int sizeLCB[2];
  fixedPoint **LCA;
  int sizeLCA[2];

  int sizeA_BK[2];
  fixedPoint **A_BK;

  int sizeBug[2];
  fixedPoint **Bug;

  int sizeCusum[2];
  fixedPoint **Cusum;
  int sizeNu[2];
  fixedPoint **Nu;
  int sizeTau[2];
  fixedPoint **Tau;

  fixedPoint **uTilder;
  int sizeuTilder[2];
  fixedPoint **xgamma;
  int sizexgamma[2];
  fixedPoint **xHatk;
  fixedPoint **yp;
  int sizexHatk[2];
  fixedPoint **uk;
  int sizeuk[2];

  // double **xr_ne;
  // double **ur_ne;
  double **xk_ne;
  double **zk_ne;
  double **A_ne;
  double **B_ne;
  double **C_ne;
  double **uk_ne;
  double **L_ne;
  double **K_ne;
  double **gamma3_ne;
  double **gamma2_ne;
  double **gamma1_ne;
  double **LC_ne;
  double **LCB_ne;
  double **LCA_ne;
  double **A_BK_ne;
  double **Bug_ne;
  double **Nu_ne;
  double **Tau_ne;
  double **uTilder_ne;
  double **xgamma_ne;
  

  subSystem() {}

  

  // Reveals the value of u[k] to the system
  void measureState(fixedPoint **uk) {
    for (int i = 0; i < this->sizeur[0]; i++) 
      for (int j = 0; j < this->sizeur[1]; j++) 
        uk[i][j].reveal<double>(ALICE);
  }

  // Creates z[k] to be used by the emp library.
  // Value is set to zero since the cloud party does not have the plaintext of z[k]
  void computezk() {
    setZero_GC( this->zk, this->sizezk, ALICE);
  }

  // Creates a representation of the constants related to the controller to be used with the emp toolkit
  // The values are initialized as zero here since the cloud should not learn the plaintexts
  // Nomenclature based on 
  // Encrypted LQG using Labeled Homomorphic Encryption. https://www.georgejpappas.org/papers/Paper264.pdf
  void garbleConstants(int load_data) {
    
    // Initialize Cloud's secret
    setData_GC( this->K, this->K_ne, this->sizeK, BOB);
    setData_GC( this->L, this->L_ne, this->sizeL, BOB);
    setData_GC( this->B, this->B_ne, this->sizeB, BOB);
    setData_GC( this->Tau, this->Tau_ne, this->sizeTau, BOB);
    setData_GC( this->Nu, this->Nu_ne, this->sizeNu, BOB);

    // Initialize System's secret
    setZero_GC( this->zk,    this->sizezk,    ALICE);  
    setZero_GC( this->xk,    this->sizexk,    ALICE);
    setZero_GC( this->xr,    this->sizexr,    ALICE);
    setZero_GC( this->ur,    this->sizeur,    ALICE);  
    setZero_GC( this->xHatk, this->sizexHatk, ALICE);
    setZero_GC( this->yp,    this->sizexHatk, ALICE);

    setData_GC(this->gamma1, this->gamma1_ne, this->sizegamma1, BOB);
    setData_GC(this->gamma2, this->gamma2_ne, this->sizegamma2, BOB);
    setData_GC(this->gamma3, this->gamma3_ne, this->sizegamma3, BOB);
    setData_GC(this->A_BK, this->A_BK_ne, this->sizeA, BOB);
    
    
  }

  // Creates a representation of the constants related to references to be used with the emp toolkit
  // The values are initialized as zero here since the cloud should not learn the plaintexts
  void garbleReferenceConstants(int load_data) {

    this->sizeuTilder[0] = sizeur[0];
    this->sizeuTilder[1] = sizeur[1];
    this->sizexgamma[0] = sizexk[0];
    this->sizexgamma[1] = sizexk[1];
    
    this->uTilder = initSize_GC( this->sizeuTilder);
    this->xgamma = initSize_GC( this->sizexgamma);

    
    setData_GC( this->uTilder, this->uTilder_ne, this->sizeuTilder, BOB);
    setData_GC( this->xgamma, this->xgamma_ne, this->sizexgamma, BOB);

    for (int i = 0; i < this->sizeB[0]; i++) {
      for (int j = 0; j < this->sizexk[1]; j++) {
        this->Bug[i][j] = fixedPoint(this->Bug_ne[i][j], decimalBits, integerBits, BOB);
      }
    }
    
    
  }

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

  void computeReferenceConstants() {
    this->sizeuTilder[0] = sizeur[0];
    this->sizeuTilder[1] = sizeur[1];
    this->sizexgamma[0] = sizexr[0];
    this->sizexgamma[1] = sizexr[1];

    this->uTilder = initSize_GC( this->sizeuTilder );
    this->xgamma  = initSize_GC( this->sizexgamma );
    

    fixedPoint **Kxr = new fixedPoint *[this->sizeK[0]];
    for (int i = 0; i < this->sizeK[0]; i++) {
      Kxr[i] = new fixedPoint[this->sizexr[1]];
    } 
    matrixMul(this->K, this->xr, Kxr, this->sizeK, this->sizexr);
    for (int i = 0; i < this->sizeuTilder[0]; i++) {
      for (int j = 0; j < this->sizeuTilder[1]; j++) {
        this->uTilder[i][j] = this->ur[i][j] + Kxr[i][j];
      }
    }
    fixedPoint **gamma2xr = new fixedPoint *[this->sizegamma2[0]];
    for (int i = 0; i < this->sizegamma2[0]; i++) {
      gamma2xr[i] = new fixedPoint[this->sizexr[1]];
    }
    fixedPoint **gamma3ur = new fixedPoint *[this->sizegamma3[0]];
    for (int i = 0; i < this->sizegamma3[0]; i++) {
      gamma3ur[i] = new fixedPoint[this->sizeur[1]];
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
  }


/*-------------------------------------------------------------------------
Functions required for the operations. Not focused on the control system
---------------------------------------------------------------------------*/

  // Creates instances of vectors and matrices to be used by the emp toolkit
  void inputData(int load_data) {
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
    // Load system matrices A, B, C, K, L
    this->A_ne  = init_size_file( data_folder + "A.txt", this->sizeA);
    this->B_ne  = init_size_file( data_folder + "B.txt", this->sizeB);
    this->C_ne  = init_size_file( data_folder + "C.txt", this->sizeC);    
    this->K_ne  = init_size_file( data_folder + "K.txt", this->sizeK);
    this->L_ne  = init_size_file( data_folder + "L.txt", this->sizeL);
    this->Tau_ne = init_size_file( data_folder + "Tau.txt", this->sizeTau);
    this->Nu_ne  = init_size_file( data_folder + "Nu.txt", this->sizeNu);

    // Load data of matrices A, B, C, K, L
    readFile(this->A_ne,   data_folder + "A.txt", this->sizeA);
    readFile(this->B_ne,   data_folder + "B.txt", this->sizeB);
    readFile(this->C_ne,   data_folder + "C.txt", this->sizeC);
    readFile(this->K_ne,   data_folder + "K.txt", this->sizeK);
    readFile(this->L_ne,   data_folder + "L.txt", this->sizeL);
    readFile(this->Tau_ne, data_folder + "Tau.txt", this->sizeTau);
    readFile(this->Nu_ne,  data_folder + "Nu.txt", this->sizeNu);
    for (int i = 0; i < this->sizeK[0]; i++) 
      for (int j = 0; j < this->sizeK[1]; j++) 
        this->K_ne[i][j] = -this->K_ne[i][j];
    
    // this->zk = init_size_using_file_GC( data_folder + "L.txt", this->sizezk );

    
    
    
    // Initializes system's secrets
    
    this->ur     = init_size_using_file_GC( data_folder + "ur.txt",  this->sizeur );
    this->xr     = init_size_using_file_GC( data_folder + "xr.txt",  this->sizexr );
    this->xk     = initSize_GC( this->sizexr );
    this->sizezk[0] = this->sizeC[0];
    this->sizezk[1] = this->sizexr[1];
    this->zk     = initSize_GC( this->sizezk );
    this->xHatk  = init_size_using_file_GC( data_folder + "x0.txt", this->sizexHatk );
    this->yp     = initSize_GC( this->sizezk );

    this->sizexk[0] = this->sizexr[0];
    this->sizexk[1] = this->sizexr[1];
    // Initializes cloud's secrets
    this->K   = initSize_GC( this->sizeK );
    this->L   = initSize_GC( this->sizeL );
    this->B   = initSize_GC( this->sizeB );
    this->Nu  = initSize_GC( this->sizeNu );
    this->Tau = initSize_GC( this->sizeTau );
    this->sizeuk[0] = this->sizeur[0];
    this->sizeuk[1] = this->sizeur[1];
    



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

    
    this->gamma3_ne  = init_size( this->sizegamma3 );
    this->LC_ne      = init_size( this->sizeLC );
    this->LCB_ne     = init_size( this->sizeLCB );
    this->gamma2_ne  = init_size( this->sizegamma2 );
    this->gamma1_ne  = init_size( this->sizegamma1 );
    this->LCA_ne     = init_size( this->sizeA);
    this->A_BK_ne    = init_size( this->sizeA_BK );
    this->Bug_ne     = init_size( this->sizeBug );
     
    this->gamma1    = initSize_GC( this->sizegamma1 );
    this->gamma2    = initSize_GC( this->sizegamma2 );
    this->gamma3    = initSize_GC( this->sizegamma3 );
    this->Bug       = initSize_GC( this->sizeBug );
    this->A_BK      = initSize_GC( this->sizeA_BK );


  }

};

#endif
