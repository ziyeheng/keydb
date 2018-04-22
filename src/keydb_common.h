/***************************************************************************
 *  *  * 
 *   *   *  author:dodng
 *    *    * e-mail:dodng12@163.com
 *     *     *    2018/3/29
 *      *      *   
 *       *       **************************************************************************/

#ifndef KEYDB_COMMON_H_
#define KEYDB_COMMON_H_

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>

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

class AUTO_LOCK
{
    public:
        AUTO_LOCK(pthread_rwlock_t *p_rwlock,bool is_wrlock)
        {
            if (0 == p_rwlock)
            {return;}
            p_lock_ = p_rwlock;

            if (is_wrlock)
            {
                pthread_rwlock_wrlock(p_lock_);
            }
            else
            {
                pthread_rwlock_rdlock(p_lock_);
            }
        }

        ~AUTO_LOCK()
        {
            if (0 != p_lock_)
            {
                pthread_rwlock_unlock(p_lock_);
            }
        }

    private:
        pthread_rwlock_t *p_lock_;
};

// BKDR Hash Function
unsigned int BKDRHash(char *str);

inline std::string int_to_string(int value)
{
    std::string ret_str;
    char value_arr[64] = {0};
    snprintf(value_arr,sizeof(value_arr),"%d",value);
    ret_str = value_arr;
    return ret_str;
}

inline std::string int_to_string(uint32_t value)
{
    std::string ret_str;
    char value_arr[64] = {0};
    snprintf(value_arr,sizeof(value_arr),"%u",value);
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


#endif
