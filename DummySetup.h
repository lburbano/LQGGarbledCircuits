#ifndef SETUP_H
#define SETUP_H
#include "fixedPoint.h"

class Setup {
public:
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
  fixedPoint **A;
  int sizeA[2];
  fixedPoint **B;
  int sizeB[2];
  fixedPoint **C;
  int sizeC[2];
  fixedPoint **LC;
  int sizeLC[2];
  fixedPoint **LCB;
  int sizeLCB[2];
  fixedPoint **LCA;
  int sizeLCA[2];

  fixedPoint **A_BK;

  int sizeCusum[2];
  fixedPoint **Cusum;
  int sizeNu[2];
  fixedPoint **Nu;
  int sizeTau[2];
  fixedPoint **Tau;

  Setup() {}

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

  void matrixMulNE(double **A, double **B, double **ret, int *ASize,
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

  void matrixVecMul(fixedPoint **A, fixedPoint **B, fixedPoint *ret,
                    int *size) {
    fixedPoint zero(0, 24, 24, PUBLIC);
    for (int i = 0; i < size[0]; i++) {
      ret[i] = (A[i][0]) * (B[0][0]);
      for (int j = 1; j < size[1]; j++) {
        ret[i] = ret[i] + ((A[i][j]) * (B[j][0]));
      }
    }
  }
  void matrixVecMulNE(double **A, double **B, double *ret,
                    int *size) {
    for (int i = 0; i < size[0]; i++) {
      ret[i] = (A[i][0]) * (B[0][0]);
      for (int j = 1; j < size[1]; j++) {
        ret[i] = ret[i] + ((A[i][j]) * (B[j][0]));
      }
    }
  }

  void getFileSize(string input, int *size) {
    int rowSize = 0, colSize = 0;
    fstream file;
    string inputLine, stringinput;
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
        j++;
        //cout << stringinput << endl;
      }
      i++;
    }
  }

  void inputData() {

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

    this->A_BK = new fixedPoint *[this->sizeA[0]];
    for (int i = 0; i < this->sizeA[0]; i++) {
      this->A_BK[i] = new fixedPoint[this->sizeA[1]];
    }
  }

  void computeConstants() {
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
};

#endif
