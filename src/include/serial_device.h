#include "device.h"

#include <iostream>
#include <string.h>
#include <vector>
#include <queue>

#ifdef CLIENT
#include <gtk/gtk.h>
#endif

#ifndef _SERIAL_DEVICE_H
#define _SERIAL_DEVICE_H

namespace csys
{
#ifdef CLIENT
class window;
#endif
  
namespace dev
{
  class serialDevice: public device
  {
    /**********************************   ENUMS   *****************************************/ 
  public:
    struct err
    { enum error { NOERR, NOLNK, WRERR, TMERR, MSERR}; };
    
    struct cmd
    { enum command { NOCMD, POLL, INIT }; };
    /********************************   DEVICE DATA   ************************************/
  private:
    struct dd
    {
      int            error;
      int            command;
      bool          link;
      double      data;
      dd(): error(err::NOERR), 
              command(cmd::NOCMD),
              link(false), data(0.0){}
      bool operator == (const dd& rhs)
      {
        if(error != rhs.error || 
            command != rhs.command ||
            data != rhs.data ||
            link != rhs.link)
           { return false; }
        return true; 
      }
      bool operator != (const dd& rhs)  { return !operator == (rhs);  }
    };
    /**************************************  COMMON  ********************************************/
  private:
    /*  current scan  */  
    struct dd io_cs;    
    /*  previous scan  */
    struct dd io_ps;
            
  public:
    ~serialDevice();
    void serialize();
    void unserialize();
    bool is_valid() { return (err::NOERR == io_cs.error) ? true : false ; }
      
#ifdef SERVER
    /**********************************************************************************************/
  private:
    serial&         port;
    message&   msg;
        
    /*  signal to send, reset by sender immediately  */ 
    bool             send;
    /*  signal to receive, reset by receiver when data available  */
    bool             receive;
    /*   no response signal, reset by error handler and sender  */
    bool             norep;
    char             eot;
    char             dev_addr;  
    char*            request;
    char*            reply;
    std::queue<dd> reqQue;
        
    /*   internal methods   */
    void try_push();
    void try_pop();
    void sender();
    void receiver();
    void error_handler();
    void request_handler();
    void patch();
    void dispatch();
    void set_checksum();
    bool checksum_isok();
        
  public:
    serialDevice(const char* lbl, const char* port, char addr);
    void process(io &sys_io);
    double get_data() { return io_cs.data; }
#endif


#ifdef CLIENT
    /************************************   WIDGET   *********************************************/
  private:
    class widget
        {
        private:
          serialDevice*    pObj;
          static window*  pWin;
          
          GtkWidget *frame;
          GtkWidget *align;
          GtkWidget *fixed;
          GtkWidget *tboxState;
          GtkWidget *presTbox;
          GtkWidget *serialDeviceImage;
          GtkWidget *linkImage;
          GtkWidget *errorImage;
          GtkWidget *linkLabel;
          GtkWidget *errorLabel;
          GtkWidget *serialDeviceLabel;
          GtkWidget *presLabel;
          GtkWidget *initButton;
          
          static gboolean time_handler(widget* pWid);
          static void init(GtkWidget* pWid, gpointer pVal);
          static gint delete_event(GtkWidget *pWid, GdkEvent  *event, gpointer pVal);
          const char* get_state_string();
        public:
          widget(serialDevice* p);
        };
        
        /***************************************************************************************************/
        private:
          widget*      pWidget;
    public:
      serialDevice(const char* lbl);
      void process();
      void build_widget();
      struct dd& get_dd() { return io_cs; }
      
      void translator();
      void translate_error(int);
      void translate_state(int);
      void translate_command(int);
      #endif
      
    };  
    
  }
}
#endif




