/***************************************************************************
 *  * 
 *   *  author:dodng
 *    * e-mail:dodng12@163.com
 *     *    2017/3/16
 *      *   
 *       **************************************************************************/

#ifndef COMSE_COMMON_H_
#define COMSE_COMMON_H_

#include <pthread.h>
#include <set>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <sys/time.h>

class AutoLock_Mutex
{
    public:
        AutoLock_Mutex(pthread_mutex_t *p_mutex)
        {
            if (p_mutex == 0 )
                return;
            m_pmutex = p_mutex;
            pthread_mutex_lock(m_pmutex);
        }
        ~AutoLock_Mutex()
        {
            if (m_pmutex == 0)
                return;
            pthread_mutex_unlock(m_pmutex);
        }
    private:
        pthread_mutex_t *m_pmutex;
};

void filter_dup_id(std::vector<uint32_t> & _vec_in, std::vector<uint32_t> & _vec_out);
void filter_dup_id(std::vector<long> & _vec_in, std::vector<long> & _vec_out);
void filter_dup_id2(std::vector<uint32_t> & _vec_in, 
        std::vector<uint32_t> & _vec_out,
        std::vector<int> & _vec_in2, 
        std::vector<int> & _vec_out2);
void filter_dup_id2(std::vector<long> & _vec_in, 
        std::vector<long> & _vec_out,
        std::vector<int> & _vec_in2, 
        std::vector<int> & _vec_out2);

inline std::string long_to_string(long value)
{
    std::string ret_str;
    char value_arr[64] = {0};
    snprintf(value_arr,sizeof(value_arr),"%ld",value);
    ret_str = value_arr;
    return ret_str;
}

inline std::vector<std::string> split(std::string str,std::string pattern)
{
    std::string::size_type pos;
    std::vector<std::string> result;
    str+=pattern;//extern str,so find never return -1
    int size=str.size();

    for(int i=0; i<size; i++)
    {   
        pos=str.find(pattern,i);
        if(pos<size)
        {   
            std::string s=str.substr(i,pos-i);
            if (s.size() > 0)
            {   
                result.push_back(s);
                i=pos+pattern.size()-1;
            }   
        }   
    }   
    return result;
}

inline int my_time_diff(struct timeval &start,struct timeval &end)
{
    if (start.tv_usec <= end.tv_usec)
    {
        return (end.tv_usec-start.tv_usec);
    }
    else
        return (1000000 - start.tv_usec + end.tv_usec);
}

#endif
