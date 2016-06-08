#include <time.h>
#include <sstream>
#include <cstdlib>

#include "tools.h"
#include "logger.h"
#include "vocab.h"
#include "commac.h"
#include "window.h"

using namespace std;
using namespace csys;
using namespace tools;

window* diModule::widget::pWin = &window::instance();
window* doModule::widget::pWin = &window::instance();
window* aiModule::widget::pWin = &window::instance();
window* aoModule::widget::pWin = &window::instance();

diModule::diModule(): device("diModule", time::Sec1), nchan(DI_NCHAN)
{
  pair<deviceMapIterator, bool> ret;
  ret = deviceMap.insert ( pair<string, device*>(label, this));
  if (false == ret.second) 
  {
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE) << label << delim 
    << "ctror: device already exists" <<  endl;
    cout << "ctor: " << label << " already exists" << endl;
    exit(1);
  }
}

diModule::~diModule()
{ delete pWidget; }

void diModule::process()
{
}

void diModule::serialize()
{
}

void diModule::unserialize()
{
  is >> io_cs.DI;
  pthread_mutex_lock(glock());
  set_update_flag();
  pthread_mutex_unlock(glock());
}

void diModule::build_widget()
{ pWidget = new widget(this); }

/****************************************     GUI     *****************************************************/
gboolean diModule::widget::time_handler(widget* pWid)
{
  if(NULL == pWid)
  {return false;}
  
  diModule& iRef = *pWid->pObj;
  
  pthread_mutex_lock(iRef.glock());
  if(!iRef.get_update_flag())
  {
    pthread_mutex_unlock(iRef.glock());
    return true;
  }
  
  for(int i = 0; i < iRef.nchan; i ++)
  {
    if(1 & (iRef.io_cs.DI>>i))
    { gtk_image_set_from_file((GtkImage*)pWid->diImage[i], UIPATH"bs1_green.png"); }
    else
    { gtk_image_set_from_file((GtkImage*)pWid->diImage[i], UIPATH"bs1_grey.png"); }
  }
  
  iRef.reset_update_flag();
  pthread_mutex_unlock(iRef.glock());
  return true;
}


/*        TODO  
 *   REVIEW TO ENSURE PROPER CLEANUP
 */
gint diModule::widget::delete_event(GtkWidget *pWid, GdkEvent  *event, gpointer pVal)
{
  if(NULL == pWid || NULL == event || NULL == pVal)
  {}
  //   valve& valRef = *((widget*)pVal)->pObj;
  //   valRef.~valve();
  cout << "delete_event" << endl;
  
  return(FALSE);
}


diModule::widget::widget(diModule* pDev)
{
  if(!pDev)
  { exit(1); }
   
  pObj = pDev;

  gint usize                   = pWin->usize + 1;
  gdouble alignVal         = 0.5;
  gint col1                      = 8;
  gint row1                     =  8;
  gint row2                     =  30;
  gint row3                     =  50;
  gint row4                     =  72;
  
  frame = gtk_frame_new (NULL);
  gtk_box_pack_start (GTK_BOX (pWin->systemVbox), frame, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_OUT);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 2);
  gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.36);
  
  moduleLabel = gtk_label_new (pObj->get_label().c_str());
  gtk_frame_set_label_widget (GTK_FRAME (frame), moduleLabel);
  gtk_misc_set_alignment (GTK_MISC (moduleLabel), 0, 0);
  
  align = gtk_alignment_new (0, 0, 1, 1);
  gtk_container_add (GTK_CONTAINER (frame), align);
  
  fixed = gtk_fixed_new ();
  gtk_container_add (GTK_CONTAINER (align), fixed);

  int nrow = pObj->nchan/2;
  for(int i = 0; i < nrow; i ++)
  {
    diImage[i] = gtk_image_new_from_file(UIPATH"bs1_grey.png");
    gtk_fixed_put (GTK_FIXED (fixed), diImage[i], col1 + i*usize, row1);
    gtk_misc_set_alignment (GTK_MISC (diImage[i]), alignVal, alignVal);
    gtk_widget_set_size_request (diImage[i], usize, usize);
    
    diLabel[i] = gtk_label_new (int_to_str(i).c_str());
    gtk_fixed_put (GTK_FIXED (fixed), diLabel[i], col1 + i*usize + 8, row2);
  }
  
  for(int i = 0; i < nrow; i ++)
  {
    diImage[i + nrow] = gtk_image_new_from_file(UIPATH"bs1_grey.png");
    gtk_fixed_put (GTK_FIXED (fixed), diImage[i + nrow], col1 + i*usize, row3);
    gtk_misc_set_alignment (GTK_MISC (diImage[i + nrow]), alignVal, alignVal);
    gtk_widget_set_size_request (diImage[i + nrow], usize, usize);
    
    diLabel[i + nrow] = gtk_label_new (int_to_str(i + nrow).c_str());
    gtk_fixed_put (GTK_FIXED (fixed), diLabel[i + nrow], col1 + i*usize + 8, row4);
  }
  
  g_timeout_add(200, (GSourceFunc) time_handler, (gpointer) this);
  gtk_signal_connect (GTK_OBJECT (pWin->mainWindow), "delete_event", GTK_SIGNAL_FUNC (delete_event), (gpointer)this);
  time_handler(this);
}






