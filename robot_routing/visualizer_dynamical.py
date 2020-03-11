import argparse
import itertools
import ast
from collections import namedtuple
from copy import deepcopy
import sys
import matplotlib.pyplot as plt
import numpy as np
import imageio

from read_laser_log import read_laser_log_by_timestamp
from make_gif import make_gif_for_problem

if __name__ == '__main__':



    # static solution
    # path = [(2, 3), (3, 3), (4, 3), (5, 3), (6, 3), (6, 2), (6, 1), (6, 0), (7, 0), (8, 0), (8, 1)]
    # dynamic solution
    # path = [(2, 3), (2, 2), (3, 2), (3, 1), (4, 1), (4, 0), (5, 0), (6, 0), (7, 0), (8, 0), (8, 1)]
    # solution with dynamic laser and wormholes
    # path = [(2, 3), (3, 3), (4, 3), (5, 3), (5, 2), (6, 2), (6, 1), (6, 0), (7, 0), (8, 0), (8, 1)]
    # path = [(2, 3), (2, 2), (3, 2), (3, 1), (3, 0), (4, 0), (5, 0), (6, 0), (7, 0), (8, 0), (8, 1)]
    # path = [(2, 3), (2, 2), (3, 2), (4, 2), (5, 2), (6, 2), (6, 1), (6, 0), (7, 0), (8, 0), (8, 1)]
    # path = [(2, 3), (3, 3), (3, 2), (3, 1), (3, 0), (4, 0), (5, 0), (6, 0), (7, 0), (8, 0), (8, 1)]

    problem_file = "problem2"
    origin = [2, 3]
    destination = [8, 1]
    bounds = [-1, -3, 14, 7]
    barriers = [(8, 2), (7, 1), (8, 0), (9, 2), (10, 2)]
    wormholes = []
    path = [(2, 3), (3, 3), (4, 3), (4, 2), (4, 1), (4, 0), (5, 0), (5, -1), (6, -1), (7, -1), (8, -1), (9, -1), (9, 0), (9, 1), (8, 1)]


    # problem_file = "problem4"
    # origin = [10, 4]
    # destination = [0, 8]
    # bounds = [-6, -6, 20, 15]
    # barriers = [(4, 7), (9, 3), (9, 2), (10, 1), (11, 2), (11, 3), (12, 4)]
    # wormholes = [[(10, 2), (0, 7)]]
    # path = [(10, 4), (9, 4), (8, 4), (7, 4), (6, 4), (5, 4), (4, 4), (3, 4), (2, 4), (1, 4), (0, 4), (0, 5), (0, 6), (0, 7), (0, 8)] # problem4

    lasers_add = True
    wormholes_add = True

    ts = 0
    total_ts = len(path)
    # total_ts = 3

    for ts in range(total_ts):
        fig = plt.figure(frameon=False)
        x = np.arange(bounds[0], bounds[2], step=1)
        y = np.arange(bounds[1], bounds[3], step=1)

        plt.text(origin[0], origin[1], "origin", color='green')
        plt.text(destination[0], destination[1], "destination", color='green')

        # plot barriers
        for i in range(len(barriers)):
            if i == 0:
                plt.scatter(barriers[i][0], barriers[i][1], c='black', s=100, marker=(5, 0), label="barriers")
            else:
                plt.scatter(barriers[i][0], barriers[i][1], c='black', s=100, marker=(5, 0))

        if lasers_add:
            lasers = read_laser_log_by_timestamp(problem_file, ts)
            # print(len(lasers))
            for i in range(len(lasers)):
                if i == 0:
                    plt.scatter(lasers[i][0], lasers[i][1], c='red', s=30, marker=(5, 2), label = "lasers")
                else:
                    plt.scatter(lasers[i][0], lasers[i][1], c='red', s=30, marker=(5, 2))

        if wormholes_add:
            plt.scatter([], [], c='hotpink', s=180, marker='o', label = "wormhole")
            if ts % 3 == 0:
                for i in range(len(wormholes)):
                    plt.scatter(wormholes[i][0], wormholes[i][1], c='hotpink', s=180, marker='o')

        sub_path = path[: ts + 1]
        for i in range(1, len(sub_path)):
            if i == 1:
                plt.plot([path[i - 1][0], path[i][0]], [path[i - 1][1], path[i][1]], c='blue', marker='*', label = "path")
            else:
                plt.plot([path[i-1][0],path[i][0]], [path[i-1][1],path[i][1]], c = 'blue', marker='*')

        plt.xticks(x)
        plt.yticks(y)
        plt.legend(loc= 'lower left')
        plt.xlim((bounds[0], bounds[2]))
        plt.ylim((bounds[1], bounds[3]))
        plt.title("timestamp:" + str(ts))
        plt.grid(True)

        fig_name = problem_file + "/plots/" + str(ts) + ".png"
        plt.savefig(fig_name)

        # plt.show()
    make_gif_for_problem(problem_file, total_ts)
