#include "db_core.h"
#include "keydb_common.h"
#include <fstream>
#include <stdlib.h>

//--------------tools-------------------
void insert_listnode(hardnode &hnode, std::tr1::unordered_map<uint32_t, listnode_p> &map, uint32_t map_key)
{
    if (map.find(map_key) != map.end())
    {
        listnode * p_lnode = (listnode *)map[map_key];

        while(p_lnode != 0)
        {
            if (p_lnode->_data.key == hnode.key)
            {
                //overwrite same key
                p_lnode->_data = hnode;
                break;
            }
            if (0 == p_lnode->_p_next_node)
            {
                //p_lnode is last node,so new a one
                listnode * p_lnode_new = new listnode();
                //insert a new listnode
                p_lnode_new->_data = hnode;
                p_lnode_new->_p_next_node = 0;
                p_lnode->_p_next_node = p_lnode_new;
                break;
            }
            p_lnode = p_lnode->_p_next_node;
        }
    }
    else
    {
        listnode * p_lnode_new = new listnode();
        //insert a new listnode
        p_lnode_new->_data = hnode;
        p_lnode_new->_p_next_node = 0;
        map[map_key] = (listnode_p)p_lnode_new;
    }

}

void delete_listnode(hardnode &hnode, std::tr1::unordered_map<uint32_t, listnode_p> &map, uint32_t map_key)
{
    if (map.find(map_key) != map.end())
    {
        listnode * p_lnode = (listnode *)map[map_key];
        listnode * p_lnode_prev = (listnode *)map[map_key];

        while(p_lnode != 0)
        {
            if (p_lnode->_data.key == hnode.key)
            {
                if (p_lnode == p_lnode_prev)
                {
                    //first node
                    if (0 == p_lnode->_p_next_node)
                    {//last node,so just one node
                        map.erase(map_key);
                        delete p_lnode;
                    }
                    else
                    {//not last node  
                        map[map_key] = p_lnode->_p_next_node;
                        delete p_lnode;
                    }
                }
                else
                {
                    //not first node
                    p_lnode_prev->_p_next_node = p_lnode->_p_next_node;
                    delete p_lnode;
                }
                break;
            }
            p_lnode_prev = p_lnode;
            p_lnode = p_lnode->_p_next_node;
        }
    }
}

void find_listnode(std::string key, std::tr1::unordered_map<uint32_t, listnode_p> &map, uint32_t map_key, hardnode **pp_hnode)
{
    if (0 == pp_hnode) {return;}

    *pp_hnode = 0;
    if (map.find(map_key) != map.end())
    {
        //founded hash code
        listnode * p_lnode = (listnode *)map[map_key];

        while(p_lnode != 0)
        {
            if (p_lnode->_data.key == key)
            {
                //founded
                *pp_hnode = &(p_lnode->_data);
                break;
            }
            p_lnode = p_lnode->_p_next_node;
        }
    }

}

void delete_list(std::tr1::unordered_map<uint32_t, listnode_p> &map, uint32_t map_key)
{
    if (map.find(map_key) != map.end())
    {
        //founded hash code
        listnode * p_lnode = (listnode *)map[map_key];
        listnode * p_lnode_prev = (listnode *)map[map_key];

        while(p_lnode != 0)
        {
            p_lnode_prev = p_lnode;
            p_lnode = p_lnode->_p_next_node;
            if (0 != p_lnode_prev)
            {
                delete p_lnode_prev;
                p_lnode_prev = 0;
            }
        }
    }

}

void data2str(int type, hardnode & hnode,std::string & str)
{
    str += int_to_string(type) + "\t";
    str += hnode.key + "\t"; 
    str += int_to_string(hnode.file_child_index) + "\t"; 
    str += int_to_string(hnode.offset) + "\t"; 
    str += int_to_string(hnode.length) + "\t"; 
    str += int_to_string(hnode.size); 
}

bool str2data(std::string str, int &type, hardnode & hnode)
{
    std::vector<std::string> split_rst = split(str,"\t");
    if (split_rst.size() != 6)
    {
        return false;
    }
    type = atoi(split_rst[0].c_str());
    hnode.key = split_rst[1];
    hnode.file_child_index = atoi(split_rst[2].c_str());
    hnode.offset = (uint32_t)atol(split_rst[3].c_str());
    hnode.length = atoi(split_rst[4].c_str());
    hnode.size = atoi(split_rst[5].c_str());
    return true;
}
//--------------------------------------
Db_Core::Db_Core(std::string parent_file_, std::string load_file, std::string dump_file)
{
    parent_file = parent_file_;
    _load_file = load_file;
    _dump_file = dump_file;

    child_num = 0;
    data_size = 0;
    data_real_size = 0;
    one_min_unit_size = 1024;
    one_file_size = (long(4) * 1024 * 1024 * 1024);

    pthread_rwlock_init(&map_lock,NULL);
    load_from_file(_load_file);
}

