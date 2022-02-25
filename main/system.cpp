// Implements the system side of the protocol
// CHange all Alice to BOB?
#include "Cloud.h"
#include "emp-tool/execution/circuit_execution.h"
#include "subsystem.h"

void print_init(Cloud *cloud, subSystem *subsystem, int k) {
  cout << endl << endl;
  cout << "z" << k << ":  " << endl;
  for (int i = 0; i < subsystem->sizexk[0]; i++) {
    for (int j = 0; j < subsystem->sizexk[1]; j++) {
      cout << fixed << setprecision(5) << subsystem->zk[i][j].reveal<double>(ALICE) << ", " <<subsystem->zk_ne[i][j] << ", ";
    }
    cout << endl;
  }
  cout << endl << endl;

  cout << "s" << k << ":  " << endl;
  for (int i = 0; i < cloud->sizeCusum[0]; i++) {
    for (int j = 0; j < cloud->sizeCusum[1]; j++) {
      cout << cloud->Cusum[i][j].reveal<double>(ALICE) << ", ";
    }
    cout << endl;
  }
  cout << endl << endl;
}

void print_rest(Cloud *cloud, subSystem *subsystem, int k) {
  cout << endl << endl;
  cout << "u" << k << ":  " << endl;
  for (int i = 0; i < cloud->sizeuk[0]; i++) {
    for (int j = 0; j < cloud->sizeuk[1]; j++) {
      cout << cloud->uk[i][j].reveal<double>(ALICE) << ", ";
    }
    cout << endl;
  }
  cout << endl << endl;
  cout << "z" << k << ":  " << endl;
  for (int i = 0; i < subsystem->sizezk[0]; i++) {
    for (int j = 0; j < subsystem->sizezk[1]; j++) {
      cout << subsystem->zk[i][j].reveal<double>(ALICE) << ", " << subsystem->zk_ne[i][j] << ", ";
    }
    cout << endl;
  }
  cout << endl << endl;
  cout << "s" << k << ":  " << endl;
  for (int i = 0; i < cloud->sizeCusum[0]; i++) {
    for (int j = 0; j < cloud->sizeCusum[1]; j++) {
      cout << cloud->Cusum[i][j].reveal<double>(ALICE) << ", ";
    }
    cout << endl;
  }
  cout << endl << endl;

  cout << "Alarms: k=" << k << ":  " << endl;
  for (int i = 0; i < cloud->sizeyp[0]; i++) {
    if(cloud->HARD_CODE_ZEROES == 1){
      cout << cloud->alarm_ne[i][0] << ", "; 
    }else{
     cout << cloud->alarm[i][0].reveal(PUBLIC) << ", "; 
    }
    cout << endl;
  }
  cout << endl << endl;
}

int main(int argc, char **argv) {
  int port, party;
  parse_party_and_port(argv, &party, &port);
  NetIO *io = new NetIO(party == ALICE ? nullptr : "127.0.0.1", port);
  setup_semi_honest(io, party);
  bool print = 1;


  // fixedPoint gamma3 = fixedPoint(0, 2, 2, BOB);
  // gamma3 = gamma3.absolute_value();
  // cout << gamma3.reveal<double>(ALICE) << endl;
 
 
  subSystem *subsystem = new subSystem();
  Cloud *cloud = new Cloud();

  int system_load_data = 0;
  // Loads data related to controller and system
  subsystem->inputData( system_load_data );

  // Computes controller matrices 
  if(system_load_data == 1){
    subsystem->computeControlConstants();
  }
  subsystem->garbleControlConstants( system_load_data );
  subsystem->computeReferenceConstants();
  

  // Computes reference related constants
  
  

  // cloud offline
  cloud->getInputs(subsystem->L, subsystem->sizeL, subsystem->K, subsystem->sizeK,
                   subsystem->gamma1, subsystem->sizegamma1, subsystem->xgamma, subsystem->sizexgamma,
                   subsystem->uTilder, subsystem->sizeuTilder,
                   subsystem->Nu, subsystem->sizeNu, subsystem->Tau,
                   subsystem->sizeTau, subsystem->Cusum, subsystem->sizeCusum,
                   subsystem->A_BK, subsystem->sizeA_BK, subsystem->Bug, subsystem->sizeBug, //maybe changed
                   subsystem->xr, subsystem->sizexr, subsystem->ur,
                   subsystem->sizeur, subsystem->xk, subsystem->sizexk,
                   subsystem->xHatk, subsystem->yp);
 

  int k = 0;

  if (print) {
    print_init(cloud, subsystem, k);
  }
  cout << endl;
  // Control loop


  
  for (k = 0; k < 20; k++) {

    if (k > 0){
      cloud->predict();
      cloud->computexHat(subsystem->zk);
    }
    cloud->computeuk();
    cloud->computeResidues(subsystem->zk);
    cloud->computeCusum();
    subsystem->measureState(cloud->uk);
    subsystem->computezk();
    if (print) 
      print_rest(cloud, subsystem, k+1);
    cloud->reveal_alarm( PUBLIC );
    
  }
  cout << "Finished" << endl;
  
  delete io;
  return 0;
}