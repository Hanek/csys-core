#include <string>
#include <sstream>
#include <map>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <string.h>


#ifndef _VOCAB_H
#define _VOCAB_H


namespace vcb
{
  

enum list{csys, mnemoSchema, log, plot, 
                  main, lockers, recipe, systema,
                  sources, gases, valves, pumps,
                  opend, closed, control, error,
                  open_cmd, close_cmd, init_cmd, shutter,
                  ETMOP, ETMCL, EBNOP, EBNCL, ECMD, ETMNC,
                  air, argon, oxygen, lnk,
                  TMERR, CSERR, PRERR, 
                  freqHz, thickA, rateAs, tempC,
                  tool, mat, out, in, presMb,
                  ctrl1, ctrl2, middle, ETMMD, EBNMD,
                  SPERR, RGERR, NOLNK, WRERR,
                  full_speed, start_cmd, stop_cmd,
                  ETMON, ETMOF, INERR,
                  set, reset, DEVERR
};
                  
  struct vocab:std::map<unsigned int, const char*>
  { 
    vocab(); 
    static vocab& get();
  };
}


#endif



