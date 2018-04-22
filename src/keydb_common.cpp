
/***************************************************************************
 *  *  * 
 *   *   *  author:dodng
 *    *    * e-mail:dodng12@163.com
 *     *     *    2018/3/29
 *      *      *   
 *       *       **************************************************************************/

#include "keydb_common.h"

unsigned int BKDRHash(char *str)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    while (*str)
    {
        hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);
}
