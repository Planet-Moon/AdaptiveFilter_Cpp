from cProfile import label
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
        a_h, a_w =  np.array([complex(s.replace('i','j')) for s in data['adaptive_freqz']['h']]), np.array(data['adaptive_freqz']['w'])
        a_w = a_w/(2*math.pi)
        a_w = a_w[:math.floor(a_w.size*0.5)]
        a_h = a_h[:a_w.size]

        e_h, e_w =  np.array([complex(s.replace('i','j')) for s in data['expected_freqz']['h']]), np.array(data['expected_freqz']['w'])
        e_w = e_w/(2*math.pi)
        e_w = e_w[:math.floor(e_w.size*0.5)]
        e_h = e_h[:e_w.size]

        fig, ax = plt.subplots(3, 1, constrained_layout=True)
        ax[0].plot(a_w, abs(a_h), label="adaptive")
        ax[0].plot(e_w, abs(e_h), label="expected")
        ax[0].set_title("Frequency response")
        ax[0].set_xlabel("\u03C9")
        ax[0].legend()

        ax[1].plot(a_w, 20 * np.log10(abs(a_h)), label="adaptive")
        ax[1].plot(e_w, 20 * np.log10(abs(e_h)), label="expected")
        ax[1].set_title("Logarithmic frequency response")
        ax[1].set_xlabel("\u03C9")
        ax[1].legend()

        ax[2].plot(a_w, np.unwrap(np.angle(a_h)), label="adaptive")
        ax[2].plot(e_w, np.unwrap(np.angle(e_h)), label="expected")
        ax[2].set_title("Phase response")
        ax[2].set_xlabel("\u03C9")
        ax[2].legend()

    def plot_filter_parameters(data):
        param_data = data["filter_parameters_time"]

        x = np.arange(0, len(param_data[0]))
        fix, ax = plt.subplots(1, 1)
        for i in param_data:
            ax.plot(x, i)
        ax.set_title("Param 0")


    plot_filter_response(data)

    plot_filter_parameters(data)

    plt.show()
    pass


if __name__ == '__main__':
    main()
