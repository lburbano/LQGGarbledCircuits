#ifndef GARBLEDCIRCUITUSER_H
#define GARBLEDCIRCUITUSER_H
#include "fixedPoint.h"
class garbled_circuit_user{
    public:
      int decimalBits = 24;
      int integerBits = decimalBits;
      int totalBits = decimalBits + integerBits;
    // Computes the multiplication between matrices A and B

  garbled_circuit_user() {}
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
  fixedPoint** init_size_using_file_GC( string fileName, int *size){
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
  double** init_size_file( string fileName, int *size){
    getFileSize(fileName, size);
    double **in = new double *[ size[0] ];
    for( int i = 0; i < size[0]; i++ ){
      in[i] = new double[ size[1] ];
    }
    return in;
  }

  double** init_size(int *size){
    double **in = new double *[ size[0] ];
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