# this one treats each H-Si as an array value


import random
import sys

import matplotlib.pyplot as plt
import numpy as np

np.set_printoptions(threshold=sys.maxsize)
from matplotlib.axes._axes import _log as matplotlib_axes_logger

matplotlib_axes_logger.setLevel('ERROR')


def rand_int(min, max):
    return random.randit(min, max)


class defect_surface:
    # creates a 2d array of the surface
    # the surface is initialized with each surface dimer with a value of -1
    # surface_width is the number of dimers within a row
    # surface_height is the number of dimer rows (1 dimer is made of 2 HSi atoms)
    # self.surface_lattice is the array of the surface
    def __init__(self, surface_width=100, surface_height=100):
        self.defects_name = None
        self.defect_params = None
        self.defect_plotting = None
        self.total_defect_lattice_points = None

        self.surface_width = surface_width
        self.surface_height = surface_height

        self.a1 = 3.84  # (width) in nm
        self.a2 = 7.68  # (height)
        self.b2 = 2.25  # (distance within dimer)

        self.surface_lattice = np.full((self.surface_height, self.surface_width), 0)

    # add_defects function allows you to choose total coverage of defects (fully defected surface is coverage = 1.)
    # self.defect_params is array used for configuration of ratio and size of defects on the surface
    # each entry is formated as [array_value,width,height,ratio]
    # array_value is number assigned to each lattice point in the self.surface_lattice
    # width and height are how many HSi atoms are used (note this different than surface_height in init)
    # ratio is given as fractional percent (0.05 = 5%)
    # Following
    def add_defects(self, coverage=0.05):
        self.defects_name = ['H-Si', 'DB', 'si_vacancy', 'dihydride_pair', 'single_dihydride', 'onebyone', 'threebyone',
                             'siloxane', 'raised_Si', 'etch_pit', 'missing_dimer']
        self.defect_params = [[0, 1, 1, 1.],  # HSi
                              [1, 1, 1, 0.0],  # [array_value,width,height,ratio] DB
                              [2, 1, 2, 0.0],  # si_vacancy 0.05
                              [3, 1, 2, 0.2],  # dihydride_pair
                              [4, 1, 2, 0.05],  # single_dihydride
                              [5, 4, 2, 0.05],  # onebyone
                              [6, 4, 4, 0.05],  # threebyone
                              [7, 1, 2, 0.1],  # siloxane
                              [8, 1, 2, 0.1],  # raised_si
                              [9, 3, 2, 0.05],  # etch pit
                              [10, 1, 2, 0.05]  # missing_dimer
                              ]

        self.defect_plotting = [[(255 / 255, 0, 0), 'o'],  # HSi
                                [(0, 0, 255 / 255), 'o'],  # [colour,shape] DB
                                [(0, 0, 0), 's'],  # si_vacancy
                                [(58 / 255, 0, 83 / 255), 's'],  # dihydride_pair
                                [(72 / 255, 53 / 255, 4 / 255), 's'],  # single_dihydride
                                [(42 / 255, 56 / 255, 14 / 255), 's'],  # onebyone
                                [(173 / 255, 255 / 255, 47 / 255), 's'],  # threebyone
                                [(64 / 255, 224 / 255, 208 / 255), 's'],  # siloxane
                                [(119 / 255, 136 / 255, 153 / 255), 's'],  # raised_si
                                [(255 / 255, 255 / 255, 0), 's'],  # etch_pit
                                [(227 / 255, 28 / 255, 121 / 255), 's']  # missing_dimer
                                ]

        self.total_defect_lattice_points = int(self.surface_width * self.surface_height * coverage)
        for defect in self.defect_params:
            num_of_defects = int(defect[3] * self.total_defect_lattice_points)
            for i in range(1, num_of_defects):

                random_width = random.randint(0, self.surface_width - 1)
                random_height = random.randint(0, self.surface_height - 1)

                if (random_width > random_width + defect[1] and random_height > random_height + defect[2]) or (
                        random_height % 2 == 1 and defect[2] % 2 == 0):
                    i = i - 1
                    pass
                else:

                    if np.all(self.surface_lattice[random_height:random_height + defect[2],
                              random_width:random_width + defect[1]] == 0):
                        self.surface_lattice[random_height:random_height + defect[2],
                        random_width:random_width + defect[1]] = defect[0]

                    else:
                        i = i - 1
                        pass

    def draw_panels(self):  # DB_panels,DB_pattern_extended, pattern):
        # draws the DB_pattern_extended with rectangles to show each pannel

        width_nm = self.a1 * self.surface_width
        height_nm = self.a2 * self.surface_height
        # print(height_nm,width_nm)
        # lattice_points = np.where(self.surface_lattice>=-1)
        # DB_top_points = np.where(self.surface_lattice==0)
        # DB_bottom_points = np.where(self.surface_lattice==1)
        # print(lattice_points)
        fig = plt.figure(figsize=((width_nm + 1) / 10, (height_nm + 1) / 10), dpi=100)
        ax = fig.add_subplot(1, 1, 1)
        plt.gca().invert_yaxis()

        label_list = np.full((np.shape(self.defect_params)[0]), False)

        for y in range(0, self.surface_height):
            for x in range(0, self.surface_width):
                if not label_list[self.surface_lattice[y][x]]:
                    lab = self.defects_name[self.surface_lattice[y][x]]
                    label_list[self.surface_lattice[y][x]] = True
                else:
                    lab = ''

                if y % 2 == 0:
                    plt.scatter(x * self.a1, y * self.a2 + self.b2 / 2,
                                c=self.defect_plotting[self.surface_lattice[y][x]][0],
                                label=lab,
                                marker=self.defect_plotting[self.surface_lattice[y][x]][1])
                elif y % 2 == 1:
                    plt.scatter(x * self.a1, y * self.a2 - self.b2 / 2,
                                c=self.defect_plotting[self.surface_lattice[y][x]][0],
                                label=lab,
                                marker=self.defect_plotting[self.surface_lattice[y][x]][1])

        plt.axis('equal')
        plt.xticks([])
        plt.yticks([])
        plt.legend()
        # print(self.DB_pattern_extended.shape)
        plt.show()

    def save_to_file(self, filename='test.txt'):
        np.savetxt(filename, self.surface_lattice)
        print('file_saved')


surface_width = 740
surface_height = 1090
coverage = 0.005
surface = defect_surface(surface_width=surface_width, surface_height=surface_height)
surface.add_defects(coverage=coverage)
print(surface.surface_lattice)
# surface.draw_panels()
# surface.save_to_file('test.txt')