/************************************************************************************************/
/*                                                               doModule                                                                        */
/***********************************************************************************************/

doModule::doModule(): device("doModule", time::Sec1), nchan(DO_NCHAN)
{
  pair<deviceMapIterator, bool> ret;
  ret = deviceMap.insert ( pair<string, device*>(label, this));
  if (false == ret.second) 
  {
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE) << label << delim 
    << "ctror: device already exists" <<  endl;
    cout << "ctor: " << label << " already exists" << endl;
    exit(1);
  }
}

doModule::~doModule()
{ delete pWidget; }

void doModule::process()
{
}

void doModule::serialize()
{
}

void doModule::unserialize()
{
  is >> io_cs.DO;
  pthread_mutex_lock(glock());
  set_update_flag();
  pthread_mutex_unlock(glock());
}

void doModule::build_widget()
{ pWidget = new widget(this); }

/****************************************     GUI     *****************************************************/
gboolean doModule::widget::time_handler(widget* pWid)
{
  if(NULL == pWid)
  {return false;}
  
  doModule& iRef = *pWid->pObj;
  
  pthread_mutex_lock(iRef.glock());
  if(!iRef.get_update_flag())
  {
    pthread_mutex_unlock(iRef.glock());
    return true;
  }
  
  for(int i = 0; i < iRef.nchan; i ++)
  {
    if(1 & (iRef.io_cs.DO>>i))
    { gtk_image_set_from_file((GtkImage*)pWid->doImage[i], UIPATH"bs1_green.png"); }
    else
    { gtk_image_set_from_file((GtkImage*)pWid->doImage[i], UIPATH"bs1_grey.png"); }
  }
  
  iRef.reset_update_flag();
  pthread_mutex_unlock(iRef.glock());
  return true;
}


/*        TODO  
 *   REVIEW TO ENSURE PROPER CLEANUP
 */
gint doModule::widget::delete_event(GtkWidget *pWid, GdkEvent  *event, gpointer pVal)
{
  if(NULL == pWid || NULL == event || NULL == pVal)
  {}
  //   valve& valRef = *((widget*)pVal)->pObj;
  //   valRef.~valve();
  cout << "delete_event" << endl;
  
  return(FALSE);
}


doModule::widget::widget(doModule* pDev)
{
  if(!pDev)
  { exit(1); }
  
  pObj = pDev;
  
  gint usize                   = pWin->usize + 1;
  gdouble alignVal         = 0.5;
  gint col1                      = 8;
  gint row1                     =  8;
  gint row2                     =  30;
  gint row3                     =  50;
  gint row4                     =  72;
  
  frame = gtk_frame_new (NULL);
  gtk_box_pack_start (GTK_BOX (pWin->systemVbox), frame, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_OUT);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 2);
  gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.36);
  
  moduleLabel = gtk_label_new (pObj->get_label().c_str());
  gtk_frame_set_label_widget (GTK_FRAME (frame), moduleLabel);
  gtk_misc_set_alignment (GTK_MISC (moduleLabel), 0, 0);
  
  align = gtk_alignment_new (0, 0, 1, 1);
  gtk_container_add (GTK_CONTAINER (frame), align);
  
  fixed = gtk_fixed_new ();
  gtk_container_add (GTK_CONTAINER (align), fixed);
  
  int nrow = pObj->nchan/2;
  for(int i = 0; i < nrow; i ++)
  {
    doImage[i] = gtk_image_new_from_file(UIPATH"bs1_grey.png");
    gtk_fixed_put (GTK_FIXED (fixed), doImage[i], col1 + i*usize, row1);
    gtk_misc_set_alignment (GTK_MISC (doImage[i]), alignVal, alignVal);
    gtk_widget_set_size_request (doImage[i], usize, usize);
    
    doLabel[i] = gtk_label_new (int_to_str(i).c_str());
    gtk_fixed_put (GTK_FIXED (fixed), doLabel[i], col1 + i*usize + 8, row2);
  }
  
  for(int i = 0; i < nrow; i ++)
  {
    doImage[i + nrow] = gtk_image_new_from_file(UIPATH"bs1_grey.png");
    gtk_fixed_put (GTK_FIXED (fixed), doImage[i + nrow], col1 + i*usize, row3);
    gtk_misc_set_alignment (GTK_MISC (doImage[i + nrow]), alignVal, alignVal);
    gtk_widget_set_size_request (doImage[i + nrow], usize, usize);
    
    doLabel[i + nrow] = gtk_label_new (int_to_str(i + nrow).c_str());
    gtk_fixed_put (GTK_FIXED (fixed), doLabel[i + nrow], col1 + i*usize + 8, row4);
  }
  
  g_timeout_add(200, (GSourceFunc) time_handler, (gpointer) this);
  gtk_signal_connect (GTK_OBJECT (pWin->mainWindow), "delete_event", GTK_SIGNAL_FUNC (delete_event), (gpointer)this);
  time_handler(this);
}














