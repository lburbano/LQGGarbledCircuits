// Change all parties[0] to BOB?
#ifndef CLOUD_H
#define CLOUD_H
#include "fixedPoint.h"
#include "garbled_circuit_user.h"

class Cloud: public garbled_circuit_user {
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

  int **alarm_ne;

  int HARD_CODE_ZEROES;
  int parties[2];

  

  Cloud(int *parties, int integer_bits) {
    this->parties[0] = parties[0];
    this->parties[1] = parties[1];
    this->integerBits = integer_bits;
    this->decimalBits = integer_bits;
    this->totalBits = integerBits + decimalBits;
  }

  // class receives matrices to compute the controller and the anomaly detection
  void getInputs(fixedPoint **L, int *sizeL, fixedPoint **K, int *sizeK,
                 fixedPoint **gamma1, int *sizegamma1, fixedPoint **xgamma, int *sizexgamma, 
                 fixedPoint **uTilder, int *sizeuTilder, 
                 fixedPoint **nu, int *sizenu, fixedPoint **tau, int *sizetau,
                 fixedPoint **Cusum, int *sizeCusum, fixedPoint **A_BK, int *sizeA_BK,
                 fixedPoint **Bug, int *sizeBug, fixedPoint **xr, int *sizexr, 
                 fixedPoint **ur, int *sizeur,fixedPoint **xk, int *sizexk,
                 fixedPoint **xHat, fixedPoint **yp) {
    this->HARD_CODE_ZEROES = 1;

    this->sizeL[0]        = sizeL[0];
    this->sizeL[1]        = sizeL[1];
    this->sizeK[0]        = sizeK[0];
    this->sizeK[1]        = sizeK[1];
    this->sizegamma1[0]   = sizegamma1[0];
    this->sizegamma1[1]   = sizegamma1[1];
    this->sizeuTilder[0]  = sizeuTilder[0];
    this->sizeuTilder[1]  = sizeuTilder[1];
    this->sizexgamma[0]   = sizexgamma[0];
    this->sizexgamma[1]   = sizexgamma[1];
    this->sizexHatk[0]    = sizexk[0];
    this->sizexHatk[1]    = sizexk[1];
    this->sizenu[0]       = sizenu[0];
    this->sizenu[1]       = sizenu[1];
    this->sizetau[0]      = sizetau[0];
    this->sizetau[1]      = sizetau[1];
    this->sizeCusum[0]    = sizexk[0];
    this->sizeCusum[1]    = sizexk[1];
    this->sizeA_BK[0]     = sizeA_BK[0];
    this->sizeA_BK[1]     = sizeA_BK[1];
    this->sizeBug[0]      = sizeBug[0];
    this->sizeBug[1]      = sizeBug[1];
    this->sizeyp[0]       = sizeA_BK[0];
    this->sizeyp[1]       = sizexk[1];
    this->sizeresidues[0] = sizexk[0];
    this->sizeresidues[1] = sizexk[1];
    this->sizeuk[0]       = sizeur[0];
    this->sizeuk[1]       = sizeur[1];


    this->L        = initSize_GC( this->sizeL );
    this->K        = initSize_GC( this->sizeK );
    this->gamma1   = initSize_GC( this->sizegamma1 );
    this->uTilder  = initSize_GC( this->sizeuTilder );
    this->xgamma   = initSize_GC( this->sizexgamma );
    this->xHatk    = initSize_GC( this->sizexHatk );
    this->nu       = initSize_GC( this->sizenu );
    this->tau      = initSize_GC( this->sizetau );
    this->Cusum    = initSize_GC( this->sizeCusum );
    this->A_BK     = initSize_GC( this->sizeA_BK );
    this->Bug      = initSize_GC( this->sizeBug );
    this->yp       = initSize_GC( this->sizeyp );
    this->residues = initSize_GC( this->sizeresidues );
    this->uk       = initSize_GC( this->sizeuk ); 

    for (int i = 0; i < this->sizeL[0]; i++) 
      for (int j = 0; j < this->sizeL[1]; j++) 
        this->L[i][j] = L[i][j];
    
    for (int i = 0; i < this->sizeK[0]; i++) 
      for (int j = 0; j < this->sizeK[1]; j++) 
        this->K[i][j] = K[i][j];
    
    for (int i = 0; i < this->sizegamma1[0]; i++) 
      for (int j = 0; j < this->sizegamma1[1]; j++) 
        this->gamma1[i][j] = gamma1[i][j];
    
    for (int i = 0; i < this->sizeuTilder[0]; i++) 
      for (int j = 0; j < this->sizeuTilder[1]; j++) 
        this->uTilder[i][j] = uTilder[i][j];
    
    for (int i = 0; i < this->sizexgamma[0]; i++) 
      for (int j = 0; j < this->sizexgamma[1]; j++) 
        this->xgamma[i][j] = xgamma[i][j];
    
    for (int i = 0; i < this->sizexHatk[0]; i++) 
      for (int j = 0; j < this->sizexHatk[1]; j++) 
        this->xHatk[i][j] = xHat[i][j];

    for (int i = 0; i < this->sizenu[0]; i++) 
      for (int j = 0; j < this->sizenu[1]; j++) 
        this->nu[i][j] = nu[i][j];

    for (int i = 0; i < this->sizetau[0]; i++) 
      for (int j = 0; j < this->sizetau[1]; j++) 
        this->tau[i][j] = tau[i][j];

    for (int i = 0; i < this->sizeCusum[0]; i++) 
      for (int j = 0; j < this->sizeCusum[1]; j++) 
        this->Cusum[i][j] = fixedPoint(0, decimalBits, integerBits, parties[0]);
    
    for (int i = 0; i < this->sizeA_BK[0]; i++) 
      for (int j = 0; j < this->sizeA_BK[1]; j++) 
        this->A_BK[i][j] = A_BK[i][j];
    
    for (int i = 0; i < this->sizeBug[0]; i++) 
      for (int j = 0; j < this->sizeBug[1]; j++) 
        this->Bug[i][j] = Bug[i][j];
      
    for (int i = 0; i < this->sizeyp[0]; i++) 
      for (int j = 0; j < this->sizeyp[1]; j++) 
        this->yp[i][j] = yp[i][j];
    
    this->alarm = new Bit *[this->sizeA_BK[0]];
    this->alarm_ne = new int *[this->sizeA_BK[0]];
    for (int i = 0; i < this->sizeA_BK[0]; i++) {
      this->alarm[i] = new Bit[1];
      this->alarm_ne[i] = new int[1];
    }
    
    for (int i = 0; i < this->sizeA_BK[0]; i++) 
      this->alarm_ne[i][0] = 0;
  }

  


