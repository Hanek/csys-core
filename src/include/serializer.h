/*
 * pod serializer requirements:
 * - allocate memory when needed;
 * - named blocks with pod types defined by user; 
 */ 


#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <iostream>

#include <fstream>

#ifndef _SERIALIZER_H
#define _SERIALIZER_H

namespace csys
{
  class serializer
  {
    
/*  
 *
 *                        block structure
 * 
 *    |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|0|_|_|_|_|_|_|_|_|
 *    | dev_id  |  len  |       |             |               |
 *    | cstring | size_t|  int  |   cstring   |     float     |
 *    |s i g n a t u r e|          b l o c k   b o d y        |
 *
 *    beg  - points to the beginning of the block
 *    len  - message length
 *    hlen - length of dev_id
 */
    
  private:
    int    message_len_;
    char*  buf_;
    bool   head_;
    
    /* current position */
    char*  pos_;
    /* begininng of current block */
    char*  beg_;
    /* current block length */
    size_t block_len_;
    
    /* size of buffer */
    size_t size_;
    /* 
     * length of dev_id header
     * must be the same for each user  
     */
    const size_t hlen_;
    
    void out_of_mem(size_t sizein)
    {
      /* realloc preserving old data */
      
      size_ = (sizein) ? sizein + 1 : 2*size_;
      size_t shift1 = pos_ - buf_;
      size_t shift2 = beg_ - buf_;
      char* buf_new;
      buf_new = (char*)malloc(size_);
      if(!buf_new)
      {
        char err[64] = {0};
        sprintf(err, "[%s] malloc failed: %s\n", __func__, strerror(errno)); 
        fwrite(err, 1, strlen(err), stderr);
      }
      memset(buf_new, 0x00, size_);
      memcpy(buf_new, buf_, size_/2);
      free(buf_);
      buf_ = buf_new;
      pos_ = buf_ + shift1;
      beg_ = buf_ + shift2;
            
//       std::cout << "out_of_mem: " << size_ << std::endl;
    }
  
  public:
    serializer(): size_(32), hlen_(4) 
    { 
      buf_  = (char*)malloc(size_);
      if(!buf_)
      { /* malloc failed */}
      memset(buf_, 0x00, size_);
      message_len_ = 0;
      pos_         = buf_ + hlen_ + sizeof(block_len_); 
      beg_         = buf_;
      block_len_   = 0;
      head_        = true;
    }
    
    ~serializer() { free(buf_); }
    bool empty() { return (0 == size_) ? 1 : 0; }
    size_t get_size() { return size_; }
    size_t length() { return message_len_; }
    size_t get_hlen() { return hlen_; }
    const char* buffer_fetch() { return buf_; }
    
    void buffer_update(const char* bufin, size_t sizein)
    {
      if(size_ <= sizein)
      { out_of_mem(sizein); }
      memcpy(buf_, bufin, sizein);
      message_len_ = (int)sizein;
    }

    void clear()
    {
     reset();
     memset(buf_, 0x00, size_); 
     message_len_ = 0;
    }
    
    void reset() 
    {
      pos_       = buf_ + hlen_ + sizeof(block_len_); 
      beg_       = buf_;
      block_len_ = 0;
      head_      = true;
    }
    
    /* call when device serialization is done */
    void sign_block(const char* id)
    {
      block_len_ = (pos_ - beg_) - hlen_ - sizeof(block_len_);
      message_len_ += (pos_ - beg_);
      memcpy(beg_, id, hlen_);
      memcpy(beg_ + hlen_, &block_len_, sizeof(block_len_));
      beg_ = pos_;
      if(size_ - (pos_ - buf_) <= (hlen_ + sizeof(block_len_)))
      { out_of_mem(0); }
      pos_ += hlen_ + sizeof(block_len_);
    }
    
    bool read_block(char* id)
    {
      if(message_len_ <= pos_ - beg_)
      { return false; }
      if(!head_)
      { beg_ += block_len_ + hlen_ + sizeof(block_len_); }
      
      if(0x00 == *beg_) { return false; }
      pos_ = beg_ + hlen_ + sizeof(block_len_);
      memcpy(id, beg_, hlen_);
      memcpy(&block_len_, beg_ + hlen_, sizeof(block_len_));
      head_ = false;
      
      return true;
    }
    
    
    void serialize_cstring(const char* str) 
    { 
      if(strlen(str) >= size_ - (pos_ - buf_) - 1) 
      { out_of_mem(0); }
      
      memcpy(pos_, str, strlen(str)); 
      pos_ += strlen(str); 
      *pos_ = 0x00; 
      pos_ += 1;
    }
    
    void deserialize_cstring(char* str)
    { 
      memcpy((void*)str, pos_, strlen(pos_)); 
      pos_ += strlen(pos_) + 1; 
    } 
    
    template <class T> void serialize(T var) 
    { 
      if(sizeof(T) >= size_ - (pos_ - buf_)) 
      { 
        out_of_mem(0); 
      }
      
      
      memcpy(pos_, &var, sizeof(T)); 
      pos_ += sizeof(T);
    }
    
    template <class T> void deserialize(T* var) 
    { 
      memcpy((void*)var, pos_, sizeof(T)); 
      pos_ += sizeof(T);
    }
    
    void dump()
    {
      
      std::ofstream file("buf.bin", std::ios::out | std::ios::binary);
      file.write(buf_, size_);
      file.close();
    }
    
  };
}
#endif

