import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
import fileinput
from time import sleep
import requests
import json

style.use('fivethirtyeight')


def read_from_server():
    rr = json.loads(requests.get(
        'http://localhost:8800/data').content.decode('utf-8'))
    return rr

class Array:
    def __init__(self):
        self.max_size = 1000
        self._data = []

    def extend(self, data:list):
        self._data.extend(data)
        self._data = self._data[-1*self.max_size:]

    def data(self):
        return self._data

    def __repr__(self):
        return f"<Array size={len(self._data)}, max_size={self.max_size}>"

def main():
    fig = plt.figure()

    xs:Array = Array()
    xs.max_size = 1000
    ys:Array = Array()
    ys.max_size = 1000


    ax1 = fig.add_subplot(1, 1, 1)

    def animate(i):
        graph_data: dict = read_from_server()
        time = graph_data.get("time")
        value = graph_data.get("value")
        if time and value:
            xs.extend(time)
            ys.extend(value)
            ax1.clear()
            ax1.plot(xs.data(), ys.data())
    ani = animation.FuncAnimation(fig, animate, interval=1000)
    plt.show()


if __name__ == '__main__':
    main()
