// Change all parties[0] to BOB?
#ifndef CLOUD_H
#define CLOUD_H
#include "fixedPoint.h"
#include "garbled_circuit_user.h"

class Cloud: public garbled_circuit_user {
public:
  

  int sizeCusum[2];
  fixedPoint **Cusum;
  int sizenu[2];
  fixedPoint **nu;
  int sizetau[2];
  fixedPoint **tau;
  
  int sizeresidues[2];
  Bit **alarm;
  fixedPoint **residues;
  fixedPoint **residues1;
  fixedPoint **residues2;

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
  void getInputs(fixedPoint **nu, int *sizenu, fixedPoint **tau, int *sizetau,
                 fixedPoint **Cusum, int *sizeCusum) {
    this->HARD_CODE_ZEROES = 1;

    
    this->sizenu[0]       = sizenu[0];
    this->sizenu[1]       = sizenu[1];
    this->sizetau[0]      = sizetau[0];
    this->sizetau[1]      = sizetau[1];
    this->sizeCusum[0]    = sizenu[0];
    this->sizeCusum[1]    = sizenu[1];
    this->sizeresidues[0] = sizetau[0];
    this->sizeresidues[1] = sizetau[1];

    this->nu        = initSize_GC( this->sizenu );
    this->tau       = initSize_GC( this->sizetau );
    this->Cusum     = initSize_GC( this->sizeCusum );
    this->residues1 = initSize_GC( this->sizeCusum );
    this->residues2 = initSize_GC( this->sizeCusum );
    this->residues  = initSize_GC( this->sizeCusum );
    

    
    for (int i = 0; i < this->sizenu[0]; i++) 
      for (int j = 0; j < this->sizenu[1]; j++) 
        this->nu[i][j] = nu[i][j];

    for (int i = 0; i < this->sizetau[0]; i++) 
      for (int j = 0; j < this->sizetau[1]; j++) 
        this->tau[i][j] = tau[i][j];

    for (int i = 0; i < this->sizeCusum[0]; i++) 
      for (int j = 0; j < this->sizeCusum[1]; j++) 
        this->Cusum[i][j] = fixedPoint(0, decimalBits, integerBits, parties[0]);
    
    this->alarm = new Bit *[this->sizetau[0]];
    this->alarm_ne = new int *[this->sizetau[0]];
    for (int i = 0; i < this->sizetau[0]; i++) {
      this->alarm[i] = new Bit[1];
      this->alarm_ne[i] = new int[1];
    }
    
    for (int i = 0; i < this->sizetau[0]; i++) 
      this->alarm_ne[i][0] = 0;
  }
  void compute_residues(){
    for(int i=0; i<this->sizetau[0]; i++){
      for(int j=0; j<this->sizetau[1]; j++){
        this->residues1[i][j] = fixedPoint( 0, integerBits, integerBits, parties[1] );
        this->residues2[i][j] = fixedPoint( 0, integerBits, integerBits, parties[0] );
      }
    } 
  }

  void computeCusum()//fixedPoint **xHat, fixedPoint **uk)
  {
    Bit *clipBelow = new Bit[this->sizeCusum[0]]; 
    // Bit zero(0, parties[0]);
    Bit zero(0, PUBLIC);
    for (int i = 0; i < this->sizeCusum[0]; i++) {
      
      if(this->alarm_ne[i][0] == 1){
        this->Cusum[i][0] = fixedPoint(0, this->decimalBits, this->integerBits, parties[0]);
        // this->Cusum[i][0] = fixedPoint(0, this->decimalBits, this->integerBits, PUBLIC);
        this->alarm[i][0] = zero;
      } else{
        this->Cusum[i][0]    = this->Cusum[i][0] + this->residues1[i][0] + this->residues2[i][0] - this->nu[i][0];
        clipBelow[i]         = this->Cusum[i][0].isPositive(); 
        this->Cusum[i][0]    = this->Cusum[i][0].reset(clipBelow[i]);
        this->alarm[i][0]    = zero;
        this->alarm[i][0]    = this->Cusum[i][0].operator>(this->tau[i][0]);
      }
      
    }
  }
  void reveal_alarm(int party){
    for (int i = 0; i < this->sizeCusum[0]; i++) {
      this->alarm_ne[i][0] = this->alarm[i][0].reveal(party);
    }
  }
 
  
};

#endif
