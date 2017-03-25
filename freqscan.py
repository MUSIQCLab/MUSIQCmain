
import time

from luca import Luca
from freq import FreqDriver
from ni_digital_io import NiDriver
from ni_simple_digital_io import NiDriver as NiSimpleDriver
import numpy as np


class Experiment:
    PockelsChan = "PXI1Slot9/port0/line2"
    RedChan = "PXI1Slot9/port0/line7"
    BlueChan = "PXI1Slot9/port0/line6"
    OrangeChan = "PXI1Slot9/port0/line5"
    ShelveChan = "PXI1Slot9/port0/line1"
    Chans = ",".join([PockelsChan, RedChan,
                      BlueChan, OrangeChan, ShelveChan])

    def __init__(self, nruns, experiment, ion_position_file, desired_order, out, conn=None):
        self.AOM_freq_driver = FreqDriver(u'USB0::0x1AB1::0x0641::DG4B142100247::INSTR')
        self.ni_card_driver = NiDriver(self.Chans)
        self.control_variable = str(experiment.control_var())
        self.desired_order = desired_order
        self.desired_bright = sum(map(lambda x: 1 if x else 0, self.desired_order))
        self.reorder_time = 0.5

        self.ion_positions = self.get_positions(ion_position_file)
        self.camera = Luca()
        self.threshold, self.background = self.measure_background()
        self.conn = conn

        experiment.setup(self.AOM_freq_driver, self.ni_card_driver)
        self.run_experiment(experiment, nruns, out)

    def run_experiment(self, experiment, nruns, out):
        output = open(out, 'w')

        try:
            print("Ion positions before experiment:")
            print(self.ion_positions)

            while experiment.step(self.AOM_freq_driver, self.ni_card_driver):
                for i in range(nruns):
                    data = self.build_data()

                    background_value = data[-1]
                    self.update_background_and_threshold(background_value)

                    ion_order = [d > self.threshold for d in data]

                    data = self.order_and_find_brightness(data, ion_order)
                    # can get stuck in line above if ions are hot or you have lost an ion.

                    self.ni_card_driver.run()  # runs the pulse sequence

                    postdata = self.build_data()
                    data.extend(postdata)

                    outdata = self.send_data_to_GUI(data)

                    output.write('\t'.join(outdata) + '\n')
                    output.flush()

                    d = NiSimpleDriver(self.OrangeChan)
                    d.write_single(True)
                    d.close()

                    time.sleep(0.2)  #deshelve all ions


        finally:
            self.camera.shutdown()

    def send_data_to_GUI(self, data):
        outdata = [self.control_variable]
        outdata.extend(str(d) for d in data)
        print '\t'.join(outdata)
        if self.conn is not None:
            self.conn.send('\t'.join(outdata))
        return outdata

    def update_background_and_threshold(self, background_value):
        self.background.append(background_value)
        if len(self.background) > 1000:
            self.background = self.background[100:]
        self.threshold = np.mean(self.background) + 3 * np.std(self.background)

    def order_and_find_brightness(self, data, ion_order):
        while ion_order != self.desired_order:
            bright = sum(map(lambda x: 1 if x else 0, ion_order))
            if bright == self.desired_bright: #correct number of brights
                self.reorder_bright_ions()
            time.sleep(0.5)  # let the ions cool off after reordering.

            self.camera.get_image()  # why am I taking an image here? John thought necessary?
            data = self.build_data()

            prev_order, ion_order = ion_order, \
                                    [d > self.threshold for d in data]
            print("{} -> {}".format(prev_order, ion_order))

            self.adjust_reorder_time(ion_order, prev_order)

            if self.conn is not None:
                self.send_data_to_GUI(data)
        return data

    def adjust_reorder_time(self, ion_order, prev_order):
        if any(prev_order):
            if prev_order == ion_order:
                self.reorder_time *= 1.1
            else:
                self.reorder_time *= 0.9
            print("New reorder time: {}".format(self.reorder_time))
        if self.reorder_time < 0.1:  self.reorder_time = 0.1
        if self.reorder_time > 10.0: self.reorder_time = 10.0

    def measure_background(self):
        bg = []
        for i in range(30):
            data = self.build_data()
            bg.append(data[-1])
        threshold = np.mean(bg) + 3 * np.std(bg)
        return threshold, bg

    def get_positions(self, ion_position_file):
        ion_positions = []
        with open(ion_position_file, 'r') as ionfile:
            for l in ionfile:
                pos = tuple(int(x) for x in l.split())
                ion_positions.append(pos)
        return ion_positions

    def reorder_bright_ions(self):
            r = NiSimpleDriver(self.RedChan)
            r.write_single(False)
            time.sleep(self.reorder_time)
            r.write_single(True)
            r.close()

    def build_data(self):
        image = self.camera.get_image()
        data = []
        sum_dist = 20
        for p in self.ion_positions:
            val = 0
            for ox in range(-sum_dist, sum_dist):
                for oy in range(-sum_dist, sum_dist):
                    val += image[(p[1] + oy) * self.camera.width + p[0] + ox]
            data.append(val)
        return data

    def binStdrderr(self, n, p, z=1.0):
        # finds standard error for a binomial distribution
        # n = # of runs, p = proportion of successes
        se = z * np.sqrt((1 / (n * 1.0)) * p * (1 - p) + (1 / (4.0 * n * n)) * z * z)
        return se


if __name__ == '__main__':
    import sys

    if len(sys.argv) < 4:
        print "usage: {0} <nruns> <high freq (MHz)> <low freq (MHz)> \
            <freq step(MHz)> <1762 pulse length (us)> <ion_positions> \
            <output>".format(sys.argv[0])
        sys.exit(0)
    nruns = int(sys.argv[1])
    current_freq = float(sys.argv[2])
    stop_freq = float(sys.argv[3])
    freq_step = float(sys.argv[4])
    pulse_len = int(sys.argv[5])
    ions = sys.argv[6]
    output = sys.argv[7]

    Experiment(nruns, current_freq, stop_freq, freq_step, pulse_len,
               ions, output, None)
