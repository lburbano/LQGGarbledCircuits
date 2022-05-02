#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Feb 28 20:50:50 2022

@author: juanitagomez
"""

#%%
import os
import subprocess
import csv
import shutil


#%%

def run_commands(bits, iterations, destiny):
    
    for b in bits:
        
        # System is the garbler and cloud is the evaluator
        for j in range(iterations):
            # Compute program n times
            command = f'build/bin/cloud_hybrid 1 12345 100 {b}  > {destiny}times_hybrid_{b}_{j}.csv'
            command2 = f'build/bin/system_hybrid 2 12345 100 {b}'
            execute_commands(command, command2)

def execute_commands(cm1, cm2):
    process = subprocess.Popen(cm1, shell=True, stdout=subprocess.PIPE)
    process2 = subprocess.Popen(cm2, shell=True, stdout=subprocess.PIPE)
    process2.wait()        
    process.terminate()
    process2.terminate()

#%%

def calculate_averages(name_file, direc): 
    offlineT = []
    online_round0T = []
    online_round1T = []
    for name in os.listdir(direc):
        if name.startswith(name_file):
            file = "".join((direc,name))
            with open((file), 'r') as f:
                text = f.read()
                text1 = text.split('Online')
                text2 = text1[0].split()
         
                # Offline
                offline = text2[2]       
                offline_li = [int(x) for x in list(offline.split(","))]
                offlineT.append(tuple(offline_li))
                
                
                # Online
                text3 = text1[1]
                online = text3.split("\n")
                
                # First round
                online_round0 = online[1]
                online_round0_li = [int(x) for x in list(online_round0.split(","))]
                online_round0T.append(tuple(online_round0_li))
                
                # Round k>1
                online_round1 = online[2]
                online_round1_li = [int(x) for x in list(online[2:-2])]
                online_round1T.append(tuple(online_round1_li))
    offlineT_averages = [sum(y) / len(y) for y in zip(*offlineT)]
    offlineT_averages.append( sum( offlineT_averages ) )
    
    online_round0T_averages = [sum(y) / len(y) for y in zip(*online_round0T)]
    online_round0T_averages.append( sum( online_round0T_averages ) )

    online_round1T_averages = [sum(*online_round1T) / len(*online_round1T)]
    online_round1T_averages.append( sum(online_round1T_averages) )
    return offlineT_averages, online_round1T_averages

def write_results(row, averages, csvwriter):
        row1 = row+averages
        csvwriter.writerow(row1)



#%%

def write_results_in_file(direc, data_directory):

    cloud_hybrid_48_averages_offline, cloud_hybrid_48_averages_online = calculate_averages('times_hybrid_48',direc)
    
    cloud_hybrid_32_averages_offline, cloud_hybrid_32_averages_online = calculate_averages('times_hybrid_32',direc)
    
    row1 = ['System Evaluator (48 bits)']
    
    row5 = ['System Evaluator (32 bits)']
    
    results = ''.join((data_directory, '_hybrid_results.csv'))
    with open(results, 'w', newline='') as csvfile:
    
        fields_offline = ['', 'Generate labels (us)', 'Compute constants (us)', 'Total Time (us)']
        csvwriter = csv.writer(csvfile, delimiter=',')
        csvwriter.writerow(fields_offline)
        
        write_results(row1, cloud_hybrid_48_averages_offline, csvwriter)
        
        write_results(row5, cloud_hybrid_32_averages_offline, csvwriter)
        
        fields_online = ['', 'Time (us)', 'Total Time (us)']
        csvwriter.writerow(fields_online)
        
        write_results(row1, cloud_hybrid_48_averages_online, csvwriter)
        
        write_results(row5, cloud_hybrid_32_averages_online, csvwriter)
        



#%% -------------------------------------------------------------------------------------------------------------------------------------

# MAIN


def main():
    rewrite = [1, 1, 1]
    end_path = 'results/times/'
    # Where the outputs will be stored
    output_direc = f'./{end_path}'

    # Source and destination of the data with matrices
    source = 'all_data'
    destination = 'Data'
    sourcePath= './'+source
    destinationPath = './'+destination

    # Input values to run de commands 
    bits = [32, 48]
    iterations = 1
    os.system("mkdir results")
    os.system("mkdir results/times")

    
    # Iterate through all folders of Source Path
    for root, subdirectories, files in os.walk(sourcePath):
        i = 0
        for subdirectory in subdirectories:
            source_folder = os.path.join(root, subdirectory)
            os.system("mkdir results/times/"+subdirectory)
            print(subdirectory)
            
            copy_folder = f'rm -r Data && cp -r all_data/{subdirectory} ./Data'
            os.system(copy_folder)
            
            if rewrite[i]:
                # Write results of executed commands -------------------------------
                run_commands(bits, iterations, end_path + subdirectory + "/")
            i=i+1
            # Execute commands with data replaced ------------------------------
            write_results_in_file(output_direc + subdirectory + "/", output_direc + subdirectory)

if __name__ == "__main__":
    main()