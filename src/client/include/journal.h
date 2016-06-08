#include <iostream>
#include <string>
#include <queue>
#include <gtk/gtk.h>


#ifndef _JOURNAL_H
#define _JOURNAL_H

namespace csys
{
class window;

class journal
{
  
/**********************************   ENUMS   *****************************************/  
public:
struct mod
{ enum mode{ MANUAL, PUMPDOWN, PROCESS }; };
/************************************   WIDGET   *********************************************/
private:
class  widget
{
private:
  journal*            pObj;
  static window* pWin;

  GtkWidget          *scrolled;
  GtkWidget          *view;
  GtkTextBuffer     *buffer;
  GtkTextIter start, end;
  GtkTextIter iter;
  GdkColor color;
  
  static gboolean time_handler(widget*);
public:
  widget(journal*);
};
  
/***************************************************************************************************/


protected:
  static journal* pInstance;
  
friend class Cleanup;
class Cleanup
{
public:
  ~Cleanup();
};
  

private:
  journal();
  virtual ~journal();
  widget*   pWidget;
  
/*   synchronization between main loop and gui done with this lock   */
  pthread_mutex_t lock; 
  /* on everytime unserialize called
   * time_handler() doens't update widget 
   * if update_flag is off      */
  bool  update_flag; 
  
  struct data
  {
    /*  display buffer  */
    std::string strbuf;  
    /*  error message  */  
    bool error;
    /*  message from pumdown  */
    bool pmpd;
    data(): error(false), pmpd(false) {}
  };
  std::queue<data>data_queue;


public:
  static journal& instance();
  
  /*  used to pass messages to journal, ordinary devices  */
  void update(const std::string&, bool);
  
  
  pthread_mutex_t* getlock() { return &lock; }
  bool get_update_flag() { return update_flag; }
  void set_update_flag() { update_flag = true; }
  void reset_update_flag() { update_flag = false; }
  void build_widget();
  
  void push(const data& req) { data_queue.push(req); }
  bool pop(data& req)
  { 
    if(data_queue.empty()) { return false; }
    req = data_queue.front();
    data_queue.pop();
    return true;
  }


};

}
#endif

