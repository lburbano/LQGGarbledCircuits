// Implements the cloud side of the protocol
// CHange all Alice to BOB?
#include <iostream>
#include "cloud_hybrid.h"
#include "emp-tool/execution/circuit_execution.h"
#include "dummy_subsystem_hybrid.h"

void print_init(Cloud *cloud, subSystem *subsystem, int party, int k) {
  cout << "s" << k << ":  " << endl;
  for (int i = 0; i < cloud->sizeCusum[0]; i++) {
    for (int j = 0; j < cloud->sizeCusum[1]; j++) {
      cout << cloud->Cusum[i][j].reveal<double>(party) << ", ";
    }
    cout << endl;
  }
  cout << endl << endl;

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
    cout << "Number of bits should be even" <<endl;
    return 0;
  }
  integer_bits = integer_bits / 2;

  NetIO *io = new NetIO(party == ALICE ? nullptr : "127.0.0.1", port);
  setup_semi_honest(io, party);
  
  
  int print = 0;
  int parties[2];
  parties[0] = party == BOB ? ALICE:BOB;
  parties[1] = party == BOB ? BOB:ALICE;   
  
  
  
    subSystem *subsystem = new subSystem( parties, integer_bits );
  Cloud *cloud = new Cloud( parties, integer_bits );
  
  subsystem->inputData();
  auto init_garbling_offline = high_resolution_clock::now();
  subsystem->garbleConstants();
  auto end_garbling_offline = high_resolution_clock::now();

  auto TofflineConstants = std::chrono::duration_cast<std::chrono::microseconds>(end_garbling_offline  - init_garbling_offline).count();

  cloud->getInputs(subsystem->Nu, subsystem->sizeNu, subsystem->Tau, 
                    subsystem->sizeTau, subsystem->Cusum, subsystem->sizeCusum);
  cout <<"Offline_garbling_time"<< endl;
  cout << TofflineConstants<< endl;
  cout << "Online"<<endl;


  auto init_computations = high_resolution_clock::now();
  auto end_computations = high_resolution_clock::now();
  int computation_time;
  for(int k=0; k<niter; k++){
    if(print)
        print_rest( cloud, subsystem, parties[0], k);
    usleep(0.5*1000*1000);
    init_computations = high_resolution_clock::now();
    cloud->compute_residues();
    cloud->computeCusum();
    cloud->reveal_alarm( PUBLIC );
    end_computations = high_resolution_clock::now();
    computation_time = std::chrono::duration_cast<std::chrono::microseconds>(end_computations  - init_computations).count();
    cout<< computation_time <<endl;
    }
  cout<< "Finished"<<endl;

  delete io;
  
  return 0;
}