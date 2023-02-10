// Class used to implement the circuits and represent the numbers to use them in
// the emp toolkit
#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H

#include "emp-sh2pc/emp-sh2pc.h"
#include "emp-tool/emp-tool.h"
#include <iomanip>
using namespace std;
using namespace emp;

/*
        24-47 is Integer bits
        0-23 is fractional bits

*/

class fixedPoint {
public:
  Bit *bits;
  // Integer val;
  int length = 0;
  int decimalBits; 
  int integerBits;
  int totalBits;
  int party;
  double multiplier;

  // Class initializer
  fixedPoint(double input, int fractLen, int intLen, int partyIn = PUBLIC) {
    decimalBits = fractLen;
    integerBits = intLen;
    totalBits = decimalBits + integerBits;
    length = fractLen + intLen;
    bits = new Bit[length];
    party = partyIn;
    multiplier = pow(2, decimalBits);
    to_binary(input, bits, length, party);
  }

  fixedPoint() {}
  fixedPoint(const fixedPoint &in) {
    length = in.length;
    party = in.party;
    bits = new Bit[length];
    decimalBits = in.decimalBits;
    integerBits = in.integerBits;
    totalBits = in.totalBits;
    multiplier = in.multiplier;
    memcpy(bits, in.bits, sizeof(Bit) * length);
  }

  template <typename O> O reveal(int party = PUBLIC);

  template <typename... Args> size_t bool_s(size_t size, Args... args) {
    return size;
  }

  fixedPoint operator*(fixedPoint &rhs);

  // Converts integers into binary
  void bool_dat(bool *data, size_t len, long long num, size_t startLoc) {
    string str, bin;
    int temp = 0;
    str = to_string(num);
    bin = std::bitset<32>(num).to_string();
    
    reverse(bin.begin(), bin.end());
    int l = (bin.size() > (size_t)len ? len : bin.size());
    for (int i = temp; i < l + temp; ++i)
      data[startLoc + i - temp] = (bin[i] == '1');
    for (size_t i = l; i < len; ++i) {
      data[startLoc + i] = data[startLoc + l - 1];
    }
    
  }

  // Is this use to generate the labels? I am not sure
  void in_it(Bit *bits, const bool *b, int length, int party) {
    block *bbits = (block *)bits;
    if (party == PUBLIC) {
      block one = CircuitExecution::circ_exec->public_label(true);
      block zero = CircuitExecution::circ_exec->public_label(false);
      for (int i = 0; i < length; ++i)
        bbits[i] = b[i] ? one : zero;
    } else {
      ProtocolExecution::prot_exec->feed((block *)bits, party, b, length);
    }
  }

  // Converts numbers into binary
  void to_binary(double x, Bit *dest, int length, int party){ 
    double intPart, fractPart;
    bool *b = new bool[totalBits];
    fractPart = modf(x, &intPart);
    for(int i = 0; i < decimalBits; i++){
      b[i] = 0;
      b[i+decimalBits] = (intPart<0? 1: 0);
    }
    x = round(x * multiplier)/multiplier;
    if(x < 0){
      intPart = floor(x);
      if (intPart != x){
        fractPart = 1 + fractPart;
      }
    }
    bool_dat(b, decimalBits, (long long)intPart, decimalBits);
    fractPart = round( fractPart* multiplier );
    fractPart = (fractPart > multiplier-1)? multiplier-1:fractPart;
    
    bool_dat(b, decimalBits, (long long)fractPart, 0);
    in_it(dest, b, totalBits, party);
  }
  
  // takes a number and set it to zero if s=1
  void _reset(Bit *x, Bit s, Bit *result, int size)
  {
    for (int i = 0; i < size ; i++) {
      result[i] = x[i] & s;
    }
  }

  // takes a number and sets it to zero if s=1
  fixedPoint reset(Bit s)
  {
    fixedPoint ret(*this);
    _reset(this->bits, s, ret.bits,  totalBits);
    return ret;
  }

  // Circuit to add two numbers
  void full_adder(Bit *a, Bit *b, Bit *cin, Bit *cout, Bit *res, int size) {
    Bit carry, bxc, axc, t, carryTemp;
    for (int i = 0; i < size - 1; i++) {
      axc = a[i] ^ carry;
      bxc = b[i] ^ carry;
      res[i] = a[i] ^ bxc;
      t = axc & bxc;
      carry = carry ^ t;
    }
    carryTemp = carry;
    axc = a[size - 1] ^ carry;
    bxc = b[size - 1] ^ carry;
    res[size - 1] = a[size - 1] ^ bxc;
    t = axc & bxc;
    carry = carry ^ t;
    // Will truncate if there is overflow, but is not needed on the current
    // inputs
    //              if(size == totalBits){
    //                      if((carry^carryTemp).reveal<bool>()){
    //                              for(int i = decimalBits; i < totalBits - 1; i++){
    //                                      res[i] = 1;
    //                              }
    //                              res[totalBits - 1] = 0;
    //                              std::cout<<"OVERFLOW"<<endl;
    //                      }
    //              }
  }

  // adds two numbers
  fixedPoint operator+(const fixedPoint &rhs) {
    fixedPoint ret(*this);
    full_adder(this->bits, rhs.bits, nullptr, nullptr, ret.bits, totalBits);
    return ret;
  }

  // circuit to compute the absolute value
  void _abs(Bit *a, Bit *b, Bit s, Bit *res, int size)
  {
    Bit carry, xr, ad, t, carryTemp;
    for (int i = 0; i < size ; i++) {
      xr = a[i] ^ b[i];
      ad = xr & s;
      res[i] = a[i] ^ ad;
    }
  }

