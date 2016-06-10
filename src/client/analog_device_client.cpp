#include <time.h>
#include <sstream>
#include <cstdlib>

#include "analog_device.h"
#include "logger.h"
#include "vocab.h"
#include "commac.h"
#include "window.h"


using namespace std;
using namespace csys;
using namespace dev;

window* analogDevice::widget::pWin = &window::instance();

analogDevice::analogDevice(const char *lbl): device(lbl, time::Sec1)
{
  state  = 0;  
  pair<deviceMapIterator, bool> ret;
  ret = deviceMap.insert(pair<string, device*>(label, this));
  if (ret.second==false) 
  {
    cout <<  __func__ << "falied: label " << label << " already exists" << endl;
    exit(1);
  }
}  


analogDevice::~analogDevice()
{
  dLog.getstream(logLevel::DEFAULT, logType::DEVICE) << label << delim 
  <<  __func__ << endl;
  delete pWidget;
}


void analogDevice::process()
{
  if(emit)
  {
    dLog.getstream(logLevel::DDEBUG, logType::DEVICE) << label << delim 
    <<  __func__ << ":command issued: "<< io_cs.command << delim << io_cs.aoh << endl;
  }
}


void analogDevice::serialize()
{
  os << label << delim 
  << io_cs.command << delim 
  << io_cs.aoh << delim;
  dLog.getstream(logLevel::DDEBUG, logType::DEVICE) << label << delim 
  <<  __func__ << delim << io_cs.command << delim << io_cs.aoh << endl;
}


void analogDevice::unserialize()
{
  is >> state
  >> io_cs.error 
  >> io_cs.command 
  >> io_cs.aiv
  >> io_cs.aih
  >> io_cs.aov
  >> io_cs.aoh;
  
  dLog.getstream(logLevel::DDEBUG, logType::DEVICE) << label << delim 
  <<  __func__ << delim
  << io_cs.command << delim
  << state << delim 
  << io_cs.error << delim
  << io_cs.command << delim
  << io_cs.aiv << delim
  << io_cs.aih << delim
  << io_cs.aov << delim
  << io_cs.aoh << endl;
  
  pthread_mutex_lock(glock());
  
  /*TODO===========>*/
  translator();
  io_ps = io_cs;
  
  set_update_flag();
  pthread_mutex_unlock(glock());
}



void analogDevice::build_widget()
{
  pWidget = new widget(this);
}



/********       translate device  RT parameters into readable format for disaplying at Journal      *********/

void analogDevice::translator()
{
  /*   clear stream  */
  hdev_state.clear();
  hdev_state.str(string());
  
  if(io_cs.error != err::NOERR)
  {/*   pass message to journal, error   */
    hdev_state << time_global << delim << label << delim;
    
    translate_error(io_cs.error);
    
    jOur.update(hdev_state.str(), true);
    return;
  }
  
  if(io_cs.command == cmd::NOCMD)
  {/*   pass message to journal, command completed   */
    hdev_state << time_global << delim << label << delim;
    
    translate_command(io_ps.command);
    
    jOur.update(hdev_state.str(), false);
  }
  
}



void analogDevice::translate_error(int err)
{
  if(err){}
}

void analogDevice::translate_command(int com)
{      
  string command;
  
  switch(com)
  {
    case cmd::NOCMD:
      break;
      
    case cmd::INIT:
      command =  vcb::vocab::get()[vcb::init_cmd];
      break;
      
    case cmd::SETP:
      command = vcb::vocab::get()[vcb::set];
      command += delim;
      command += dbl_to_str(io_cs.aoh, 3);
      break;
      
    default:
      break;
  }
  
  hdev_state << command << endl;
}

void analogDevice::translate_state(int com)
{
  if(com){}
  
}

/****************************************     GUI     *****************************************************/


gboolean analogDevice::widget::time_handler(widget* pWid)
{
  if(NULL == pWid)
  {return false;}
  
  analogDevice& dRef = *pWid->pObj;
  
  pthread_mutex_lock(dRef.glock());
  if(!dRef.get_update_flag())
  { 
    pthread_mutex_unlock(dRef.glock());
    return true;
  }
  
  /*   update state textbox   */
  gtk_entry_set_text (GTK_ENTRY(pWid->tboxState), pWid->get_state_string() );
  
  gtk_entry_set_text (GTK_ENTRY(pWid->cvTboxFLT), dbl_to_str(dRef.get_dd().aiv, 4).c_str());
  gtk_entry_set_text (GTK_ENTRY(pWid->cvTboxHMI), dbl_to_str(dRef.get_dd().aih, 4).c_str());
  gtk_entry_set_text (GTK_ENTRY(pWid->spTboxFLT), dbl_to_str(dRef.get_dd().aov, 4).c_str());
  gtk_entry_set_text (GTK_ENTRY(pWid->spTboxHMI), dbl_to_str(dRef.get_dd().aoh, 4).c_str());
  
  gtk_image_set_from_file((GtkImage*)pWid->analogDeviceImage, ntrans("rect_v", dRef.get_state()).c_str());
  
  
  dRef.reset_update_flag();
  pthread_mutex_unlock(dRef.glock());
  
  return true;
}


