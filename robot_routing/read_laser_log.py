import re
import matplotlib.pyplot as plt
import numpy as np


def read_laser_log_by_timestamp(timestamp=0, plot_disabled = False):
    laser_log_file = "sample/laser_log.txt"

    with open(laser_log_file) as f:
        file_contents = f.readlines()
    file_contents = [line.strip() for line in file_contents]

    str1 = file_contents[timestamp]
    list1 = str1.split(",")
    list2 = []
    for s in list1:
        s = re.sub(r'[^\w\s]','',s)
        if len(s) > 0:
            list2.append(int(s))

    lasers = np.array(list2)
    lasers = lasers.reshape((-1, 2))

    if plot_disabled:
        plt.scatter(lasers[:, 0], lasers[:, 1])
        plt.show()
    # print lasers
    return lasers


if __name__ == '__main__':
    read_laser_log_by_timestamp(0, True)
