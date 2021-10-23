#ifndef SUBSYSTEM_H
#define SUBSYSTEM_H
#include "fixedPoint.h"

class subSystem {
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
  fixedPoint **xtemp;
  int sizextemp[2];

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
    for (int i = 0; i < this->sizezk[0]; i++) {
      for (int j = 0; j < this->sizezk[1]; j++) {
        this->zk[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }
  }

  // Creates a representation of the constants related to the controller to be used with the emp toolkit
  // The values are initialized as zero here since the cloud should not learn the plaintexts
  // Nomenclature based on 
  // Encrypted LQG using Labeled Homomorphic Encryption. https://www.georgejpappas.org/papers/Paper264.pdf
  void garbleConstants() {
    // compute gamma3
    for (int i = 0; i < this->sizegamma3[0]; i++) {
      for (int j = 0; j < this->sizegamma3[1]; j++) {
        this->gamma3[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    // compute gamma2
    for (int i = 0; i < this->sizegamma2[0]; i++) {
      for (int j = 0; j < this->sizegamma2[0]; j++) {
        this->gamma2[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }
    // compute gamma1
    for (int i = 0; i < this->sizegamma1[0]; i++) {
      for (int j = 0; j < this->sizegamma1[0]; j++) {
        this->gamma1[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeA[0]; i++) {
      for (int j = 0; j < this->sizeA[0]; j++) {
        this->A_BK[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

  }

  // Creates a representation of the constants related to references to be used with the emp toolkit
  // The values are initialized as zero here since the cloud should not learn the plaintexts
  void garbleReferenceConstants() {

    this->sizeuTilder[0] = sizeur[0];
    this->sizeuTilder[1] = sizeur[1];
    this->sizexgamma[0] = sizexr[0];
    this->sizexgamma[1] = sizexr[1];
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
        this->uTilder[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }
    for (int i = 0; i < this->sizexgamma[0]; i++) {
      for (int j = 0; j < this->sizexgamma[1]; j++) {
        this->xgamma[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }
    for (int i = 0; i < this->sizeB[0]; i++) {
      for (int j = 0; j < this->sizexk[1]; j++) {
        this->Bug[i][j] = fixedPoint(0, 24, 24, ALICE);
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
        ret[i][j] = (A[i][0] * B[0][j]);
        for (int k = 1; k < ASize[1]; k++) {
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
      ret[i] = ((A[i][0]) * (B[0][0]));
      for (int j = 1; j < size[1]; j++) {
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

  // Creates instances of vectors and matrices to be used by the emp toolkit
  void inputData() {
    // The size of matrices are retrieved from the .txt files, however the information is
    // never used in the function. It does not make sense the cloud learns the plaintext of 
    // matrices
    // Vectors and matrices are initialized with values of zero
    getFileSize("Data/A.txt", this->sizeA);
    this->A = new fixedPoint *[this->sizeA[0]];
    for (int i = 0; i < this->sizeA[0]; i++) {
      this->A[i] = new fixedPoint[this->sizeA[1]];
    }

    
    getFileSize("Data/B.txt", this->sizeB);
    this->B = new fixedPoint *[this->sizeB[0]];
    for (int i = 0; i < this->sizeB[0]; i++) {
      this->B[i] = new fixedPoint[this->sizeB[1]];
    }

    getFileSize("Data/C.txt", this->sizeC);
    this->C = new fixedPoint *[this->sizeC[0]];
    for (int i = 0; i < this->sizeC[0]; i++) {
      this->C[i] = new fixedPoint[this->sizeC[1]];
    }

    getFileSize("Data/ur.txt", this->sizeur);
    this->ur = new fixedPoint *[this->sizeur[0]];
    for (int i = 0; i < this->sizeur[0]; i++) {
      this->ur[i] = new fixedPoint[this->sizeur[1]];
    }

    getFileSize("Data/xr.txt", this->sizexr);
    this->xr = new fixedPoint *[this->sizexr[0]];
    for (int i = 0; i < this->sizexr[0]; i++) {
      this->xr[i] = new fixedPoint[this->sizexr[1]];
    }

    getFileSize("Data/x0.txt", this->sizex0);
    this->x0 = new fixedPoint *[this->sizex0[0]];
    for (int i = 0; i < this->sizex0[0]; i++) {
      this->x0[i] = new fixedPoint[this->sizex0[1]];
    }

    getFileSize("Data/K.txt", this->sizeK);
    this->K = new fixedPoint *[this->sizeK[0]];
    for (int i = 0; i < this->sizeK[0]; i++) {
      this->K[i] = new fixedPoint[this->sizeK[1]];
    }

    getFileSize("Data/L.txt", this->sizeL);
    this->L = new fixedPoint *[this->sizeL[0]];
    for (int i = 0; i < this->sizeL[0]; i++) {
      this->L[i] = new fixedPoint[this->sizeL[1]];
    }

    getFileSize("Data/A.txt", this->sizeA);
    this->A = new fixedPoint *[this->sizeA[0]];
    for (int i = 0; i < this->sizeA[0]; i++) {
      this->A[i] = new fixedPoint[this->sizeA[1]];
    }

    getFileSize("Data/B.txt", this->sizeB);
    this->B = new fixedPoint *[this->sizeB[0]];
    for (int i = 0; i < this->sizeB[0]; i++) {
      this->B[i] = new fixedPoint[this->sizeB[1]];
    }

    getFileSize("Data/C.txt", this->sizeC);
    this->C = new fixedPoint *[this->sizeC[0]];
    for (int i = 0; i < this->sizeC[0]; i++) {
      this->C[i] = new fixedPoint[this->sizeC[1]];
    }

    getFileSize("Data/Nu.txt", this->sizeNu);
    this->Nu = new fixedPoint *[this->sizeNu[0]];
    for (int i = 0; i < this->sizeNu[0]; i++) {
      this->Nu[i] = new fixedPoint[this->sizeNu[1]];
    }

    getFileSize("Data/Tau.txt", this->sizeTau);
    this->Tau = new fixedPoint *[this->sizeTau[0]];
    for (int i = 0; i < this->sizeTau[0]; i++) {
      this->Tau[i] = new fixedPoint[this->sizeTau[1]];
    }

    // cout << "Load data x0" << endl;

    this->sizezk[0] = this->sizeC[0];
    this->sizezk[1] = this->sizex0[1];
    this->zk = new fixedPoint *[this->sizezk[0]];
    for (int i = 0; i < this->sizezk[0]; i++) {
      this->zk[i] = new fixedPoint[this->sizezk[1]];
    }

    this->sizexk[0] = this->sizex0[0];
    this->sizexk[1] = this->sizex0[1];
    this->xk = new fixedPoint *[this->sizexk[0]];
    for (int i = 0; i < this->sizexk[0]; i++) {
      this->xk[i] = new fixedPoint[this->sizexk[1]];
    }

    this->sizextemp[0] = this->sizex0[0];
    this->sizextemp[1] = this->sizex0[1];
    this->xtemp = new fixedPoint *[this->sizextemp[0]];
    for (int i = 0; i < this->sizextemp[0]; i++) {
      this->xtemp[i] = new fixedPoint[this->sizextemp[1]];
    }

    for (int i = 0; i < this->sizeA[0]; i++) {
      for (int j = 0; j < this->sizeA[1]; j++) {
        this->A[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeB[0]; i++) {
      for (int j = 0; j < this->sizeB[1]; j++) {
        this->B[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeC[0]; i++) {
      for (int j = 0; j < this->sizeC[1]; j++) {
        this->C[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeur[0]; i++) {
      for (int j = 0; j < this->sizeur[1]; j++) {
        // cout << "ur: " << dataur[i][j] << endl;
        this->ur[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizexr[0]; i++) {
      for (int j = 0; j < this->sizexr[1]; j++) {
        this->xr[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizex0[0]; i++) {
      for (int j = 0; j < this->sizex0[1]; j++) {
        this->x0[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizexk[0]; i++) {
      for (int j = 0; j < this->sizexk[1]; j++) {
        this->xk[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizextemp[0]; i++) {
      for (int j = 0; j < this->sizextemp[1]; j++) {
        this->xtemp[i][j] = fixedPoint(0, 24, 24, ALICE);;
      }
    }

    for (int i = 0; i < this->sizezk[0]; i++) {
      for (int j = 0; j < this->sizezk[1]; j++) {
        this->zk[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    // negative because K.txt holds the nagation of the control gain K
    for (int i = 0; i < this->sizeK[0]; i++) {
      for (int j = 0; j < this->sizeK[1]; j++) {
        this->K[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeL[0]; i++) {
      for (int j = 0; j < this->sizeL[1]; j++) {
        this->L[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeA[0]; i++) {
      for (int j = 0; j < this->sizeA[1]; j++) {
        this->A[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeB[0]; i++) {
      for (int j = 0; j < this->sizeB[1]; j++) {
        this->B[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeC[0]; i++) {
      for (int j = 0; j < this->sizeC[1]; j++) {
        this->C[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    for (int i = 0; i < this->sizeNu[0]; i++) {
      for (int j = 0; j < this->sizeNu[1]; j++) {
        this->Nu[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }
 
    for (int i = 0; i < this->sizeTau[0]; i++) {
      for (int j = 0; j < this->sizeTau[1]; j++) {
        this->Tau[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    this->sizegamma3[0] = this->sizeB[0];
    this->sizegamma3[1] = this->sizeB[1];
    this->gamma3 = new fixedPoint *[this->sizegamma3[0]];
    for (int i = 0; i < this->sizegamma3[0]; i++) {
      this->gamma3[i] = new fixedPoint[this->sizegamma3[1]];
    }

    this->sizeLC[0] = this->sizeL[0];
    this->sizeLC[1] = this->sizeC[1];
    this->LC = new fixedPoint *[this->sizeLC[0]];
    for (int i = 0; i < this->sizeLC[1]; i++) {
      this->LC[i] = new fixedPoint[this->sizeLC[1]];
    }

    this->LCB = new fixedPoint *[this->sizeLC[0]];
    for (int i = 0; i < this->sizeLC[0]; i++) {
      this->LCB[i] = new fixedPoint[this->sizeB[1]];
    }

    this->sizegamma2[0] = this->sizegamma3[0];
    this->sizegamma2[1] = this->sizeK[1];
    this->gamma2 = new fixedPoint *[this->sizegamma2[0]];
    for (int i = 0; i < this->sizegamma2[0]; i++) {
      this->gamma2[i] = new fixedPoint[this->sizegamma2[1]];
    }

    this->sizegamma1[0] = this->sizeA[0];
    this->sizegamma1[1] = this->sizeA[1];
    this->gamma1 = new fixedPoint *[this->sizegamma1[0]];
    for (int i = 0; i < this->sizegamma1[0]; i++) {
      this->gamma1[i] = new fixedPoint[this->sizegamma1[1]];
    }

    this->LCA = new fixedPoint *[this->sizeA[0]];
    for (int i = 0; i < this->sizeA[0]; i++) {
      this->LCA[i] = new fixedPoint[this->sizeA[1]];
    }


    this->sizeA_BK[0] = this->sizeA[0];
    this->sizeA_BK[1] = this->sizeA[1];
    this->A_BK = new fixedPoint *[this->sizeA_BK[0]];
    for (int i = 0; i < this->sizeA_BK[0]; i++) {
      this->A_BK[i] = new fixedPoint[this->sizeA_BK[1]];
    }

    this->sizeBug[0] = this->sizeB[0];
    this->sizeBug[1] = this->sizexk[1];
    this->Bug = new fixedPoint *[this->sizeBug[0]];
    for (int i = 0; i < this->sizeB[0]; i++) {
      this->Bug[i] = new fixedPoint[ this->sizeBug[1] ];
    }
  }

};

#endif
