// Implements the cloud side of the protocol
// CHange all Alice to BOB?
#include "Cloud.h"
#include "emp-tool/execution/circuit_execution.h"
#include "DummySubsystem.h"

void print_init(Cloud *cloud, subSystem *subsystem, int k) {
  cout << "z" << k << ":  " << endl;
  for (int i = 0; i < subsystem->sizexk[0]; i++) {
    for (int j = 0; j < subsystem->sizexk[1]; j++) {
      cout << fixed << setprecision(5) << subsystem->zk[i][j].reveal<double>(ALICE) << ", ";
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

void print_rest( Cloud *cloud, subSystem *subsystem, int k) {
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
      cout << subsystem->zk[i][j].reveal<double>(ALICE) << ", ";
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
  bool print = 0;

  
  // fixedPoint gamma3 = fixedPoint(-1, 2, 2, BOB);
  // gamma3 = gamma3.absolute_value();
  // cout << gamma3.reveal<double>(ALICE) << endl;
  
  
  subSystem *subsystem = new subSystem();
  Cloud *cloud = new Cloud();

  // client offline
  int cloud_load_data = 1;
  subsystem->inputData( cloud_load_data );
  if(cloud_load_data == 1){
    subsystem->computeControlConstants();
    
  }
  subsystem->garbleConstants( cloud_load_data );
  subsystem->computeReferenceConstants();
  
  

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

  // cloud->computeConstants();


  

  int k = 0;
 
  if (print) {
    print_init( cloud, subsystem, k);
  }

  cout << endl;
  // Control loop

  cout << "Reveal U" << ", ";
  cout << "New Labels" << ", ";
  cout << "totalTime" << ", ";
  cout << "predictionTime" << ", ";
  cout << "estimationTime" << ", ";
  cout << "residuesTime" << ", ";
  cout << "controlTime" << ", ";
  cout << "cusumTime" << ", ";
  cout << "revealAlarm" << ", ";    
  cout << endl;
  
  auto init = high_resolution_clock::now();
  auto predictT = high_resolution_clock::now();
  auto estimateT = high_resolution_clock::now();
  auto residuesT = high_resolution_clock::now();
  auto ukT = high_resolution_clock::now();
  auto cusumT = high_resolution_clock::now();
  auto simT = high_resolution_clock::now();
  auto newLabelsT = high_resolution_clock::now();
  
  auto end = high_resolution_clock::now();
  for (k = 0; k < 20; k++) {
    init = high_resolution_clock::now();
    if (k > 0){
      cloud->predict();
      predictT = high_resolution_clock::now();

      cloud->computexHat(subsystem->zk);
      estimateT = high_resolution_clock::now();
    }
    
    cloud->computeuk();
    ukT = high_resolution_clock::now();
    
    cloud->computeResidues(subsystem->zk);
    residuesT = high_resolution_clock::now();

    cloud->computeCusum();
    cusumT = high_resolution_clock::now();

    subsystem->measureState(cloud->uk);
    simT = high_resolution_clock::now();

    subsystem->computezk();
    newLabelsT = high_resolution_clock::now();
    
    
    cloud->reveal_alarm( PUBLIC );
    end = high_resolution_clock::now();
    

    auto totalTime      = std::chrono::duration_cast<std::chrono::microseconds>(end       - init).count();
    auto predictionTime = std::chrono::duration_cast<std::chrono::microseconds>(predictT  - init).count();
    auto estimationTime = std::chrono::duration_cast<std::chrono::microseconds>(estimateT - predictT).count();
    auto controlTime    = std::chrono::duration_cast<std::chrono::microseconds>(ukT       - estimateT).count();
    auto residuesTime   = std::chrono::duration_cast<std::chrono::microseconds>(residuesT - ukT).count();
    auto cusumTime      = std::chrono::duration_cast<std::chrono::microseconds>(cusumT    - residuesT).count();
    auto revealAlarm    = std::chrono::duration_cast<std::chrono::microseconds>(end   - newLabelsT).count();
    

    cout << totalTime << ", ";
    cout << predictionTime << ", ";
    cout << estimationTime << ", ";
    cout << residuesTime << ", ";
    cout << controlTime << ", ";
    cout << cusumTime << ", ";
    cout << revealAlarm << ", ";
    cout << endl;

    if (print) 
      print_rest( cloud, subsystem, k+1);
    
    
  }
  cout << "Finished" << endl;
  
  delete io;
  return 0;
}