  // Compute estimate xHat
  void computexHat(fixedPoint **zk) {
    fixedPoint *xHatGamma = new fixedPoint[this->sizegamma1[0]];
    fixedPoint *Lzk = new fixedPoint[this->sizegamma1[0]];
    matrixVecMul(this->gamma1, this->xHatk, xHatGamma, this->sizegamma1);
    matrixVecMul(this->L, zk, Lzk, this->sizegamma1);
    for (int i = 0; i < this->sizegamma1[0]; i++) {
      xHatk[i][0] = xHatGamma[i] + Lzk[i] + xgamma[i][0];
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
    }
  }

  void computeCusum()//fixedPoint **xHat, fixedPoint **uk)
  {
    Bit *clipBelow = new Bit[this->sizeCusum[0]]; 
    Bit zero(0, parties[0]);                                      // Should we change this?
                                                             // Maybe this should be a parameter of the function
                                                             // cloud should learn nothing about this value
    for (int i = 0; i < this->sizeCusum[0]; i++) {
      
      if(this->HARD_CODE_ZEROES == 1){
        if(this->alarm_ne[i][0] == 1){
          this->Cusum[i][0] = fixedPoint(0, this->decimalBits, this->integerBits, parties[0]);
          this->alarm[i][0] = zero;
        } else{
          this->Cusum[i][0] = this->Cusum[i][0] + this->residues[i][0] - this->nu[i][0];
          clipBelow[i] = this->Cusum[i][0].isPositive(); 
          this->Cusum[i][0] = this->Cusum[i][0].reset(clipBelow[i]);
          this->alarm[i][0] = zero;
          this->alarm[i][0] = this->Cusum[i][0].operator>(this->tau[i][0]);
        }

      } else{
        this->Cusum[i][0] = this->Cusum[i][0] + this->residues[i][0] - this->nu[i][0];
        this->Cusum[i][0] = this->Cusum[i][0].reset(this->alarm[i][0]);
        clipBelow[i] = this->Cusum[i][0].isPositive(); 
        this->Cusum[i][0] = this->Cusum[i][0].reset(clipBelow[i]);
        this->alarm[i][0] = zero;
        this->alarm[i][0] = this->Cusum[i][0].operator>(this->tau[i][0]);
      }
      
      
    }
  }
  void reveal_alarm(int party){
    for (int i = 0; i < this->sizeCusum[0]; i++) {
      this->alarm_ne[i][0] = this->alarm[i][0].reveal(party);
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