/************************************************************************************************/
/*                                                               aiModule                                                                        */
/***********************************************************************************************/

aiModule::aiModule(): device("aiModule", time::Sec1), nchan(AI_NCHAN)
{
  pair<deviceMapIterator, bool> ret;
  ret = deviceMap.insert ( pair<string, device*>(label, this));
  if (false == ret.second) 
  {
    cout <<  __func__ << delim << label << " already exists" << endl;
    exit(1);
  }
}

aiModule::~aiModule()
{ delete pWidget; }

void aiModule::process()
{
}

void aiModule::serialize()
{
}

void aiModule::unserialize()
{  
  for(int ch = 0; ch < nchan; ch++)
  { is >> io_cs.ai[ch]; }
  
  pthread_mutex_lock(glock());
  set_update_flag();
  pthread_mutex_unlock(glock());
}

void aiModule::build_widget()
{ pWidget = new widget(this); }

/****************************************     GUI     *****************************************************/
gboolean aiModule::widget::time_handler(widget* pWid)
{
  if(NULL == pWid)
  {return false;}
  
  aiModule& iRef = *pWid->pObj;
  
  pthread_mutex_lock(iRef.glock());
  if(!iRef.get_update_flag())
  {
    pthread_mutex_unlock(iRef.glock());
    return true;
  }
  
  for(int i = 0; i < iRef.nchan; i ++)
  { gtk_entry_set_text (GTK_ENTRY(pWid->aiTbox[i]), dbl_to_str(iRef.get_dd().ai[i], 2).c_str()); }
  
  iRef.reset_update_flag();
  pthread_mutex_unlock(iRef.glock());
  return true;
}


/*        TODO  
 *   REVIEW TO ENSURE PROPER CLEANUP
 */
gint aiModule::widget::delete_event(GtkWidget *pWid, GdkEvent  *event, gpointer pVal)
{
  if(NULL == pWid || NULL == event || NULL == pVal)
  {}
  //   valve& valRef = *((widget*)pVal)->pObj;
  //   valRef.~valve();
  cout << "delete_event" << endl;
  
  return(FALSE);
}


aiModule::widget::widget(aiModule* pDev)
{
  if(!pDev)
  { exit(1); }
  
  pObj = pDev;
  
  gint tboxWidth            = 50;
  gint tboxHeight           = 22;
  gint usize                    = tboxWidth + 1; 
  gint col1                      = 8;
  gint row1                     =  8;
  gint row2                     =  row1 + tboxHeight + 2;
  
  frame = gtk_frame_new (NULL);
  gtk_box_pack_start (GTK_BOX (pWin->systemVbox), frame, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_OUT);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 2);
  gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.36);
  
  moduleLabel = gtk_label_new (pObj->get_label().c_str());
  gtk_frame_set_label_widget (GTK_FRAME (frame), moduleLabel);
  gtk_misc_set_alignment (GTK_MISC (moduleLabel), 0, 0);
  
  align = gtk_alignment_new (0, 0, 1, 1);
  gtk_container_add (GTK_CONTAINER (frame), align);
  
  fixed = gtk_fixed_new ();
  gtk_container_add (GTK_CONTAINER (align), fixed);
  
  
  for(int i = 0; i < pObj->nchan; i ++)
  {
    aiTbox[i] = gtk_entry_new ();
    gtk_fixed_put (GTK_FIXED (fixed), aiTbox[i], col1 + i*usize, row1);
    gtk_widget_set_size_request (aiTbox[i], tboxWidth, tboxHeight);
    gtk_editable_set_editable (GTK_EDITABLE (aiTbox[i]), FALSE);
    
    aiLabel[i] = gtk_label_new (int_to_str(i).c_str());
    gtk_fixed_put (GTK_FIXED (fixed), aiLabel[i], col1 + tboxWidth/2 + i*usize - 4, row2);
  }
  
  g_timeout_add(200, (GSourceFunc) time_handler, (gpointer) this);
  gtk_signal_connect (GTK_OBJECT (pWin->mainWindow), "delete_event", GTK_SIGNAL_FUNC (delete_event), (gpointer)this);
  time_handler(this);
}






