#!/usr/bin/env python
# -*- coding:utf-8 -*-

# g++ gen_c.cpp -fPIC -O0 -shared -o libjibian.so
# cp libjibian.so /lib/x86_64-linux-gnu/

import math
import numpy as np
from scipy.spatial import cKDTree
import struct
import progressbar

__FINAL_COL = 1920
__FINAL_ROW = 1080
__ORI_COL = 1920
__ORI_ROW = 720


def do_kdtree(combined_x_y_arrays,points):
    mytree = cKDTree(combined_x_y_arrays)
    dists, indexes = mytree.query(points)
    return dists, indexes

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

final_x_data_f = final_x_data.flatten()
final_y_data_f = final_y_data.flatten()
combined_x_y_arrays = np.dstack([final_x_data_f,final_y_data_f])[0]

final_x_pixel = np.repeat(np.array(range(__FINAL_COL)).reshape((1,-1)), repeats=__FINAL_ROW, axis=0)
final_y_pixel = np.repeat(np.array(range(__FINAL_ROW)).reshape((-1,1)), repeats=__FINAL_COL, axis=1)
combined_x_y_pixel = list(np.dstack([final_x_pixel.ravel(),final_y_pixel.ravel()])[0])

dists, indexes = do_kdtree(combined_x_y_arrays,combined_x_y_pixel)

succus_indexs = indexes[np.where(dists<math.sqrt(2.0))]
succus_ori_row_col = np.unravel_index(succus_indexs, (__ORI_ROW, __ORI_COL))
succes_final_row_col = np.unravel_index(np.where(dists<math.sqrt(2.0)), (__FINAL_ROW, __FINAL_COL))
succes_final_indexs = succes_final_row_col[0] * __FINAL_COL + succes_final_row_col[1]
succes_final_indexs_list = list(succes_final_indexs[0])

final_indexs = np.zeros((__FINAL_ROW * __FINAL_COL, 1))
final_indexs[succes_final_indexs] = 1

c_codes = []
c_codes.append('float offset_mat[' + str(2 * __FINAL_ROW * __FINAL_COL + 1) + '] = {')

bar = progressbar.ProgressBar(0, __FINAL_ROW * __FINAL_COL)
bar.start()
bor_count = 0
bor_str = [str(__ORI_ROW) + ', ',  '0, ']

ori_idx = 0

for idx in range(__FINAL_ROW * __FINAL_COL):
    if final_indexs[idx] == 1: # 有映射关系
        if bor_count > 0:
            c_codes.extend(bor_str * bor_count)
            bor_count = 0
        c_codes.append(str(succus_ori_row_col[0][ori_idx]) + ', ')
        c_codes.append(str(succus_ori_row_col[1][ori_idx]) + ', ')
        ori_idx = ori_idx + 1
    else: # 无映射关系
        bor_count = bor_count + 1
        # c_codes.append(str(__ORI_ROW + 1) + ', ')
        # c_codes.append(str(0) + ', ')
    bar.update(idx)
        
# for row in range(__FINAL_ROW):
#     for col in range(__FINAL_COL):
#         # 有映射关系     
#         if len(np.where(succes_final_indexs == row * __FINAL_COL + col)[0] > 0): 
#             idx_in_succes_final_indexs = int(np.where(succes_final_indexs == row * __FINAL_COL + col)[0])
#             c_codes.append(str(succus_ori_row_col[0][idx_in_succes_final_indexs]) + ', ')
#             c_codes.append(str(succus_ori_row_col[1][idx_in_succes_final_indexs]) + ', ')
#         # 无映射关系
#         else:
#             c_codes.append(str(__ORI_ROW + 1) + ', ')
#             c_codes.append(str(0) + ', ')

c_codes.append('0.0')
c_codes.append('};')
c_code = '\n'.join(c_codes)

with open('gen_offset_mat.cpp', 'w') as f:
    f.write(c_code)
