#include "policy_interface.h"
#include "easy_log.h"
#include <string.h>
#include "db_core.h"
#include "common.h"
////////////////////////////////
//notice multithread is safe ? yes,add mutex lock

Json::Reader g_json_reader;
pthread_mutex_t g_json_reader_lock = PTHREAD_MUTEX_INITIALIZER;
Json::FastWriter g_json_writer;
pthread_mutex_t g_json_writer_lock = PTHREAD_MUTEX_INITIALIZER;

//log
extern easy_log g_log;

extern int g_recv_buff;

extern std::string g_index_parent_file; 
extern std::string g_key_load_file; 
extern std::string g_key_dump_file; 

Db_Core *g_db_core_p = 0;

////////////////////////////////

inline std::string int_to_string(int value)
{
    std::string ret_str;
    char value_arr[32] = {0};
    snprintf(value_arr,sizeof(value_arr),"%d",value);
    ret_str = value_arr;
    return ret_str;
}

inline void cook_send_buff(std::string & str,char *buff_p,int buff_len,int &cooked_len)
{
    if (str.size() <= 0
            || 0 == buff_p
            || buff_len <= 0
            || cooked_len < 0)
    {return;}

    if ( (cooked_len + str.size())> buff_len) {return;}

    memcpy(buff_p + cooked_len,str.c_str(),str.size());
    cooked_len += str.size();
}

inline void vector_to_json(std::vector<std::string> &out_vec,std::string key_name,Json::Value &out_json)
{
    //cook return data
    std::string ret_array_str;
    Json::Value ret_json;

    for (int i = 0;i < out_vec.size();i++)
    {
        if (i == 0) {ret_array_str += "[";}

        if (i == (out_vec.size() - 1))
        { ret_array_str += out_vec[i] + "]"; }
        else
        { ret_array_str += out_vec[i] + ","; }
    }

    {//enter auto mutex lock
        AutoLock_Mutex auto_lock0(&g_json_reader_lock);
        if (g_json_reader.parse(ret_array_str, ret_json))
        {
        }
    }

    out_json[key_name] = ret_json;
}

inline void vector_to_json(std::vector<int> &out_vec,std::string key_name,Json::Value &out_json)
{
    //cook return data
    std::string ret_array_str;
    Json::Value ret_json;

    for (int i = 0;i < out_vec.size();i++)
    {
        if (i == 0) {ret_array_str += "[";}

        if (i == (out_vec.size() - 1))
        { ret_array_str += int_to_string(out_vec[i] ) + "]"; }
        else
        { ret_array_str += int_to_string(out_vec[i] ) + ","; }
    }

    {//enter auto mutex lock
        AutoLock_Mutex auto_lock0(&g_json_reader_lock);
        if (g_json_reader.parse(ret_array_str, ret_json))
        {
        }
    }

    out_json[key_name] = ret_json;

}
////////////////////////////////

//true return 0,other return > 0
int policy_entity::parse_in_json()
{
    if (0 == it_http) {return 1;}
    if (!it_http->parse_over())	{return 2;}

    std::map<std::string,std::string>::iterator it;
    //find data field
    it = it_http->body_map.find("data");
    if (it == it_http->body_map.end()) {return 3;}

    std::string data_str = url_decode(it->second);

    {//enter mutex auto lock
        AutoLock_Mutex auto_lock0(&g_json_reader_lock);
        //parse use json
        if (g_json_reader.parse(data_str, json_in)) 
        {
            return 0;
        }
    }
    return 5;
}