/************************************************************************************************/
/*                                                               aoModule                                                                        */
/***********************************************************************************************/

aoModule::aoModule(): device("aoModule", time::Sec1), nchan(AO_NCHAN)
{
  pair<deviceMapIterator, bool> ret;
  ret = deviceMap.insert ( pair<string, device*>(label, this));
  if (false == ret.second) 
  {
    cout <<  __func__ << delim << label << " already exists" << endl;
    exit(1);
  }
}

aoModule::~aoModule()
{ delete pWidget; }

void aoModule::process()
{
}

void aoModule::serialize()
{
}

void aoModule::unserialize()
{  
  for(int ch = 0; ch < nchan; ch++)
  { is >> io_cs.ao[ch]; }
  
  pthread_mutex_lock(glock());
  set_update_flag();
  pthread_mutex_unlock(glock());
}

void aoModule::build_widget()
{ pWidget = new widget(this); }

/****************************************     GUI     *****************************************************/
gboolean aoModule::widget::time_handler(widget* pWid)
{
  if(NULL == pWid)
  {return false;}
  
  aoModule& iRef = *pWid->pObj;
  
  pthread_mutex_lock(iRef.glock());
  if(!iRef.get_update_flag())
  {
    pthread_mutex_unlock(iRef.glock());
    return true;
  }
  
  for(int i = 0; i < iRef.nchan; i ++)
  { gtk_entry_set_text (GTK_ENTRY(pWid->aoTbox[i]), dbl_to_str(iRef.get_dd().ao[i], 2).c_str()); }
  
  iRef.reset_update_flag();
  pthread_mutex_unlock(iRef.glock());
  return true;
}


/*        TODO  
 *   REVIEW TO ENSURE PROPER CLEANUP
 */
gint aoModule::widget::delete_event(GtkWidget *pWid, GdkEvent  *event, gpointer pVal)
{
  if(NULL == pWid || NULL == event || NULL == pVal)
  {}
  //   valve& valRef = *((widget*)pVal)->pObj;
  //   valRef.~valve();
  cout << "delete_event" << endl;
  
  return(FALSE);
}


aoModule::widget::widget(aoModule* pDev)
{
  if(!pDev)
  { exit(1); }
  
  pObj = pDev;
  
  gint tboxWidth            = 50;
  gint tboxHeight           = 22;
  gint usize                    = tboxWidth + 1; 
  gint col1                      = 8;
  gint row1                     =  8;
  gint row2                     =  row1 + tboxHeight + 2;
  
  frame = gtk_frame_new (NULL);
  gtk_box_pack_start (GTK_BOX (pWin->systemVbox), frame, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_OUT);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 2);
  gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.36);
  
  moduleLabel = gtk_label_new (pObj->get_label().c_str());
  gtk_frame_set_label_widget (GTK_FRAME (frame), moduleLabel);
  gtk_misc_set_alignment (GTK_MISC (moduleLabel), 0, 0);
  
  align = gtk_alignment_new (0, 0, 1, 1);
  gtk_container_add (GTK_CONTAINER (frame), align);
  
  fixed = gtk_fixed_new ();
  gtk_container_add (GTK_CONTAINER (align), fixed);
  
  for(int i = 0; i < pObj->nchan; i ++)
  {
    aoTbox[i] = gtk_entry_new ();
    gtk_fixed_put (GTK_FIXED (fixed), aoTbox[i], col1 + i*usize, row1);
    gtk_widget_set_size_request (aoTbox[i], tboxWidth, tboxHeight);
    gtk_editable_set_editable (GTK_EDITABLE (aoTbox[i]), FALSE);
    
    aoLabel[i] = gtk_label_new (int_to_str(i).c_str());
    gtk_fixed_put (GTK_FIXED (fixed), aoLabel[i], col1 + tboxWidth/2 + i*usize - 4, row2);
  }
  
  
  
  g_timeout_add(200, (GSourceFunc) time_handler, (gpointer) this);
  gtk_signal_connect (GTK_OBJECT (pWin->mainWindow), "delete_event", GTK_SIGNAL_FUNC (delete_event), (gpointer)this);
  time_handler(this);
}
