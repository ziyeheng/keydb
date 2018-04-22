#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "file_io.h"
#include "keydb_common.h"
#include <unistd.h>

File_Io::File_Io()
{
    pthread_mutex_init(&fd_map_lock, NULL);
}

File_Io::~File_Io()
{
    for (std::map<std::string, int>::iterator it=fd_map.begin(); it!=fd_map.end(); ++it)
    {
        close(it->second);
    }
    pthread_mutex_destroy(&fd_map_lock);
}

int File_Io::io_read(std::string file, void *buff, uint32_t buff_size,uint32_t offset)
{
    int file_fd = -1;
    std::map<std::string, int>::iterator it;
    int ret = 0;

    { //enter lock space
        AutoLock_Mutex auto_lock0(&fd_map_lock);
        it = fd_map.find(file);

        if (it != fd_map.end() )
        {
            //founded
            file_fd = it->second; 
        }
        else
        {
            //not founded ,so create a new one
            if ((file_fd = open(file.c_str(), O_RDWR | O_CREAT, S_IRWXU)) < 0) 
            {
                //file already exist
                ret = -1;
                return ret;
            } 

            fd_map.insert ( std::pair<std::string, int>(file, file_fd) );
        }

        //seek to location offset
        if (lseek(file_fd, offset, SEEK_SET) < 0)
        {
            ret = -2;
            return ret;
        }

        //write
        ret = read(file_fd, buff, buff_size);

        if (ret < 0)
        {
            ret = -3;
            return ret;
        }
        else
        {
            return ret;
        }
    } //lock end
}

int File_Io::io_write(std::string file, void *buff, uint32_t buff_size,uint32_t offset)
{
    int file_fd = -1;
    std::map<std::string, int>::iterator it;
    int ret = 0;

    { //enter lock space
        AutoLock_Mutex auto_lock0(&fd_map_lock);
        it = fd_map.find(file);

        if (it != fd_map.end() )
        {
            //founded
            file_fd = it->second; 
        }
        else
        {
            //not founded ,so create a new one
            if ((file_fd = open(file.c_str(), O_RDWR | O_CREAT, S_IRWXU)) < 0) 
            {
                //file already exist
                ret = -1;
                return ret;
            } 

            fd_map.insert ( std::pair<std::string, int>(file, file_fd) );
        }

        //seek to location offset
        if (lseek(file_fd, offset, SEEK_SET) < 0)
        {
            ret = -2;
            return ret;
        }

        //write
        ret = write(file_fd, buff, buff_size);
        //::fsync(file_fd); 
        if (ret < 0)
        {
            ret = -3;
            return ret;
        }
        else
        {
            return ret;
        }
    } // lock end
}

