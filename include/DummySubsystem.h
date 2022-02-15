#ifndef SUBSYSTEM_H
#define SUBSYSTEM_H
#include "fixedPoint.h"
#include "utility_functions.h"
// CHange all Alice to BOB?
class subSystem: public utility_functions {
public:
  // declares the constants for the system.
  // unlike the subsystem class, this class does not have
  // plaintexts
  fixedPoint **x0;
  int sizex0[2];
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
  int sizexHatk[2];
  fixedPoint **uk;
  int sizeuk[2];

  

  subSystem() {}

  

  // Reveals the value of u[k] to the system
  void measureState(fixedPoint **uk) {
    for (int i = 0; i < this->sizeur[0]; i++) {
      for (int j = 0; j < this->sizeur[1]; j++) {
        uk[i][j].reveal<double>(ALICE);
      }
    }
    for (int i = 0; i < this->sizeA[0]; i++) {
      //this->xk_ne[i][0] = 0 /*+ wk[i][0]*/;
    }
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
  void garbleConstants() {
    // compute gamma3
    setZero_GC( this->gamma3, this->sizegamma3, ALICE);
    setZero_GC( this->gamma2, this->sizegamma2, ALICE);
    setZero_GC( this->gamma1, this->sizegamma1, ALICE);
    setZero_GC( this->A_BK, this->sizeA_BK, ALICE );
  }

  // Creates a representation of the constants related to references to be used with the emp toolkit
  // The values are initialized as zero here since the cloud should not learn the plaintexts
  void garbleReferenceConstants() {

    this->sizeuTilder[0] = sizeur[0];
    this->sizeuTilder[1] = sizeur[1];
    this->sizexgamma[0] = sizexr[0];
    this->sizexgamma[1] = sizexr[1];
    
    this->uTilder = initSize_GC( this->sizeuTilder);
    this->xgamma = initSize_GC( this->sizexgamma);
    setZero_GC(this->uTilder, this->sizeuTilder, ALICE);
    setZero_GC(this->xgamma, this->sizexgamma, ALICE);

    for (int i = 0; i < this->sizeB[0]; i++) {
      for (int j = 0; j < this->sizexk[1]; j++) {
        this->Bug[i][j] = fixedPoint(0, decimalBits, integerBits, ALICE);
      }
    }
  }


///////////////////////////////////////////////////////////////////////////
// Functions required for the operations. Not focused on the control system
///////////////////////////////////////////////////////////////////////////

  // Creates instances of vectors and matrices to be used by the emp toolkit
  void inputData() {
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
    
    this->A = initSizeFile_GC( "Data/A.txt", this->sizeA );
    this->B = initSizeFile_GC( "Data/B.txt", this->sizeB );
    this->C = initSizeFile_GC( "Data/C.txt", this->sizeC );
    this->ur = initSizeFile_GC( "Data/ur.txt", this->sizeur );
    this->xr = initSizeFile_GC( "Data/xr.txt", this->sizexr );
    this->x0 = initSizeFile_GC( "Data/x0.txt", this->sizex0 );
    this->K = initSizeFile_GC( "Data/K.txt", this->sizeK );
    this->L = initSizeFile_GC( "Data/L.txt", this->sizeL );
    this->Nu = initSizeFile_GC( "Data/Nu.txt", this->sizeNu );
    this->Tau = initSizeFile_GC( "Data/Tau.txt", this->sizeTau );
    
    
    this->sizezk[0] = this->sizeC[0];
    this->sizezk[1] = this->sizex0[1];
    this->zk = initSize_GC( this->sizezk );    
    
    this->sizexk[0] = this->sizex0[0];
    this->sizexk[1] = this->sizex0[1];
    this->xk = initSize_GC( this->sizexk );

    setZero_GC(this->A, this->sizeA, ALICE);
    setZero_GC(this->B, this->sizeB, ALICE);
    setZero_GC(this->C, this->sizeC, ALICE);
    setZero_GC(this->ur, this->sizeur, ALICE);
    setZero_GC(this->xr, this->sizexr, ALICE);
    setZero_GC(this->x0, this->sizex0, ALICE);
    setZero_GC(this->xk, this->sizexk, ALICE);
    setZero_GC(this->zk, this->sizezk, ALICE);
    setZero_GC(this->K, this->sizeK, ALICE);
    setZero_GC(this->L, this->sizeL, ALICE);
    setZero_GC(this->Nu, this->sizeNu, ALICE);
    setZero_GC(this->Tau, this->sizeTau, ALICE);


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


    this->gamma3 = initSize_GC( this->sizegamma3 );
    this->LC = initSize_GC(  this->sizeLC );    
    this->LCB = new fixedPoint *[this->sizeLC[0]];
    for (int i = 0; i < this->sizeLC[0]; i++) {
      this->LCB[i] = new fixedPoint[this->sizeB[1]];
    }
    this->gamma2 = initSize_GC(  this->sizegamma2 ); 
    this->gamma1 = initSize_GC( this->sizegamma1 ); 
    this->LCA = initSize_GC( this->sizeA );     
    this->A_BK = initSize_GC( this->sizeA_BK );     
    this->Bug = initSize_GC( this->sizeBug );
  }

};

#endif
