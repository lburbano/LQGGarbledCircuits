// Implements the system side of the protocol
// CHange all Alice to BOB?
#include "cloud_hybrid.h"
#include "emp-tool/execution/circuit_execution.h"
#include "subsystem_hybrid.h"

void print_init(Cloud *cloud, subSystem *subsystem, int party, int k) {
  return;
}

void print_rest( Cloud *cloud, subSystem *subsystem, int party, int k) {
  for (int i = 0; i < cloud->sizeCusum[0]; i++) {
    if(cloud->HARD_CODE_ZEROES == 1){
      cout << cloud->alarm_ne[i][0] << ", "; 
    //   cout << cloud->alarm[i][0].reveal(PUBLIC) << ", "; 
    }else{
     cout << cloud->alarm[i][0].reveal(PUBLIC) << ", "; 
    }
  }
  cout << endl;
}

int main(int argc, char **argv) {
  int port, party;
  parse_party_and_port(argv, &party, &port);
  int niter = atoi( argv[3] );
  int integer_bits = atoi( argv[4] );
  if (integer_bits % 1 != 0 || integer_bits > 48){
    cout << "Number of bits should be even and less than 48" <<endl;
    return 0;
  }
  integer_bits = integer_bits / 2;
  
  NetIO *io = new NetIO(party == ALICE ? nullptr : "127.0.0.1", port);
  setup_semi_honest(io, party);
  int print = 0;
  
  int parties[2];
  parties[0] = party == ALICE ? ALICE:BOB;
  parties[1] = party == ALICE ? BOB:ALICE;

  // Bit one(1, parties[0]);
  // for(int i=0; i<1000; i++){
  //   one & one;
  // }
  // one.reveal(parties[0]);
  
  
    subSystem *subsystem = new subSystem( parties, integer_bits );
  Cloud *cloud = new Cloud( parties, integer_bits );
  
  subsystem->inputData();
  subsystem->garbleConstants();
  

  cloud->getInputs(subsystem->Nu, subsystem->sizeNu, subsystem->Tau, 
                    subsystem->sizeTau, subsystem->Cusum, subsystem->sizeCusum);
  
  for(int k=0; k<niter; k++){
    if(print)
        print_rest( cloud, subsystem, parties[0], k);
    cloud->compute_residues();
    cloud->computeCusum();
    cloud->reveal_alarm( PUBLIC );
    }
  cout<< "Finished"<<endl;

  delete io;
  
  return 0;
}