from cProfile import label
import matplotlib.pyplot as plt
import numpy as np
import math
from data import get_data


def main():
    data = get_data()
    LMS_data = data["LMS"]
    RLS_data = data["RLS"]

    error_LMS = np.zeros(len(LMS_data['update_stats']))
    for i in range(len(LMS_data['update_stats'])):
        error_LMS[i] = LMS_data['update_stats'][i]['error']
    error_LMS = np.array(error_LMS)

    error_RLS = np.zeros(len(RLS_data['update_stats']))
    for i in range(len(RLS_data['update_stats'])):
        error_RLS[i] = RLS_data['update_stats'][i]['error']
    error_RLS = np.array(error_RLS)

    print("LMS b: {}".format(LMS_data['update_stats'][-1]['b']))
    print("RLS b: {}".format(RLS_data['update_stats'][-1]['b']))

    fig, ax = plt.subplots(3, 1, constrained_layout=True)
    ax[0].plot(20*np.log10(error_LMS), label="LMS")
    ax[0].plot(20*np.log10(error_RLS), label="RLS")
    # ax[0].plot(error)
    ax[0].set_title("Prediction test")
    ax[0].set_xlabel("samples")
    ax[0].set_ylabel("Error MSE [dB]")
    ax[0].legend()

    LMS_predicted_values = LMS_data['output']
    RLS_predicted_values = RLS_data['output']
    input_signal = RLS_data['input']

    l1, l2, l3 = ax[1].plot(LMS_predicted_values, "-x",
                            RLS_predicted_values, "-o", input_signal)
    ax[1].legend((l1, l2, l3), ('LMS Prediction',
                 'RLS Prediction', 'Input'))
    ax[1].set_xlabel("samples")

    sig_max = max([max(input_signal), max(LMS_predicted_values)])
    sig_min = min([min(input_signal), min(RLS_predicted_values)])
    range_signal = sig_max - sig_min
    ax[1].set_ylim(sig_min - range_signal*0.1, sig_max + range_signal*0.1)

    system_parameters = data['fir_ref']['filter_parameters']
    LMS_identified_parameters = LMS_data['filter_parameters']
    RLS_identified_parameters = RLS_data['filter_parameters']

    l4, l5, l6 = ax[2].plot(system_parameters, "-x",
                            LMS_identified_parameters, "-o", RLS_identified_parameters, "--")
    ax[2].legend(
        (l4, l5, l6), ('System', 'LMS Identification', 'RLS Identification'))
    ax[2].set_xlabel("b")

    def find_min_max(v1, v2):
        _max = max(max(v1), max(v2))
        _min = min(min(v1), min(v1))
        return _max, _min

    param_max, param_min = find_min_max(
        find_min_max(system_parameters, LMS_identified_parameters),
        find_min_max(system_parameters, RLS_identified_parameters))
    ax[2].set_ylim(param_min-param_min*0.05, param_max*1.05)

    def plot_filter_response(data):
        LMS_data = data["LMS"]
        RLS_data = data["RLS"]
        system_data = data["fir_ref"]

        sys_e_h, sys_e_w = np.array([complex(s.replace('i', 'j'))
                                     for s in system_data['expected_freqz']['h']]), np.array(system_data['expected_freqz']['w'])
        sys_e_w = sys_e_w/(2*math.pi)
        sys_e_w = sys_e_w[:math.floor(sys_e_w.size*0.5)]
        sys_e_h = sys_e_h[:sys_e_w.size]

        lms_a_h, lms_a_w = np.array([complex(s.replace('i', 'j'))
                                     for s in LMS_data['adaptive_freqz']['h']]), np.array(LMS_data['adaptive_freqz']['w'])
        lms_a_w = lms_a_w/(2*math.pi)
        lms_a_w = lms_a_w[:math.floor(lms_a_w.size*0.5)]
        lms_a_h = lms_a_h[:lms_a_w.size]

        rls_a_h, rls_a_w = np.array([complex(s.replace('i', 'j'))
                                     for s in RLS_data['adaptive_freqz']['h']]), np.array(RLS_data['adaptive_freqz']['w'])
        rls_a_w = rls_a_w/(2*math.pi)
        rls_a_w = rls_a_w[:math.floor(rls_a_w.size*0.5)]
        rls_a_h = rls_a_h[:rls_a_w.size]

        fig, ax = plt.subplots(3, 1, constrained_layout=True)
        ax[0].plot(sys_e_w, abs(sys_e_h), label="Fir System")
        ax[0].plot(lms_a_w, abs(lms_a_h), label="LMS adaptive")
        ax[0].plot(rls_a_w, abs(rls_a_h), label="RLS adaptive")
        ax[0].set_title("Frequency response")
        ax[0].set_xlabel("\u03C9")
        ax[0].legend()

        ax[1].plot(sys_e_w, 20 * np.log10(abs(sys_e_h)), label="Fir System")
        ax[1].plot(lms_a_w, 20 * np.log10(abs(lms_a_h)), label="LMS adaptive")
        ax[1].plot(rls_a_w, 20 * np.log10(abs(rls_a_h)), label="RLS adaptive")
        ax[1].set_title("Logarithmic frequency response")
        ax[1].set_xlabel("\u03C9")
        ax[1].legend()

        ax[2].plot(sys_e_w, np.unwrap(np.angle(sys_e_h)), label="Fir System")
        ax[2].plot(lms_a_w, np.unwrap(np.angle(lms_a_h)), label="LMS adaptive")
        ax[2].plot(rls_a_w, np.unwrap(np.angle(rls_a_h)), label="RLS adaptive")
        ax[2].set_title("Phase response")
        ax[2].set_xlabel("\u03C9")
        ax[2].legend()

    def plot_filter_parameters(data):
        LMS_data = data["LMS"]
        RLS_data = data["RLS"]

        lms_param_data = LMS_data["filter_parameters_time"]
        rls_param_data = RLS_data["filter_parameters_time"]

        x = np.arange(0, len(lms_param_data[0]))
        fix, ax = plt.subplots(1, 1)
        for i in range(len(lms_param_data)):
            ax.plot(x, lms_param_data[i], label='LMS Param {}'.format(i))
            ax.plot(x, rls_param_data[i], label='RLS Param {}'.format(i))
        ax.legend()
        ax.set_title("Param 0")

    plot_filter_response(data)

    plot_filter_parameters(data)

    plt.show()
    pass


if __name__ == '__main__':
    main()
