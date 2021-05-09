#!/usr/local/bin/python3
import json
import os
from collections import defaultdict
from pylab import *
import numpy as np
import argparse

#
fontSize = 17
from matplotlib import rc
font = {'size'   : fontSize, 'family':'sans-serif', 'sans-serif':['Arial']}
rc('font', **font)
from matplotlib import rcParams
rcParams.update({'figure.autolayout': True})

def getTimeFromBmkJson(fileName):
    try:
        with open(fileName) as json_file:
            data = json.load(json_file)
            return data['benchmarks'][0]['cpu_time']
    except:
        print("Failed to parse '%s'"%(fileName))

sortAlgsLabels = ['std::stable_sort', 'boost::spread_sort', 'radix_sort_msd', 'radix_sort_lsd', 'radix_sort_hybrid']
distribTicksLabels = ['almostsorted', 'all equal', 'few unique', 'ascending', 'shuffled', 'descending']

colorRed = '#0A3FF4'
colorBlack = 'black'
colorBlue = '#FF231F'
colorGreen = '#036d15'
colorGray = 'gray' #'#18455c'
colorOrange = '#EE7F2D' # old '#C44910'

sortAlgsColors = [colorRed, colorGreen, colorBlue, colorBlack, colorOrange]

sortNamesMap = { 
    'BoostSpreadSort' : 1,
    'MSDRadixSort' : 2,
    'LSDRadixSort' : 3,
    'HybridRadixSort' : 4,
    'StdStableSort' : 0 }

resultsDir = "./results"

def customLegend():
    import matplotlib.patches as mpatches
    handles = []
    for i in range(0, len(sortAlgsColors)):
        handles.append(mpatches.Patch(color=sortAlgsColors[i], label=sortAlgsLabels[i]))
    legend(handles=handles, loc="lower right")


def plotDictOfDict(vals):
    shift = 0.0
    for distrib, sorts in vals.items():
        print (distrib)
        times = [0]*len(sortNamesMap)
        for sort in sorted(sorts):
            ind = sortNamesMap[sort]
            times[ind] = sorts[sort]
            print(sort + " " + str(sorts[sort]))
        pos = np.arange(len(times)-1, -1, -1)*0.5+.5 + shift 
        barh(pos, times, align='center', height=0.5, color=sortAlgsColors)
        shift += 3.0
    
    yticks(np.arange(1.25, 1.25 + 3*len(distribTicksLabels), 3), distribTicksLabels)

    customLegend()

def plotWithBars(outputFileName):
    vals = defaultdict(dict)
    for fileName in os.listdir(resultsDir):
        if fileName.endswith(".json"):
            tokens = fileName.split('_')
            distrib = tokens[1]
            sort = tokens[2].split('.')[0]
            vals[distrib][sort] = getTimeFromBmkJson( os.path.join(resultsDir, fileName) )
            #print(distrib + ' ' + sort + ' ' + str(vals[distrib][sort]))

    figure(1)
    plotDictOfDict(vals)
    xlabel(r"Time, ms", labelpad=3, fontsize=fontSize)
    plt.savefig(outputFileName, dpi=300)

def main():
    parser = argparse.ArgumentParser(description='Plot one benchmarks for different distribution')
    parser.add_argument('-o','--output', help='Output image file', required=False, default="diffdistrib.png")
    args = vars(parser.parse_args())

    plotWithBars(args['output'])

if __name__ == "__main__":
    main()
