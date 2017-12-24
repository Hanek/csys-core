#include <iostream>
#include <string.h>
#include "dev.h"


#ifdef CLIENT
#include <gtk/gtk.h>
#endif

#ifndef _DISCRETE_H
#define _DISCRETE_H


namespace csys
{
  #ifdef CLIENT
  class window;
  #endif

  class dI : public dev
  {
  public:
    struct err{enum error{NOERR, HWERR};};
    struct cmd{enum command {NOCMD};};
        
  private:
    struct data
    {
      char error_;
      bool state_;
      data(): error_(err::NOERR), state_(false) {}
      bool operator == (const data& rhs)
            {
              if(error_ != rhs.error_ || state_ != rhs.state_)
                {return false;}
              return true;
            }
            bool operator != (const data& rhs) {return !operator == (rhs);} 
          
    };
    
  private:
    data cs_;
    data ps_;
            
#ifdef SERVER
        /* input bit position on hw stack */ 
    int pos_;     
    
    void error_handler();
    void request_handler();
  public:
    dI(const char *lbl, int position);
    ~dI();
#endif
    
#ifdef CLIENT
  public:
    dI(const char* lbl);
    ~dI();
    void build_widget();
  
  private:
    class widget
    {
    private:
      dI* obj_;
      static window* pWin_;
      
      GtkWidget *frame_;
      GtkWidget *align_;
      GtkWidget *fixed_;
           
      GtkWidget *bitState_;
      GtkWidget *label_;
      
      static gboolean time_handler(widget* pWid);
      static void init(GtkWidget* pWid, gpointer pVal);
      static gint delete_event(GtkWidget *pWid, GdkEvent  *event, gpointer pVal);
      const char* get_state_string();
    
    public:
      widget(dI* p);
    };
  
  private:
    widget* widget_;
    
    
#endif
    void serialize();
    void unserialize();
    bool is_valid() { return (err::NOERR == cs_.error_) ? true : false ; }
    void process();
    struct data& get_data() { return cs_; }
  };
  
  
  
  
  
  class dO
  {
  public:
    struct err{enum error{NOERR, HWERR};};
    struct cmd{enum command {NOCMD, SET, UNSET};};
    
  private:
    struct data
    {
      char error_;
      bool state_;
      data(): error_(err::NOERR), state_(false) {}
      bool operator == (const data& rhs)
            {
              if(error_ != rhs.error_ || state_ != rhs.state_)
                {return false;}
              return true;
            }
            bool operator != (const data& rhs) {return !operator == (rhs);} 
          
    };
  
  private:
    data cs_;
    data ps_;
    
    /* output bit position on hw stack */ 
    int pos_;             
    
    void error_handler();
    void request_handler();
  
  public:
    dO(const char* lbl, int position);
    ~dO();
    void serialize();
    void unserialize();
    bool is_valid() { return (err::NOERR == cs_.error_) ? true : false ; }
    void process();
    
#ifdef CLIENT
    /* CLI for device managment */

#endif
  };
};
#endif






