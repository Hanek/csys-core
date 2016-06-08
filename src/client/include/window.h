#include <map>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#ifndef _WINDOW_H
#define _WINDOW_H

namespace csys
{
  

class window
{
public:
  static window& instance();
  gint width, height, mnemoWidth, mnemoHeight,
         dataWidth, dataHeight, controlsWidth, controlsHeight,
         lockersWidth, lockersHeight;
         
  gint iconWidth, iconHeight, buttonWidth, buttonHeight,
         labelWidth, labelHeight, usize, tboxWidth, tboxHeight;
  
  
  GtkWidget *mainWindow;
  GtkWidget *layoutMain;
  GtkSettings *settings;
  
  /*************MnemoSchema************************/
  GtkWidget *mnemoFrame;
  GtkWidget *mnemoAlignment;
  GtkWidget *mnemoFixed;
  GtkWidget *mnemoLabel;
  
  /****************RT Data*********************/
  
  GtkWidget *rtNbook;
  GtkWidget *logLabel;
  GtkWidget *logVbox;  
  GtkWidget *plotLabel;
  GtkWidget *plotVbox;
  
  /****************Lockers*********************/
  
  GtkWidget *lockersFrame;
  GtkWidget *lockersAlignment;
  GtkWidget *lockersLabel;
  
  /*****************Controls********************/
  
  GtkWidget *controlsNbook;
  
  GtkWidget *systemScrolled;
  GtkWidget *systemViewport;
  GtkWidget *systemVbox;
  GtkWidget *systemLabel;
  
  GtkWidget *gasesScrolledWindow;
  GtkWidget *gasesViewport;
  GtkWidget *gasesVbox;
  GtkWidget *gasesLabel;
 
  GtkWidget *valvesScrolledWindow;
  GtkWidget *valvesViewport;
  GtkWidget *valvesVbox;
  GtkWidget *valvesLabel;
  
  GtkWidget *pumpsScrolledWindow;
  GtkWidget *pumpsViewport;
  GtkWidget *pumpsVbox;
  GtkWidget *pumpsLabel;

  GtkWidget *statsLabel;
  
  friend class Cleanup;
  class Cleanup
  {
  public:
    ~Cleanup();
  };
  
  static void show();
  
protected:
  static window* pInstance;
  
private:
  window();
  virtual ~window();
};



}
#endif

