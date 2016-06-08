// Copyright (C)  2014, 2015, 2016  Alexander Golant
// 
// This file is part of csys project. This project is free 
// software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>


#include "logger.h"
#include <cstdio>
#include <cstdlib>


using namespace std;
using namespace csys;

#define LOGDIR "../logs/"
const char* const logger::fileDev        = LOGDIR"device.log";
const char* const logger::fileCom       = LOGDIR"fifo.log";
const char* const logger::fileClock      = LOGDIR"clock.log";
const char* const logger::fileSerial      = LOGDIR"serial.log";
const char* const logger::fileIO            = LOGDIR"io.log";
const char* const logger::fileRemote   = LOGDIR"remote.log";
const char* const logger::fileJournal    = LOGDIR"journal.data";

logger* logger::pInstance = 0;
csys::time logger::time_global(false);


logger& logger::instance()
{
  static Cleanup cleanup;
  
  if (pInstance == 0)
    pInstance = new logger();
  return *pInstance;
}

logger::Cleanup::~Cleanup()
{
  delete logger::pInstance;
  logger::pInstance = 0;
}



logger::~logger()
{
  devStream.close();
  comStream.close();
  clockStream.close();
  serialStream.close();
  ioStream.close();
  remoteStream.close();
}

logger::logger()
{
  currentLevel = logLevel::DEFAULT;
  devStream.open(fileDev, ios_base::app | ios_base::out);
  comStream.open(fileCom, ios_base::app | ios_base::out);
  clockStream.open(fileClock, ios_base::app | ios_base::out);
  serialStream.open(fileSerial, ios_base::app | ios_base::out);
  ioStream.open(fileIO, ios_base::app | ios_base::out);
  remoteStream.open(fileRemote, ios_base::app | ios_base::out);
  journalStream.open(fileJournal, ios_base::app | ios_base::out);

  if(!devStream.good() || !comStream.good() || 
      !clockStream.good() || !serialStream.good() ||
      !ioStream.good() || !remoteStream.good() || !journalStream.good())
  { 
    cout << "failed to start logger..." << endl;
    exit(1); 
  }  
}   


ostream& logger::getstream(const int restrictedLevel, const int type)
{  
  string stamp;
  string level;
  
  switch (restrictedLevel)
  { /*  has to pass valid stream all the time */
    case logLevel::DEFAULT:
      level = "[DEFAULT] ";
      break;
      
    case logLevel::SPECIAL:
      if(currentLevel != logLevel::SPECIAL || currentLevel == logLevel::DEFAULT)
      { return nullStream; }
      level = "[SPECIAL] ";
      break;
      
    case logLevel::CDEBUG:
      if(currentLevel != logLevel::CDEBUG || currentLevel == logLevel::DEFAULT)
      { return nullStream; }
      level = " [CDEBUG] ";
      break;
      
    case logLevel::FDEBUG:
      if(currentLevel != logLevel::FDEBUG || currentLevel == logLevel::DEFAULT)
      { return nullStream; }
      level = "  [FDEBUG] ";
      break;
      
    case logLevel::DDEBUG:
      if(currentLevel != logLevel::DDEBUG || currentLevel == logLevel::DEFAULT)
      { return nullStream; }
      level = " [DDEBUG] ";
      break;
      
    case logLevel::SDEBUG:
      if(currentLevel != logLevel::SDEBUG || currentLevel == logLevel::DEFAULT)
      { return nullStream; }
      level = " [SDEBUG] ";
      break;
      
    case logLevel::IODEBUG:
      if(currentLevel != logLevel::IODEBUG || currentLevel == logLevel::DEFAULT)
      { return nullStream; }
      level = " [IODEBUG] ";
      break;
      
    case logLevel::EDEBUG:
      if(currentLevel != logLevel::EDEBUG || currentLevel == logLevel::DEFAULT)
      { return nullStream; }
      level = " [EDEBUG] ";
      break;
      
    default:
      level = "[UNDEFINED] ";
      
  }
  
  
  
#ifdef SERVER
  stamp = "[SERVER] ";   
#endif
  
#ifdef CLIENT
  stamp = "[CLIENT]  ";
#endif
  
  
  switch (type)
  {
    case logType::DEVICE:
      return devStream << stamp << "[" << time_global << "] " << level;
      break;
      
    case logType::FIFO:
      return comStream << stamp << "[" << time_global << "] " << level;
      break;
      
    case logType::CLOCK:
      return clockStream << stamp << "[" << time_global << "] " << level;
      break;
      
    case logType::SERIAL:
      return serialStream << stamp << "[" << time_global << "] " << level;
      break;
      
    case logType::IO:
      return ioStream << stamp << "[" << time_global << "] " << level;
      break;
      
    case logType::ETH:
      return remoteStream << stamp << "[" << time_global << "] " << level;
      break;
      
    default:
    {}
  }
  
  return nullStream;
}









