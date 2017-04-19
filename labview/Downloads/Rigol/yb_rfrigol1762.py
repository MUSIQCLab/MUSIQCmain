#=============================================================================
# Adiabatic Passage sweeps
# writeen for sequencer_2 api on the Yellow Box pulse programmer
# Aaron Avril 6/8/2010
#=============================================================================

import time, logging, sys

# Program accepts 
#	Shelving Frequency (MHz)
#	AP Frequency Width (MHz)
#	Frequency Step (MHz)
#	Pulse Duration (usec)
#	Power Level (Units -12 Maximum)
#	Optical Pumping Time (usec)
#	RF Expose Time (usec)
if len(sys.argv) < 5:
	print ( 	"Usage: %s shelve_frequency(MHz) AP_frequency_width(MHz) \
				frequency_step(MHz) pulse_duration(usec) power_level(units -12 max) \
				op_time(usec) rf_time(usec)" 
				% sys.argv[0] 	)
	sys.exit(-1)
	
rf_frequency, rf_expose, shelve_pulse_duration, power_level, op_time = [ float( sys.argv[i+1] ) for i in range( 5 ) ]
sys.argv = sys.argv[:1]

if '/home/RA/sf/hgroot/sequencer2' not in sys.path:
	# Running from windows append cygwin path
	sys.path.append( 'C:\\cygwin\\home\\RA\\sf\\hgroot\\sequencer2' )
	
# Using bank 2 for optical pumping control
GREEN_CTL = 0x02
PUMP_CTL = 0x08
RED_CTL = 0x10
RF_CTL = 0x20
SHELVE_CTL = 0x40
LABVIEW_1762 = 0x80
IR_CTL = 0x200
SWEEP = 0x04 #Same as BURST

from sequencer2 import sequencer
from sequencer2 import api
from sequencer2 import comm
from sequencer2 import ad9910
from sequencer2 import ptplog

# Set to true for testing without actual hardware
nonet = False

#Init sequencer, API and DDS
my_sequencer=sequencer.sequencer()
my_api=api.api(my_sequencer)
dds_device0 = ad9910.AD9910(0, 800) #args: device_addr, ref_freq, clk_divider=8

my_api.init_dds(dds_device0)
my_api.wait(1.0)

my_api.dac_value(-12, 0) # -12 is ~the largest signal the DDS can output
my_api.set_dds_freq(dds_device0, .5, 0)

my_api.wait(1.0)
my_api.update_dds(dds_device0)

#the following would set up the dds to be able to output a fixed frequency with a settable initial phase
my_api.init_dds(dds_device0)
my_api.wait(1.0)
my_api.dds_to_serial(2**13, 32, 0x00, 0)
my_api.wait(10.0)
my_api.update_dds(dds_device0)
my_api.dds_to_serial(0, 32, 0x01, 0)
my_api.wait(10.0)
my_api.update_dds(dds_device0)
my_api.dac_value(-12, 0) # -12 is ~the largest signal the DDS can output
my_api.set_dds_freq(dds_device0, rf_frequency, 0, 0)
my_api.wait(1.0)
my_api.update_dds(dds_device0)
my_api.dds_to_serial(2**15, 16, 0x08, 0)
my_api.wait(10.0)
my_api.update_dds(dds_device0)
#my_api.init_digital_ramp_generator('freq', dds_device0, .1, .1, .1, .1, 10, 100)
#my_api.wait(20.0)
#my_api.configure_ramping(dds_device0, 1) # increasing slope
my_api.wait(20.0)
#=============================================================================
# Program Start (After boilerplate initialization (including DDS init)
#=============================================================================

my_api.ttl_value(0x04 | 0x08, 3)

# note: no DAC for now

my_api.ttl_value(RED_CTL | GREEN_CTL | IR_CTL, 2)

# Now ramp is initialized.  Create loop point, wait for LabView to say "GO!"
my_api.label("listen_start")
my_api.jump_trigger("start_experiment", 0x40)
my_api.jump("listen_start")

my_api.label("start_experiment")
my_api.wait(1.0)

#Turn on cooling lasers while initializing things, because. . . why not?
my_api.ttl_value(RED_CTL | GREEN_CTL | IR_CTL, 2)
my_api.wait(10.0)

#do optical pumping
my_api.ttl_value(RED_CTL | PUMP_CTL | IR_CTL, 2)
my_api.wait(op_time)

#Turn off pump switch and wait a us to be sure that the signal gets to the pumping aom before you turn off red
my_api.ttl_value(RED_CTL, 2)
my_api.wait(1.0)

#Turn everything off
my_api.ttl_value(0x00, 2)
my_api.wait(1.0)

#apply rf
my_api.ttl_value(RF_CTL, 2)
my_api.wait(rf_expose)
my_api.ttl_value(0x00, 2)
my_api.wait(1.0)

#shelve by pi pulse
my_api.ttl_value(SWEEP, 2) # Signal to the RIGOL to sweep
my_api.wait(1)
my_api.ttl_value(SHELVE_CTL | LABVIEW_1762, 2) # Turn on 1762 SPAOM RF switch and signal to LabView that shelving is happening
my_api.wait(shelve_pulse_duration)

#Turn off 1762	
my_api.ttl_value(LABVIEW_1762, 2) 

my_api.wait(20.0)
#turn on cooling lasers for detection
my_api.ttl_value(RED_CTL | GREEN_CTL | IR_CTL, 2)

my_api.label("deshelve_start")
my_api.jump_trigger("listen_start", 0x40, invert = True)
my_api.jump_trigger("deshelve", 0x20)
my_api.jump("deshelve_start")




my_api.label("deshelve")

my_api.ttl_value(RED_CTL | GREEN_CTL | IR_CTL, 2)
my_api.wait(1.0)
my_api.ttl_value(SHELVE_CTL | LABVIEW_1762 | GREEN_CTL | IR_CTL, 2)
my_api.wait(10000)
#turn off 1762 first, wait to make sure, then turn on red
my_api.ttl_value(LABVIEW_1762 | GREEN_CTL | IR_CTL, 2) 
my_api.wait(5.0)
my_api.ttl_value(LABVIEW_1762 | GREEN_CTL | RED_CTL | IR_CTL, 2) 
my_api.wait(20.0)
#tell LabView that deshelve procedure is done by making LABVIEW_1762 go low
my_api.ttl_value(RED_CTL | GREEN_CTL | IR_CTL, 2)

my_api.label("deshelve_end_loop")
my_api.jump_trigger("deshelve_start", 0x20, invert = True)
my_api.jump("deshelve_end_loop")


#=============================================================================
# Program end: Insert Boiler plate compilation below
#=============================================================================

#Compile sequence:
my_sequencer.compile_sequence()
#Debug sequence
# my_sequencer.debug_sequence()
time2=time.time()
# logger.logger.info("compile time: "+str(time2-time1))
ptp1 = comm.PTPComm(nonet=nonet)
ptp1.send_code(my_sequencer.word_list)
# time3=time.time()
# logger.logger.info("ptp time: "+str(time3-time2))
