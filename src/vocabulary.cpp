#include "vocab.h"

vcb::vocab::vocab()
{
  this->operator[](csys) = "Control System";
  this->operator[](mnemoSchema) = "MnemoSchema";
  this->operator[](log) = "Log";
  this->operator[](plot) = "Plot";
  
  this->operator[](main) = "Main";
  this->operator[](lockers) = "Lockers";
  this->operator[](recipe) = "Recipe";
  this->operator[](systema) = "System";
  
  this->operator[](sources) = "Sources";
  this->operator[](gases) = "Gases";
  this->operator[](valves) = "Valves";
  this->operator[](pumps) = "Pumps";
  
  this->operator[](opend) = "Open";
  this->operator[](closed) = "Closed";
  this->operator[](control) = "Control";
  this->operator[](error) = "Error";  
  
  this->operator[](open_cmd) = "Open";
  this->operator[](close_cmd) = "Close";
  this->operator[](init_cmd) = "Init";
  this->operator[](shutter) = "Shutter";
  
  this->operator[](ETMOP) = "Open: exceeded timeout";
  this->operator[](ETMCL) = "Close: exceeded timeout";
  this->operator[](EBNOP) = "Open contact bouncing";
  this->operator[](EBNCL) = "Closed contact bouncing";
  this->operator[](ECMD) = "Invalid command";
  this->operator[](ETMNC) = "Flow is out of range";
  
  this->operator[](air) = "Air";
  this->operator[](argon) = "Argon";
  this->operator[](oxygen) = "Oxygen";
  this->operator[](lnk) = "Link";
  
  this->operator[](TMERR) = "Exceeded timeout";
  this->operator[](CSERR) = "Checksum failure";
  this->operator[](PRERR) = "Parser error";
  
  this->operator[](freqHz) = "Freq Hz";
  this->operator[](thickA) = "Thick A";
  this->operator[](rateAs) = "Rate A/s";
  this->operator[](tempC) = "Temp C";
  this->operator[](tool) = "Tooling";
  this->operator[](mat) = "Material";
  
  this->operator[](out) = "Out";
  this->operator[](in) = "In";
  this->operator[](presMb) = "Pres mb";
  
  this->operator[](ctrl1) = "Control1";
  this->operator[](ctrl2) = "Control2";
  this->operator[](middle) = "Middle";
  this->operator[](ETMMD) = "Middle: exceeded timeout";
  this->operator[](EBNMD) = "Middle contact bouncing";
  this->operator[](SPERR) = "Setpoint doens't match";
  this->operator[](RGERR) = "Value out of range";
  
  this->operator[](NOLNK) = "No link";
  this->operator[](WRERR) = "Write error";
  this->operator[](full_speed) = "Full speed";
  this->operator[] (start_cmd) = "Start";
  this->operator[] (stop_cmd) = "Stop";
  
  this->operator[] (ETMON) = "Enabling: exceeded timeout";
  this->operator[] (ETMOF) = "Disabling: exceeded timeout";
  this->operator[] (INERR) = "Device error";
  
  this->operator[](set) = "Set";
  this->operator[](reset) = "Reset";
  this->operator[](DEVERR) = "Device error";
};


vcb::vocab& vcb::vocab::get()
{
  /*   static initialization order fiasco workaround  */
  static vocab* ans = new vocab();
  return *ans;
}





  
  

  
  
  
