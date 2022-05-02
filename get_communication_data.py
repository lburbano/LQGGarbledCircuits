from signal import pause
from unicodedata import name
import pyshark
import os
import time
import multiprocessing


name_data_summary = 'summary_hybrid.txt'
name_all_data = 'all_data_hybrid.txt'

def read_communication_data(name):
    cap = pyshark.FileCapture(name)
    A_to_B = 0
    B_to_A = 0
    for pkt in cap:
        if pkt.tcp.dstport == "12345":
            A_to_B = A_to_B + int(pkt.captured_length)
        else:
            B_to_A = B_to_A + int(pkt.captured_length)
    return A_to_B, B_to_A

def main_caller():
    pool = multiprocessing.Pool(16)
    # pool.imap_unordered(print_time_system_evaluator())
    systems = os.listdir("./all_data")
    # systems = ["y20_u4"]
    bits = ["32", "48"]
    iterations = ["0", "1"]
    names_system_evaluator, names_system_generator, names_hybrid = gen_names(systems, iterations, bits)
    # pool.map(print_time_system_evaluator, names_system_evaluator )
    # pool.map(print_time_system_generator, names_system_generator )
    # pool.map(print_time_system_evaluator, names_hybrid )
    for bit in bits:
        for system in systems:
            for iteration in iterations:
                # summarize_data(system, iteration, bit, "evaluator")
                # summarize_data(system, iteration, bit, "generator")
                summarize_data(system, iteration, bit, "hybrid")
                pass
    

def summarize_data(system_target, iteration_target, bit_target, system_party):
    with open(name_all_data) as f:
        lines = f.readlines()
    lines.sort()
    cloud_system_average = 0
    system_cloud_average = 0
    counter = 0
    for line in lines:
        name = line.split(",")
        system_cloud = name[1]
        system_cloud = int( system_cloud.split(":")[-1] )
        cloud_system = name[2]
        cloud_system = int( cloud_system.split(":")[-1] )
        name = name[0]
        if system_target in name and system_party in name and ("iter_"+iteration_target) in name and (bit_target+"_bit") in name:
            cloud_system_average = cloud_system_average + cloud_system
            system_cloud_average = system_cloud_average + system_cloud
            counter += 1
    if counter > 0:
        cloud_system_average = cloud_system_average / counter
        system_cloud_average = system_cloud_average / counter
    data = f'{system_target}_bit_{bit_target}_system_{system_party}_iter_{iteration_target}, system-cloud: {system_cloud_average}, cloud-system: {cloud_system_average}\n'
    with open(name_data_summary, 'a') as f:
        f.write(data)

    

def print_time_system_evaluator(name):
    A_to_B, B_to_A = read_communication_data(name)
    name = name.split("/")
    strng = f'{name[5]}_{name[3]}_{name[4]}{name[-1][-12:-5]}, {name[6]}: system-cloud: {A_to_B}, cloud-system: {B_to_A}'
    print(strng)
    with open(name_all_data, 'a') as f:
        f.write(strng + "\n")

def print_time_system_generator(name):
    A_to_B, B_to_A = read_communication_data(name)
    name = name.split("/")
    strng = f'{name[5]}_{name[3]}_{name[4]}{name[-1][-12:-5]}, {name[6]}: system-cloud: {B_to_A}, cloud-system: {A_to_B}'
    print(strng)
    with open(name_all_data, 'a') as f:
        f.write(strng + "\n")

def gen_names(systems, iterations, bits):
    names_system_evaluator = []
    names_system_generator = []
    names_hybrid = []
    repetitions = ["iteration_"+str(i) for i in range(0, 61)]
    for bit in bits:
        for system in systems:
            for iteration in iterations:
                folder = f'./results/communication/{bit}_bit/system_evaluator/{system}/'
                for directory in repetitions:
                    folder1 = folder + directory + "/"
                    name = folder1 + f'captura_system_evaluator_bit_{bit}_iter_{iteration}.pcap'
                    names_system_evaluator.append(name)

                    folder2 = f'./results/communication/{bit}_bit/system_generator/{system}/'
                    folder2 = folder2 + directory + "/"
                    name = folder2 + f'captura_system_generator_bit_{bit}_iter_{iteration}.pcap'
                    names_system_generator.append(name)

                    folder3 = f'./results/communication/{bit}_bit/hybrid/{system}/'
                    folder3 = folder3 + directory + "/"
                    name = folder3 + f'captura_system_evaluator_bit_{bit}_iter_{iteration}.pcap'
                    names_hybrid.append(name)
                
    return names_system_evaluator, names_system_generator, names_hybrid

if __name__ == "__main__":
    main_caller()
