import time

from luca import Luca
from freq import FreqDriver
from ni_digital_io import NiDriver
from ni_simple_digital_io import NiDriver as NiSimpleDriver
from experiment_base import *
import numpy as np
from skimage.feature import peak_local_max
from scipy.ndimage.filters import gaussian_filter

class Experiment:
    PockelsChan = "PXI1Slot9/port0/line2"
    RedChan = "PXI1Slot9/port0/line7"
    BlueChan = "PXI1Slot9/port0/line6"
    OrangeChan = "PXI1Slot9/port0/line5"
    ShelveChan = "PXI1Slot9/port0/line1"
    Chans = ",".join( [PockelsChan, RedChan,
        BlueChan, OrangeChan, ShelveChan] )

    def __init__(self, nruns, experiment, ions, desired_order, out, conn=None):
        # 0 = no sym, use order 1 = symmetrize on order 2 = no order specificity
        camera = Luca()
        freq_src = FreqDriver(u'USB0::0x1AB1::0x0641::DG4B142100247::INSTR')
        ni = NiDriver(self.Chans)
        reorder_time = 0.05

        output = open(out, 'w')
        desired_bright_number = sum(map(lambda x: 1 if x else 0, desired_order))
        debug = open("debug.dat", 'w')
        try:
            ion_positions = []
            with open(ions, 'r') as ionfile:
                for l in ionfile:
                    pos = tuple( int(x) for x in l.split() )
                    ion_positions.append( pos )
            print("ion_positions:")
            print(ion_positions)
            bg = []
            brights = []
            crosstalk = []
            for i in range(30):
                print(i)
                data = self.build_data(camera, ion_positions, camera.get_image())
                bg.append( data[-1] )
            data.sort()
            data.reverse()
            brights.extend(data[0:desired_bright_number])
            crosstalk.append(data[desired_bright_number])

            print("brightest:", np.max(data), "bg:", np.mean(bg), "x-talk", np.mean(crosstalk))

            threshold = (np.mean(brights) + np.mean(bg) - np.std(bg)) / 2.

            if threshold > np.mean(brights) - 2.5 * np.std(brights):
                raise RuntimeError("Threshold too close to bright values. Increase ion brightness or exposure time.")

            experiment.setup( freq_src, ni )
            print("bg, threshold:")
            print(np.mean(bg), threshold)
            i = 0  # counter to keep track of how often debugging information is displayed in console output
            while experiment.step( freq_src, ni ):
                ion_positions = self.find_shift(camera, ion_positions)
                print("new ion positions:", ion_positions)
                for i in range( nruns ):
                    i += 1
                    data = self.build_data(
                        camera, ion_positions, camera.get_image())
                    ion_order = [d > threshold for d in data]

                    bg.append(data[-1])
                    for x in data:
                        if x > threshold:
                            brights.append(x)
                    if len(brights) > 12:
                        brights = brights[10:]
                    if len(bg) > 20:
                        bg = bg[10:]
                    # threshold = (np.mean(crosstalk) + np.mean(brights))/2.
                    #                    threshold = np.mean(crosstalk) + 2.5 * np.std(brights)
                    threshold = (np.mean(brights) + np.mean(bg) - np.std(bg)) / 2.
                    # threshold = (np.mean(brights) * np.std(brights) + np.mean(bg) * np.std(bg)) / (
                    # np.std(bg) + np.std(brights))
                    if i % 10 == 0:
                        print("bg mean:", np.mean(bg), "std:", np.std(bg), "Threshhold:", threshold, "Min Brightest:",
                          threshold + np.std(brights))

                    while ion_order != desired_order:
                        curr_bright_number = sum(map(lambda x: 1 if x else 0, ion_order))
                        if curr_bright_number == desired_bright_number:
                            r = NiSimpleDriver(self.RedChan)
                            r.write_single(False)
                            time.sleep(reorder_time)
                            r.write_single(True)
                            r.close()
                        time.sleep(0.4)

                        camera.get_image()  # Inconsistent results on whether this is necessary. OH BUT IT'S ONLY FOR REORDER!!!
                        data = self.build_data(
                            camera, ion_positions, camera.get_image())
                        for x in data:
                            if x > threshold:
                                brights.append(x)
                        if len(brights) > 12:
                            brights = brights[10:]
                        dim_iterations = 0  #how many times have we been dim?
                        while np.max(data) < threshold + np.std(brights):
                            dim_iterations += 1
                            print("ions are too dim. Brights:")
                            data = self.build_data(
                                camera, ion_positions, camera.get_image())
                            sorted = np.array(data)
                            sorted.sort()

                            print(sorted[0:desired_bright_number])
                            if dim_iterations % 5 == 0:
                                threshold = (np.mean(brights) + np.mean(bg) - np.std(bg)) / 2.
                                print("bg mean:", np.mean(bg),)
                                print("bg std :", np.std(bg))
                                print("Threshhold:", threshold)
                                print("bright std:", np.std(brights))
                                print("Min Brightest:", threshold + np.std(brights))
                            time.sleep(1)

                        prev_order, ion_order = ion_order, \
                            [ d > threshold for d in data ]
                        print( "{} -> {}".format( prev_order, ion_order ) )

                        if any( prev_order ):
                            if prev_order == ion_order:
                                reorder_time *= 1.05
                            else:
                                reorder_time *= 0.95
                            print("New reorder time: {}".format(reorder_time))
                        print(np.max(data))
                        if reorder_time < 0.01:  reorder_time = 0.01
                        if reorder_time > 10.0: reorder_time = 10.0

                        if conn is not None:
                            outdata = [str(experiment.control_var())]
                            outdata.extend(str(d) for d in data)
                            outdata.extend(str(d) for d in data)
                            conn.send('reordata ' + '\t'.join(outdata))

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

                    time.sleep(0.4)
                time.sleep(1)


        finally:
            camera.shutdown()

    def build_data(self, camera, ionpos, image):
        data = []
        sum_dist = 10
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

    def find_shift(self, camera, ion_positions):
        normal_exposure = camera.exposure_time
        camera.set_exposure(5)

        raw = camera.get_image()
        left_border = ion_positions[0][0] - 40
        right_border = ion_positions[-2][0] + 40
        vert = ion_positions[0][1]
        filtered = gaussian_filter(raw[left_border - 15:right_border + 15, vert - 15, vert + 15], 3)
        peaks = peak_local_max(filtered, min_distance=25, threshold_rel=0.7)

        shifts = [peak[0] - position[0] if 1 < np.abs(peak[0] - position[0]) < 15 else 0
                  for peak in peaks for position in ion_positions]
        total_shift = np.sum(shifts)
        shift_sign = np.sign(total_shifts)
        if total_shift > 3 or total_shift < -3:
            ion_positions = [(position[0] + 2 * shift_sign, position[1]) for position in ion_positions]

        camera.set_exposure(normal_exposure)
        return ion_positions


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
