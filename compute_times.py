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
            command = f'build/bin/system_time 1 12345 2 {b}  > {destiny}times_system_garbler_{b}_{j}.csv'
            process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
            
            command2 = f'build/bin/cloud_time 2 12345 2 {b} > {destiny}times_cloud_evaluator_{b}_{j}.csv'
            process2 = subprocess.Popen(command2, shell=True, stdout=subprocess.PIPE)
            process2.wait()
            
            process.terminate()
            process2.terminate()
            
        
        # System is the evaluator and cloud is the garbler
        for j in range(iterations):
            # Compute program n times
            command = f'build/bin/system_time 2 12345 2 {b}  > {destiny}times_system_evaluator_{b}_{j}.csv'
            process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
            
            command2 = f'build/bin/cloud_time 1 12345 2 {b} > {destiny}times_cloud_garbler_{b}_{j}.csv'
            process2 = subprocess.Popen(command2, shell=True, stdout=subprocess.PIPE)
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
                online_round0 = online[2]
                online_round0_li = [int(x) for x in list(online_round0.split(","))]
                online_round0T.append(tuple(online_round0_li))
                
                # Round k>1
                online_round1 = online[3]
                online_round1_li = [int(x) for x in list(online_round1.split(","))]
                online_round1T.append(tuple(online_round1_li))
    offlineT_averages = [sum(y) / len(y) for y in zip(*offlineT)]
    
    online_round0T_averages = [sum(y) / len(y) for y in zip(*online_round0T)]
    online_round0T_averages.append( sum( online_round0T_averages[:-1] ) )

    online_round1T_averages = [sum(y) / len(y) for y in zip(*online_round1T)]
    online_round1T_averages.append( sum(online_round1T_averages[:-1]) )
    return offlineT_averages, online_round1T_averages

def write_results(row, averages, csvwriter):
        row1 = row+averages
        csvwriter.writerow(row1)



#%%

def write_results_in_file(direc, data_directory):

    system_generator_48_averages_offline, system_generator_48_averages_online = calculate_averages('times_system_garbler_48',direc)
    cloud_evaluator_48_averages_offline, cloud_evaluator_48_averages_online = calculate_averages('times_cloud_evaluator_48',direc)
    system_evaluator_48_averages_offline, system_evaluator_48_averages_online = calculate_averages('times_system_evaluator_48',direc)
    cloud_generator_48_averages_offline, cloud_generator_48_averages_online = calculate_averages('times_cloud_garbler_48',direc)
    
    system_generator_32_averages_offline, system_generator_32_averages_online = calculate_averages('times_system_garbler_32',direc)
    cloud_evaluator_32_averages_offline, cloud_evaluator_32_averages_online = calculate_averages('times_cloud_evaluator_32',direc)
    system_evaluator_32_averages_offline, system_evaluator_32_averages_online = calculate_averages('times_system_evaluator_32',direc)
    cloud_generator_32_averages_offline, cloud_generator_32_averages_online = calculate_averages('times_cloud_garbler_32',direc)
    
    row1 = ['System Generator (48 bits)']
    row2 = ['Cloud Evaluator (48 bits)']
    row3 = ['System Evaluator (48 bits)']
    row4 = ['Cloud Generator (48 bits)']
    
    row5 = ['System Generator (32 bits)']
    row6 = ['Cloud Evaluator (32 bits)']
    row7 = ['System Evaluator (32 bits)']
    row8 = ['Cloud Generator (32 bits)']
    
    results = ''.join((data_directory, '_results.csv'))
    with open(results, 'w', newline='') as csvfile:
    
        fields_offline = ['', 'Generate labels', 'Compute constants']
        csvwriter = csv.writer(csvfile, delimiter=',')
        csvwriter.writerow(fields_offline)
        
        write_results(row1, system_generator_48_averages_offline, csvwriter)
        write_results(row2, cloud_evaluator_48_averages_offline, csvwriter)
        write_results(row3, system_evaluator_48_averages_offline, csvwriter)
        write_results(row4, cloud_generator_48_averages_offline, csvwriter)
        
        write_results(row5, system_generator_32_averages_offline, csvwriter)
        write_results(row6, cloud_evaluator_32_averages_offline, csvwriter)
        write_results(row7, system_evaluator_32_averages_offline, csvwriter)
        write_results(row8, cloud_generator_32_averages_offline, csvwriter)    
        
        fields_online = ['', 'Prediction', 'Estimation', 'Residues', 'Control Action', 'CUSUM', 'Reveal Alarm', 'Reveal U', "Measurement labels", 'Total Time']
        csvwriter.writerow(fields_online)
        
        write_results(row1, system_generator_48_averages_online, csvwriter)
        write_results(row2, cloud_evaluator_48_averages_online, csvwriter)
        write_results(row3, system_evaluator_48_averages_online, csvwriter)
        write_results(row4, cloud_generator_48_averages_online, csvwriter)
        
        write_results(row5, system_generator_32_averages_online, csvwriter)
        write_results(row6, cloud_evaluator_32_averages_online, csvwriter)
        write_results(row7, system_evaluator_32_averages_online, csvwriter)
        write_results(row8, cloud_generator_32_averages_online, csvwriter)



#%% -------------------------------------------------------------------------------------------------------------------------------------

# MAIN


def main():
    rewrite = 1
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
    iterations = 10


    
    # Iterate through all folders of Source Path
    for root, subdirectories, files in os.walk(sourcePath):
        for subdirectory in subdirectories:
            source_folder = os.path.join(root, subdirectory)
            print(subdirectory)
            
            copy_folder = f'rm -r Data && cp -r all_data/{subdirectory} ./Data'
            os.system(copy_folder)
            
            if rewrite:
                # Write results of executed commands -------------------------------
                run_commands(bits, iterations, end_path + subdirectory + "/")
            
            # Execute commands with data replaced ------------------------------
            write_results_in_file(output_direc + subdirectory + "/", output_direc + subdirectory)

if __name__ == "__main__":
    main()