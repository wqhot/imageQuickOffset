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
# for row in range(__ORI_ROW):
#     for col in range(__ORI_COL):
#         z_col = int(round(final_x_data[row][col]) if round(final_x_data[row][col]) < __FINAL_COL else __FINAL_COL - 1)
#         z_row = int(round(final_y_data[row][col]) if round(final_y_data[row][col]) < __FINAL_ROW else __FINAL_ROW - 1)
#         distance_x = final_x_data[row][col] - z_col
#         distance_y = final_y_data[row][col] - z_row
#         distance = math.sqrt(distance_x * distance_x + distance_y * distance_y)
#         mapping_table[z_row][z_col].append([row, col])
#         distance_table[z_row][z_col].append(distance)

c_codes = []
c_codes.append('#include <stdio.h>')
c_codes.append('#include <string.h>')
c_codes.append('int tran_img(unsigned char *res, unsigned char *dst)')
c_codes.append('{')
c_codes.append('    memset(dst, 0, ' + str(3 * __FINAL_ROW * __FINAL_COL) + ');')
save_bin = b''

dst_idxs = []
res_idxs = []
for idx in range(len(succes_final_row_col[0][0])):

    dst_idx = (succes_final_row_col[0][0][idx] * __FINAL_COL + succes_final_row_col[1][0][idx])
    res_idx = (succus_ori_row_col[0][idx] * __ORI_COL + succus_ori_row_col[1][idx])
    dst_idxs.append(dst_idx)
    res_idxs.append(res_idx)

dst_idxs = np.array(dst_idxs)
res_idxs = np.array(res_idxs)
dst_idxs_moveon_1 = np.concatenate([dst_idxs[1:],np.array([0])], axis=0)
res_idxs_moveon_1 = np.concatenate([res_idxs[1:],np.array([0])], axis=0)

diff_dst = dst_idxs_moveon_1 - dst_idxs
diff_res = res_idxs_moveon_1 - res_idxs

diff_dst_not_1 = np.where(diff_dst != 1)[0]
diff_res_not_1 = np.where(diff_res != 1)[0]

dst_pos = 0
res_pos = 0
start_pos = 0
end_pos = 0

image_size = __FINAL_COL * __FINAL_ROW

formula = '    dst[' + str(dst_idxs[0]) + '] = res[' + str(res_idxs[0]) + '];'
c_codes.append(formula)
formula = '    dst[' + str(dst_idxs[0]+1) + '] = res[' + str(res_idxs[0]+1) + '];'
c_codes.append(formula)
formula = '    dst[' + str(dst_idxs[0]+2) + '] = res[' + str(res_idxs[0]+2) + '];'
c_codes.append(formula)

while dst_pos <diff_dst_not_1.size or res_pos < diff_res_not_1.size:

    # 查找最近的diff pos
    if diff_dst_not_1[dst_pos] < diff_res_not_1[res_pos]:
        end_pos = diff_dst_not_1[dst_pos]
        dst_pos = dst_pos + 1
    elif diff_dst_not_1[dst_pos] > diff_res_not_1[res_pos]:
        end_pos = diff_res_not_1[res_pos]
        res_pos = res_pos + 1
    else:
        end_pos = diff_dst_not_1[dst_pos]
        dst_pos = dst_pos + 1
        res_pos = res_pos + 1
    
    start_dst_idx = dst_idxs[start_pos]
    start_res_idx = res_idxs[start_pos]
    length = end_pos - start_pos
    formula = '    memcpy(dst + ' + str(start_dst_idx * 3 + 3) + \
              ', res + ' + str(start_res_idx * 3 + 3) + ', ' + str(3 * length) + ');'
    c_codes.append(formula)
    # formula = '    memcpy(dst + ' + str(start_dst_idx + 1 + image_size) + \
    #           ', res + ' + str(start_res_idx + 1 + image_size) + ',' + str(length) + ' );'
    # c_codes.append(formula)
    # formula = '    memcpy(dst + ' + str(start_dst_idx + 1 + 2 * image_size) + \
    #           ', res + ' + str(start_res_idx + 1 + 2 * image_size) + ',' + str(length) + ' );'
    c_codes.append(formula)
    start_pos = end_pos



    # formula = '    dst[' + str(dst_idx) + '] = res[' + str(res_idx) + '];'
    # c_codes.append(formula)

    # formula = '    dst[' + str(dst_idx + 1) + '] = res[' + str(res_idx + 1) + '];'
    # c_codes.append(formula)

    # formula = '    dst[' + str(dst_idx + 2) + '] = res[' + str(res_idx + 2) + '];'
    # c_codes.append(formula)

# bar = progressbar.ProgressBar(0, __FINAL_ROW)
# bar.start()

# for row in range(__FINAL_ROW):
#     for col in range(__FINAL_COL):
#         if len(distance_table[row][col]) > 0:
#             sum_dis = sum(distance_table[row][col])
#             min_idx = np.argmin(np.array(distance_table[row][col]))
#             dst_idx = row * __FINAL_COL + col
#             formula = '    dst[' + str(dst_idx) + '] = '
#             res_idx = mapping_table[row][col][min_idx][0] * __ORI_COL + mapping_table[row][col][min_idx][1]
#             formula = formula + \
#                       'res[' + str(res_idx) + '];'
#             # save_bin = save_bin + struct.pack('i', dst_idx)
#             # save_bin = save_bin + struct.pack('i', res_idx)
#             # for idx in range(len(distance_table[row][col])): 
#             #     res_idx = mapping_table[row][col][idx][0] * __ORI_COL + mapping_table[row][col][idx][1]
#             #     weight = distance_table[row][col][idx] / sum_dis
#             #     formula = formula + \
#             #               ' + res[' + str(mapping_table[row][col][idx][0]) + ' * ' + str(__ORI_COL) + ' + ' + str(mapping_table[row][col][idx][1]) + '] * ' + \
#             #               str(distance_table[row][col][idx] / sum_dis)
#             #     save_bin = save_bin + struct.pack('i', dst_idx)
#             #     save_bin = save_bin + struct.pack('i', res_idx)
#             #     save_bin = save_bin + struct.pack('f', weight)
#             c_codes.append(formula)
#     bar.update(row)

c_codes.append('}')

c_code = '\n'.join(c_codes)

with open('gen_c.cpp', 'w') as f:
    f.write(c_code)

with open('trans.bin', 'wb') as f:
    f.write(save_bin)