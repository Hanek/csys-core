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
 *    beg  - points to the begininng of a block
 *    len  - message length
 *    
 */
    
  public:
    char*  buf;
    bool   head;
    
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
    serializer(): size(16), hlen(4) 
    { 
      buf  = (char*)malloc(size);
      if(!buf)
      { /* malloc failed */}
      memset(buf, 0x00, size);                                     
      pos = buf + hlen + sizeof(len); 
      beg = buf;
      len = 0;
      head = true;
    }
    
//     ~serializer() { free(buf); }
    
    void out_of_mem()
    {
      /* realloc if memory is over */
      
      size_t shift1 = pos - buf;
      size_t shift2 = beg - buf;
      char buf_new[size] = {0};
      memcpy(buf_new, buf, size);
      free(buf);
      size *= 2;
      buf = (char*)malloc(size);
      if(!buf)
      { /* malloc failed */}
      pos = buf + shift1;
      beg = buf + shift2;
      memset(buf, 0x00, size);
      memcpy(buf, buf_new, size/2);
      
      std::cout << "realloc: " << size << std::endl;
    }
    
    void reset() 
    {
      pos = buf + hlen + sizeof(len); 
      beg = buf;
      len = 0;
      head = true;
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
      if(!head)
      { beg += len + hlen + sizeof(len); }
      pos = beg + hlen + sizeof(len);
      memcpy(id, beg, hlen);
      memcpy(&len, beg + hlen, sizeof(len));
      head = false;
    }
    
    
    void serialize_cstring(const char* str) 
    { 
      std::cout << "cstring::remaining space: " << (int)size - (pos - buf) << std::endl;
      
      if(strlen(str) >= (int)size - (pos - buf) - 1) 
      { out_of_mem(); }
      
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
    {
      std::cout << "remaining space: " << (int)size - (pos - buf) << std::endl;
      
      if(sizeof(T) >= (int)size - (pos - buf)) 
      { 
        out_of_mem(); 
      }
      
      
      memcpy(pos, &var, sizeof(T)); 
      pos += sizeof(T);
    }
    
    template <class T> void deserialize(T* var) 
    { 
      memcpy((void*)var, pos, sizeof(T)); 
      pos += sizeof(T);
    }
    
    void dump()
    {
      
//       std::ofstream file("buf.bin", std::ios::out | std::ios::binary);
//       file.write(buf, size);
//       file.close();
      
    }
    
  };
}
#endif

