import matplotlib.pyplot as plt
import numpy as np
import math
from data import get_data

def main():
    data = get_data()
    error = np.zeros(len(data['update_stats']))
    for i in range(len(data['update_stats'])):
        error[i] = data['update_stats'][i]['error']
    error = np.array(error)

    print("b: {}".format(data['update_stats'][-1]['b']))

    fig, ax = plt.subplots(3, 1, constrained_layout=True)
    ax[0].plot(20*np.log10(error))
    # ax[0].plot(error)
    ax[0].set_title("Prediction test")
    ax[0].set_xlabel("samples")
    ax[0].set_ylabel("Error MSE [dB]")

    predicted_values = data['output']
    signal = data['input']

    l1, l2 = ax[1].plot(predicted_values, "-x", signal)
    ax[1].legend((l1, l2), ('Prediction', 'Signal'))
    ax[1].set_xlabel("samples")
    range_signal = max(signal) - min(signal)
    ax[1].set_ylim(min(signal)-range_signal*0.1, max(signal)+range_signal*0.1)

    system_parameters = data['filter_parameters']['system']
    identified_parameters = data['filter_parameters']['identification']

    l3, l4 = ax[2].plot(system_parameters, "-x", identified_parameters, "-o")
    ax[2].legend((l3, l4), ('System', 'Identification'))
    ax[2].set_xlabel("b")

    def find_min_max(v1, v2):
        _max = max(max(v1), max(v2))
        _min = min(min(v1), min(v1))
        return _max, _min

    param_max, param_min = find_min_max(system_parameters, identified_parameters)
    ax[2].set_ylim(param_min+param_min*0.05, param_max+param_max*0.05)

    def plot_filter_response(data):
        h, w =  np.array([complex(s.replace('i','j')) for s in data['freqz']['h']]), np.array(data['freqz']['w'])
        w = w/(2*math.pi)
        w = w[:math.floor(w.size*0.5)]
        h = h[:w.size]

        fig, ax = plt.subplots(3, 1, constrained_layout=True)
        ax[0].plot(w, abs(h))
        ax[0].set_title("Frequency response")
        ax[0].set_xlabel("\u03C9")

        ax[1].plot(w, 20 * np.log10(abs(h)))
        ax[1].set_title("Logarithmic frequency response")
        ax[1].set_xlabel("\u03C9")

        ax[2].plot(w, np.unwrap(np.angle(h)))
        ax[2].set_title("Phase response")
        ax[2].set_xlabel("\u03C9")


    plot_filter_response(data)

    plt.show()
    pass


if __name__ == '__main__':
    main()