Db_Core::~Db_Core()
{
    pthread_rwlock_destroy(&map_lock);

    for (std::tr1::unordered_map<uint32_t, listnode_p>::iterator it = using_node_map.begin(); 
            it != using_node_map.end(); 
            ++it)
    {
        delete_list(using_node_map, it->first);
        using_node_map.erase(it);
    }

    for (std::tr1::unordered_map<uint32_t, listnode_p>::iterator it = deled_node_map.begin(); 
            it != deled_node_map.end(); 
            ++it)
    {
        delete_list(deled_node_map, it->first);
        deled_node_map.erase(it);
    }
}

int Db_Core::get_key(std::string key,void *val_buff,uint32_t buff_size)
{
    if (key.size() <= 0 ||
            0 == val_buff ||
            buff_size <= 0)
    {return -1;}

    {// enter lock space
        AUTO_LOCK auto_lock(&map_lock, false);

        //1.find
        hardnode * hnode = 0;
        int ret = 0;

        if (find_key(key, &hnode) == true && (hnode != 0 ))
        {
            std::string file = parent_file + "." + int_to_string(hnode->file_child_index);
            if (buff_size > hnode->length)
            {//buff size is enough
                file_io.io_read(file, val_buff, hnode->length, hnode->offset);
                ((char *)val_buff)[hnode->length] = '\0';
            }
            else
            {
                ret = file_io.io_read(file, val_buff, buff_size, hnode->offset);
                ((char *)val_buff)[buff_size - 1] = '\0';
                ret = 1;
            }
        }
        else
        {
            ret = -1;
        }

        return ret;
    }
}

int Db_Core::put_key(std::string key,void *val_buff,uint32_t buff_size)
{
    if (key.size() <= 0 ||
            0 == val_buff ||
            buff_size <= 0)
    {return -1;}

    {// enter lock space
        AUTO_LOCK auto_lock(&map_lock, true);

        //1.find
        hardnode * hnode = 0;
        int ret = 0;

        if (find_key(key, &hnode) == true && (hnode != 0 ))
        {
            //founded
            if (buff_size <= hnode->size)
            {
                std::string file = parent_file + "." + int_to_string(hnode->file_child_index);
                //if can use
                ret = file_io.io_write(file, val_buff, buff_size, hnode->offset);
                hnode->length = buff_size;
            }
            else
            {
                shift_key(key);
                new_key(key, val_buff, buff_size);
            }
        }
        else
        {
            //not founded
            new_key(key, val_buff, buff_size);
        }


        return ret;
    }
}

int Db_Core::del_key(std::string key)
{
    if (key.size() <= 0 )
    {return -1;}

    {// enter lock space
        AUTO_LOCK auto_lock(&map_lock, true);
        int ret = 0;

        if (shift_key(key) == true)
        {
            ret = 0;
        }
        else
        {
            ret = -1;
        }

        return ret;
    }

}

bool Db_Core::shift_key(std::string key)
{
    hardnode * hnode = 0;
    //1.get hash code
    uint32_t hash_code = BKDRHash((char *)key.c_str());
    find_listnode(key, using_node_map, hash_code, &hnode);

    if (hnode == 0)
    {
        //not founded
        return false;
    }
    else
    {
        //insert to  deled_node_map
        insert_listnode(*hnode, deled_node_map, hnode->size);

        //erase from using_node_map
        delete_listnode(*hnode, using_node_map, hash_code);

    }

    return true;
}