void analogDevice::widget::scale_changed(GtkAdjustment* get, gpointer p)
{
  if(NULL == get || NULL == p)
  {return;}
  
  analogDevice& dRef = *((widget*)p)->pObj;
  
  pthread_mutex_lock(dRef.glock());
  
  double flow =  dRef.get_dd().flow;
  double uin  = get->value;
  if(uin > flow)
  { uin = flow; }
  
  if(uin < 0.0f)
  { uin = 0.0f; }
  
  dRef.get_dd().aoh = uin;
  gtk_entry_set_text (GTK_ENTRY(dRef.pWidget->spTboxHMI), dbl_to_str(uin, 4).c_str());
  
  pthread_mutex_unlock(dRef.glock()); 
}




void analogDevice::widget::changed(GtkAdjustment* set, gpointer p)
{
  if(NULL == set || NULL == p)
  {return;}
  
  set->value = 0.0;
  gtk_adjustment_set_value (set, 0.0);
}



void analogDevice::widget::sp(GtkWidget* pWid, gpointer p)
{
  if(NULL == pWid || NULL == p)
  {return;}
  
  analogDevice& dRef = *((widget*)p)->pObj;
  
  pthread_mutex_lock(dRef.glock());
  
  double uin = strtof(gtk_entry_get_text (GTK_ENTRY(dRef.pWidget->spTboxHMI)), NULL);
  
  if(0 == uin)
  { dRef.get_dd().command = cmd::INIT; }
  else
  { dRef.get_dd().command = cmd::SETP; }
  
  dRef.set_emit();
  pthread_mutex_unlock(dRef.glock()); 
}


void analogDevice::widget::init(GtkWidget* pWid, gpointer p)
{
  if(NULL == pWid || NULL == p)
  {return;}
  
  analogDevice& dRef = *((widget*)p)->pObj;
  
  pthread_mutex_lock(dRef.glock());
  
  /*  have no idea why, but only odd signals would actually change GtkAdjustment   */ 
  g_signal_emit_by_name( ((widget*)p)->adj, "changed");  
  g_signal_emit_by_name( ((widget*)p)->adj, "changed");  
  
  dRef.get_dd().command = cmd::INIT;
  dRef.set_emit();
  pthread_mutex_unlock(dRef.glock()); 
}


gint analogDevice::widget::delete_event(GtkWidget *pWid, GdkEvent  *event, gpointer data)
{
  if(NULL == pWid || NULL == event || NULL == data)
  {}
  gtk_main_quit ();
  return(FALSE);
}


