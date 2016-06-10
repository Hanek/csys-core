#include "device.h"
#include <iostream>
#include <string.h>
#include <vector>

#ifdef CLIENT
#include <gtk/gtk.h>
#endif


#ifndef _ANALOG_DEVICE_H
#define _ANALOG_DEVICE_H

namespace csys
{
#ifdef CLIENT
class window;
#endif
  
namespace dev
{
  class analogDevice: public device
  {
    /**********************************   ENUMS   *****************************************/ 
  public:
    struct err
    { enum error { NOERR }; };
    
  struct cmd
    { enum command { NOCMD, INIT, SETP }; };
    /********************************   DEVICE DATA   ************************************/
  private:
    struct dd
    {
      int            error;
      int            command;
      
      float         aiv;      /*       ai voltage                                               */   
      float         aih;     /*        ai human notation                                 */
      
      float         aov;      /*      ao voltage                */
      float         aoh;     /*       ao human notation  */ 

      float         flow;      /*    device "scale" limit   0 ... flow       */ 
      float         vmax;   /*     device "voltage" limit  0 ... vmax   */
      
      dd(): error(err::NOERR),
      command(cmd::NOCMD), 
      flow(200.0f), vmax(5.0f) {}
      bool operator == (const dd& rhs)
      {
        if(error             != rhs.error || 
          command     != rhs.command ||
          aih                != rhs.aih ||
          aoh               != rhs.aoh)
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
    ~analogDevice();
    void serialize();
    void unserialize();
    bool is_valid() { return (err::NOERR == io_cs.error) ? true : false ; }
    
#ifdef SERVER
    /**********************************************************************************************/
  private:
  /*   internal methods   */
    void error_handler();
    void request_handler();
    int     ai_ch;
    int     ao_ch;

  public:
    analogDevice(const char* lbl, int ai, int ao);
    void process(io &sys_io);
    void humanize_ai();
    void humanize_ao();
#endif
    
#ifdef CLIENT
    /************************************   WIDGET   *********************************************/
  private:
    class widget
    {
    private:
      analogDevice* pObj;
      static window* pWin;
      
      GtkWidget *frame;
      GtkWidget *align;
      GtkWidget *fixed;
      
      GtkWidget *analogDeviceImage;
      
      GtkWidget *spTboxHMI;
      GtkWidget *spTboxFLT;
      GtkWidget *cvTboxHMI;
      GtkWidget *cvTboxFLT;
      
      GtkWidget *initButton;
      GtkWidget *setButton;
      GtkWidget *tboxState;
      GtkWidget *voutScale;
      GtkObject *adj;
      
      GtkWidget *voutLabel;
      GtkWidget *vinLabel;
      GtkWidget *analogDeviceLabel;
      
      static gboolean time_handler(widget* pWid);
      static void init(GtkWidget* pWid, gpointer pVal);
      static void sp(GtkWidget* pWid, gpointer pVal);
      static void scale_changed(GtkAdjustment* get, gpointer p);
      static void changed(GtkAdjustment* get, gpointer p);
      static gint delete_event(GtkWidget *pWid, GdkEvent  *event, gpointer pVal);
      const char* get_state_string();
      
    public:
      widget(analogDevice* p);
    };
    /***************************************************************************************************/
  private:
    widget* pWidget;
  public:
    analogDevice(const char* lbl);
    void process();
    void build_widget();
    struct dd& get_dd() { return io_cs; }
    
    void translator();
    void translate_error(int);
    void translate_state(int) ;
    void translate_command(int);
    #endif
    
  };  
  
}
}

#endif



