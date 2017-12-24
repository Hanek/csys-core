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
  gint width_, height_, controlsWidth_, controlsHeight_;
         
  gint iconWidth_, iconHeight_, buttonWidth, buttonHeight,
         labelWidth, labelHeight, usize, tboxWidth, tboxHeight;
  
  
  GtkWidget *mainWindow;
  GtkWidget *layoutMain;
  GtkSettings *settings;
  
  /*****************Controls********************/
  
  GtkWidget *controlsNbook;
  
  GtkWidget *systemScrolled;
  GtkWidget *systemViewport;
  GtkWidget *systemVbox;
  GtkWidget *systemLabel;
  
  GtkWidget *analogScrolledWindow;
  GtkWidget *analogViewport;
  GtkWidget *analogVbox;
  GtkWidget *analogLabel;
 
  GtkWidget *discreteScrolledWindow;
  GtkWidget *discreteViewport;
  GtkWidget *discreteVbox;
  GtkWidget *discreteLabel;
  GtkWidget* discreteTable_;
  
  GtkWidget *serialScrolledWindow;
  GtkWidget *serialViewport;
  GtkWidget *serialVbox;
  GtkWidget *serialLabel;

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

