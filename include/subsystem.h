// This class represents the physical system.
// Change all Alice to BOB?
#ifndef SUBSYSTEM_H
#define SUBSYSTEM_H
#include "fixedPoint.h"

class subSystem {
public:
  // declares the constants for the system.
  // _ne refers to variables in plaintext
  fixedPoint **x0;
  double **x0_ne;
  int sizex0[2];
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

  int decimalBits = 24;
  int integerBits = decimalBits;
  int totalBits = decimalBits + integerBits;

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
  void garbleControlConstants() {
    // gamma3
    setData_GC(this->gamma3, this->gamma3_ne, this->sizegamma3, ALICE);
    setData_GC(this->gamma2, this->gamma2_ne, this->sizegamma2, ALICE);
    setData_GC(this->gamma1, this->gamma1_ne, this->sizegamma1, ALICE);
    setData_GC(this->A_BK, this->A_BK_ne, this->sizeA, ALICE);
  }


  // Computes the constants related to the system references.
  // It also computes in plaintext matrices to compute the prediction
  void computeReferenceConstants() {
    this->sizeuTilder[0] = sizeur[0];
    this->sizeuTilder[1] = sizeur[1];
    this->uTilder_ne = initSize(this->uTilder_ne, this->sizeuTilder);

    this->sizexgamma[0] = sizexr[0];
    this->sizexgamma[1] = sizexr[1];
    this->xgamma_ne = initSize(this->xgamma_ne, this->sizexgamma);
    

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
  void garbleReferenceConstants() {
    this->uTilder = initSize_GC( this->sizeuTilder );
    this->xgamma = initSize_GC( this->sizexgamma );
    setData_GC( this->uTilder, this->uTilder_ne, this->sizeuTilder, ALICE);
    setData_GC( this->xgamma, this->xgamma_ne, this->sizexgamma, ALICE);

    for (int i = 0; i < this->sizeB[0]; i++) {
      for (int j = 0; j < this->sizexk[1]; j++) {
        this->Bug[i][j] = fixedPoint(this->Bug_ne[i][j], decimalBits, integerBits, ALICE);
      }
    }

  }  
  // loads all the matrices required to compute the controller
  // and to simulate the system behavior
  void inputData() {
    
    // Load matrix A
    this->A_ne = initSizeFile( "Data/A.txt", this->sizeA);
    readFile(this->A_ne, "Data/A.txt", this->sizeA);
    this->A = initSize_GC(this->sizeA);

    // Load matrix B
    this->B_ne = initSizeFile( "Data/B.txt", this->sizeB);
    readFile(this->B_ne, "Data/B.txt", this->sizeB);
    this->B = initSize_GC(this->sizeB);

    // Load matrix C
    this->C_ne = initSizeFile( "Data/C.txt", this->sizeC);
    readFile(this->C_ne, "Data/C.txt", this->sizeC);
    this->C = initSize_GC(this->sizeC);

    // Load vector ur
    this->ur_ne = initSizeFile( "Data/ur.txt", this->sizeur);
    readFile(this->ur_ne, "Data/ur.txt", this->sizeur);

    // Initializa vector uk
    this->sizeuk[0] = this->sizeur[0];
    this->sizeuk[1] = this->sizeur[1];
    this->uk_ne = initSize(this->uk_ne, this->sizeuk);

    // Load vector ur
    this->ur = initSize_GC( this->sizeur );
    this->xr_ne = initSizeFile( "Data/xr.txt", this->sizexr);
    readFile(this->xr_ne, "Data/xr.txt", this->sizexr);
    this->xr = initSize_GC( this->sizexr );

    // Load vector x0
    this->x0_ne = initSizeFile( "Data/x0.txt", this->sizex0);
    readFile(this->x0_ne, "Data/x0.txt", this->sizex0);
    this->x0 = initSize_GC( this->sizex0 );
    
    // Load matrix K
    this->K_ne = initSizeFile( "Data/K.txt", this->sizeK);
    readFile(this->K_ne, "Data/K.txt", this->sizeK);
    this->K = initSize_GC( this->sizeK );

    // Load matrix L
    this->L_ne = initSizeFile( "Data/L.txt", this->sizeL);
    readFile(this->L_ne, "Data/L.txt", this->sizeL);
    this->L = initSize_GC( this->sizeL );
    
    // Load \nu
    this->Nu_ne = initSizeFile( "Data/Nu.txt", this->sizeNu);
    readFile(this->Nu_ne, "Data/Nu.txt", this->sizeNu);
    this->Nu = initSize_GC( this->sizeNu );


    
    this->Tau_ne = initSizeFile( "Data/Tau.txt", this->sizeTau);
    readFile(this->Tau_ne, "Data/Tau.txt", this->sizeTau);
    this->Tau = initSize_GC( this->sizeTau );

    // Load zk
    this->sizezk[0] = this->sizeC[0];
    this->sizezk[1] = this->sizex0[1];
    this->zk = initSize_GC( this->sizezk );
    this->zk_ne = initSize( this->zk_ne, this->sizezk );
    
    this->sizexk[0] = this->sizex0[0];
    this->sizexk[1] = this->sizex0[1];
    this->xk = initSize_GC( this->sizexk);
    this->xk_ne = initSize( this->xk_ne, this->sizexk);
    
    setData_GC( this->A, this->A_ne, this->sizeA, ALICE);
    setData_GC( this->B, this->B_ne, this->sizeB, ALICE);
    setData_GC( this->C, this->C_ne, this->sizeC, ALICE);
    setData_GC( this->ur, this->ur_ne, this->sizeur, ALICE);  
    setData_GC( this->xr, this->xr_ne, this->sizexr, ALICE);
    setData_GC( this->x0, this->x0_ne, this->sizex0, ALICE);
    setData_GC( this->xk, this->x0_ne, this->sizexk, ALICE);
    setData( this->xk_ne,  this->x0_ne, this->sizexk);

    setData_GC( this->zk, this->x0_ne, this->sizezk, ALICE);
    setData( this->zk_ne,  this->x0_ne, this->sizezk);
    

    // negative because K.txt holds the nagation of the control gain K
    for (int i = 0; i < this->sizeK[0]; i++) {
      for (int j = 0; j < this->sizeK[1]; j++) {
        this->K_ne[i][j] = -this->K_ne[i][j];
        this->K[i][j] = fixedPoint( this->K_ne[i][j], decimalBits, integerBits, ALICE);
      }
    }

    setData_GC( this->L, this->L_ne, this->sizeL, ALICE);
    setData_GC( this->Nu, this->Nu_ne, this->sizeNu, ALICE);
    setData_GC( this->Tau, this->Tau_ne, this->sizeTau, ALICE);


    

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

    this->gamma3 = initSize_GC( this->sizegamma3 );
    this->gamma3_ne = initSize(this->gamma3_ne, this->sizegamma3);

    this->LC = initSize_GC( this->sizeLC );
    this->LC_ne = initSize(this->LC_ne, this->sizeLC);
    
    this->LCB = initSize_GC( this->sizeLCB );
    this->LCB_ne = initSize(this->LCB_ne, this->sizeLCB);

    this->gamma2 = initSize_GC( this->sizegamma2 );
    this->gamma2_ne = initSize( this->gamma2_ne, this->sizegamma2);
    
    this->gamma1 = initSize_GC( this->sizegamma1 );
    this->gamma1_ne = initSize( this->gamma1_ne, this->sizegamma1);

    this->LCA = initSize_GC( this->sizeA );
    this->LCA_ne = initSize( this->LCA_ne, this->sizeA);

    this->A_BK = initSize_GC( this->sizeA_BK );
    this->A_BK_ne = initSize( this->A_BK_ne, this->sizeA_BK);

    this->Bug = initSize_GC( this->sizeBug );
    this->Bug_ne = initSize( this->Bug_ne, this->sizeBug);

    

  }
  ///////////////////////////////////////////////////////////////////////////
  // Functions required for the operations. Not focused on the control system
  /////////////////////////////////////x//////////////////////////////////////

  // Computes the multiplication between matrices A and B
  void matrixMul(fixedPoint **A, fixedPoint **B, fixedPoint **ret, int *ASize,
                 int *BSize) {
    for (int i = 0; i < ASize[0]; i++) {
      for (int j = 0; j < BSize[1]; j++) {
        ret[i][j] = (A[i][0] * B[0][j]);
        for (int k = 1; k < ASize[1]; k++) {
          ret[i][j] = ret[i][j] + (A[i][k] * B[k][j]);
        }
      }
    }
  }

  // Computes the multiplication between matrices A and B which are in plaintext
  void matrixMulNE(double **A, double **B, double **ret, int *ASize,
                 int *BSize) {
    for (int i = 0; i < ASize[0]; i++) {
      for (int j = 0; j < BSize[1]; j++) {
        ret[i][j] = 0;
        for (int k = 0; k < ASize[1]; k++) {
          ret[i][j] = ret[i][j] + (A[i][k] * B[k][j]);
        }
      }
    }
  }

  // Computes the multiplication between matrix A and vector B
  void matrixVecMul(fixedPoint **A, fixedPoint **B, fixedPoint *ret,
                    int *size) {
    for (int i = 0; i < size[0]; i++) {
      ret[i] = ((A[i][0]) * (B[0][0]));
      for (int j = 1; j < size[1]; j++) {
        ret[i] = ret[i] + ((A[i][j]) * (B[j][0]));
      }
    }
  }
  // Computes the multiplication between matrix A and vector B which are in plaintext
  void matrixVecMulNE(double **A, double **B, double *ret,
                    int *size) {
    for (int i = 0; i < size[0]; i++) {
      ret[i] = 0;
      for (int j = 0; j < size[1]; j++) {
        ret[i] = ret[i] + ((A[i][j]) * (B[j][0]));
      }
    }
  }

  // Reads the size of matrices stored inside the file input
  void getFileSize(string input, int *size) {
    int rowSize = 0, colSize = 0;
    fstream file;
    string inputLine, stringinput;
    // cout<<input<<endl;
    file.open(input, ios::in);
    if (!file.is_open()) {
      cout << "ERROR: file not opened" << endl;
      return;
    }
    while (getline(file, inputLine)) {
      rowSize++;
      stringstream line(inputLine);
      if (rowSize == 1) {
        while (getline(line, stringinput, ',')) {
          colSize++;
        }
      }
    }
    size[0] = rowSize;
    size[1] = colSize;
    file.close();
  }

  // Retrieves the matrices stored inside the file input
  void readFile(double **data, string input, int *size) {
    int i = 0;
    int j = 0;
    fstream file;
    string inputLine, stringinput;
    file.open(input, ios::in);
    while (getline(file, inputLine)) {
      j = 0;
      stringstream line(inputLine);
      while (getline(line, stringinput, ',')) {
        data[i][j] = stod(stringinput);
        // cout << "load data" << stringinput << endl;
        j++;
      }
      i++;
    }
  }
  
  // Initialize variables used for Garbled Circuits
  fixedPoint** initSizeFile_GC( string fileName, int *size){
    getFileSize(fileName, size);
    fixedPoint **in = new fixedPoint *[ size[0] ];
    for( int i = 0; i < size[0]; i++ ){
      in[i] = new fixedPoint[ size[1] ];
    }
    return in;
  }

  fixedPoint** initSize_GC( int *size ){
    fixedPoint **in = new fixedPoint *[ size[0] ];
    for( int i = 0; i < size[0]; i++ ){
      in[i] = new fixedPoint[ size[1] ];
    }
    return in;
  }

  void setZero_GC(fixedPoint **in, int *size, int party){
    for (int i = 0; i < size[0]; i++) {
      for (int j = 0; j < size[1]; j++) {
        in[i][j] = fixedPoint(0, decimalBits, integerBits, party);
      }
    }
  }
  
  void setData_GC(fixedPoint **in, double **data, int *size, int party){
    for (int i = 0; i < size[0]; i++) {
      for (int j = 0; j < size[1]; j++) {
        in[i][j] = fixedPoint(data[i][j], decimalBits, integerBits, party);
      }
    }
  }
  // Initialize matrices double
  double** initSizeFile( string fileName, int *size){
    getFileSize(fileName, size);
    double **in = new double *[ size[0] ];
    for( int i = 0; i < size[0]; i++ ){
      in[i] = new double[ size[1] ];
    }
    return in;
  }

  double** initSize(double **in, int *size){
    in = new double *[ size[0] ];
    for( int i = 0; i < size[0]; i++ ){
      in[i] = new double[ size[1] ];
    }
    return in;
  }

  void setZero(double **in, int *size, int party){
    for (int i = 0; i < size[0]; i++) {
      for (int j = 0; j < size[1]; j++) {
        in[i][j] = 0;
      }
    }
  }

  void setData(double **in, double **data, int *size){
    for (int i = 0; i < size[0]; i++) {
      for (int j = 0; j < size[1]; j++) {
        in[i][j] = data[i][j];
      }
    }
  }
};

#endif
