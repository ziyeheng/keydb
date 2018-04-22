/***************************************************************************
 *  *  * 
 *   *   *  author:dodng
 *    *    * e-mail:dodng12@163.com
 *     *     *    2018/3/29
 *      *      *   
 *       *       **************************************************************************/

#ifndef __KEYDB_FILE_IO_H_
#define __KEYDB_FILE_IO_H_

#include <stdint.h>
#include <pthread.h>
#include <string>
#include <map>

class File_Io
{
    public:
        File_Io();
        ~File_Io();
        int io_read(std::string file, void *buff, uint32_t buff_size,uint32_t offset);
        int io_write(std::string file, void *buff, uint32_t buff_size,uint32_t offset);

    private:
        std::map<std::string, int> fd_map;
        pthread_mutex_t fd_map_lock;

};

#endif
