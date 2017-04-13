
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

    def __init__( self, nruns, experiment, ions, desired_order, out, conn=None ):
        camera = Luca()
        freq_src = FreqDriver( u'USB0::0x1AB1::0x0641::DG4B142100247::INSTR' )
        ni = NiDriver( self.Chans )
        reorder_time = 0.2

        output = open( out, 'w' )
        desired_bright_number = sum(map(lambda x: 1 if x else 0, desired_order))

        try:
            ion_positions = []
            with open( ions, 'r' ) as ionfile:
                for l in ionfile:
                    pos = tuple( int(x) for x in l.split() )
                    ion_positions.append( pos )
            print("ion_positions:")
            print(ion_positions)
            bg = []
            brights = []
            for i in range(30):
                print(i)
                data = self.build_data(camera, ion_positions, camera.get_image())
                bg.append( data[-1] )
                bright = (x if x > (np.max(data) + np.mean(bg)) / 2 for x in data)
                brights.append(bright)
            crosstalk = np.std(brights) * np.sqrt(0.16)  # estimate 16% crosstalk. Could measure programatically.
            threshold = np.mean(bg) + 3 * np.std(bg) + 2 * crosstalk

            if threshold > np.mean(bright) - 3 * np.std(bright):
                raise RuntimeError("Threshold too close to bright values. Increase ion brightness or exposure time.")

            experiment.setup( freq_src, ni )
            print("bg, threshold:")
            print(bg, threshold)
            while experiment.step( freq_src, ni ):
                for i in range( nruns ):
                    data = self.build_data(
                        camera, ion_positions, camera.get_image() )
                    ion_order = [ d > threshold for d in data ]

                    bg.append( data[-1] )
                    if len( bg ) > 200:
                        bg = bg[100:]
                    threshold = np.mean(bg) + 3*np.std(bg)
                    print("bg mean:", np.mean(bg), "std:", np.std(bg), "Threshhold:", threshold, "Min Brightest:",
                          threshold + 3000)

                    while ion_order != desired_order:
                        curr_bright_number = sum(map(lambda x: 1 if x else 0, ion_order))
                        if curr_bright_number == desired_bright_number:

                            r = NiSimpleDriver( self.RedChan )
                            r.write_single( False )
                            time.sleep( reorder_time )
                            r.write_single( True )
                            r.close()
                        time.sleep( 0.3 )

                        #camera.get_image() # Inconsistent results on whether this is necessary.
                        data = self.build_data(
                            camera, ion_positions, camera.get_image() )
                        while np.max(data) < threshold + 3000:
                            print("ions are too dim. Brightest:")
                            data = self.build_data(
                                camera, ion_positions, camera.get_image() )
                            print(np.max(data))
                            print("bg mean:", np.mean(bg), "std:", np.std(bg), "Threshhold:", threshold, "Min Brightest:",
                          threshold + 3000)
                    

                        prev_order, ion_order = ion_order, \
                            [ d > threshold for d in data ]
                        print( "{} -> {}".format( prev_order, ion_order ) )

                        if any( prev_order ):
                            if prev_order == ion_order:
                                reorder_time *= 1.1
                            else:
                                reorder_time *= 0.9
                            print( "New reorder time: {}".format( reorder_time ) )
                            print("bg mean:", np.mean(bg), "std:", np.std(bg), "Threshhold:", threshold, "Min Brightest:",
                          threshold + 3000)
                        if reorder_time < 0.1:  reorder_time = 0.1
                        if reorder_time > 10.0: reorder_time = 10.0

                        if conn is not None:
                            outdata = [str( experiment.control_var() )]
                            outdata.extend( str(d) for d in data )
                            outdata.extend( str(d) for d in data )
                            conn.send( 'reordata ' + '\t'.join(outdata) )

                    ni.run()
                    postdata = self.build_data(
                        camera, ion_positions, camera.get_image())
##                    post_ion_order = [d > threshold for d in postdata]
##                    ion_number = np.sum(ion_order)
##                    wrong_brights = [1 if desired == 0 and post == 1
##                                         else 0 for desired, post in
##                                         zip(desired_order, post_ion_order)]
##                    if np.any(wrong_brights) or  
                    data.extend(postdata)

                    outdata = [str( experiment.control_var() )]
                    outdata.extend( str(d) for d in data )
                    print '\t'.join(outdata)
                    if conn is not None:
                        conn.send('\t'.join(outdata) )
                    output.write( '\t'.join(outdata) + '\n' )
                    output.flush()

                    d = NiSimpleDriver( self.OrangeChan )
                    d.write_single( True )
                    d.close()

                    time.sleep( 0.2 )


        finally:
            camera.shutdown()


    def build_data( self, camera, ionpos, image ):
        data = []
        sum_dist = 10
        for p in ionpos:
            val = 0
            for ox in range( -sum_dist, sum_dist ):
                for oy in range( -sum_dist, sum_dist ):
                    val += image[ (p[1] + oy) * camera.width + p[0] + ox ]
            data.append( val )
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
            <output>".format( sys.argv[0] )
        sys.exit( 0 )
    nruns = int( sys.argv[1] )
    current_freq = float( sys.argv[2] )
    stop_freq = float( sys.argv[3] )
    freq_step = float( sys.argv[4] )
    pulse_len = int( sys.argv[5] )
    ions = sys.argv[6]
    output = sys.argv[7]

    Experiment( nruns, current_freq, stop_freq, freq_step, pulse_len,
        ions, output, None )
