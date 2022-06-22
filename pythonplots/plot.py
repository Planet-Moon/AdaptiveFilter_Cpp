import matplotlib.pyplot as plt
from data import get_data

def main():
    plt.plot(get_data())
    plt.show()


if __name__ == '__main__':
    main()