//true return 0,other return > 0
int policy_entity::get_out_json(char *recv_buff_p, int recv_buff_len, int thread_id)
{
    //get time
    char log_buff[1024] = {0};
    struct timeval l_time[12] = {0};
    int l_time_pos = 0;

    int ret = -100;
    std::string method;
    std::string key;
    std::string value;
    //parse
    if ( !json_in["method"].isNull()
            && json_in["method"].isString() )
    {
        method = json_in["method"].asString();
    }
    if ( !json_in["key"].isNull()
            && json_in["key"].isString() )
    {
        key = json_in["key"].asString();
    }
    if ( !json_in["value"].isNull()
            && json_in["value"].isString() )
    {
        value = json_in["value"].asString();
    }
    //check
    gettimeofday(&l_time[l_time_pos++],0);

    if (method == "get" && 
            key.size() > 0)
    {
        ret = g_db_core_p->get_key(key, recv_buff_p, recv_buff_len); 
        json_out["ret_key"] = key;
        if (ret >= 0)
        {
            json_out["ret_value_length"] = strlen(recv_buff_p);
            json_out["ret_value"] = recv_buff_p;
        }

    }
    else if (method == "put" && 
            key.size() > 0 &&
            value.size() > 0)
    {
        ret = g_db_core_p->put_key(key, (void *)value.c_str(), value.length());
        json_out["ret_key"] = key;
        if (ret >= 0)
        {
            json_out["ret_value_length"] = value.length();
        }
    }
    else if (method == "delete" &&
            key.size() > 0)
    {
        ret = g_db_core_p->del_key(key);
        json_out["ret_key"] = key;

    }
    else if (method == "dump_key_file")
    {
        ret = g_db_core_p->dump_to_file(g_key_dump_file);
        json_out["ret_dump_file"] = g_key_dump_file;
    }

    gettimeofday(&l_time[l_time_pos++],0);
    snprintf(log_buff,sizeof(log_buff),"Thread[%d][get_out_json]\tprocess=%d"
            ,thread_id
            ,my_time_diff(l_time[0], l_time[1])  
            );
    g_log.write_record(log_buff);

    json_out["ret_code"] = ret;
    return ret;
}

//true return 0,other return > 0
int policy_entity::cook_senddata(char *send_buff_p,int buff_len,int &send_len)
{
    if (0 == it_http || 0 == send_buff_p) {return 1;}
    if (buff_len <= 0 || send_len < 0) {return 3;}

    std::string json_str;
    {//enter mutex auto lock
        AutoLock_Mutex auto_lock0(&g_json_writer_lock);
        json_str = g_json_writer.write(json_out);
    }
    if (json_str.size() <= 3) {return 2;}//null is 3 length
    if (json_str.size() >= buff_len) {return 3;}//json_str is too long

    std::string str = "HTTP/1.1 200 OK\r\nServer: comse\r\n";
    cook_send_buff(str,send_buff_p,buff_len,send_len);
    str = "Content-Length: " + int_to_string(json_str.size()) + "\r\n\r\n";
    cook_send_buff(str,send_buff_p,buff_len,send_len);
    cook_send_buff(json_str,send_buff_p,buff_len,send_len);
    return 0;

}

#if 0
std::string policy_entity::print_all()
{
    std::string rst;
    rst += "json in\n";
    rst += json_in.toStyledString();
    rst += "json out\n";
    rst += json_out.toStyledString();
    return rst;

}
#endif

int policy_entity::do_one_action(http_entity *it_http_p,char *send_buff_p,int buff_len,int &send_len, char *recv_buff_p, int recv_buff_len,int thread_id)
{
    //get time
    char log_buff[1024] = {0};
    struct timeval l_time[12] = {0};
    int l_time_pos = 0;

    int ret1 = 0,ret2 = 0,ret3 = 0;
    gettimeofday(&l_time[l_time_pos++],0);

    reset();
    set_http(it_http_p);
    ret1 = parse_in_json();
    gettimeofday(&l_time[l_time_pos++],0);

    ret2 = get_out_json(recv_buff_p, recv_buff_len, thread_id);
    gettimeofday(&l_time[l_time_pos++],0);

    ret3 = cook_senddata(send_buff_p,buff_len,send_len);
    gettimeofday(&l_time[l_time_pos++],0);

    snprintf(log_buff,sizeof(log_buff),"Thread[%d][do_one_action]\tparse_in_json=%d|get_out_json=%d|cook_senddata=%d"
            ,thread_id
            ,my_time_diff(l_time[0], l_time[1])  
            ,my_time_diff(l_time[1], l_time[2])  
            ,my_time_diff(l_time[2], l_time[3])  
            );
    g_log.write_record(log_buff);

    //if error occur
    if (ret3 != 0)
    {
        std::string str = "HTTP/1.1 200 OK\r\nServer: comse\r\n";
        std::string body_str = "parse_in_json: " + int_to_string(ret1) + "&";
        body_str += "get_out_json: " + int_to_string(ret2) + "&"; 
        body_str += "cook_senddata: " + int_to_string(ret3);
        //jisuan size
        str += "Content-Length: " + int_to_string(body_str.size()) + "\r\n\r\n";

        cook_send_buff(str,send_buff_p,buff_len,send_len);
        cook_send_buff(body_str,send_buff_p,buff_len,send_len);
    }
    return ret3;
}

bool policy_interface_init_once()
{
    g_db_core_p = new Db_Core(g_index_parent_file, g_key_load_file, g_key_dump_file); 
    if (0 == g_db_core_p)
        return false;
    else
        return true;
}
