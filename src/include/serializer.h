#include <cstring>
#include <cstdlib>
#include <iostream>
#include <typeinfo>

#include <fstream>

#ifndef _SERIALIZER_H
#define _SERIALIZER_H

namespace csys
{
  class serializer
  {
    
/*                          block structure
 * 
 *    |_|_|_|_|0|_|_|_|_|_|_|_|_|_|_|_|_|_|_|0|_|_|_|_|_|_|_|_|
 *    | dev_id  |  len  |       |             |               |
 *    | cstring | size_t|  int  |   cstring   |     float     |
 *    |s i g n a t u r e|            m e s s a g e            |
 *
 *    sign - points to the begininng of a block
 *    len  - message length
 *    
 *    there is neither end nor start byte yet..   
 */
    
  public:
    char*  buf;
    
    /* current position */
    char*  pos;
    /* begininng of current block */
    char*  beg;
    /* current message len */
    size_t len;
    
    /* size of buffer */
    size_t size;
    /* length of dev_id header */
    size_t hlen;
  
  public:
    serializer(): size(512), hlen(4) 
    { 
      buf  = (char*)malloc(size);
      if(!buf)
      { /* malloc failed */}
      memset(buf, 0x00, size);                                     
      pos = buf + hlen + sizeof(len); 
      beg = buf;
      len = 0;
    }
    
    void reset() 
    {
      pos = buf + hlen + sizeof(len); 
      beg = buf;
      len = 0;
    }
    
    /* call when device serialization is done */
    void sign_block(const char* id)
    {
      len  = (pos - beg) - hlen - sizeof(len);
      memcpy(beg, id, hlen);
      memcpy(beg + hlen, &len, sizeof(len));
      beg = pos;
      pos += hlen + sizeof(len);
    }
    
    void read_block(char* id)
    {
      if(beg != buf)
      { beg = beg + len + hlen + sizeof(len) + 1; }
      pos = beg + hlen + sizeof(len);
      memcpy(id, beg, hlen);
      memcpy(&len, beg + hlen, sizeof(len));
    }
    
    
    void serialize_cstring(const char* str) 
    { 
      memcpy(pos, str, strlen(str)); 
      pos += strlen(str); 
      *pos = 0x00; 
      pos += 1;
    }
    void deserialize_cstring(char* str)
    { 
      memcpy((void*)str, pos, strlen(pos)); 
      pos += strlen(pos) + 1; 
      
    } 
    
    template <class T> void serialize(T var) 
    { memcpy(pos, &var, sizeof(T)); pos += sizeof(T); } 
    template <class T> void deserialize(T* var) 
    { memcpy((void*)var, pos, sizeof(T)); pos += sizeof(T); }
    
    void dump()
    {
      std::ofstream file("buf.bin", std::ios::out | std::ios::binary);
      file.write(buf, size);
      file.close();
      
    }
    
  };
}
#endif




