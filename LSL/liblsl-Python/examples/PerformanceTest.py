import time
import random
import numpy as np
from pylsl import StreamInfo, StreamInlet, StreamOutlet, local_clock, resolve_byprop,\
    resolve_bypred, proc_clocksync, proc_dejitter, proc_monotonize
try:
    from pyfftw.interfaces.numpy_fft import rfft, irfft       # Performs much better than numpy's fftpack
except ImportError:
    from numpy.fft import rfft, irfft
try:
    import pyqtgraph as pg
    import sys
    haspyqtgraph = True
except ImportError:
    haspyqtgraph = False

# The code for pink noise generation is taken from
# https://github.com/python-acoustics/python-acoustics/blob/master/acoustics/generator.py
# which is distributed under the BSD license.
def ms(x):
    """Mean value of signal `x` squared.
    :param x: Dynamic quantity.
    :returns: Mean squared of `x`.
    """
    return (np.abs(x) ** 2.0).mean()

def normalize(y, x=None):
    """normalize power in y to a (standard normal) white noise signal.
    Optionally normalize to power in signal `x`.
    #The mean power of a Gaussian with :math:`\\mu=0` and :math:`\\sigma=1` is 1.
    """
    #return y * np.sqrt( (np.abs(x)**2.0).mean() / (np.abs(y)**2.0).mean() )
    if x is not None:
        x = ms(x)
    else:
        x = 1.0
    return y * np.sqrt( x / ms(y) )
    #return y * np.sqrt( 1.0 / (np.abs(y)**2.0).mean() )

