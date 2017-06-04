#include <clocale>
#include <pthread.h>
#include <typeinfo>
#include <iostream>
#include <string>
#include <sstream>
#include <queue>

#include "serializer.h"
#include "protocol.h"
#include "queue.h"

using namespace csys;


#include <vector>

void test_serializer()
{ 
  char c = 'a';
  char p[64];
  strcpy(p, "test string");
  float f = 3.141592;
   
  int n = 11;
  csys::serializer is;
  csys::serializer os;
 
  for(int i = 0; i < n; i++)
  {
    is.serialize<int>(i + 0xffff);  
    is.serialize<char>(c + i);
    p[1] = i + 0x30;
    is.serialize_cstring(p);
    is.serialize<float>(f); 
    char block[8];
    strcpy(block, "bl");
    block[strlen(block)] = 0x30 + i;
    is.sign_block(block);
  }
  
  is.reset();
//   is.dump();
  std::cout << "length: " << is.length() << std::endl;  
  
  os.buffer_update(is.buffer_fetch(), is.get_size());  
  os.dump();
  
  int ii = 0;
  char cc = 0;
  char pp[64] = {0};
  float ff = 0;   
  
  for(int i = 0; i < 12; i ++)
  {
    memset(p, 0x00, sizeof(p));
    bool res = os.read_block(p);
    os.deserialize<int>(&ii);
    os.deserialize<char>(&cc);
    os.deserialize_cstring(pp);
    os.deserialize<float>(&ff);
    
    std::cout << "block: " << p << " " << res << ":\t";
    std::cout << ii << "\t";
    std::cout << cc << "\t";
    std::cout << pp << "\t";
    std::cout << ff << std::endl;  
  }
    
 

}   
    
   

void test_std_stream()
{ 
  char h[16];
  strcpy(h, "header 12 bt");
  char c = 'a';
  char p[64];
  strcpy(p, "test_string");
  float f = 3.141592;
  
  int n = 10000000;
  
  std::stringstream os;
   
  for(int i = 0; i < n; i++)
  {
    os << h << " "
       << 0xffff + i << " " 
       << (char)(c + i) << " "
       << p << " "
       << f << " ";
  }
  
  std::cout << "length: " << os.str().length() << std::endl;
  
 
  std::string hh;
  int ii = 0;
  char cc = 0;
  std::string pp;
  float ff = 0;
  
  
  for(int i = 0; i < n; i ++)
  {
    pp.clear();
    os >> hh
       >> ii 
       >> cc
       >> pp 
       >> ff;
  }
  
//   std::cout << ii << std::endl;
//   std::cout << cc << std::endl;
//   std::cout << pp << std::endl;
//   std::cout << ff << std::endl;
}


void test_queue()
{
  int lim = 1000;
  csys::queue qu;
  char p[64];
  strcpy(p, "test_string");
  for(int n = 0; n < lim; n++)
  {
    p[3] = 0x30 + n;
    qu.push(p, 11);
  }
  
//   qu.dump();
  
  for(int n = 0; n < lim; n++)
  {
    char* b;
    int len;
    qu.pop(&b, len);
    if(!b)
    { std::cout << "empty\n"; continue; }
//     std::cout << "node: " << len << "\t" << b << std::endl;
    qu.release_mem(b);
  }
}

void test_std_queue()
{
  int lim = 1000;
  std::queue<std::string> qu;
  
  std::string p = "test string";
  for(int n = 0; n < lim; n++)
  {
    p[3] = 0x30 + n;
    qu.push(p);
  }
 
  
  for(int n = 0; n < lim; n++)
  {
    std::string pp;
    pp = qu.front();
    qu.pop();
//     std::cout << "node: " << pp << std::endl;
  }
}


int main()
{
//  test_serializer();
//   test_std_stream();
//   test_queue();
  test_std_queue();
  
  return 0;
}