/***************************************************************************
 *  *  * 
 *   *   *  author:dodng
 *    *    * e-mail:dodng12@163.com
 *     *     *    2018/3/29
 *      *      *   
 *       *       **************************************************************************/

#ifndef __KEYDB_DB_CORE_H_
#define __KEYDB_DB_CORE_H_


#include "file_io.h"
#include <string>
#include <tr1/unordered_map>
#include <map>
#include <vector>

typedef struct hardnode{
    std::string key;
    uint32_t file_child_index;
    uint32_t offset;
    uint32_t length;
    uint32_t size;
}HARDNODE;

typedef struct listnode{
    hardnode _data;
    listnode *_p_next_node;
}LISTNODE;

typedef listnode * listnode_p;

typedef struct db_file_info{
    std::string db_file;
    uint64_t use_size;
}DB_FILE_INFO;

class Db_Core
{
    public:
        Db_Core(std::string parent_file_, std::string load_file, std::string dump_file);
        ~Db_Core(); //xxx release heap memory resource
        //------lock in-------
        int get_key(std::string key,void *val_buff,uint32_t buff_size);
        int put_key(std::string key,void *val_buff,uint32_t buff_size);
        int del_key(std::string key);
        int load_from_file(std::string load_file);
        int dump_to_file(std::string dump_file);
        //------lock out-------
        //load
        //get_size
        //output
    private:
        //---------------not lock--------------------------------
        bool find_key(std::string key, hardnode ** hnode); 
        bool new_key(std::string key, void *val_buff, uint32_t buff_size); 
        // insert to deled_node_map, delete from using_node_map
        bool shift_key(std::string key);
        //--------------------------------------------------------- 
        std::string parent_file;
        std::string _load_file;
        std::string _dump_file;
        uint32_t child_num;
        uint32_t one_min_unit_size;
        uint64_t one_file_size;
        //--------------------------------------------------------
        //inner data 
        File_Io file_io;
        //hash code->hardnode
        std::tr1::unordered_map<uint32_t, listnode_p> using_node_map;
        //real_size->hardnode
        std::tr1::unordered_map<uint32_t, listnode_p> deled_node_map;
        //record use file size ,if size up to xx to create a new file
        std::map<std::string, db_file_info> db_file_map;
        pthread_rwlock_t map_lock;
        //ext record
        uint64_t data_size;
        uint64_t data_real_size;

};

#endif
