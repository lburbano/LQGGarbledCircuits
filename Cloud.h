#ifndef CLOUD_H
#define CLOUD_H
#include "fixedPoint.h"

class Cloud {
public:
  fixedPoint **gamma1;
  int sizegamma1[2];
  fixedPoint **L;
  int sizeL[2];
  fixedPoint **K;
  int sizeK[2];
  fixedPoint **uTilder;
  int sizeuTilder[2];
  fixedPoint **xgamma;
  int sizexgamma[2];
  fixedPoint **xHatk;
  int sizexHatk[2];
  fixedPoint **uk;
  int sizeuk[2];

  int sizeCusum[2];
  fixedPoint **Cusum;
  int sizenu[2];
  fixedPoint **nu;
  int sizetau[2];
  fixedPoint **tau;
  int sizeA_BK[2];
  fixedPoint **A_BK;
  int sizeBug[2];
  fixedPoint **Bug;
  int sizeyp[2];
  fixedPoint **yp;
  int sizeresidues[2];
  fixedPoint **residues;
  Bit **alarm;

  Cloud() {}

  // class receives matrices to compute the controller and the anomaly detection
  void getInputs(fixedPoint **L, int *sizeL, fixedPoint **K, int *sizeK,
                 fixedPoint **gamma1, int *sizegamma1, fixedPoint **xgamma, int *sizexgamma, 
                 fixedPoint **uTilder, int *sizeuTilder, 
                 fixedPoint **nu, int *sizenu, fixedPoint **tau, int *sizetau,
                 fixedPoint **Cusum, int *sizeCusum, fixedPoint **A_BK, int *sizeA_BK,
                 fixedPoint **Bug, int *sizeBug, fixedPoint **xr, int *sizexr, 
                 fixedPoint **ur, int *sizeur,fixedPoint **xk, int *sizexk) {
    this->sizeL[0] = sizeL[0];
    this->sizeL[1] = sizeL[1];
    this->L = new fixedPoint *[this->sizeL[0]];
    for (int i = 0; i < this->sizeL[0]; i++) {
      this->L[i] = new fixedPoint[this->sizeL[1]];
    }
    for (int i = 0; i < this->sizeL[0]; i++) {
      for (int j = 0; j < this->sizeL[1]; j++) {
        this->L[i][j] = L[i][j];
      }
    }

    this->sizeK[0] = sizeK[0];
    this->sizeK[1] = sizeK[1];
    this->K = new fixedPoint *[this->sizeK[0]];
    for (int i = 0; i < this->sizeK[0]; i++) {
      this->K[i] = new fixedPoint[this->sizeK[1]];
    }
    for (int i = 0; i < this->sizeK[0]; i++) {
      for (int j = 0; j < this->sizeK[1]; j++) {
        this->K[i][j] = K[i][j];
      }
    }
    //

    

    this->sizegamma1[0] = sizegamma1[0];
    this->sizegamma1[1] = sizegamma1[1];
    this->gamma1 = new fixedPoint *[this->sizegamma1[0]];
    for (int i = 0; i < this->sizegamma1[0]; i++) {
      this->gamma1[i] = new fixedPoint[this->sizegamma1[1]];
    }
    for (int i = 0; i < this->sizegamma1[0]; i++) {
      for (int j = 0; j < this->sizegamma1[1]; j++) {
        this->gamma1[i][j] = gamma1[i][j];
      }
    }

    this->sizeuTilder[0] = sizeuTilder[0];
    this->sizeuTilder[1] = sizeuTilder[1];
    this->uTilder = new fixedPoint *[this->sizeuTilder[0]];
    for (int i = 0; i < this->sizeuTilder[0]; i++) {
      this->uTilder[i] = new fixedPoint[this->sizeuTilder[1]];
    }
    for (int i = 0; i < this->sizeuTilder[0]; i++) {
      for (int j = 0; j < this->sizeuTilder[1]; j++) {
        this->uTilder[i][j] = uTilder[i][j];
      }
    }

    this->sizexgamma[0] = sizexgamma[0];
    this->sizexgamma[1] = sizexgamma[1];
    this->xgamma = new fixedPoint *[this->sizexgamma[0]];
    for (int i = 0; i < this->sizexgamma[0]; i++) {
      this->xgamma[i] = new fixedPoint[this->sizexgamma[1]];
    }
    for (int i = 0; i < this->sizexgamma[0]; i++) {
      for (int j = 0; j < this->sizexgamma[1]; j++) {
        this->xgamma[i][j] = xgamma[i][j];
      }
    }

    this->sizexHatk[0] = sizexk[0];
    this->sizexHatk[1] = sizexk[1];
    this->xHatk = new fixedPoint *[this->sizexHatk[0]];
    for (int i = 0; i < this->sizexHatk[0]; i++) {
      this->xHatk[i] = new fixedPoint[this->sizexHatk[1]];
    }
    for (int i = 0; i < this->sizexHatk[0]; i++) {
      for (int j = 0; j < this->sizexHatk[1]; j++) {
        this->xHatk[i][j] = xk[i][j];
      }
    }


    this->sizenu[0] = sizenu[0];
    this->sizenu[1] = sizenu[1];
    this->nu = new fixedPoint *[this->sizenu[0]];
    for (int i = 0; i < this->sizenu[0]; i++) {
      this->nu[i] = new fixedPoint[this->sizenu[1]];
    }
    for (int i = 0; i < this->sizenu[0]; i++) {
      for (int j = 0; j < this->sizenu[1]; j++) {
        this->nu[i][j] = nu[i][j];
      }
    }

    this->sizetau[0] = sizetau[0];
    this->sizetau[1] = sizetau[1];
    this->tau = new fixedPoint *[this->sizetau[0]];
    for (int i = 0; i < this->sizetau[0]; i++) {
      this->tau[i] = new fixedPoint[this->sizetau[1]];
    }
    for (int i = 0; i < this->sizetau[0]; i++) {
      for (int j = 0; j < this->sizetau[1]; j++) {
        this->tau[i][j] = tau[i][j];
      }
    }

    this->sizeCusum[0] = sizexk[0];
    this->sizeCusum[1] = sizexk[1];
    this->Cusum = new fixedPoint *[this->sizeCusum[0]];
    for (int i = 0; i < this->sizeCusum[0]; i++) {
      this->Cusum[i] = new fixedPoint[this->sizeCusum[1]];
    }
    for (int i = 0; i < this->sizeCusum[0]; i++) {
      for (int j = 0; j < this->sizeCusum[1]; j++) {
        this->Cusum[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    this->sizeA_BK[0] = sizeA_BK[0];
    this->sizeA_BK[1] = sizeA_BK[1];
    this->A_BK = new fixedPoint *[this->sizeA_BK[0]];
    for (int i = 0; i < this->sizeA_BK[0]; i++) {
      this->A_BK[i] = new fixedPoint[this->sizeA_BK[1]];
    }
    for (int i = 0; i < this->sizeA_BK[0]; i++) {
      for (int j = 0; j < this->sizeA_BK[1]; j++) {
        this->A_BK[i][j] = A_BK[i][j];
      }
    }

    this->sizeBug[0] = sizeBug[0];
    this->sizeBug[1] = sizeBug[1];
    this->Bug = new fixedPoint *[this->sizeBug[0]];
    for (int i = 0; i < this->sizeBug[0]; i++) {
      this->Bug[i] = new fixedPoint[this->sizeBug[1]];
    }
    for (int i = 0; i < this->sizeBug[0]; i++) {
      for (int j = 0; j < this->sizeBug[1]; j++) {
        this->Bug[i][j] = Bug[i][j];
      }
    }

    this->sizeyp[0] = sizeA_BK[0];
    this->sizeyp[1] = sizexk[1];
    this->yp = new fixedPoint *[this->sizeyp[0]];
    for (int i = 0; i < this->sizeyp[0]; i++) {
      this->yp[i] = new fixedPoint[this->sizeyp[1]];
    }
    for (int i = 0; i < this->sizeyp[0]; i++) {
      for (int j = 0; j < this->sizeyp[1]; j++) {
        this->yp[i][j] = xk[i][j];
      }
    }

    this->sizeresidues[0] = sizeA_BK[0];
    this->sizeresidues[1] = sizexk[1];
    this->residues = new fixedPoint *[this->sizeresidues[0]];
    for (int i = 0; i < this->sizeresidues[0]; i++) {
      this->residues[i] = new fixedPoint[this->sizeresidues[1]];
    }
    for (int i = 0; i < this->sizeresidues[0]; i++) {
      for (int j = 0; j < this->sizeresidues[1]; j++) {
        this->residues[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    }

    this->sizeuk[0] = sizeur[0];
    this->sizeuk[1] = sizeur[1];
    this->uk = new fixedPoint *[this->sizeuk[0]];
    for (int i = 0; i < this->sizeuk[0]; i++) {
      this->uk[i] = new fixedPoint[this->sizeuk[1]];
    }
    for (int i = 0; i < this->sizeuk[0]; i++) {
      for (int j = 0; j < this->sizeuk[1]; j++) {
        this->uk[i][j] = fixedPoint(0, 24, 24, ALICE);
      }
    } 
    
    
    this->alarm = new Bit *[this->sizeA_BK[0]];
    for (int i = 0; i < this->sizeA_BK[0]; i++) {
      this->alarm[i] = new Bit[1];
    }
    for (int i = 0; i < this->sizeA_BK[0]; i++) {
      Bit zero(0, ALICE);
      this->alarm[i][0] = zero;
    }
  }

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


  // Compute estimate xHat
  void computexHat(fixedPoint **zk) {
    fixedPoint *xHatGamma = new fixedPoint[this->sizegamma1[0]];
    fixedPoint *Lzk = new fixedPoint[this->sizegamma1[0]];
    matrixVecMul(this->gamma1, this->xHatk, xHatGamma, this->sizegamma1);
    matrixVecMul(this->L, zk, Lzk, this->sizegamma1);
    for (int i = 0; i < this->sizegamma1[0]; i++) {
      xHatk[i][0] = xHatGamma[i] + Lzk[i] + xgamma[i][0];
      // cout<<xHatk[i][0].reveal<double>(BOB)<<endl;
    }
    delete[] xHatGamma;
    delete[] Lzk;
  }

  void predict(){
    fixedPoint *A_BKx = new fixedPoint[this->sizeA_BK[0]];  
    matrixVecMul(this->A_BK, this->xHatk, A_BKx, this->sizeA_BK);
    for (int i = 0; i < this->sizeA_BK[0]; i++) {
      this->yp[i][0] = A_BKx[i] + this->Bug[i][0];
    }
    delete[] A_BKx;
  }

  void computeResidues(fixedPoint **zk){
    for (int i = 0; i < this->sizegamma1[0]; i++) {
      this->residues[i][0] = this->yp[i][0]-zk[i][0]; 
      this->residues[i][0] = this->residues[i][0] * this->residues[i][0]; // For square
      // this->residues[i][0] = this->residues[i][0].absolute_value(); // For absolute value
      // cout << this->residues[i][0].reveal<double>(ALICE)<<endl;
    }
  }

  void computeCusum()//fixedPoint **xHat, fixedPoint **uk)
  {
    Bit *clipBelow = new Bit[this->sizexHatk[0]]; 
    for (int i = 0; i < this->sizexHatk[0]; i++) {
      this->Cusum[i][0] = this->Cusum[i][0] + this->residues[i][0] - this->nu[i][0];
      this->Cusum[i][0] = this->Cusum[i][0].toZero(this->alarm[i][0]);
      clipBelow[i] = fixedPoint(0, 24, 24, ALICE).operator>( this->Cusum[i][0] ); 
      this->Cusum[i][0] = this->Cusum[i][0].toZero(clipBelow[i]);
      Bit zero(0, ALICE);
      this->alarm[i][0] = zero;
      this->alarm[i][0] = this->Cusum[i][0].operator>(this->tau[i][0]);
    }
  }
 
  // Computes control input uk
  void computeuk() {
    fixedPoint *kxHat = new fixedPoint[this->sizeK[0]];
    matrixVecMul(this->K, this->xHatk, kxHat, this->sizeK);
    for (int i = 0; i < this->sizeK[0]; i++) {
      this->uk[i][0] = this->uTilder[i][0] - kxHat[i];
    }
    delete[] kxHat;
  }
};

#endif
