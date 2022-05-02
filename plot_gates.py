import numpy as np
import matplotlib.pyplot as plt
import xlrd
import pandas as pd

def main():
    name = "./results/gates/gates_number.xlsx"
    nbit = [32, 48]
    combinations = [ [10, 2], [50, 10], [250, 50] ] 
    niter = 3
    gate_names = ["AND", "XOR", "OR", "NOT"]
    name_sheet = "gates"
    data = pd.read_excel(name, sheet_name= name_sheet)
    data = data.values
    data = data[1:, 3:]
    
    pass

    


def plotData(title, ylable, labels, data, labelNames, stacked, stacked_title=""):
    x = np.arange( len(labels) )  # the label locations
    color = ["C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7"]
    
    
    width = 0.7/len(data[0][0])         # the width of the bars
    c = -width*len(data[0][0])/2+width/2
    w = np.linspace( c, -c, len(data[0][0]))

    fig, ax = plt.subplots( figsize=(18, 6) )
    
    tck = []
    
    
    for i in range( len(data) ): # Iterate over stacked data
        for j in range( len(data[0][0]) ): # iterate over configuration
            ax.bar(x + w[j],  data[i, :, j], bottom=sum(data[:i, :, j]), width=width, color= color[i], edgecolor="black")
        
    for i in range( len(x) ):
        for j in range( len(w) ):
            if w[j]  == 0:
                tck.append( x[i] + w[j] + 0.001 )
            else:
                tck.append( x[i] + w[j])
    for i in range( len(data) ): # Iterate over stacked data
        ax.bar(x,  data[i, :, 0]*0, width=width, color= color[i], edgecolor="black", label=stacked_title[i])
    conf = []
    for j in range( len(data[0][0]) ): # iterate over configuration
        conf.append( labelNames[j] )
    conf = conf*len(data[0])
    # Add some text for labels, title and custom x-axis tick labels, etc.

    
    plt.legend( )
    fig.tight_layout()
    ax.set_xticks(x)
    ax.set_xticklabels(labels)
    ax.set_xticks(tck, minor=True)
    ax.set_xticklabels(conf, minor=True)
    ax.tick_params(axis='x', which='minor', pad=5, labelsize=10, rotation = 90)    
    ax.tick_params(axis='x', which='major', pad=120, labelsize=15, rotation = 0)    
    
    
    ax.legend(loc='center left', bbox_to_anchor=(1, 0.5))
    ax.set_ylabel(ylable)
    ax.set_title(title)
    plt.savefig(title + ".pdf", bbox_inches='tight')

if __name__ == "__main__":
    main()