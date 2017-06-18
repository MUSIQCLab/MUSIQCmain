class ExperimentBase:
    POCKEL_TIME = 300
    RIGOL_VOLTAGE = 2.5

    Pockels = 1 << 2
    Red = 1 << 7
    Blue = 1 << 6
    Orange = 1 << 5
    Shelve = 1 << 1

    def build_waveform(self, dark_time, shelve_time):
        waveform = [self.Pockels | self.Red | self.Blue] * self.POCKEL_TIME
        waveform.extend([self.Pockels | self.Red] * 10)
        waveform.extend([self.Pockels] * 10)

        waveform.extend([self.Orange] * dark_time)
        waveform.extend([0] * 10)

        waveform.extend([self.Shelve] * shelve_time)
        waveform.extend([0] * 10)
        waveform.extend([self.Red | self.Blue] * 10)
        return waveform


class RabiFlop(ExperimentBase):
    def __init__(self, start_time, stop_time, time_step, frequency):
        self.current_time = start_time
        self.stop_time = stop_time
        self.time_step = time_step
        self.frequency = frequency

    def setup(self, freq_driver, ni_driver):
        self.freq_driver = freq_driver
        self.ni_driver = ni_driver
        self.freq_driver.set_frequency(self.frequency, self.RIGOL_VOLTAGE)

    def step(self, freq_driver, ni_driver):
        if self.current_time > self.stop_time:
            return False
        self.ni_driver.set_samples(self.build_waveform(0, self.current_time))
        self.current_time += self.time_step
        return True

    def control_var(self):
        return self.current_time - self.time_step


class FreqScan(ExperimentBase):
    def __init__(self, start_freq, stop_freq, freq_step, pulse_duration):
        self.current_freq = start_freq
        self.stop_freq = stop_freq
        self.freq_step = freq_step
        self.pulse_duration = pulse_duration

    def setup(self, freq_driver, ni_driver):
        self.freq_driver = freq_driver
        self.ni_driver = ni_driver
        self.ni_driver.set_samples(self.build_waveform(0, self.pulse_duration))

    def step(self, freq_driver, ni_driver):
        if self.current_freq > self.stop_freq:
            return False
        self.freq_driver.set_frequency(self.current_freq, self.RIGOL_VOLTAGE)
        self.current_freq += self.freq_step
        return True

    def control_var(self):
        return self.current_freq - self.freq_step
