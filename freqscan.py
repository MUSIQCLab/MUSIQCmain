from __future__ import print_function
import time

from luca import Luca
from freq import FreqDriver
from ni_digital_io import NiDriver
from ni_simple_digital_io import NiDriver as NiSimpleDriver
from experiment_base import *

import numpy as np


class Experiment:
    PockelsChan = "PXI1Slot9/port0/line2"
    RedChan = "PXI1Slot9/port0/line7"
    BlueChan = "PXI1Slot9/port0/line6"
    OrangeChan = "PXI1Slot9/port0/line5"
    ShelveChan = "PXI1Slot9/port0/line1"
    Chans = ",".join( [PockelsChan, RedChan,
                       BlueChan, OrangeChan, ShelveChan] )

    def __init__(self):
        for i in range(15):
            print("CHECK WHETHER THE TRAPCONTROL VI OR ANDOR ARE ON/OPEN!!!!!!!")
        self.background = []
        self.brightness = []

    def run(self, nruns, experiment, ions, desired_order, out, conn=None):
        camera = Luca()
        freq_src = FreqDriver(u'USB0::0x1AB1::0x0641::DG4B142100247::INSTR')
        ni = NiDriver(self.Chans)
        reorder_time = 0.05
        reorder_free_record = []
        number_between_reorder = 0

        output = open(out, 'w')
        desired_bright_number = sum(map(lambda x: 1 if x else 0, desired_order))
        try:
            ion_positions = self.get_ion_positions(ions)

            data = self.initialize_bright_levels(camera, desired_bright_number, ion_positions)
            threshold = self.calculate_threshold(np.mean(self.brightness), np.mean(self.background))

            if np.mean(self.brightness) < 8000:
                raise RuntimeError("Threshold too close to bright values. Increase ion brightness or exposure time.")

            experiment.setup(freq_src, ni)
            print("brightest:", np.max(data), "bg:", np.mean(self.background), "threshold:", threshold)


            while experiment.step( freq_src, ni ):
                for run in range( nruns ):
                    print(run)
                    # Pre-control waveform image for ion position verification:
                    data = self.build_data(camera, ion_positions, camera.get_image())
                    ion_order = [d > threshold for d in data]

                    # update bright and bg averages to inform threshold
                    self.background.append(data[-1])
                    for x in data:
                        if x > threshold:
                            self.brightness.append(x)
                    if len(self.brightness) > 20:
                        self.brightness = self.brightness[15:]
                    if len(self.background) > 20:
                        self.background = self.background[15:]
                    threshold = self.calculate_threshold(np.mean(self.brightness), np.mean(self.background))
                    if run % 10 == 0:
                        print("bg mean:", np.mean(self.background), "std:", np.std(self.background), "Threshhold:", threshold, "Min Brightest:",
                              threshold + np.std(self.brightness))
                    if run % 20 == 0:
                        print(reorder_free_record)
                    if ion_order == desired_order:
                        number_between_reorder += 1
                    while ion_order != desired_order:
                        if number_between_reorder != 0:
                            reorder_free_record.append(number_between_reorder)
                        number_between_reorder = 0
                        if len(reorder_free_record) > 30:
                            reorder_free_record = reorder_free_record[10:]
                        curr_bright_number = sum(map(lambda x: 1 if x else 0, ion_order))
                        print("number of bright ions:")
                        print(curr_bright_number)
                        if curr_bright_number == desired_bright_number:
                            self.red_reorder(reorder_time)
                        time.sleep(1.5)

                        #camera.get_image()  # Inconsistent results on whether this is necessary. OH BUT IT'S ONLY FOR REORDER!!!
                        data = self.build_data(camera, ion_positions, camera.get_image())
                        for x in data:
                            if x > threshold:
                                self.brightness.append(x)
                        if len(self.brightness) > 20:
                            self.brightness = self.brightness[15:]


                        dim_iterations = 0  #how many times have we been dim?
                        # while np.max(data) < threshold + np.std(brights):
                        # code block to check whether ion is very dim.
                        while np.mean(self.brightness) < np.mean(self.background) + 3 * np.std(self.background) or np.mean(
                                self.brightness) < threshold + 0.7 * np.std(self.brightness):
                            dim_iterations += 1
                            print("ions are very dim. Possibly uncool or melted. Bright values:")
                            print(str(experiment.control_var()))
                            data = self.build_data(
                                camera, ion_positions, camera.get_image())
                            print(data)
                            sorted = np.array(data)
                            sorted.sort()
                            if np.max(data) > 3 * np.std(self.background):
                                break
                            self.ensure_orange_on()
                            time.sleep(1.5)
                            print(sorted[0:desired_bright_number])
                            if dim_iterations % 10 == 0:
                                threshold = self.calculate_threshold(np.mean(self.brightness), np.mean(self.background))
                                print("bg mean:", np.mean(self.background), )
                                print("bg std :", np.std(self.background))
                                print("Threshhold:", threshold)
                                print("bright std:", np.std(self.brightness))
                                print("Min Brightest:", threshold + np.std(self.brightness))

                        prev_order = ion_order
                        ion_order = [ d > threshold for d in data ]
                        if ion_order == desired_order:
                            time.sleep(2.5)
                        print( "{} -> {}".format( prev_order, ion_order ) )
                        if any( prev_order ):
                            if prev_order == ion_order:
                                reorder_time *= 1.2
                            else:
                                reorder_time *= 0.83333
                            print("New reorder time: {}".format(reorder_time))
                        print(np.max(data))
                        if reorder_time < 0.04:  reorder_time = 0.04
                        if reorder_time > 10.0: reorder_time = 10.0
                        if not any(ion_order):
                            print("all dim?")
                            self.set_lasers_to_cool()
                            time.sleep(3)

                        if conn is not None:
                            outdata = [str(experiment.control_var())]
                            outdata.extend(str(d) for d in data)
                            outdata.extend(str(d) for d in data)
                            conn.send('reordata ' + '\t'.join(outdata))

                    ni.run()
                    postdata = self.build_data(camera, ion_positions, camera.get_image())
                    data.extend(postdata)
                    data = [round(datum) for datum in data]
                    outdata = [str(experiment.control_var())]
                    outdata.extend(str(d) for d in data)
                    print('\t'.join(outdata))
                    if conn is not None:
                        conn.send('\t'.join(outdata))
                    output.write('\t'.join(outdata) + '\n')
                    output.flush()

                    self.ensure_orange_on()
                    time.sleep(0.3)

        finally:
            camera.shutdown()

    def initialize_bright_levels(self, camera, desired_bright_number, ion_positions):
        for i in range(15):
            print(i)
            data = self.build_data(camera, ion_positions, camera.get_image())
            self.background.append(data[-1])
        data.sort()
        data.reverse()
        self.brightness = (data[0:desired_bright_number])
        self.background = [b - np.mean(self.background) for b in self.background]
        self.brightness = [br - np.mean(self.background) for br in self.brightness]
        return data

    def set_lasers_to_cool(self):
        self.ensure_orange_on()
        d = NiSimpleDriver(self.RedChan)
        d.write_single(True)
        d.close()
        d = NiSimpleDriver(self.PockelsChan)
        d.write_single(False)
        d.close()
        d = NiSimpleDriver(self.BlueChan)
        d.write_single(True)
        d.close()

    def get_ion_positions(self, ions):
        ion_positions = []
        with open(ions, 'r') as ionfile:
            for l in ionfile:
                pos = tuple(int(x) for x in l.split())
                ion_positions.append(pos)
        print("ion_positions:")
        print(ion_positions)
        return ion_positions

    def ensure_orange_on(self):
        d = NiSimpleDriver(self.OrangeChan)
        d.write_single(True)
        d.close()

    def red_reorder(self, reorder_time):
        r = NiSimpleDriver(self.RedChan)
        r.write_single(False)
        time.sleep(reorder_time)
        r.write_single(True)
        r.close()

    def build_data(self, camera, ionpos, image):
        data = []
        sum_dist = 10
        for p in ionpos:
            val = 0
            for ox in range(-sum_dist, sum_dist):
                for oy in range(-sum_dist, sum_dist):
                    val += image[(p[1] + oy) * camera.width + p[0] + ox]
            data.append(val)
        return data - np.mean(self.background)

    def calculate_threshold(self, bright, bg):
        # return 5000
        # # I'm starting to realize that the dynamic threshold doesn't really work very well.
        # # I'm going to start using a static one...
        # # return (bright * 0.4 + bg * 0.6)

        return (bright * 0.5 + bg * 0.5)
      # return 8000


#
#
# if __name__ == '__main__':
#     import sys
#     if len(sys.argv) < 4:
#         print "usage: {0} <nruns> <high freq (MHz)> <low freq (MHz)> \
#             <freq step(MHz)> <1762 pulse length (us)> <ion_positions> \
#             <output>".format(sys.argv[0])
#         sys.exit(0)
#     nruns = int(sys.argv[1])
#     current_freq = float(sys.argv[2])
#     stop_freq = float(sys.argv[3])
#     freq_step = float(sys.argv[4])
#     pulse_len = int(sys.argv[5])
#     ions = sys.argv[6]
#     output = sys.argv[7]
#
#     Experiment(nruns, current_freq, stop_freq, freq_step, pulse_len,
#                ions, output, None)
