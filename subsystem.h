// This class represents the physical system.
// 
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
  fixedPoint **xtemp;
  double **xtemp_ne;
  int sizextemp[2];
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
    for (int i = 0; i < this->sizezk[0]; i++) {
      this->zk[i] = new fixedPoint[this->sizezk[1]];
      for (int j = 0; j < this->sizezk[1]; j++) {
        this->zk[i][j] = fixedPoint(zk_ne[i][j], 24, 24, ALICE);
      }
    }
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
        cout<< this->A_BK_ne[i][j] << endl;
      }
      cout<< endl;
    }   
     
  }

  // Create a representation of the control matrices to use the emp library
  // Assumes that the constants are already computed in plaintext
  void garbleControlConstants() {
    // gamma3
    for (int i = 0; i < this->sizegamma3[0]; i++) {
      for (int j = 0; j < this->sizegamma3[1]; j++) {
        this->gamma3[i][j] = fixedPoint(this->gamma3_ne[i][j], 24, 24, ALICE);
      }
    }

    // gamma2
    for (int i = 0; i < this->sizegamma2[0]; i++) {
      for (int j = 0; j < this->sizegamma2[1]; j++) {
        this->gamma2[i][j] = fixedPoint(this->gamma2_ne[i][j], 24, 24, ALICE);
      }
    }

    // gamma1
    for (int i = 0; i < this->sizegamma1[0]; i++) {
      for (int j = 0; j < this->sizegamma1[1]; j++) {
        this->gamma1[i][j] = fixedPoint(this->gamma1_ne[i][j], 24, 24, ALICE);
      }
    }
    
    // A-BK 
    for (int i = 0; i < this->sizeA[0]; i++) {
      for (int j = 0; j < this->sizeA[1]; j++) {
        this->A_BK[i][j] = fixedPoint(this->A_BK_ne[i][j], 24, 24, ALICE);
      }
    }   
  }


  // Computes the constants related to the system references.
  // It also computes in plaintext matrices to compute the prediction
  void computeReferenceConstants() {
    this->sizeuTilder[0] = sizeur[0];
    this->sizeuTilder[1] = sizeur[1];
    this->uTilder_ne = new double *[this->sizeuTilder[0]];
    for (int i = 0; i < this->sizeuTilder[0]; i++) {
      this->uTilder_ne[i] = new double[this->sizeuTilder[1]];
    }

    this->sizexgamma[0] = sizexr[0];
    this->sizexgamma[1] = sizexr[1];
    this->xgamma_ne = new double *[this->sizexgamma[0]]; 
    for (int i = 0; i < this->sizexgamma[0]; i++) {
      this->xgamma_ne[i] = new double[this->sizexgamma[1]];
    }

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
    this->uTilder = new fixedPoint *[this->sizeuTilder[0]];
    for (int i = 0; i < this->sizeuTilder[0]; i++) {
      this->uTilder[i] = new fixedPoint[this->sizeuTilder[1]];
    }

    this->xgamma = new fixedPoint *[this->sizexgamma[0]];
    for (int i = 0; i < this->sizexgamma[0]; i++) {
      this->xgamma[i] = new fixedPoint[this->sizexgamma[1]];
    }    

    for (int i = 0; i < this->sizeuTilder[0]; i++) {
      for (int j = 0; j < this->sizeuTilder[1]; j++) {
        this->uTilder[i][j] = fixedPoint(this->uTilder_ne[i][j], 24, 24, ALICE);
      }
    }
    for (int i = 0; i < this->sizexgamma[0]; i++) {
      for (int j = 0; j < this->sizexgamma[1]; j++) {
        this->xgamma[i][j] = fixedPoint(this->xgamma_ne[i][j], 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeB[0]; i++) {
      for (int j = 0; j < this->sizexk[1]; j++) {
        this->Bug[i][j] = fixedPoint(this->Bug_ne[i][j], 24, 24, ALICE);
        
      }
    }
  }


///////////////////////////////////////////////////////////////////////////
// Functions required for the operations. Not focused on the control system
///////////////////////////////////////////////////////////////////////////

  // Computes the multiplication between matrices A and B
  void matrixMul(fixedPoint **A, fixedPoint **B, fixedPoint **ret, int *ASize,
                 int *BSize) {
    fixedPoint zero(0, 24, 24, PUBLIC);
    for (int i = 0; i < ASize[0]; i++) {
      for (int j = 0; j < BSize[1]; j++) {
        ret[i][j] = zero;
        for (int k = 0; k < ASize[1]; k++) {
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
    fixedPoint zero(0, 24, 24, PUBLIC);
    for (int i = 0; i < size[0]; i++) {
      ret[i] = zero;
      for (int j = 0; j < size[1]; j++) {
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

  // loads all the matrices required to compute the controller
  // and to simulate the system behavior
  void inputData() {

    getFileSize("Data/A.txt", this->sizeA);
    this->A_ne = new double *[this->sizeA[0]];
    for (int i = 0; i < this->sizeA[0]; i++) {
      this->A_ne[i] = new double[this->sizeA[1]];
    }

    readFile(this->A_ne, "Data/A.txt", this->sizeA);
    this->A = new fixedPoint *[this->sizeA[0]];
    for (int i = 0; i < this->sizeA[0]; i++) {
      this->A[i] = new fixedPoint[this->sizeA[1]];
    }

    getFileSize("Data/B.txt", this->sizeB);
    this->B_ne = new double *[this->sizeB[0]];
    for (int i = 0; i < this->sizeB[0]; i++) {
      this->B_ne[i] = new double[this->sizeB[1]];
    }

    readFile(this->B_ne, "Data/B.txt", this->sizeB);
    this->B = new fixedPoint *[this->sizeB[0]];
    for (int i = 0; i < this->sizeB[0]; i++) {
      this->B[i] = new fixedPoint[this->sizeB[1]];
    }

    getFileSize("Data/C.txt", this->sizeC);
    this->C_ne = new double *[this->sizeC[0]];
    for (int i = 0; i < this->sizeC[0]; i++) {
      this->C_ne[i] = new double[this->sizeC[1]];
    }

    readFile(this->C_ne, "Data/C.txt", this->sizeC);
    this->C = new fixedPoint *[this->sizeC[0]];
    for (int i = 0; i < this->sizeC[0]; i++) {
      this->C[i] = new fixedPoint[this->sizeC[1]];
    }

    getFileSize("Data/ur.txt", this->sizeur);
    this->ur_ne = new double *[this->sizeur[0]];
    this->uk_ne = new double *[this->sizeur[0]];
    for (int i = 0; i < this->sizeur[0]; i++) {
      this->ur_ne[i] = new double[this->sizeur[1]];
      this->uk_ne[i] = new double[this->sizeur[1]];
    }

    readFile(this->ur_ne, "Data/ur.txt", this->sizeur);
    this->ur = new fixedPoint *[this->sizeur[0]];
    for (int i = 0; i < this->sizeur[0]; i++) {
      this->ur[i] = new fixedPoint[this->sizeur[1]];
    }

    getFileSize("Data/xr.txt", this->sizexr);
    this->xr_ne = new double *[this->sizexr[0]];
    for (int i = 0; i < this->sizexr[0]; i++) {
      this->xr_ne[i] = new double[this->sizexr[1]];
    }

    readFile(this->xr_ne, "Data/xr.txt", this->sizexr);
    this->xr = new fixedPoint *[this->sizexr[0]];
    for (int i = 0; i < this->sizexr[0]; i++) {
      this->xr[i] = new fixedPoint[this->sizexr[1]];
    }

    getFileSize("Data/x0.txt", this->sizex0);
    this->x0_ne = new double *[this->sizex0[0]];
    for (int i = 0; i < this->sizex0[0]; i++) {
      this->x0_ne[i] = new double[this->sizex0[1]];
    }
    readFile(this->x0_ne, "Data/x0.txt", this->sizex0);
    this->x0 = new fixedPoint *[this->sizex0[0]];
    for (int i = 0; i < this->sizex0[0]; i++) {
      this->x0[i] = new fixedPoint[this->sizex0[1]];
    }

    double **dataK;
    getFileSize("Data/K.txt", this->sizeK);
    this->K_ne = new double *[this->sizeK[0]];
    dataK = new double *[this->sizeK[0]];
    for (int i = 0; i < this->sizeK[0]; i++) {
      dataK[i] = new double[this->sizeK[1]];
      this->K_ne[i] = new double[this->sizeK[1]];
    }

    readFile(dataK, "Data/K.txt", this->sizeK);
    this->K = new fixedPoint *[this->sizeK[0]];
    for (int i = 0; i < this->sizeK[0]; i++) {
      this->K[i] = new fixedPoint[this->sizeK[1]];
    }
    // cout<< dataK<<endl;

    double **dataL;
    getFileSize("Data/L.txt", this->sizeL);
    this->L_ne = new double *[this->sizeL[0]];
    dataL = new double *[this->sizeL[0]];
    for (int i = 0; i < this->sizeL[0]; i++) {
      this->L_ne[i] = new double[this->sizeL[1]];
      dataL[i] = new double[this->sizeL[1]];
    }

    readFile(dataL, "Data/L.txt", this->sizeL);
    this->L = new fixedPoint *[this->sizeL[0]];
    for (int i = 0; i < this->sizeL[0]; i++) {
      this->L[i] = new fixedPoint[this->sizeL[1]];
    }

    double **dataA;
    getFileSize("Data/A.txt", this->sizeA);
    this->A_ne = new double *[this->sizeA[0]];
    dataA = new double *[this->sizeA[0]];
    for (int i = 0; i < this->sizeA[0]; i++) {
      this->A_ne[i] = new double[this->sizeA[1]];
      dataA[i] = new double[this->sizeA[1]]; 
    }

    readFile(dataA, "Data/A.txt", this->sizeA);
    this->A = new fixedPoint *[this->sizeA[0]];
    for (int i = 0; i < this->sizeA[0]; i++) {
      this->A[i] = new fixedPoint[this->sizeA[1]];
    }

    double **dataB;
    getFileSize("Data/B.txt", this->sizeB);
    this->B_ne = new double *[this->sizeB[0]];
    dataB = new double *[this->sizeB[0]];
    for (int i = 0; i < this->sizeB[0]; i++) {
      this->B_ne[i] = new double[this->sizeB[1]];
      dataB[i] = new double[this->sizeB[1]];
    }

    readFile(dataB, "Data/B.txt", this->sizeB);
    this->B = new fixedPoint *[this->sizeB[0]];
    for (int i = 0; i < this->sizeB[0]; i++) {
      this->B[i] = new fixedPoint[this->sizeB[1]];
    }

    double **dataC;
    getFileSize("Data/C.txt", this->sizeC);
    this->C_ne = new double *[this->sizeC[0]];
    dataC = new double *[this->sizeC[0]];
    for (int i = 0; i < this->sizeC[0]; i++) {
      this->C_ne[i] = new double[this->sizeC[1]];
      dataC[i] = new double[this->sizeC[1]];
    }

    readFile(dataC, "Data/C.txt", this->sizeC);
    this->C = new fixedPoint *[this->sizeC[0]];
    for (int i = 0; i < this->sizeC[0]; i++) {
      this->C[i] = new fixedPoint[this->sizeC[1]];
    }

    // Read CUSUM parameters Nu
    double **dataNu;
    getFileSize("Data/Nu.txt", this->sizeNu);
    dataNu = new double *[this->sizeNu[0]];
    for (int i = 0; i < this->sizeNu[0]; i++) {
      dataNu[i] = new double[this->sizeNu[1]];
    }

    readFile(dataNu, "Data/Nu.txt", this->sizeNu);
    this->Nu = new fixedPoint *[this->sizeNu[0]];
    for (int i = 0; i < this->sizeNu[0]; i++) {
      this->Nu[i] = new fixedPoint[this->sizeNu[1]];
    }

    double **dataTau;
    getFileSize("Data/Tau.txt", this->sizeTau);
    dataTau = new double *[this->sizeTau[0]];
    for (int i = 0; i < this->sizeTau[0]; i++) {
      dataTau[i] = new double[this->sizeTau[1]];
    }

    readFile(dataTau, "Data/Tau.txt", this->sizeTau);
    this->Tau = new fixedPoint *[this->sizeTau[0]];
    for (int i = 0; i < this->sizeTau[0]; i++) {
      this->Tau[i] = new fixedPoint[this->sizeTau[1]];
    }


    // cout << "Load data x0" << endl;

    this->sizezk[0] = this->sizeC[0];
    this->sizezk[1] = this->sizex0[1];
    this->zk = new fixedPoint *[this->sizezk[0]];
    this->zk_ne = new double *[this->sizezk[0]];
    for (int i = 0; i < this->sizezk[0]; i++) {
      this->zk[i] = new fixedPoint[this->sizezk[1]];
      this->zk_ne[i] = new double[this->sizezk[1]];
    }

    this->sizexk[0] = this->sizex0[0];
    this->sizexk[1] = this->sizex0[1];
    this->xk = new fixedPoint *[this->sizexk[0]];
    this->xk_ne = new double *[this->sizexk[0]];
    for (int i = 0; i < this->sizexk[0]; i++) {
      this->xk[i] = new fixedPoint[this->sizexk[1]];
      this->xk_ne[i] = new double[this->sizexk[1]];
    }

    this->sizextemp[0] = this->sizex0[0];
    this->sizextemp[1] = this->sizex0[1];
    this->xtemp = new fixedPoint *[this->sizextemp[0]];
    this->xtemp_ne = new double *[this->sizextemp[0]];
    for (int i = 0; i < this->sizextemp[0]; i++) {
      this->xtemp[i] = new fixedPoint[this->sizextemp[1]];
      this->xtemp_ne[i] = new double[this->sizextemp[1]];
    }

    for (int i = 0; i < this->sizeA[0]; i++) {
      for (int j = 0; j < this->sizeA[1]; j++) {
        this->A[i][j] = fixedPoint(this->A_ne[i][j], 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeB[0]; i++) {
      for (int j = 0; j < this->sizeB[1]; j++) {
        this->B[i][j] = fixedPoint(this->B_ne[i][j], 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeC[0]; i++) {
      for (int j = 0; j < this->sizeC[1]; j++) {
        this->C[i][j] = fixedPoint(this->C_ne[i][j], 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeur[0]; i++) {
      for (int j = 0; j < this->sizeur[1]; j++) {
        // cout << "ur: " << this->ur_ne[i][j] << endl;
        this->ur[i][j] = fixedPoint(this->ur_ne[i][j], 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizexr[0]; i++) {
      for (int j = 0; j < this->sizexr[1]; j++) {
        this->xr[i][j] = fixedPoint(this->xr_ne[i][j], 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizex0[0]; i++) {
      for (int j = 0; j < this->sizex0[1]; j++) {
        this->x0[i][j] = fixedPoint(this->x0_ne[i][j], 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizexk[0]; i++) {
      for (int j = 0; j < this->sizexk[1]; j++) {
        this->xk[i][j] = fixedPoint(this->x0_ne[i][j], 24, 24, ALICE);
        this->xk_ne[i][j] = this->x0_ne[i][j];
      }
    }

    for (int i = 0; i < this->sizextemp[0]; i++) {
      for (int j = 0; j < this->sizextemp[1]; j++) {
        this->xtemp[i][j] = fixedPoint(this->x0_ne[i][j], 24, 24, ALICE);;
        this->xtemp_ne[i][j] = this->xk_ne[i][j];
      }
    }

    for (int i = 0; i < this->sizezk[0]; i++) {
      for (int j = 0; j < this->sizezk[1]; j++) {
        this->zk[i][j] = fixedPoint(this->x0_ne[i][j], 24, 24, ALICE);; 
        this->zk_ne[i][j] = this->xk_ne[i][j];
      }
    }

    // negative because K.txt holds the nagation of the control gain K
    for (int i = 0; i < this->sizeK[0]; i++) {
      for (int j = 0; j < this->sizeK[1]; j++) {
        this->K_ne[i][j] = -dataK[i][j];
        this->K[i][j] = fixedPoint(-(dataK[i][j]), 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeL[0]; i++) {
      for (int j = 0; j < this->sizeL[1]; j++) {
        this->L_ne[i][j] = dataL[i][j];
        this->L[i][j] = fixedPoint(dataL[i][j], 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeA[0]; i++) {
      for (int j = 0; j < this->sizeA[1]; j++) {
        A_ne[i][j] = dataA[i][j];
        this->A[i][j] = fixedPoint(dataA[i][j], 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeB[0]; i++) {
      for (int j = 0; j < this->sizeB[1]; j++) {
        this->B_ne[i][j] = dataB[i][j];
        this->B[i][j] = fixedPoint(dataB[i][j], 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeC[0]; i++) {
      for (int j = 0; j < this->sizeC[1]; j++) {
        this->C_ne[i][j] = dataC[i][j];
        this->C[i][j] = fixedPoint(dataC[i][j], 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeNu[0]; i++) {
      for (int j = 0; j < this->sizeNu[1]; j++) {
        this->Nu[i][j] = fixedPoint(dataNu[i][j], 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeTau[0]; i++) {
      for (int j = 0; j < this->sizeTau[1]; j++) {
        this->Tau[i][j] = fixedPoint(dataTau[i][j], 24, 24, ALICE);
      }
    }

    this->sizegamma3[0] = this->sizeB[0];
    this->sizegamma3[1] = this->sizeB[1];
    this->gamma3 = new fixedPoint *[this->sizegamma3[0]];
    this->gamma3_ne = new double *[this->sizegamma3[0]];
    for (int i = 0; i < this->sizegamma3[0]; i++) {
      this->gamma3[i] = new fixedPoint[this->sizegamma3[1]];
      this->gamma3_ne[i] = new double[this->sizegamma3[1]];
    }

    this->sizeLC[0] = this->sizeL[0];
    this->sizeLC[1] = this->sizeC[1];
    this->LC = new fixedPoint *[this->sizeLC[0]];
    this->LC_ne = new double *[this->sizeLC[0]];
    for (int i = 0; i < this->sizeLC[1]; i++) {
      this->LC[i] = new fixedPoint[this->sizeLC[1]];
      this->LC_ne[i] = new double[this->sizeLC[1]];
    }

    this->LCB = new fixedPoint *[this->sizeLC[0]];
    this->LCB_ne = new double *[this->sizeLC[0]];
    for (int i = 0; i < this->sizeLC[0]; i++) {
      this->LCB[i] = new fixedPoint[this->sizeB[1]];
      this->LCB_ne[i] = new double[this->sizeB[1]];
    }

    this->sizegamma2[0] = this->sizegamma3[0];
    this->sizegamma2[1] = this->sizeK[1];
    this->gamma2 = new fixedPoint *[this->sizegamma2[0]];
    this->gamma2_ne = new double *[this->sizegamma2[0]];
    for (int i = 0; i < this->sizegamma2[0]; i++) {
      this->gamma2[i] = new fixedPoint[this->sizegamma2[1]];
      this->gamma2_ne[i] = new double[this->sizegamma2[1]];
    }

    this->sizegamma1[0] = this->sizeA[0];
    this->sizegamma1[1] = this->sizeA[1];
    this->gamma1 = new fixedPoint *[this->sizegamma1[0]];
    this->gamma1_ne = new double *[this->sizegamma1[0]];
    for (int i = 0; i < this->sizegamma1[0]; i++) {
      this->gamma1[i] = new fixedPoint[this->sizegamma1[1]];
      this->gamma1_ne[i] = new double[this->sizegamma1[1]];
    }

    this->LCA = new fixedPoint *[this->sizeA[0]];
    this->LCA_ne = new double *[this->sizeA[1]];
    for (int i = 0; i < this->sizeA[0]; i++) {
      this->LCA[i] = new fixedPoint[this->sizeA[1]];
      this->LCA_ne[i] = new double[this->sizeA[1]];
    }

    this->sizeA_BK[0] = this->sizeA[0];
    this->sizeA_BK[1] = this->sizeA[1];
    this->A_BK = new fixedPoint *[this->sizeA_BK[0]]; 
    this->A_BK_ne = new double *[this->sizeA_BK[0]];
    for (int i = 0; i < this->sizeA_BK[0]; i++) {
      this->A_BK[i] = new fixedPoint[this->sizeA_BK[1]];
      this->A_BK_ne[i] = new double[this->sizeA_BK[1]];
    }

    this->sizeBug[0] = this->sizeB[0];
    this->sizeBug[1] = this->sizexk[1];
    this->Bug = new fixedPoint *[this->sizeBug[0]];
    this->Bug_ne = new double *[this->sizeBug[0]];
    for (int i = 0; i < this->sizeB[0]; i++) {
      this->Bug[i] = new fixedPoint[ this->sizeBug[1] ];
      this->Bug_ne[i] = new double[ this->sizeBug[1] ];
    }

  }
};

#endif
