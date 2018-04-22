/***************************************************************************
 *  * 
 *   *  author:dodng
 *    * e-mail:dodng12@163.com
 *     *    2017/3/16
 *      *   
 *       **************************************************************************/

#include "common.h"


void filter_dup_id(std::vector<uint32_t> & _vec_in, std::vector<uint32_t> & _vec_out)
{
    std::set<uint32_t> filter_set;

    for (int i = 0 ; i < _vec_in.size();i++)
    {
        if (filter_set.find(_vec_in[i]) == filter_set.end())
        {
            //not founded
            _vec_out.push_back(_vec_in[i]);
            filter_set.insert(_vec_in[i]);
        }
    }

}

void filter_dup_id(std::vector<long> & _vec_in, std::vector<long> & _vec_out)
{
    std::set<long> filter_set;

    for (int i = 0 ; i < _vec_in.size();i++)
    {
        if (filter_set.find(_vec_in[i]) == filter_set.end())
        {
            //not founded
            _vec_out.push_back(_vec_in[i]);
            filter_set.insert(_vec_in[i]);
        }
    }

}

void filter_dup_id2(std::vector<uint32_t> & _vec_in, 
        std::vector<uint32_t> & _vec_out,
        std::vector<int> & _vec_in2, 
        std::vector<int> & _vec_out2)
{
    std::set<uint32_t> filter_set;

    for (int i = 0 ; i < _vec_in.size();i++)
    {
        if (filter_set.find(_vec_in[i]) == filter_set.end())
        {
            //not founded
            _vec_out.push_back(_vec_in[i]);
            _vec_out2.push_back(_vec_in2[i]);
            filter_set.insert(_vec_in[i]);
        }
    }
}

void filter_dup_id2(std::vector<long> & _vec_in, 
        std::vector<long> & _vec_out,
        std::vector<int> & _vec_in2, 
        std::vector<int> & _vec_out2)
{
    std::set<long> filter_set;

    for (int i = 0 ; i < _vec_in.size();i++)
    {
        if (filter_set.find(_vec_in[i]) == filter_set.end())
        {
            //not founded
            _vec_out.push_back(_vec_in[i]);
            _vec_out2.push_back(_vec_in2[i]);
            filter_set.insert(_vec_in[i]);
        }
    }
}
