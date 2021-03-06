import argparse
import itertools
import ast
from collections import namedtuple
from copy import deepcopy
import sys
import matplotlib.pyplot as plt
import numpy as np

from read_laser_log import read_laser_log_by_timestamp

if __name__ == '__main__':

    # problem = "problem1"
    # origin = [2, 3]
    # destination = [8, 1]
    #
    # bounds = [-1, -3, 16, 8]
    # # barriers = [(8, 2), (7, 1), (4,5)]`
    # # lasers = [(9, 5, 'N'), (9, 6, 'E')]
    # # wormholes = [[(2, 2), (10, 1)]]
    # #     # lasers = read_laser_log_by_timestamp(0)
    # barriers = [(8, 2), (7, 1), (8, 0), (9, 2), (10, 2)]  # problem1
    # lasers = []
    # wormholes = []
    # path = [(2, 3), (3, 3), (4, 3), (5, 3), (6, 3), (6, 2), (6, 1), (6, 0), (6, -1), (7, -1), (8, -1), (9, -1), (9, 0), (9, 1), (8, 1)]

    problem = "problem3"
    origin = [2, 3]
    destination = [8, 1]

    bounds = [-1, -3, 18, 14]
    barriers = [(8, 2), (7, 1), (8, 0), (9, 2), (10, 2), (11, 2), (12, 1), (12, 0), (11, 0), (10, 0), (9, 0)]
    lasers = [(11, 5, 'S'), (6, 0, 'N')]
    wormholes = [[(11, 1), (11, 6)]]
    path = []
    # path = [(2, 3), (3, 3), (4, 3), (5, 3), (6, 3), (6, 2), (6, 1), (6, 0), (6, -1), (7, -1), (8, -1), (9, -1), (9, 0),
    #         (9, 1), (8, 1)]

    # static solution
    # path = [(2, 3), (3, 3), (4, 3), (5, 3), (6, 3), (6, 2), (6, 1), (6, 0), (7, 0), (8, 0), (8, 1)]
    # dynamic solution
    # path = [(2, 3), (2, 2), (3, 2), (3, 1), (4, 1), (4, 0), (5, 0), (6, 0), (7, 0), (8, 0), (8, 1)]
    # solution with dynamic laser and wormholes
    # path = [(2, 3), (3, 3), (4, 3), (5, 3), (5, 2), (6, 2), (6, 1), (6, 0), (7, 0), (8, 0), (8, 1)]
    # path = [(2, 3), (2, 2), (3, 2), (3, 1), (3, 0), (4, 0), (5, 0), (6, 0), (7, 0), (8, 0), (8, 1)]
    # path = [(2, 3), (2, 2), (3, 2), (4, 2), (5, 2), (6, 2), (6, 1), (6, 0), (7, 0), (8, 0), (8, 1)]
    # path = [(2, 3), (3, 3), (3, 2), (3, 1), (3, 0), (4, 0), (5, 0), (6, 0), (7, 0), (8, 0), (8, 1)]

    lasers_add = True
    wormholes_add = True

    fig = plt.figure(frameon=False, figsize=(10, 8))
    x = np.arange(bounds[0], bounds[2], step=1)
    y = np.arange(bounds[1], bounds[3], step=1)

    # a = np.random.rand(10)
    # b = np.random.rand(10)
    # z = np.sqrt(a ** 2 + b ** 2)

    plt.text(origin[0], origin[1], "origin")
    plt.text(destination[0], destination[1], "destination")

    # plt.plot(origin[0], origin[1], label = "origin")
    # plt.plot(destination[0], destination[1], label = "destination")

    for i in range(len(barriers)):
        if i==0:
            plt.scatter(barriers[i][0], barriers[i][1], c='black', s=150, marker=(5, 0), label = "barriers")
        else:
            plt.scatter(barriers[i][0], barriers[i][1], c = 'black', s = 150, marker=(5, 0))
    if lasers_add:
        for i in range(len(lasers)):
            if i==0:
                plt.scatter(lasers[i][0], lasers[i][1], c='red', s=150, marker=(5, 2), label = "lasers")
            else:
                plt.scatter(lasers[i][0], lasers[i][1], c='red', s=150, marker=(5, 2))
    if wormholes_add:
        for i in range(len(wormholes)):
            plt.scatter(wormholes[i][0], wormholes[i][1], c=np.random.rand(3,), s=150, marker='o', label = "wormhole" + str(i))

    for i in range(1, len(path)):
        if i==1:
            plt.plot([path[i - 1][0], path[i][0]], [path[i - 1][1], path[i][1]], c='blue', marker='*', label = "path")
        else:
            plt.plot([path[i-1][0],path[i][0]], [path[i-1][1],path[i][1]], c = 'blue', marker='*')

    plt.grid(True)
    plt.xticks(x)
    plt.yticks(y)
    plt.legend()
    plt.title(problem)
    plt.savefig(problem + "/env.png")
    plt.show()