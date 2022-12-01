import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
import fileinput
from time import sleep
import requests
import json
import numpy as np

style.use('fivethirtyeight')


def read_from_server():
    rr = json.loads(requests.get(
        'http://localhost:8801/data').content.decode('utf-8'))
    return rr


def main():
    fig = plt.figure()

    ax1 = fig.add_subplot(1, 1, 1)

    def animate(i):
        server_data: dict = read_from_server()
        fft_data_raw = server_data.get("fft")
        fft_freq_data = [float(x) for x in server_data.get("freq")]
        fft_data = [complex(x.replace('i', 'j')) for x in fft_data_raw]
        if fft_data:
            ax1.clear()
            ax1.plot(
                fft_freq_data[:int(0.5*len(fft_freq_data))],
                [abs(x) for x in fft_data[:int(0.5*len(fft_data))]])
    ani = animation.FuncAnimation(fig, animate, interval=1000)
    plt.show()


if __name__ == '__main__':
    main()
