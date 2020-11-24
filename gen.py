#!/usr/bin/env python
# -*- coding:utf-8 -*-

import math
import numpy as np
import struct
import progressbar

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
        z_col = int(round(final_x_data[row][col]) if round(final_x_data[row][col]) < __FINAL_COL else __FINAL_COL - 1)
        z_row = int(round(final_y_data[row][col]) if round(final_y_data[row][col]) < __FINAL_ROW else __FINAL_ROW - 1)
        distance_x = final_x_data[row][col] - z_col
        distance_y = final_y_data[row][col] - z_row
        distance = math.sqrt(distance_x * distance_x + distance_y * distance_y)
        mapping_table[z_row][z_col].append([row, col])
        distance_table[z_row][z_col].append(distance)

c_codes = []
c_codes.append('#include <stdio.h>')
c_codes.append('#include <string.h>')
c_codes.append('int tran_img(unsigned char *res, unsigned char *dst)')
c_codes.append('{')
c_codes.append('    memset(dst, 0, ' + str(__FINAL_ROW * __FINAL_COL) + ');')
save_bin = b''
bar = progressbar.ProgressBar(0, __FINAL_ROW)
bar.start()

for row in range(__FINAL_ROW):
    for col in range(__FINAL_COL):
        if len(distance_table[row][col]) > 0:
            sum_dis = sum(distance_table[row][col])
            min_idx = np.argmin(np.array(distance_table[row][col]))
            dst_idx = row * __FINAL_COL + col
            formula = '    dst[' + str(dst_idx) + '] = '
            res_idx = mapping_table[row][col][min_idx][0] * __ORI_COL + mapping_table[row][col][min_idx][1]
            formula = formula + \
                      'res[' + str(res_idx) + '];'
            # save_bin = save_bin + struct.pack('i', dst_idx)
            # save_bin = save_bin + struct.pack('i', res_idx)
            # for idx in range(len(distance_table[row][col])): 
            #     res_idx = mapping_table[row][col][idx][0] * __ORI_COL + mapping_table[row][col][idx][1]
            #     weight = distance_table[row][col][idx] / sum_dis
            #     formula = formula + \
            #               ' + res[' + str(mapping_table[row][col][idx][0]) + ' * ' + str(__ORI_COL) + ' + ' + str(mapping_table[row][col][idx][1]) + '] * ' + \
            #               str(distance_table[row][col][idx] / sum_dis)
            #     save_bin = save_bin + struct.pack('i', dst_idx)
            #     save_bin = save_bin + struct.pack('i', res_idx)
            #     save_bin = save_bin + struct.pack('f', weight)
            c_codes.append(formula)
    bar.update(row)

c_codes.append('}')

c_code = '\n'.join(c_codes)

with open('gen_c.cpp', 'w') as f:
    f.write(c_code)

with open('trans.bin', 'wb') as f:
    f.write(save_bin)