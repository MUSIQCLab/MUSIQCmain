#!/usr/bin/env python
"""Provide GUI for frequency scan and Rabi flop experiments."""

from time import sleep

import tkinter as tk
from multiprocessing import Process, Pipe

import numpy as np

import sys

import matplotlib.figure as mplfig
import matplotlib.backends.backend_tkagg as tkagg

import argparse

pause = False
started=False
experiment_type=0

class App:
    "Provide GUI for frequency scan and Rabi Flop experiments."

    def __init__( self):
        self.master = tk.Tk()

        self.background = []
        self.occurences, self.shelves = {}, {}
        self.before = [0]*3
        self.after = [0]*3

        self.bright_fig = mplfig.Figure( figsize=(2.5, 6.5) )
        self.before_axes = self.bright_fig.add_axes( [0.1, 0.55, 0.8, 0.4] )
        self.after_axes = self.bright_fig.add_axes( [0.1, 0.05, 0.8, 0.4] )
        self.bright_canvas = tkagg.FigureCanvasTkAgg(
            self.bright_fig, master=self.master )
        self.bright_canvas.get_tk_widget().pack( side=tk.LEFT,
            fill=tk.Y, expand=1 )
        self.before_axes.get_xaxis().set_visible(False)
        self.before_axes.get_yaxis().set_visible(False)
        self.after_axes.get_xaxis().set_visible(False)
        self.after_axes.get_yaxis().set_visible(False)

        self.data_fig = mplfig.Figure( figsize=(6.5, 6.5) )
        self.data_axes = self.data_fig.add_axes( [0.1, 0.1, 0.8, 0.8] )
        self.data_canvas = tkagg.FigureCanvasTkAgg( 
            self.data_fig, master=self.master )
        self.data_canvas.get_tk_widget().pack( side=tk.LEFT,
            fill=tk.BOTH, expand=1 )
        self.data_axes.grid( True )

        self.setup()
        self.plot()
        self.tick()
        self.master.mainloop()

    def tick( self ):
        "Check for new data from interprocess conn."
        #Once experiment starts, self.conn is created
        if self.conn and self.conn.poll() and not pause:
            #check
            new_data = self.conn.recv().split()
            reor_data_flag = False
            if new_data[0] == 'reordata':
                reor_data_flag = True
                new_data = new_data[1:]
            #time is actually time or freq, depending on type of experiment
            time = float( new_data[0] )
            nions = (len( new_data ) - 3) / 2
            #two background positions:
            self.background.append( float(new_data[ nions+1 ]) )
            self.background.append( float(new_data[ nions*2+2 ]) )
            ###
            if len(self.background) > 1000:
                self.background = self.background[100:]

            bkg = np.mean( self.background )
            thresh = 3*np.std( self.background )

            ##pull out two lists of (nions) numbers gives counts of brightness for each of nions ions
            self.before = np.array( new_data[1:nions+1], dtype=float )
            self.after = np.array( new_data[nions+2:2*nions+2], dtype=float )
            ##before and after the experiment

            before_ions = self.before - bkg > thresh
            after_ions = self.after - bkg > thresh

            if not self.occurences.has_key( time ):
                self.occurences[ time ] = [0]*nions
                self.shelves[ time ] = [0]*nions

            if not reor_data_flag:
                for i in range( nions ):
                    if before_ions[i]:
                        self.occurences[time][i] += 1
                    if before_ions[i] and not after_ions[i]:
                        self.shelves[time][i] += 1
            self.plot()
        self.master.after( 500, self.tick )

    def plot( self ):
        "Update data plots from new data."
        self.before_axes.clear()
        self.after_axes.clear()
        self.before_axes.bar( np.arange(len(self.before)), self.before )
        self.after_axes.bar( np.arange(len(self.after)), self.after )
        self.before_axes.set_ylim( 800000, 830000 )
        self.after_axes.set_ylim( 800000, 830000 )
        self.bright_canvas.draw()

        keys = list( sorted(self.occurences.keys()) )
        vals = [[ float(s) / o if o else 0
            for s, o in zip( self.shelves[t], self.occurences[t] )]
            for t in keys ]
        
        colors = ['r', 'g', 'b', 'c', 'm', 'y', 'k']
        self.data_axes.clear()
        if vals:
            for i in range( len(vals[0]) ):
                self.data_axes.plot( keys, [v[i] for v in vals], colors[i] )
        self.data_axes.set_ylim( 0, 1 )
        self.data_canvas.draw()
    def setup(self):
        #Callback Methods
        v = tk.IntVar()
        def switch_pause():
            global pause
            pause = not pause
            self.conn.send("switch")
            if (b2["text"]=="Pause"):
                b2["text"]="Continue"
            elif (b2["text"]=="Continue"):
                b2["text"]="Pause"
        def typeChanged():
            #disable options for unselected experiment
            if(v.get()==1):
                l8["text"]="High Frequency"
                l9["text"]="Low Frequency"
                l10["text"]="Frequency Step"
                l11["text"]="Pulse Duration"
            else:
                l8["text"]="Start Time"
                l9["text"]="Stop Time"
                l10["text"]="Time Step"
                l11["text"]="Frequency"
            return
        def pre_start_experiment(self):
            #Create pipe connection and start experiment process
            PARENT_CONN, CHILD_CONN = Pipe()
            self.conn = CHILD_CONN
            PROCESS = Process( target=start_experiment(CHILD_CONN))
            PROCESS.start()
            PROCESS.join()
        def start_experiment(conn):
            from freqscan import Experiment, RabiFlop, FreqScan
            started=True
            #Disable GUI from further input
            b2["state"]=tk.ACTIVE
            b1["state"]=tk.DISABLED
            e1["state"]=tk.DISABLED
            e2["state"]=tk.DISABLED
            e3["state"]=tk.DISABLED
            e4["state"]=tk.DISABLED
            e5["state"]=tk.DISABLED
            e6["state"]=tk.DISABLED
            e7["state"]=tk.DISABLED
            e8["state"]=tk.DISABLED
            e9["state"]=tk.DISABLED
            e10["state"]=tk.DISABLED
            e11["state"]=tk.DISABLED
            r1["state"]=tk.DISABLED
            r2["state"]=tk.DISABLED
            #freqscan
            if v==1:
                freqscan = FreqScan(e9.get(), e8.get(), e10.get(), e11.get() )
                Experiment(e3.get(),freqscan,e1.get(),e6.get(),conn)
            #rabiflop
            if v==2:
                rabiflop = RabiFlop(e9.get(), e8.get(), e10.get(), e11.get() )
                Experiment(e3.get(),rabiflop,e1.get(),e6.get(),conn)
            return

        #Assign GUI elements
        b1=tk.Button(self.master,text="Start Experiment",command=pre_start_experiment(self))
        b2=tk.Button(self.master,text="Pause",justify=tk.CENTER,width=20,padx=2,state=tk.DISABLED,command=switch_pause)
        l1=tk.Label(self.master,text="Ion Position Filename")
        l2=tk.Label(self.master,text="Output Filename")
        e1=tk.Entry(self.master)
        e2=tk.Entry(self.master)
        l3=tk.Label(self.master,text="Number of Runs")
        e3=tk.Entry(self.master)
        l4=tk.Label(self.master,text="Number of Ions (bright or dark)")
        e4=tk.Entry(self.master)
        l5=tk.Label(self.master,text="Number of dark ions")
        e5=tk.Entry(self.master)
        l6=tk.Label(self.master,text="Order of ions (1=bright,0=dark)")
        e6=tk.Entry(self.master)
        l7=tk.Label(self.master,text="Symmetry")
        a1=tk.Label(self.master,text="0=none,1=symmetrize on order")
        a2=tk.Label(self.master,text="2=no order specificity")
        e7=tk.Entry(self.master)

        r1=tk.Radiobutton(self.master, text="Frequency Scan", variable=v, value=1,command=typeChanged)
        r2=tk.Radiobutton(self.master, text="Rabi Flop", variable=v, value=2,command=typeChanged)
        l8=tk.Label(self.master,text="High Frequency")
        e8=tk.Entry(self.master)
        l9=tk.Label(self.master,text="Low Frequency")
        e9=tk.Entry(self.master)
        l10=tk.Label(self.master,text="Frequency Step")
        e10=tk.Entry(self.master)
        l11=tk.Label(self.master,text="Pulse Duration")
        e11=tk.Entry(self.master)
        #Pack elements of GUI
        b1.pack()
        b2.pack()
        l1.pack()
        e1.pack()
        l2.pack()
        e2.pack()
        l3.pack()
        e3.pack()
        l4.pack()
        e4.pack()
        l5.pack()
        e5.pack()
        l6.pack()
        e6.pack()
        l7.pack()
        a1.pack()
        a2.pack()
        e7.pack()
        r1.pack()
        r2.pack()
        l7.pack()
        e7.pack()
        l8.pack()
        e8.pack()
        l9.pack()
        e9.pack()
        l10.pack()
        e10.pack()
        l11.pack()
        e11.pack()

