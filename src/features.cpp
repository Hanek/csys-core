#include <clocale>
#include <pthread.h>
#include <typeinfo>
#include <iostream>
#include <string>
#include <sstream>
#include <queue>
#include <vector>
#include <glib.h>
#include "serializer.h"
#include "protocol.h"

using namespace csys;




void test_serializer()
{ 
  char c = 'a';
  char p[64];
  strcpy(p, "test string");
  float f = 3.141592;
   
  int n = 17;
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
  
  gchar* strEncoded = g_base64_encode((const guchar*)is.buffer_fetch(), is.length());
  is.reset();
//   is.dump();
  std::cout << "encoded: " << strEncoded << std::endl;  
  
  gsize lenDecoded;
  guchar* strDecoded = g_base64_decode(strEncoded, &lenDecoded);
  g_free(strDecoded);
  
  os.buffer_update((const char*)strDecoded, lenDecoded);  
  os.dump();
  
  int ii = 0;
  char cc = 0;
  char pp[64];
  float ff = 0;   
  
  for(int i = 0; i < n + 3; i ++)
  {
    memset(pp, 0x00, sizeof(pp));
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

// gcc `pkg-config --cflags --libs glib-2.0` features.cpp 


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
 test_serializer();
//   test_std_stream();
//   test_std_queue();
  
  return 0;
}