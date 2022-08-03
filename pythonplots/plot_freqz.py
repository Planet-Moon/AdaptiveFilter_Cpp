import json
import matplotlib.pyplot as plt
from math import pi as Pi
from math import log10

def half_vector_size(vector):
    return vector[:int(len(vector)/2)]

class Freqz:
    def __init__(self, frequency, abs, arg):
        self.frequency = frequency
        self.abs = abs
        self.arg = arg
        self.abs_mode = ''
        self.frequency_mode = ''

    def make_half(self):
        self.frequency = half_vector_size(self.frequency)
        self.abs = half_vector_size(self.abs)
        self.arg = half_vector_size(self.arg)

    def normalize_frequency(self):
        self.frequency = [ i/Pi for i in self.frequency ]
        self.frequency_mode = 'normalized'

    def log_magnitude(self):
        self.abs = [ 20*log10(x) for x in self.abs ]
        self.abs_mode = 'log'

class FreqzDecoder(json.JSONDecoder):
    def __init__(self, *args, **kwargs):
        json.JSONDecoder.__init__(self, object_hook=self.object_hook, *args, **kwargs)
    def object_hook(self, dct) -> Freqz:
        return Freqz(dct['Frequency'], dct['Magnitude'], dct['Phase'])

def read_json(filename)->Freqz:
    with open(filename, 'r') as f:
        return json.load(f, cls=FreqzDecoder)

def plot_data(data:Freqz):
    fix, ax = plt.subplots(2, 1, sharex=True)
    ax[0].plot(data.frequency, data.abs)
    ax[0].set_title("Magnitude")
    if(data.abs_mode == 'log'):
        ax[0].set_ylabel("dB")
    ax[1].plot(data.frequency, data.arg)
    if(data.frequency_mode == 'normalized'):
        ax[1].set_xlabel("Pi")
    ax[1].set_title("Phase")
    plt.show()

def main():
    data = read_json('build/test/freqz_result.json')
    data.make_half()
    data.normalize_frequency()
    data.log_magnitude()
    plot_data(data)
    pass

if __name__ == '__main__':
    main()
