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

    def __init__(self, nruns, experiment, ions, desired_order, out, conn=None):
        # 0 = no sym, use order 1 = symmetrize on order 2 = no order specificity
        camera = Luca()
        freq_src = FreqDriver(u'USB0::0x1AB1::0x0641::DG4B142100247::INSTR')
        ni = NiDriver(self.Chans)
        reorder_time = 0.5

        output = open(out, 'w')
        desired_bright_number = sum(map(lambda x: 1 if x else 0, desired_order))

        try:
            ion_positions = []
            with open(ions, 'r') as ionfile:
                for l in ionfile:
                    pos = tuple(int(x) for x in l.split())
                    ion_positions.append(pos)

            bg = []
            bright = []
            for i in range(30):
                data = self.build_data(camera, ion_positions, camera.get_image())
                bg.append(data[-1])
                bright.append(min(map(lambda x: data[x] if x else 0, desired_order)))
            threshold = np.mean(bg) + 3 * np.std(bg)

            experiment.setup(freq_src, ni)
            print(ion_positions)
            while experiment.step(freq_src, ni):
                for i in range(nruns):
                    data = self.build_data(
                        camera, ion_positions, camera.get_image())
                    ion_order = [d > threshold for d in data]

                    bg.append(data[-1])
                    if len(bg) > 1000:
                        bg = bg[100:]

                    if len(bright) > 500:
                        bright = bright[100:]

                    threshold = np.mean(bg) + 3 * np.std(bg)
                    conservative_brightness = np.mean(bright) - np.std(bright)

                    while not conservative_brightness > threshold:
                        camera.get_image()
                        data = self.build_data(
                            camera, ion_positions, camera.get_image())
                        bright.append(min(map(lambda x: data[x] if x else 0, desired_order)))
                        if len(bright) > 500:
                            bright = bright[100:]
                        conservative_brightness = np.mean(bright) - np.std(bright)
                        print("Ions too dim. Adjust lasers or trap parameters.")

                    while ion_order != desired_order:
                        curr_bright_number = sum(map(lambda x: 1 if x else 0, ion_order))
                        if curr_bright_number == desired_bright_number:
                            r = NiSimpleDriver(self.RedChan)
                            r.write_single(False)
                            time.sleep(reorder_time)
                            r.write_single(True)
                            r.close()
                        time.sleep(0.5)

                        old_ion_order = ion_order
                        while curr_bright_number != desired_bright_number:
                            camera.get_image()
                            data = self.build_data(
                                camera, ion_positions, camera.get_image())

                            prev_order, ion_order = ion_order, \
                                [d > threshold for d in data]
                            curr_bright_number = sum(map(lambda x: 1 if x else 0, ion_order))
                            print("Incorrect number of bright ions before experiment run.")

                        print("{} -> {}".format(old_ion_order, ion_order))

                        if curr_bright_number == desired_bright_number:
                            if old_ion_order == ion_order:
                                reorder_time *= 1.1
                            else:
                                reorder_time *= 0.9
                            print("New reorder time: {}".format(reorder_time))
                        if reorder_time < 0.1:  reorder_time = 0.1
                        if reorder_time > 10.0: reorder_time = 10.0

                        if conn is not None:
                            outdata = [str(experiment.control_var())]
                            outdata.extend(str(d) for d in data)
                            outdata.extend(str(d) for d in data)
                            conn.send('reordata ' + '\t'.join(outdata))

                    bright.append(min(map(lambda x: data[x] if x else 0, desired_order)))

                    ni.run()
                    postdata = self.build_data(
                        camera, ion_positions, camera.get_image())
                    data.extend(postdata)

                    outdata = [str(experiment.control_var())]
                    outdata.extend(str(d) for d in data)
                    print '\t'.join(outdata)
                    if conn is not None:
                        conn.send('\t'.join(outdata))
                    output.write('\t'.join(outdata) + '\n')
                    output.flush()

                    d = NiSimpleDriver(self.OrangeChan)
                    d.write_single(True)
                    d.close()

                    time.sleep(0.2)

        finally:
            camera.shutdown()

    def build_data(self, camera, ionpos, image):
        data = []
        sum_dist = 20
        for p in ionpos:
            val = 0
            for ox in range(-sum_dist, sum_dist):
                for oy in range(-sum_dist, sum_dist):
                    val += image[(p[1] + oy) * camera.width + p[0] + ox]
            data.append(val)
        return data

    def binstdrderr(self, n, p, z=1.0):
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
