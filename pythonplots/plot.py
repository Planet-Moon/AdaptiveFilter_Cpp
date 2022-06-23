import matplotlib.pyplot as plt
import numpy as np
from data import get_data

def main():
    data = get_data()
    error = np.zeros(len(data['update_stats']))
    for i in range(len(data['update_stats'])):
        error[i] = data['update_stats'][i]['error']
    error = np.array(error)

    print("b: {}".format(data['update_stats'][-1]['b']))

    fig, ax = plt.subplots(3, 1, constrained_layout=True)
    # ax[0].plot(20*np.log10(error))
    ax[0].plot(error)
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

    plt.show()
    pass


if __name__ == '__main__':
    main()
