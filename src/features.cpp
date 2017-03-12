#include <clocale>
#include <pthread.h>
#include <typeinfo>
#include <iostream>
#include <string>
#include <sstream>



#include "serializer.h"

using namespace csys;


#include <vector>

void test_serializer()
{ 
  char c = 'a';
  char p[64];
  strcpy(p, "test string");
  float f = 3.141592;
  
  int n = 2;
  csys::serializer ss;
  
  for(int i = 0; i < n; i++)
  {
    ss.serialize<int>(i + 0xffff);  
    ss.serialize<char>(c + i);
    ss.serialize_cstring(p);
    ss.serialize<float>(f);
    ss.sign_block("*bl*");
  }
  
  ss.reset();
  ss.dump();
  
  int ii = 0;
  char cc = 0;
  char pp[64] = {0};
  float ff = 0;
  
  
  for(int i = 0; i < n; i ++)
  {
    memset(p, 0x00, sizeof(p));
    ss.read_block(p);
    ss.deserialize<int>(&ii);
    ss.deserialize<char>(&cc);
    ss.deserialize_cstring(pp);
    ss.deserialize<float>(&ff);
  }
  
  std::cout << ii << std::endl;
  std::cout << cc << std::endl;
  std::cout << pp << std::endl;
  std::cout << ff << std::endl;
  
}



void test_std_stream()
{ 
  char c = 'a';
  char p[64];
  strcpy(p, "test_string");
  float f = 3.141592;
  
  int n = 10000000;
  
  std::stringstream os;
  csys::serializer ss;
  
  for(int i = 0; i < n; i++)
  {
    os << 0xffff + i << " " 
       << (char)(c + i) << " "
       << p << " "
       << f << " ";
  }
  
 
  int ii = 0;
  char cc = 0;
  std::string pp;
  float ff = 0;
  
  
  for(int i = 0; i < n; i ++)
  {
    pp.clear();
    os >> ii 
       >> cc
       >> pp 
       >> ff;
  }
  
  std::cout << ii << std::endl;
  std::cout << cc << std::endl;
  std::cout << pp << std::endl;
  std::cout << ff << std::endl;
}





int main()
{
 test_serializer();
  
  return 0;
}