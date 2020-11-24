#!/usr/bin/env python
# -*- coding:utf-8 -*-

import math
import numpy as np

__FINAL_COL = 1920
__FINAL_ROW = 1080
__ORI_COL = 1920
__ORI_ROW = 720
# offset_data = openpyxl.load_workbook('boundlessJDI.xlsx')

mapping_table = [[[] for col in range(__FINAL_COL)] for row in range(__FINAL_ROW)]
distance_table = [[[] for col in range(__FINAL_COL)] for row in range(__FINAL_ROW)]
offset_x_data = np.array(np.loadtxt(
    'boundlessJDI_x.csv',
    dtype=float,
    delimiter=',',
    skiprows=0,
    encoding='utf-8'))
offset_y_data = np.array(np.loadtxt(
    'boundlessJDI_y.csv',
    dtype=float,
    delimiter=',',
    skiprows=0,
    encoding='utf-8'))

ori_x_data = np.repeat(np.array(range(__ORI_COL)).reshape((1,-1)), repeats=__ORI_ROW, axis=0)
ori_y_data = np.repeat(np.array(range(__ORI_ROW)).reshape((-1,1)), repeats=__ORI_COL, axis=1)

final_x_data = ori_x_data + offset_x_data
final_y_data = ori_y_data + offset_y_data

for row in range(__ORI_ROW):
    for col in range(__ORI_COL):
        z_col = round(final_x_data[row][col]) if round(final_x_data[row][col]) < __FINAL_COL else __FINAL_COL - 1
        z_row = round(final_y_data[row][col]) if round(final_y_data[row][col]) < __FINAL_ROW else __FINAL_ROW - 1
        distance_x = final_x_data[row][col] - z_col
        distance_y = final_y_data[row][col] - z_row
        distance = math.sqrt(distance_x * distance_x + distance_y * distance_y)
        mapping_table[z_row][z_col].append([z_row, z_col])
        distance_table[z_row][z_col].append(distance)

c_codes = []
c_codes.append('#include <stdio.h>')
c_codes.append('#include <string.h>')
c_codes.append('int tran_img(unsigned char *res, unsigned char *dst)')
c_codes.append('{')
c_codes.append('    memset(dst, 0, ' + str(__FINAL_ROW * __FINAL_COL) + ');')

for row in range(__FINAL_ROW):
    for col in range(__FINAL_COL):
        if len(distance_table[row][col]) > 0:
            sum_dis = sum(distance_table[row][col])
            formula = '    dst[' + str(row) + ' * ' + str(__FINAL_COL) + ' + ' + str(col) + '] = (unsigned char)(0'
            for idx in range(len(distance_table[row][col])):
                formula = formula + \
                          ' + res[' + str(mapping_table[row][col][idx][0]) + ' * ' + str(__ORI_COL) + ' + ' + str(mapping_table[row][col][idx][1]) + '] * ' + \
                          str(distance_table[row][col][idx] / sum_dis)
            
            formula = formula + ');'
            c_codes.append(formula)

c_codes.append('}')

c_code = '\n'.join(c_codes)

with open('gen_c.cpp', 'w') as f:
    f.write(c_code)