/*
 * pod serializer requirements:
 * - allocate memory when needed;
 * - named blocks with pod types defined by user; 
 */ 


#include <cstring>
#include <cstdlib>
#include <iostream>

#include <fstream>

#ifndef _SERIALIZER_H
#define _SERIALIZER_H

namespace csys
{
  class serializer
  {
    
/*                          block structure
 * 
 *    |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|0|_|_|_|_|_|_|_|_|
 *    | dev_id  |  len  |       |             |               |
 *    | cstring | size_t|  int  |   cstring   |     float     |
 *    |s i g n a t u r e|            m e s s a g e            |
 *
 *    beg  - points to the begininng of the block
 *    len  - message length
 *    hlen - length of dev_id
 */
    
  private:
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
    const size_t hlen;
  
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
    
    ~serializer() { free(buf); }
    size_t get_size() { return size; }
    const char* buffer_fetch() { return buf; }
    void buffer_update(const char* bufin, size_t sizein)
    {
//       std::cout << __func__ << ": " << size << "\t" << strlen(bufin) << std::endl;
      if(size <= sizein)
      { realloc(sizein); }
      memcpy(buf, bufin, sizein);
    }

    void out_of_mem()
    {
      /* realloc preserving old data */
      
      size *= 2;
      size_t shift1 = pos - buf;
      size_t shift2 = beg - buf;
      char* buf_new;
      buf_new = (char*)malloc(size);
      if(!buf_new)
      { /* malloc failed */}
      memset(buf_new, 0x00, size);
      memcpy(buf_new, buf, size/2);
      free(buf);
      buf = buf_new;
      pos = buf + shift1;
      beg = buf + shift2;
            
      std::cout << "out_of_mem: " << size << std::endl;
    }
    
    void realloc(size_t sizein)
    {
      /* plain realloc  */
      size = sizein;
      char* buf_new;
      buf_new = (char*)malloc(size);
      if(!buf_new)
      { /* malloc failed */}
      free(buf);
      buf = buf_new;
      
      memset(buf, 0x00, size);                                     
      pos = buf + hlen + sizeof(len); 
      beg = buf;
      len = 0;
      head = true;     
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
//       std::cout << "sign_block: " << pos - buf << "\t" << hlen << "\t" << sizeof(len) << std::endl;
      if(size - (pos - buf) <= (hlen + sizeof(len)))
      { out_of_mem(); }
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
//       std::cout << "cstring::remaining space: " << (int)size - (pos - buf) << "\t" << size << "\t" << pos- buf << std::endl;
      
      if(strlen(str) >= size - (pos - buf) - 1) 
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
//       std::cout << "remaining space: " << (int)size - (pos - buf) << "\t" << size << "\t" << pos - buf << std::endl;
      
      if(sizeof(T) >= size - (pos - buf)) 
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
      
      std::ofstream file("buf.bin", std::ios::out | std::ios::binary);
      file.write(buf, size);
      file.close();
      
    }
    
  };
}
#endif