def pink(N):
    """
    Pink noise.

    :param N: Amount of samples.

    Pink noise has equal power in bands that are proportionally wide.
    Power density decreases with 3 dB per octave.

    """
    # This method uses the filter with the following coefficients.
    # b = np.array([0.049922035, -0.095993537, 0.050612699, -0.004408786])
    # a = np.array([1, -2.494956002, 2.017265875, -0.522189400])
    # return lfilter(B, A, np.random.randn(N))
    # Another way would be using the FFT
    # x = np.random.randn(N)
    # X = rfft(x) / N
    uneven = N % 2
    X = np.random.randn(N // 2 + 1 + uneven) + 1j * np.random.randn(N // 2 + 1 + uneven)
    S = np.sqrt(np.arange(len(X)) + 1.)  # +1 to avoid divide by zero
    y = (irfft(X / S)).real
    if uneven:
        y = y[:-1]
    return normalize(y)


class PinkNoiseGenerator(object):
    def __init__(self, nSampsPerBlock=1024):
        self.N = nSampsPerBlock
        self.uneven = self.N % 2
        lenX = self.N//2 + 1 + self.uneven
        self.S = np.sqrt(np.arange(lenX) + 1.)

    def generate(self):
        X = np.random.randn(self.N // 2 + 1 + self.uneven) + 1j * np.random.randn(self.N // 2 + 1 + self.uneven)
        y = (irfft(X / self.S)).real
        if self.uneven:
            y = y[:-1]
        return normalize(y)


class BetaGeneratorOutlet(object):
    def __init__(self, Fs=2**14, FreqBeta=20.0, AmpBeta=100.0, AmpNoise=20.0, NCyclesPerChunk=4,
                 channels=["RAW1", "SPK1", "RAW2", "SPK2", "RAW3", "SPK3"]):
        """
        :param Fs:              Sampling rate
        :param FreqBeta:        Central frequency of beta band
        :param AmpBeta:         Amplitude of beta (uV)
        :param AmpNoise:        Amplitude of pink noise (uV)
        :param NCyclesPerChunk: Minimum number of cycles of beta in a chunk.
        :param channels:        List of channel names
        """
        # Saved arguments
        self.FreqBeta = FreqBeta
        self.AmpBeta = AmpBeta                                          # Amplitude of Beta (uV)
        self.AmpNoise = AmpNoise                                        # Amplitude of pink noise
        self.channels = channels
        # Derived variables
        chunk_dur = NCyclesPerChunk / self.FreqBeta           # Duration, in sec, of one chunk
        chunk_len = int(Fs * chunk_dur)                  # Number of samples in a chunk
        self.tvec = 1.0 * (np.arange(chunk_len) + 1) / Fs     # time vector for chunk (sec)
        # Pink noise generator
        self.pinkNoiseGen = PinkNoiseGenerator(nSampsPerBlock=chunk_len)

        # Create a stream of fake 'raw' data
        raw_info = StreamInfo(name='BetaGen', type='EEG',
                           channel_count=len(self.channels), nominal_srate=Fs,
                           channel_format='float32', source_id='betagen1234')
        raw_xml = raw_info.desc()
        chans = raw_xml.append_child("channels")
        for channame in self.channels:
            chn = chans.append_child("channel")
            chn.append_child_value("label", channame)
            chn.append_child_value("unit", "microvolts")
            chn.append_child_value("type", "generated")
        self.eeg_outlet = StreamOutlet(raw_info)
        print("Created outlet with name BetaGen and type EEG")

        self.last_time = local_clock()

    def update(self, task={'phase': 'precue', 'class': 1}):
        # Convert phase and class_id into beta_amp
        if task['phase'] in ['cue', 'go']:
            beta_amp = 0 if task['class'] == 3 else self.AmpBeta
        else:
            beta_amp = self.AmpBeta / 5.0

        this_tvec = self.tvec + self.last_time  # Sample times
        # Put the signal together
        this_sig = self.AmpNoise * np.asarray(self.pinkNoiseGen.generate(), dtype=np.float32)  # Start with some pink noise
        this_sig += beta_amp * np.sin(this_tvec * 2 * np.pi * self.FreqBeta)  # Add our beta signal
        this_sig = np.atleast_2d(this_sig).T * np.ones((1, len(self.channels)), dtype=np.float32)  # Tile across channels

        time_to_sleep = max(0, this_tvec[-1] - local_clock())
        time.sleep(time_to_sleep)

        print("Beta outlet pushing signal with shape {},{} and Beta amp {}".format(this_sig.shape[0], this_sig.shape[1],
                                                                                   beta_amp))
        self.eeg_outlet.push_chunk(this_sig, timestamp=this_tvec[-1])

        self.last_time = local_clock()


class BetaInlet(object):
    def __init__(self):
        print("looking for an EEG stream...")
        streams = resolve_byprop("type", "EEG")

        # create a new inlet to read from the stream
        proc_flags = proc_clocksync | proc_dejitter | proc_monotonize
        self.inlet = StreamInlet(streams[0], processing_flags=proc_flags)

        # The following is an example of how to read stream info
        stream_info = self.inlet.info()
        stream_Fs = stream_info.nominal_srate()
        stream_xml = stream_info.desc()
        chans_xml = stream_xml.child("channels")
        chan_xml_list = []
        ch = chans_xml.child("channel")
        while ch.name() == "channel":
            chan_xml_list.append(ch)
            ch = ch.next_sibling("channel")
        self.channel_names = [ch_xml.child_value("label") for ch_xml in chan_xml_list]
        print("Reading from inlet named {} with channels {} sending data at {} Hz".format(stream_info.name(),
                                                                                          self.channel_names, stream_Fs))

    def update(self):
        max_samps = 3276*2
        data = np.nan * np.ones((max_samps, len(self.channel_names)), dtype=np.float32)
        _, timestamps = self.inlet.pull_chunk(max_samples=max_samps, dest_obj=data)
        data = data[:len(timestamps), :]
        print("Beta inlet retrieved {} samples.".format(len(timestamps)))
        return data, np.asarray(timestamps)


class MarkersGeneratorOutlet(object):
    phases = {
        'precue': {'next': 'cue', 'duration': 1.0},
        'cue': {'next': 'go', 'duration': 0.5},
        'go': {'next': 'evaluate', 'duration': 5.0},
        'evaluate': {'next': 'precue', 'duration': 0.1}
    }

    def __init__(self, class_list=[1, 3], classes_rand=True, target_list=[1, 2], targets_rand=True):
        """

        :param class_list:  A list of integers comprising different class ids. Default: [1, 3]
        :param classes_rand: If True, classes are chosen randomly from list. If False, the list is cycled. Default: True
        :param target_list: A list of integers comprising different target ids. Default: [1, 2]
        :param targets_rand: If True, targets are chosen randomly from list. If False, the list is cycled. Default: True
        """
        stream_name = 'GeneratedCentreOutMarkers'
        stream_type = 'Markers'
        outlet_info = StreamInfo(name=stream_name, type=stream_type,
                   channel_count=1, nominal_srate=0,
                   channel_format='string',
                   source_id='centreoutmarkergen1234')
        outlet_xml = outlet_info.desc()
        channels_xml = outlet_xml.append_child("channels")
        chan_xml = channels_xml.append_child("channel")
        chan_xml.append_child_value("label", "EventMarkers")
        chan_xml.append_child_value("type", "generated")
        self.outlet = StreamOutlet(outlet_info)
        print("Created outlet with name {} and type {}".format(stream_name, stream_type))

        self.class_list = class_list
        self.classes_rand = classes_rand
        self.target_list = target_list
        self.targets_rand = targets_rand
        self.next_transition = -1
        self.in_phase = 'evaluate'
        self.trial_ix = 0
        self.class_id = self.class_list[0]
        self.target_id = self.target_list[0]

    def update(self):
        now = local_clock()
        if (now > self.next_transition):

            # Transition phase
            self.in_phase = self.phases[self.in_phase]['next']
            self.next_transition = now + self.phases[self.in_phase]['duration']

            # Send markers
            out_string = "undefined"
            if self.in_phase == 'precue':
                # transition from evaluate to precue
                # print("Previous class_id: {}, target_id: {}".format(self.class_id, self.target_id))
                self.trial_ix += 1
                self.target_id = random.choice(self.target_list) if self.targets_rand else self.target_list[
                    (self.target_list.index(self.target_id) + 1) % len(self.target_list)]
                self.class_id = random.choice(self.class_list) if self.classes_rand else self.class_list[
                    (self.class_list.index(self.class_id) + 1) % len(self.class_list)]
                # print("New class_id: {}, target_id: {}".format(self.class_id, self.target_id))
                out_string = "NewTrial {}, Class {}, Target {}".format(self.trial_ix, self.class_id, self.target_id)
            elif self.in_phase == 'cue':
                # transition from precue to cue
                out_string = "TargetCue, Class {}, Target {}".format(self.class_id, self.target_id)
            elif self.in_phase == 'go':
                # transition from cue to go
                out_string = "GoCue, Class {}, Target {}".format(self.class_id, self.target_id)
            elif self.in_phase == 'evaluate':
                # transition from go to evaluate
                hit_string = "Hit" if random.randint(0, 1) == 1 else "Miss"
                out_string = "{}, Class {}, Target {}".format(hit_string, self.class_id, self.target_id)
            print("Marker outlet pushing string: {}".format(out_string))
            self.outlet.push_sample([out_string,])

            return True
        return False


class MarkerInlet(object):
    def __init__(self):
        self.task = {'phase':'precue', 'class':1, 'target':1}
        print("Looking for stream with type Markers")
        streams = resolve_bypred("type='Markers'", minimum=1)
        proc_flags = 0  # Marker events are relatively rare. No need to post-process.
        self.inlet = StreamInlet(streams[0], processing_flags=proc_flags)
        # The following is an example of how to read stream info
        stream_info = self.inlet.info()
        stream_Fs = stream_info.nominal_srate()
        stream_xml = stream_info.desc()
        chans_xml = stream_xml.child("channels")
        chan_xml_list = []
        ch = chans_xml.child("channel")
        while ch.name() == "channel":
            chan_xml_list.append(ch)
            ch = ch.next_sibling("channel")
        stream_ch_names = [ch_xml.child_value("label") for ch_xml in chan_xml_list]
        print("Reading from inlet named {} with channels {}".format(stream_info.name(), stream_ch_names))

    def update(self):
        marker_samples, marker_timestamps = self.inlet.pull_chunk(timeout=0.0)
        if (marker_timestamps):
            [phase_str, class_str, targ_str] = marker_samples[-1][0].split(', ')
            if phase_str in ['TargetCue']:
                self.task['phase'] = 'cue'
            elif phase_str in ['GoCue']:
                self.task['phase'] = 'go'
            elif phase_str in ['Miss', 'Hit']:
                self.task['phase'] = 'evaluate'
            elif phase_str[:8] == 'NewTrial':
                self.task['phase'] = 'precue'
            else:
                print(phase_str)
            self.task['class'] = int(class_str.split(' ')[1])
            self.task['target'] = int(targ_str.split(' ')[1])
            print("Marker inlet updated with task {}".format(self.task))


betaGen = BetaGeneratorOutlet()
markerGen = MarkersGeneratorOutlet()
betaIn = BetaInlet()
markerIn = MarkerInlet()

if haspyqtgraph:
    qapp = pg.QtGui.QApplication(sys.argv)
    qwindow = pg.plot()
    qwindow.clear()
    qwindow.parent().setWindowTitle("pylsl PerformanceTest")

def update():
    markerGen.update()
    markerIn.update()
    betaGen.update(task=markerIn.task)  # Rate-limiting step. Will time.sleep as needed.
    signal, tvec = betaIn.update()

    if haspyqtgraph:
        plot = qwindow.getPlotItem()
        graphs = plot.listDataItems()
        if not graphs:
            # create graphs
            for i in range(signal.shape[1]):
                plot.plot(tvec, signal[:, i])
        else:
            # update graphs
            for i in range(signal.shape[1]):
                graphs[i].setData(signal[:, i], x=tvec)

if __name__ == '__main__':
    """
    python3 -m cProfile -o pylsl.cprof PerformanceTest.py
    gprof2dot -f pstats pylsl.cprof | dot -Tpng -o pylsl_prof.png
    """
    try:
        if haspyqtgraph:
            timer = pg.QtCore.QTimer()
            timer.timeout.connect(update)
            timer.start(1)  # Delay not needed because update has time.sleep
            if (sys.flags.interactive != 1) or not hasattr(pg.QtCore, 'PYQT_VERSION'):
                sys.exit(pg.QtGui.QApplication.instance().exec_())
        else:
            while True:
                update()

    except KeyboardInterrupt:
        # No cleanup necessary?
        pass