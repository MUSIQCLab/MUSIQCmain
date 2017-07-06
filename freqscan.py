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



    def run(self, nruns, experiment, ions, desired_order, out, conn=None):
        camera = Luca()
        freq_src = FreqDriver(u'USB0::0x1AB1::0x0641::DG4B142100247::INSTR')
        ni = NiDriver(self.Chans)
        reorder_time = 0.05
        reorder_free_record = []
        non_reorder_runs = 0

        output = open(out, 'w')
        desired_bright_number = sum(map(lambda x: 1 if x else 0, desired_order))
        try:
            ion_positions = self.get_ion_positions(ions)

            data, background, brightness = self.initialize_bright_levels(camera, desired_bright_number, ion_positions)

            if np.mean(brightness) < 8000:
                raise RuntimeError("Threshold too close to bright values. Increase ion brightness or exposure time.")

            experiment.setup(freq_src, ni)
            print("brightest:", np.max(data), "bg:", np.mean(background), "threshold:", self.calculate_threshold(brightness, background))


            while experiment.step( freq_src, ni ):
                for run in range( nruns ):
                    print(run)
                    # Pre-control waveform image for ion position verification:
                    data = self.build_data(camera, ion_positions, camera.get_image(), background)
                    ion_order = [d > threshold for d in data]

                    # update bright and bg averages to inform threshold
                    threshold = self.calculate_threshold(np.mean(brightness), np.mean(background))
                    brightness, background = self.update_brightness_background(data, threshold, brightness, background)

                    if run % 10 == 0:
                        print("bg mean:", np.mean(background), "std:", np.std(background), "Threshhold:", threshold, "Min Brightest:",
                              threshold + np.std(brightness))
                        print(reorder_free_record)

                    if ion_order == desired_order:
                        non_reorder_runs += 1

                    while ion_order != desired_order:

                        # Keep track of how long we go between reordering.
                        if non_reorder_runs != 0:
                            reorder_free_record.append(non_reorder_runs)
                        non_reorder_runs = 0
                        if len(reorder_free_record) > 30:
                            reorder_free_record = reorder_free_record[10:]

                        curr_bright_number = sum(map(lambda x: 1 if x else 0, ion_order))
                        print("number of bright ions:")
                        print(curr_bright_number)

                        if curr_bright_number == desired_bright_number: # Right number of ions? Use the red to force
                            self.red_reorder(reorder_time)              # reordering
                        time.sleep(1.5)

                        data = self.build_data(camera, ion_positions, camera.get_image(), background)
                        brightness, background = self.update_brightness_background(data, threshold, brightness,
                                                                                   background)

                        dim_iterations = 0  #how many times have we been dim?
                        # while np.max(data) < threshold + np.std(brights):
                        # code block to check whether ion is very dim.
                        data, threshold = self.check_if_ion_dim(background, brightness, camera, data,
                                                                desired_bright_number, experiment,
                                                                ion_positions, threshold)

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
                            out_data = [str(experiment.control_var())]
                            out_data.extend(str(d) for d in data)
                            out_data.extend(str(d) for d in data)
                            conn.send('reordata ' + '\t'.join(out_data))

                    ni.run()
                    post_data = self.build_data(camera, ion_positions, camera.get_image(), background)
                    data.extend(post_data)
                    data = [round(datum) for datum in data]
                    out_data = [str(experiment.control_var())]
                    out_data.extend(str(d) for d in data)
                    print('\t'.join(out_data))
                    if conn is not None:
                        conn.send('\t'.join(out_data))
                    output.write('\t'.join(out_data) + '\n')
                    output.flush()

                    self.ensure_orange_on()
                    time.sleep(0.3)

        finally:
            camera.shutdown()

    def check_if_ion_dim(self, background, brightness, camera, data, desired_bright_number, experiment,
                         ion_positions, threshold):
        dim_iterations = 0
        while np.max(data) < 5 * np.std(background):
            dim_iterations += 1
            print("ions are very dim. Possibly uncool or melted. Bright values:")
            print(str(experiment.control_var()))

            data = self.build_data(camera, ion_positions, camera.get_image(), background)
            print(data)
            sorted = np.array(data)
            sorted.sort()
            if np.max(data) > 5 * np.std(background):
                break
            self.ensure_orange_on()
            time.sleep(1.5)
            print(sorted[0:desired_bright_number])
            if dim_iterations % 10 == 0:
                threshold = self.calculate_threshold(np.mean(brightness), np.mean(background))
                print("bg mean:", np.mean(background), )
                print("bg std :", np.std(background))
                print("Threshhold:", threshold)
                print("bright std:", np.std(brightness))
                print("Min Brightest:", threshold + np.std(brightness))
        return data, threshold

    def update_brightness_background(self, data, threshold, brightness, background):
        background.append(data[-1])
        for x in data:
            if x > threshold:
                brightness.append(x)
        if len(brightness) > 20:
            brightness = brightness[15:]
        if len(background) > 20:
            background = background[15:]
        return brightness, background

    def initialize_bright_levels(self, camera, desired_bright_number, ion_positions):
        background = []
        for i in range(15):
            print(i)
            data = self.build_data(camera, ion_positions, camera.get_image(), np.array([0]))
            background.append(data[-1])
        data.sort()
        data.reverse()
        brightness = (data[0:desired_bright_number])
        background = [b - np.mean(background) for b in background]
        brightness = [br - np.mean(background) for br in brightness]
        return data, background, brightness

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

    def build_data(self, camera, ionpos, image, background):
        data = []
        sum_dist = 10
        for p in ionpos:
            val = 0
            for ox in range(-sum_dist, sum_dist):
                for oy in range(-sum_dist, sum_dist):
                    val += image[(p[1] + oy) * camera.width + p[0] + ox]
            data.append(val)
        return data - np.mean(background)

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