analogDevice::widget::widget(analogDevice* pDev)
{
  if(!pDev)
  { exit(1); }
  
  pObj = pDev;
  gint rrgImWidth         = pWin->iconWidth;
  gint stateWidth          = pWin->controlsWidth - rrgImWidth - 36;
  gint usize                   = pWin->usize;
  gint buttonWidth        = pWin->buttonWidth;
  gint buttonHeight       = pWin->buttonHeight;
  gint tboxWidth            = stateWidth/2;
  gint tboxHeight           = pWin->tboxHeight;
  
  gint scaleHeight         = 32;
  gint shift                     = 7;
  gint col1                      = 8;
  gint col2                      = col1 + stateWidth/2;
  gint col3                      = col1 + stateWidth - buttonWidth;
  gint col4                      =  stateWidth + 10;
  gint row1                     = 8;
  gint row2                     = row1 + usize + 2;
  gint row3                     = row2 + tboxHeight;
  gint row4                     = row3 + shift + usize;
  gint row5                     = row4 + 16;
  gint im_col = pWin->controlsWidth - rrgImWidth - 26;
  
  frame = gtk_frame_new (NULL);
  gtk_box_pack_start (GTK_BOX (pWin->analogVbox), frame, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_OUT);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 2);
  gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.36);
  
  analogDeviceLabel = gtk_label_new (pObj->get_label().c_str());
  gtk_frame_set_label_widget (GTK_FRAME (frame), analogDeviceLabel);
  gtk_misc_set_alignment (GTK_MISC (analogDeviceLabel), 0, 0);
  
  align = gtk_alignment_new (0, 0, 1, 1);
  gtk_container_add (GTK_CONTAINER (frame), align);
  
  fixed = gtk_fixed_new ();
  gtk_container_add (GTK_CONTAINER (align), fixed);
  
  tboxState = gtk_entry_new ();
  gtk_fixed_put (GTK_FIXED (fixed), tboxState, col1, row1);
  gtk_widget_set_size_request (tboxState, stateWidth, usize);
  gtk_editable_set_editable (GTK_EDITABLE (tboxState), FALSE);
  
  analogDeviceImage = gtk_image_new_from_file(UIPATH"rect_v_disabled.png");
  gtk_fixed_put (GTK_FIXED (fixed), analogDeviceImage, im_col, row1 + 1);
  gtk_misc_set_alignment (GTK_MISC (analogDeviceImage), 0, 0);
  
  /*   first column, second row   */
  cvTboxHMI = gtk_entry_new ();
  gtk_fixed_put (GTK_FIXED (fixed), cvTboxHMI, col1, row2);
  gtk_widget_set_size_request (cvTboxHMI, tboxWidth, tboxHeight);
  gtk_editable_set_editable (GTK_EDITABLE (cvTboxHMI), FALSE);
  
  spTboxHMI = gtk_entry_new ();
  gtk_fixed_put (GTK_FIXED (fixed), spTboxHMI, col1, row3);
  gtk_widget_set_size_request (spTboxHMI, tboxWidth, tboxHeight);
  gtk_editable_set_editable (GTK_EDITABLE (spTboxHMI), FALSE);
  
  cvTboxFLT = gtk_entry_new ();
  gtk_fixed_put (GTK_FIXED (fixed), cvTboxFLT, col2, row2);
  gtk_widget_set_size_request (cvTboxFLT, tboxWidth, tboxHeight);
  gtk_editable_set_editable (GTK_EDITABLE (cvTboxFLT), FALSE);
  
  spTboxFLT = gtk_entry_new ();
  gtk_fixed_put (GTK_FIXED (fixed), spTboxFLT, col2, row3);
  gtk_widget_set_size_request (spTboxFLT, tboxWidth, tboxHeight);
  gtk_editable_set_editable (GTK_EDITABLE (spTboxFLT), FALSE);
  
  vinLabel = gtk_label_new (vcb::vocab::get()[vcb::in]);
  gtk_fixed_put (GTK_FIXED (fixed), vinLabel, col4, row2 + shift);
  
  voutLabel = gtk_label_new (vcb::vocab::get()[vcb::out]);
  gtk_fixed_put (GTK_FIXED (fixed), voutLabel, col4, row3 + shift);
  
  
  gdouble max = pObj->get_dd().flow;
  /* value, lower, upper, step_increment, page_increment, page_size */
  adj = gtk_adjustment_new (0.0, 0.0, max, max/0.01, 0.0, 0.0);
  voutScale = gtk_hscale_new (GTK_ADJUSTMENT (adj));
  gtk_fixed_put (GTK_FIXED (fixed), voutScale, col1, row4);
  gtk_widget_set_size_request (voutScale, stateWidth, scaleHeight);
  
  gint buttBorder = 6;
  initButton = gtk_button_new_with_mnemonic (vcb::vocab::get()[vcb::init_cmd]);
  gtk_fixed_put (GTK_FIXED (fixed), initButton, col3, row5 + shift + usize);
  gtk_widget_set_size_request (initButton, buttonWidth + 2*buttBorder, buttonHeight + 2*buttBorder);
  gtk_container_set_border_width (GTK_CONTAINER (initButton), buttBorder);
  
  setButton = gtk_button_new_with_mnemonic (vcb::vocab::get()[vcb::set]);
  gtk_fixed_put (GTK_FIXED (fixed), setButton, col1, row5 + shift + usize);
  gtk_widget_set_size_request (setButton, buttonWidth + 2*buttBorder, buttonHeight +2*buttBorder);
  gtk_container_set_border_width (GTK_CONTAINER (setButton), buttBorder);
  
  gtk_signal_connect (adj, "changed", G_CALLBACK (changed), (gpointer) this);
  gtk_signal_connect (adj, "value_changed", G_CALLBACK (scale_changed), (gpointer)this);
  gtk_signal_connect (GTK_OBJECT (initButton), "clicked", GTK_SIGNAL_FUNC (init), (gpointer)this);
  gtk_signal_connect (GTK_OBJECT (setButton), "clicked", GTK_SIGNAL_FUNC (sp), (gpointer)this);
  g_timeout_add(200, (GSourceFunc) time_handler, (gpointer) this);
  
  time_handler(this);
}



const char* analogDevice::widget::get_state_string()
{
  return "";
}



