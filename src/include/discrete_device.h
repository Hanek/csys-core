#include "device.h"

#include <iostream>
#include <string.h>

#ifdef CLIENT
#include <gtk/gtk.h>
#endif

#ifndef _DISCRETE_DEVICE_H
#define _DISCRETE_DEVICE_H


namespace csys
{
  #ifdef CLIENT
  class window;
  #endif
  
  namespace dev
  {
    class discreteDevice: public device
    {
      
      /**********************************   ENUMS   *****************************************/  
      public:
        struct err
        { enum error { NOERR, ETMOP, ETMCL , EBNOP, EBNCL }; };
        
        struct cmd
        { enum command { NOCMD, CLOSE, OPEN, INIT }; };
        
        
        /********************************   DEVICE DATA   ************************************/
        private:
          struct dd
          {
            int            error;
            int            command;
            bool         di_open;                               // bit state transfered from di board
            bool         di_closed;                            // bit state transfered from di board 
            bool         do_control;                         // bit state transfered to      do board
            dd(): error(err::NOERR),
            command(cmd::NOCMD),
            di_open(false),
            di_closed(false),
            do_control(false) {}
            bool operator == (const dd& rhs)
            {
              if(error != rhs.error || 
                command != rhs.command ||
                di_open != rhs.di_open || 
                di_closed != rhs.di_closed || 
                do_control != rhs.do_control)
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
      ~discreteDevice();
      void serialize();
      void unserialize();
      bool is_valid() { return (err::NOERR == io_cs.error) ? true : false ; }
      
      #ifdef SERVER
      /**********************************************************************************************/
      private:
        /*   internal methods   */
        void error_handler();
        void request_handler();
        
        int         p_open;             // input bit position connected to "open" pin switch 
        int         p_closed;           // input bit position connected to "closed" pin switch
        int         p_control;          // output bit position connected to "control" pin switch  
        
    public:
      discreteDevice(const char *lbl, int op, int cl, int cntr);
      void process(io &sys_io);
      void close() { io_cs.command = cmd::CLOSE; }
      void open() { io_cs.command = cmd::OPEN; }
      bool is_closed() { return (!io_cs.di_open && io_cs.di_closed) ? true : false; } 
      bool is_open()   { return (io_cs.di_open && !io_cs.di_closed) ? true : false; } 
      #endif
      
      #ifdef CLIENT
      /************************************   WIDGET   *********************************************/
      private:
        class widget
        {
        private:
          discreteDevice* pObj;
          static window*   pWin;
          
          GtkWidget *frame;
          GtkWidget *align;
          GtkWidget *fixed;
          GtkWidget *tboxState;
          GtkWidget *controlLabel;
          GtkWidget *closeButton;
          GtkWidget *initButton;
          GtkWidget *openImage;
          GtkWidget *closedImage;
          GtkWidget *controlImage;
          GtkWidget *errorImage;
          GtkWidget *errorLabel;
          GtkWidget *openButton;
          GtkWidget *openLabel;
          GtkWidget *discreteDeviceImage;
          GtkWidget *closedLabel;
          GtkWidget *discreteDeviceLabel;
          
          static gboolean time_handler(widget* pWid);
          static void close(GtkWidget* pWid, gpointer pVal);
          static void open(GtkWidget* pWid, gpointer pVal);
          static void init(GtkWidget* pWid, gpointer pVal);
          static gint delete_event(GtkWidget *pWid, GdkEvent  *event, gpointer pVal);
          const char* get_state_string();
        public:
          widget(discreteDevice* p);
        };
        
        /***************************************************************************************************/
        private:
          widget*                 pWidget;
    public:
      discreteDevice(const char *lbl);
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