  // Computes the absolute value
  fixedPoint absolute_value()
  {
    Bit s;
    fixedPoint v2 = -(*this);
    fixedPoint res(*this);
    s = res.isPositive();
    _abs(this->bits,v2.bits, !s, res.bits, totalBits);
    return res;
  }

  Bit isPositive(){
    return !this->bits[totalBits-1];
  }

  // Circuit to determine if a>b
  void compBigger(Bit *a, Bit *b, Bit *res, int size)
  {
    Bit borrow, bxa, bxc, t, borrowTemp;
    Bit zero(0, PUBLIC);
    borrow = zero;
    for (int i = 0; i < size-1 ; i++) {
      bxa = a[i] ^ b[i];
      bxc = borrow ^ b[i];
      t = bxa & bxc;
      borrow = borrow ^ t;      
    }
    int i = totalBits - 1;
    res[i] = a[i] ^ b[i] ^ borrow;
    
  }

  Bit operator>(const fixedPoint &rhs) {
    fixedPoint res(*this);
    compBigger(rhs.bits, this->bits, res.bits, totalBits);
    return res.bits[totalBits - 1];
  }

  // Circuit to computer a - b
  void full_sub(Bit *a, Bit *b, Bit *borrowOut, Bit *borrowIn, Bit *ret,
                int size) {
    Bit borrow, bxc, bxa, t;
    int skipLast;
    int i = 0;
    if (size == 0) {
      if (borrowIn && borrowOut)
        *borrowOut = *borrowIn;
      return;
    }
    if (borrowIn)
      borrow = *borrowIn;
    else
      borrow = false;
    // skip AND on last bit if borrowOut==NULL
    skipLast = (borrowOut == nullptr);
    while (size-- > skipLast) {
      bxa = a[i] ^ b[i];
      bxc = borrow ^ b[i];
      ret[i] = bxa ^ borrow;
      t = bxa & bxc;
      borrow = borrow ^ t;
      ++i;
    }
    if (borrowOut != nullptr) {
      *borrowOut = borrow;
    } else
      ret[i] = a[i] ^ b[i] ^ borrow;
  }
  
  fixedPoint operator-(const fixedPoint &rhs) {
    fixedPoint ret(*this);
    full_sub(this->bits, rhs.bits, nullptr, nullptr, ret.bits, totalBits);
    return ret;
  }

  fixedPoint operator-() const {
    return (fixedPoint(0, decimalBits, decimalBits, PUBLIC) - (*this));
  }

  // Circuit to compute a*b
  void full_mul(Bit *a, Bit *b, Bit *ret) {
    Bit one(1, PUBLIC);
    Bit zero(0, PUBLIC);
    Bit *sum = new Bit[2 * totalBits];
    Bit *temp = new Bit[totalBits + 1];
    Bit over = zero;
    Bit s;
    for (int i = 0; i < 2*totalBits; i++) {
      sum[i] = false;
    }
    for (int i = 0; i < totalBits; i++) {
      for (int k = 0; k < totalBits; k++) {
        temp[k] = a[i] & b[k];
      }
      if (i == 0) {
        temp[totalBits] = one;
        temp[totalBits - 1] = !temp[totalBits - 1];
      } else if (i < totalBits - 1) {
        temp[totalBits - 1] = !temp[totalBits - 1];
        temp[totalBits] = zero;
      } else {
        for (int j = 0; j < totalBits - 1; j++) {
          temp[j] = !temp[j];
        }
        temp[totalBits] = one;
      }
      full_adder(sum + i, temp, nullptr, nullptr, sum + i, totalBits + 1);
    }

    s = a[totalBits - 1] ^ b[totalBits - 1];
    for (int i = 3*decimalBits; i < 2*totalBits; i++) {
      over = over | (sum[i] ^ sum[3*decimalBits - 1]);
    }
    for (int i = totalBits; i < 3*decimalBits - 1; i++) {
      ret[i - decimalBits] = (over & (!s)) | ((!over) & sum[i]);
      ret[i - totalBits] = sum[i - decimalBits];
    }
    ret[totalBits - 1] = (over & s) | ((!over) & sum[3*decimalBits - 1]);
    ret[decimalBits - 1] = sum[totalBits - 1];

    //              memcpy(ret, sum+decimalBits, sizeof(Bit)*totalBits);
    delete[] sum;
    delete[] temp;
  }
};

// Reveal the information to the party
// Converts the binary back to double
template <> double fixedPoint::reveal<double>(int party) {
  bool *integers = new bool[decimalBits];
  bool *decimals = new bool[decimalBits];
  string wholenum = "", fraction = "";
  double result;
  ProtocolExecution::prot_exec->reveal(integers, party, (block *)bits + decimalBits, decimalBits);
  ProtocolExecution::prot_exec->reveal(decimals, party, (block *)bits, decimalBits);

  fraction += '0';
  for (int i = decimalBits - 1; i >= 0; --i) {
    wholenum += (integers[i] ? '1' : '0');
    fraction += (decimals[i] ? '1' : '0');
    // cout<<integers[i]<<endl;
  }

  int i = 0;
  if (integers[decimalBits - 1] == 1) {
    i = pow(2, decimalBits - 1);
    wholenum.erase(0, 1);
  }

  wholenum = to_string(stoi(wholenum, nullptr, 2) - i);
  fraction = to_string(std::stol(fraction, nullptr, 2)); // stol instead of stoi for a better precision
  result = stod(wholenum) + stod(fraction) / (multiplier);

  delete[] integers;
  delete[] decimals;
  return result;
}

template <> string fixedPoint::reveal<string>(int party) {
  return to_string(this->reveal<double>(party));
}

fixedPoint fixedPoint::operator*(fixedPoint &rhs) {
  fixedPoint ret(*this);
  full_mul(this->bits, rhs.bits, ret.bits);
  return ret;
}

#endif