bool Db_Core::find_key(std::string key, hardnode ** hnode)
{
    if (0 == hnode) {return false;}
    //1.get hash code
    uint32_t hash_code = BKDRHash((char *)key.c_str());

    //2.find in use
    find_listnode(key, using_node_map, hash_code, hnode);

    if (*hnode == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool Db_Core::new_key(std::string key, void *val_buff, uint32_t buff_size)
{
    //find this file can use
    std::string this_file_name = parent_file + "." + int_to_string(child_num);
    std::map<std::string, db_file_info>::iterator it;
    uint32_t real_val_size;
    uint32_t offset;
    uint32_t hash_code = BKDRHash((char *)key.c_str());

    hardnode hnode;


    real_val_size = ((buff_size / one_min_unit_size) + 1) * one_min_unit_size;
    //1.find from deled_map
    if (deled_node_map.find(real_val_size) != deled_node_map.end())
    {
        listnode * p_lnode = deled_node_map[real_val_size];
        if (0 != p_lnode)
        {
            std::string new_file = parent_file + "." + int_to_string(p_lnode->_data.file_child_index);
            hardnode new_hnode = p_lnode->_data;

            file_io.io_write(new_file, val_buff, buff_size, p_lnode->_data.offset);
            //erase from using_node_map
            delete_listnode(p_lnode->_data, deled_node_map, real_val_size);
            new_hnode.length = buff_size;
            new_hnode.key = key;
            //insert to using_node_map
            insert_listnode(new_hnode, using_node_map, hash_code);
            return true;
        }
    }
    //2.allocate a new one
    it = db_file_map.find(this_file_name);
    if ( it != db_file_map.end())
    {
        //founded
        if (it->second.use_size >= one_file_size)
        {
            //size not enough,add a new file
            child_num++;
            this_file_name = parent_file + "." + int_to_string(child_num);
            offset = 0;
            file_io.io_write(this_file_name, val_buff, buff_size, offset);
            //add using_node_map
            hnode.file_child_index = child_num;
            hnode.offset = offset;
            hnode.length = buff_size;
            hnode.size = real_val_size; 
            hnode.key = key;
            //insert to using_node_map
            insert_listnode(hnode, using_node_map, hash_code);
            //update db_file_map
            db_file_info db_info;
            db_info.db_file = this_file_name;
            db_info.use_size = real_val_size;
            db_file_map[this_file_name] = db_info;
        }
        else
        {
            offset = it->second.use_size;
            file_io.io_write(this_file_name, val_buff, buff_size, offset);
            //add using_node_map
            hnode.file_child_index = child_num;
            hnode.offset = offset;
            hnode.length = buff_size;
            hnode.size = real_val_size; 
            hnode.key = key;
            //insert to using_node_map
            insert_listnode(hnode, using_node_map, hash_code);
            //update db_file_map
            db_file_info & db_info_p = db_file_map[this_file_name];
            db_info_p.use_size += real_val_size;
        }


    }
    else
    {
        offset = 0;

        file_io.io_write(this_file_name, val_buff, buff_size, offset);	
        //add using_node_map
        hnode.file_child_index = child_num;
        hnode.offset = offset;
        hnode.length = buff_size;
        hnode.size = real_val_size;
        hnode.key = key;
        //insert to using_node_map
        insert_listnode(hnode, using_node_map, hash_code);
        //update db_file_map
        db_file_info db_info;
        db_info.db_file = this_file_name;
        db_info.use_size = real_val_size;
        db_file_map[this_file_name] = db_info;

    }

    data_size += buff_size;
    data_real_size += real_val_size;

    return true;
}

int Db_Core::load_from_file(std::string load_file)
{
    std::ifstream is;  
    is.open(load_file.c_str());
    char in_buff[1024*1024] = {0};

    if (!is.is_open())
    {
        return -1;
    }

    while (!is.eof() )  
    {  
        is.getline(in_buff,1024*1024);
        std::string str = in_buff;
        hardnode hnode;
        int type = -1;

        if (str2data(str, type, hnode))
        {
            if (type == 0)
            {
                uint32_t hash_code = BKDRHash((char *)hnode.key.c_str());
                insert_listnode(hnode, using_node_map, hash_code);
            }
            else if (type == 1)
            {
                insert_listnode(hnode, deled_node_map, hnode.size);
            }
        }


    }
    return 0;
}

int Db_Core::dump_to_file(std::string dump_file)
{
    std::ofstream os;  
    os.open(dump_file.c_str());

    if (!os.is_open())
    {
        return -1;
    }
    else
    {// enter lock space
        AUTO_LOCK auto_lock(&map_lock, false);

        for (std::tr1::unordered_map<uint32_t, listnode_p>::iterator it = using_node_map.begin(); 
                it != using_node_map.end(); 
                ++it)
        {
            std::string output_str;
            listnode * p_lnode = (listnode *)it->second;
            listnode * p_lnode_prev = (listnode *)it->second;

            while(p_lnode != 0)
            {
                p_lnode_prev = p_lnode;
                p_lnode = p_lnode->_p_next_node;
                if (0 != p_lnode_prev)
                {
                    output_str.clear();
                    data2str(0, p_lnode_prev->_data, output_str);
                    os << output_str << "\n"; 
                    p_lnode_prev = 0;
                }
            }
        }

        for (std::tr1::unordered_map<uint32_t, listnode_p>::iterator it = deled_node_map.begin(); 
                it != deled_node_map.end(); 
                ++it)
        {
            std::string output_str;
            listnode * p_lnode = (listnode *)it->second;
            listnode * p_lnode_prev = (listnode *)it->second;

            while(p_lnode != 0)
            {
                p_lnode_prev = p_lnode;
                p_lnode = p_lnode->_p_next_node;
                if (0 != p_lnode_prev)
                {
                    output_str.clear();
                    data2str(1, p_lnode_prev->_data, output_str);
                    os << output_str << "\n"; 
                    p_lnode_prev = 0;
                }
            }
        }
        os.close();
        return 0;
    }

}
