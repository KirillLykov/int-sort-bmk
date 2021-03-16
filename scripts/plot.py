#!/usr/local/bin/python3.7
import argparse
import numpy as np
import pandas as pd
import matplotlib as mlt
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

from matplotlib.colorbar import Colorbar
import matplotlib.patches as mpatches
#from cvxopt.coneprog import lp
import matplotlib.image as mpimg

fontSize = 17
from matplotlib import rc
font = {'size'   : fontSize, 'family':'sans-serif', 'sans-serif':['Arial']}
rc('font', **font)
from matplotlib import rcParams
rcParams.update({'figure.autolayout': True})
isTransparent = False
#rc('text', usetex=True)

colorRed = '#0A3FF4'
colorBlack = 'black'
colorBlue = '#FF231F'

colorGreen = '#036d15'
colorGray = 'gray' #'#18455c'
colorOrange = '#EE7F2D' # old '#C44910'


def read_data(fname):
    # want to skip the first 9 lines from the report
    try:
        data = pd.read_csv(fname, skiprows=np.arange(0, 9, 1))
    except ValueError:
        msg = 'Could not parse the benchmark data. Did you forget "--benchmark_format=csv"?'
        logging.error(msg)
        exit(1)
    data['input'] = data['name'].apply(lambda x : int(x.split('/')[-1]))
    return data

def setAxis(axes):
    axes.spines['right'].set_color('none')
    axes.spines['top'].set_color('none')
    axes.yaxis.set_ticks_position('left')
    axes.xaxis.set_ticks_position('bottom')
    for axis in ['top', 'bottom', 'left', 'right']:
        axes.spines[axis].set_linewidth(1.0)
    axes.xaxis.set_tick_params(width=1.0)
    axes.yaxis.set_tick_params(width=1.0)
    axes.grid(b=False)

def plot(firstData, secondData, thirdData, outputFileName, firstLabel, secondLabel, thirdLabel, ranges, firstTimeColumn, secondTimeColumn):
    fig, axes = plt.subplots(nrows=1, ncols=1, sharex=True, figsize=(6, 6))
    axes.plot(firstData['input']/1000, firstData[firstTimeColumn], linestyle='-', color=colorOrange, linewidth=2, marker='v', markersize=5)
    axes.plot(secondData['input']/1000, secondData[secondTimeColumn], linestyle='-', color=colorGreen, linewidth=2, marker='^', markersize=5)
    axes.plot(thirdData['input']/1000, thirdData[secondTimeColumn], linestyle='-', color=colorGray, linewidth=2, marker='o', markersize=5)

    axes.set_xlabel(r"#N, k", labelpad=3, fontsize=fontSize)
    axes.set_ylabel(r"Time,  us", labelpad=0, fontsize=fontSize)
    if ranges != None:
        axes.axis(ranges)
    #axes.xaxis.set_ticks( np.arange(xmin, xmax+1, 1.0) )
    #axes.yaxis.set_ticks( np.arange(ymin, ymax+1, 4.0) )
    axes.legend([firstLabel, secondLabel, thirdLabel], numpoints = 1, loc=2, frameon=False)
    setAxis(axes)
    plt.savefig(outputFileName, dpi=300, transparent=isTransparent)

def loadAndPlot(firstDataFileName, secondDataFileName, thirdDataFileName, outputFileName, firstLabel, secondLabel, thirdLabel, ranges = None, firstTimeColumn = 'cpu_time', secondTimeColumn = 'cpu_time'):
    firstData = read_data(firstDataFileName)
    secondData = read_data(secondDataFileName)
    thirdData = read_data(thirdDataFileName)
    plot(firstData, secondData, thirdData, outputFileName, firstLabel, secondLabel, thirdLabel, ranges, firstTimeColumn, secondTimeColumn)

def main():
    parser = argparse.ArgumentParser(description='Plot one benchmark vs the other and render the plot into the file.')
    parser.add_argument('-f1','--file1', help='First input csv', required=True)
    parser.add_argument('-f2','--file2', help='Second input csv', required=True)
    parser.add_argument('-f3','--file3', help='Third input csv', required=True)
    parser.add_argument('-o','--output', help='Output image file', required=True)
    args = vars(parser.parse_args())

    loadAndPlot(args['file1'], args['file2'], args['file3'], args['output'], "std::stable_sort", "boost::spreadsort", "naiveradix")

if __name__ == "__main__":
    main()
