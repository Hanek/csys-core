#include "device.h"


#include <iostream>
#include <string.h>

#ifdef CLIENT
#include <gtk/gtk.h>
#endif

#ifndef _TOOLS_H
#define _TOOLS_H

#define DI_NCHAN 32
#define DO_NCHAN 32
#define AI_NCHAN 8
#define AO_NCHAN 8

namespace csys
{
#ifdef CLIENT
class window;
#endif


namespace tools
{
  
/************************************************************************************************/
/*                                                               diModule                                                                        */
/***********************************************************************************************/
class diModule: public device
{ /*   32 DI channels card   */
/*************************************   DEVICE DATA   ******************************************/
private:
struct dd
{
  size_t DI;
  bool operator == (const dd& rhs)
  { if(DI != rhs.DI) { return false; } return true; }
  bool operator != (const dd& rhs)  { return !operator == (rhs);  } 
};

/**************************************  COMMON  ********************************************/
private:
  /*  current scan  */  
  struct dd io_cs;    
public:
  diModule();
  ~diModule();
  void serialize();
  void unserialize();
  bool is_valid() { return true; }
  int nchan;

#ifdef SERVER
/**********************************************************************************************/
private:
  /*  previous scan  */
  struct dd io_ps;

public:
  void process(io &sys_io);
#endif

#ifdef CLIENT
/************************************   WIDGET   *********************************************/
private:
class widget
{
private:
  diModule*           pObj;
  static window* pWin;
  
  GtkWidget *frame;
  GtkWidget *align;
  GtkWidget *fixed;
  GtkWidget *diImage[DI_NCHAN];
  GtkWidget *diLabel[DI_NCHAN];
  GtkWidget *moduleLabel;
  
  static gboolean time_handler(widget* pWid);
  static gint delete_event(GtkWidget *pWid, GdkEvent  *event, gpointer pVal);
public:
  widget(diModule* p);
};

/***************************************************************************************************/
private:
  widget* pWidget;
public:
  void process();
  void build_widget();
  struct dd& get_dd() { return io_cs; }
  
  void translator() {}
  void translate_error(int err) {if(err){} }
  void translate_state(int st)  {if(st){} }
  void translate_command(int cm)  {if(cm){} }
#endif
};


/************************************************************************************************/
/*                                                               doModule                                                                        */
/***********************************************************************************************/

class doModule: public device
{ /*   32 DO channels card   */
/*************************************   DEVICE DATA   ******************************************/
private:
struct dd
{
  size_t DO;
  bool operator == (const dd& rhs)
  { if(DO != rhs.DO) { return false; } return true; }
  bool operator != (const dd& rhs)  { return !operator == (rhs);  } 
};

/**************************************  COMMON  ********************************************/
private:
  /*  current scan  */  
  struct dd io_cs;    
public:
  doModule();
  ~doModule();
  void serialize();
  void unserialize();
  bool is_valid() { return true; }
  int nchan;

#ifdef SERVER
/**********************************************************************************************/
private:
  /*  previous scan  */
  struct dd io_ps;

public:
  void process(io &sys_io);
#endif

#ifdef CLIENT
/************************************   WIDGET   *********************************************/
private:
class widget
{
private:
  doModule*           pObj;
  static window* pWin;
  
  GtkWidget *frame;
  GtkWidget *align;
  GtkWidget *fixed;
  GtkWidget *doImage[DO_NCHAN];
  GtkWidget *doLabel[DO_NCHAN];
  GtkWidget *moduleLabel;
  
  static gboolean time_handler(widget* pWid);
  static gint delete_event(GtkWidget *pWid, GdkEvent  *event, gpointer pVal);
public:
  widget(doModule* p);
};

/***************************************************************************************************/
private:
  widget* pWidget;
public:
  void process();
  void build_widget();
  struct dd& get_dd() { return io_cs; }
  
  void translator() {}
  void translate_error(int err) {if(err){} }
  void translate_state(int st)  {if(st){} }
  void translate_command(int cm)  {if(cm){} }
#endif
};






/************************************************************************************************/
/*                                                               aiModule                                                                        */
/***********************************************************************************************/

class aiModule: public device
{ /*   8 channels card   */
  /*************************************   DEVICE DATA   ******************************************/
  private:
    struct dd
    { float ai[AI_NCHAN]; };
    
    /**************************************  COMMON  ********************************************/
    private:
      /*  current scan  */  
      struct dd io_cs;    
public:
  aiModule();
  ~aiModule();
  void serialize();
  void unserialize();
  bool is_valid() { return true; }
  int nchan;
  
  #ifdef SERVER
public:
  void process(io &sys_io);
  #endif
  
  #ifdef CLIENT
  /************************************   WIDGET   *********************************************/
  private:
    class widget
    {
    private:
      aiModule*           pObj;
      static window* pWin;
      
      GtkWidget *frame;
      GtkWidget *align;
      GtkWidget *fixed;
      GtkWidget *aiTbox[AI_NCHAN];
      GtkWidget *aiLabel[AI_NCHAN];
      GtkWidget *moduleLabel;
      
      static gboolean time_handler(widget* pWid);
      static gint delete_event(GtkWidget *pWid, GdkEvent  *event, gpointer pVal);
    public:
      widget(aiModule* p);
    };
    
    /***************************************************************************************************/
    private:
      widget* pWidget;
public:
  void process();
  void build_widget();
  struct dd& get_dd() { return io_cs; }
  
  void translator() {}
  void translate_error(int e) { if(e) {} }
  void translate_state(int s) { if(s) {} }
  void translate_command(int c) { if(c) {} }
  #endif
};




/************************************************************************************************/
/*                                                               aoModule                                                                        */
/***********************************************************************************************/

class aoModule: public device
{ /*   8 channels AO card   */
  /*************************************   DEVICE DATA   ******************************************/
  private:
    struct dd
    { float ao[AO_NCHAN]; };
    
    /**************************************  COMMON  ********************************************/
    private:
      /*  current scan  */  
      struct dd io_cs;    
public:
  aoModule();
  ~aoModule();
  void serialize();
  void unserialize();
  bool is_valid() { return true; }
  int nchan;
  
  #ifdef SERVER
public:
  void process(io &sys_io);
  #endif
  
  #ifdef CLIENT
  /************************************   WIDGET   *********************************************/
  private:
    class widget
    {
    private:
      aoModule*           pObj;
      static window* pWin;
      
      GtkWidget *frame;
      GtkWidget *align;
      GtkWidget *fixed;
      GtkWidget *aoTbox[AO_NCHAN];
      GtkWidget *aoLabel[AO_NCHAN];
      GtkWidget *moduleLabel;
      
      static gboolean time_handler(widget* pWid);
      static gint delete_event(GtkWidget *pWid, GdkEvent  *event, gpointer pVal);
    public:
      widget(aoModule* p);
    };
    
    /***************************************************************************************************/
    private:
      widget* pWidget;
public:
  void process();
  void build_widget();
  struct dd& get_dd() { return io_cs; }
  
  void translator() {}
  void translate_error(int e) { if(e) {} }
  void translate_state(int s) { if(s) {} }
  void translate_command(int c) { if(c) {} }
  #endif
};

}
}
#endif






