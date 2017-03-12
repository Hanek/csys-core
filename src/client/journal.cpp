#include <iostream>
#include <sstream>
#include <cstdlib>

#include "journal.h"
#include "commac.h"
#include "window.h"

using namespace std;
using namespace csys;

window* journal::widget::pWin = &window::instance();

journal* journal::pInstance = 0;


journal& journal::instance()
{
  static Cleanup cleanup;
  
  if (pInstance == 0)
    pInstance = new journal();
  return *pInstance;
}


journal::Cleanup::~Cleanup()
{
  delete journal::pInstance;
  journal::pInstance = 0;
}



journal::journal()
{
  pthread_mutex_init(&lock, NULL);
  update_flag = false;
} 

journal::~journal()
{
  pthread_mutex_destroy(&lock);
  delete pWidget;
}


void journal::build_widget()
{
  pWidget = new widget(this);
}






void journal::update(const string& buff, bool err)
{
  pthread_mutex_lock(getlock());
  /*   truncate string    */
  size_t pos = buff.find(" ");
  if(0 == buff.length()  || string::npos == pos)
  { pthread_mutex_unlock(getlock()); return; }
  
  data temp;
  temp.strbuf = buff.substr(pos);
  temp.error = err;
  push(temp); 
  
  set_update_flag();
  pthread_mutex_unlock(getlock());
    
}







/****************************************     GUI     *****************************************************/



gboolean journal::widget::time_handler(widget* pJrnl)
{
  if(NULL == pJrnl)
  {return false;}
  
  journal& jRef  = *pJrnl->pObj;
  
  pthread_mutex_lock(jRef.getlock());
  if(!jRef.get_update_flag())
  {
    pthread_mutex_unlock(jRef.getlock());
    return true;
  }
  
  data temp;
  
  while(jRef.pop(temp))
  {
    int res = (temp.error) ? 1 : 0;
    switch(res)
    {
      case 1:
        gtk_text_buffer_insert(pJrnl->buffer, &pJrnl->iter, temp.strbuf.c_str(), -1); 
        break;
        
      case 0:
        gtk_text_buffer_insert_with_tags_by_name(pJrnl->buffer, &pJrnl->iter, temp.strbuf.c_str(), -1, "red_fg", "lmarg",  NULL); 
        break;
    }
    
    gtk_text_buffer_get_end_iter (pJrnl->buffer, &pJrnl->iter);
    gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW (pJrnl->view), &pJrnl->iter, 0.0, FALSE, 0, 0);
    
  }

  jRef.reset_update_flag();
  pthread_mutex_unlock(jRef.getlock());
  
return true;
}


journal::widget::widget(journal* pJrnl)
{

  if(!pJrnl)
  { exit(1); }
  
  pObj = pJrnl;
  view = gtk_text_view_new();
  scrolled = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_add_events(view, GDK_BUTTON_PRESS_MASK);
  gdk_color_parse ("grey", &color);
  gtk_container_add(GTK_CONTAINER(scrolled), view);
  gtk_box_pack_start(GTK_BOX(pWin->logVbox), scrolled, TRUE, TRUE, 0);
  gtk_text_view_set_editable (GTK_TEXT_VIEW(view), FALSE);
  
  gtk_widget_modify_base (view, GTK_STATE_NORMAL, &color);
  gtk_widget_modify_base (view, GTK_STATE_ACTIVE, &color);
  gtk_widget_modify_base (view, GTK_STATE_PRELIGHT, &color);
  gtk_widget_modify_base (view, GTK_STATE_SELECTED, &color);
  gtk_widget_modify_base (view, GTK_STATE_INSENSITIVE, &color);
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
  
  
  gtk_text_buffer_create_tag(buffer, "gap", "pixels_above_lines", 30, NULL);
  gtk_text_buffer_create_tag(buffer, "lmarg", "left_margin", 5, NULL);
  gtk_text_buffer_create_tag(buffer, "blue_fg", "foreground", "blue", NULL); 
  gtk_text_buffer_create_tag(buffer, "red_fg", "foreground", "red", NULL); 
  gtk_text_buffer_create_tag(buffer, "green_bg", "background", "green", NULL); 
  gtk_text_buffer_create_tag(buffer, "red_bg", "background", "red", NULL); 
  gtk_text_buffer_create_tag(buffer, "italic", "style", PANGO_STYLE_ITALIC, NULL);
  gtk_text_buffer_create_tag(buffer, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);
  
  gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);
  
//   gtk_text_buffer_insert(buffer, &iter, "Plain text\n", -1);
//   gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Colored Text\n", -1, "blue_fg", "lmarg",  NULL);
//   gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, "Text with colored background\n", -1, "lmarg", "green_bg", NULL);
//   gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, "Text in italics\n", -1, "italic", "lmarg",  NULL);  
  gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, "Journal is running...\n", -1, "bold", "lmarg",  NULL);

  g_timeout_add(200, (GSourceFunc) time_handler, (gpointer) this);
//   time_handler(this);
  

};