if __name__ == '__main__':

#Deprecated
#command line arguments 

    #parser = argparse.ArgumentParser(description='Freq Scan Prgrm')
    #parser.add_argument('ionpos', help='ionpos filename. Must have extra ionpos at dark position at end of file for background')
    #parser.add_argument('out', help='output filename')
    #parser.add_argument('nruns', help='number of runs to do')
    #parser.add_argument('nions', type=int, help='number of ions, bright or dark, in chain')
    #parser.add_argument('ndark', type=int, help='number of dark ions in chain')
    #parser.add_argument('-order', dest='order', help='order of ions, 1 = bright 0 = dark. Must have correct # of bright/dark regardless of whether using order')
    #parser.add_argument('-symmetry', action='store_const', const='True', help='0 = no sym, use order 1 = symmetrize on order 2 = no order specificity')
    #parser.add_argument('-freqscan', nargs = 4, help='Run Freqscan. Params: High Freq, Low Freq, freqstep, pulse_duration')
    #parser.add_argument('-rabiflop', nargs = 4, help='Run RabiFlop. Params: start time, stop time, timestep, frequency')
    
    #arguments = parser.parse_args()

    #PARENT_CONN, CHILD_CONN = Pipe()

    #Add different option for experiment
    #PROCESS = Process( target=experiment, args=(CHILD_CONN) )
    #PROCESS.start()
    #

    #Pause button command
    #PROCESS2=Process(target=togglePause,args=(PARENT_CONN,arguments))
    #PROCESS2.start()
    #
    App()
    #App( PARENT_CONN )

    #PROCESS.join()


