#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <fstream>

namespace imgoffset
{
    int load_file(std::vector<int> &dst_idxs,
                  std::vector<int> &res_idxs)
    {
        std::ifstream bin_file("trans.bin", std::ios::in | std::ios::binary);

        if (!bin_file)
        {
            std::cout << "error" << std::endl;
            return 0;
        }
        unsigned char temp[8];
        int dst_idx;
        int res_idx;
        void *dst_idx_p = &dst_idx;
        void *res_idx_p = &res_idx;
        while (bin_file.read((char *)&temp[0], 8 * sizeof(unsigned char)))
        {
            for (int i = 0; i < 4; i++)
            {
                *((unsigned char *)dst_idx_p + i) = *(temp + i);
                *((unsigned char *)res_idx_p + i) = *(temp + 4 + i);
            }
            dst_idxs.push_back(dst_idx);
            res_idxs.push_back(res_idx);
        }
    }

    int tran_img(unsigned char *res,
                 unsigned char *dst,
                 const std::vector<int> &dst_idxs,
                 const std::vector<int> &res_idxs)
    {
        memset(dst, 0, 2073600);
    }

} // namespace imgoffset
