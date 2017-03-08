#include <clocale>
#include <pthread.h>
#include <typeinfo>

#include "serializer.h"

using namespace csys;


#include <vector>

int main()
{ 
  
  int a = 0xffff;
  char b = 'T';
  float c = 3.141592;
  int d = 0xffffffff;
  const char* p = "test string 1";
  const char* str2 = "test string 2";

  int aa, dd;
  char bb;
  float cc;
  char pp[32];
  char pstr2[32];
  
  csys::serializer ss;
  
  ss.serialize<int>(a);  
  ss.serialize<char>(b);
  ss.serialize_cstring(p);
  
  ss.sign_block("bl1");
    
  ss.serialize<float>(c);
  ss.serialize_cstring(str2);
  ss.serialize<int>(d);
  
  ss.sign_block("bl2");
  
  std::cout << "done\n";
//   ss.dump();
  
  ss.reset();
  
  ss.read_block(pp);
  std::cout << "block id: " << pp << " " << ss.len << std::endl;
  ss.deserialize<int>(&aa);
  ss.deserialize<char>(&bb);
  ss.deserialize_cstring(pstr2);
  std::cout << aa << std::endl << bb << std::endl;  
  
  
  
  
  ss.read_block(pp);
  std::cout << "block id: " << pp << " " << ss.len << std::endl;
  ss.deserialize<float>(&cc);
  ss.deserialize_cstring(pp);
  ss.deserialize<int>(&dd);
 std::cout << cc << std::endl << dd << std::endl;
 std::cout << ss.size << std::endl;
  ss.dump();
  
      
  
  
    
  return 0;
}