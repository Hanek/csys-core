#include <clocale>
#include <pthread.h>
#include <typeinfo>
#include <iostream>
#include <string>
#include <sstream>

#include "serializer.h"
#include "protocol.h"


using namespace csys;


#include <vector>

void test_serializer()
{ 
  char c = 'a';
  char p[64];
  strcpy(p, "test string");
  float f = 3.141592;
   
  int n = 10000000;
  csys::serializer is;
  csys::serializer os;
 
  for(int i = 0; i < n; i++)
  {
    is.serialize<int>(i + 0xffff);  
    is.serialize<char>(c + i);
    p[1] = i + 0x30;
    is.serialize_cstring(p);
    is.serialize<float>(f); 
    is.sign_block("*b");
  }
  
  is.reset();  
 // os.buffer_update(is.buffer_fetch(), is.get_size());  
  
 // os.dump();
  
  int ii = 0;
  char cc = 0;
  char pp[64] = {0};
  float ff = 0;
  
  for(int i = 0; i < n; i ++)
  {
    memset(p, 0x00, sizeof(p));
    is.read_block(p);
    is.deserialize<int>(&ii);
    is.deserialize<char>(&cc);
    is.deserialize_cstring(pp);
    is.deserialize<float>(&ff);
//     std::cout << p << ":\t";
//     std::cout << ii << "\t";
//     std::cout << cc << "\t";
//     std::cout << pp << "\t";
//     std::cout << ff << std::endl;
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





int main()
{
//  test_serializer();
//   test_std_stream();
  
  
  return 0;
}