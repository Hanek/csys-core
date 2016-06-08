#include <time.h>
#include <sstream>
#include <cstdlib>

#include "serial_device.h"
#include "logger.h"
#include "vocab.h"
#include "commac.h"
#include "window.h"


using namespace std;
using namespace csys;
using namespace dev;

window* serialDevice::widget::pWin = &window::instance();

serialDevice::serialDevice(const char* lbl): device(lbl, time::Sec1)
{
  state        = 0;
  io_cs.link = true;
  pair<deviceMapIterator, bool> ret;
  ret = deviceMap.insert ( pair<std::string, device*>(label, this));
  if (ret.second==false) 
  {
    cout <<  __func__ << "falied: label " << label << " already exists" << std::endl;
    exit(1);
  }  
}


serialDevice::~serialDevice()
{
  delete pWidget;
}





void  serialDevice::process()
{
  if(emit)
  {
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE) << label << delim 
    <<  __func__ << ":command issued: " << io_cs.command <<  endl;
  }
}




void serialDevice::serialize()
{
  os << label << delim << io_cs.command << delim;
  dLog.getstream(logLevel::DDEBUG, logType::DEVICE) << label << delim 
  <<  __func__ << delim << io_cs.command << endl;
}



void serialDevice::unserialize()
{
  is >> state 
  >> io_cs.error
  >> io_cs.command
  >> io_cs.data
  >> io_cs.link;
  
  dLog.getstream(logLevel::DDEBUG, logType::DEVICE) << label << delim 
  <<  __func__ << delim
  << state << delim 
  << io_cs.error << delim
  << io_cs.command << delim
  << io_cs.data << delim
  << io_cs.link  << endl;
  
  pthread_mutex_lock(glock());
  
  /*TODO===========>*/
  translator();
  io_ps = io_cs;
  
  set_update_flag();
  pthread_mutex_unlock(glock());
}


void  serialDevice::build_widget()
{
  pWidget = new widget(this);
} 


/********       translate device  RT parameters into readable format for disaplying at Journal      *********/

void serialDevice::translator()
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
  
}



void serialDevice::translate_error(int err)
{
  string error;
  
  switch(err)
  {
    case err::NOERR:
      return;
      
    case err::NOLNK:
      error = vcb::vocab::get()[vcb::NOLNK];
      break;
      
    case err::WRERR:
      error = vcb::vocab::get()[vcb::WRERR];
      break;
      
    case err::TMERR:
      error = vcb::vocab::get()[vcb::TMERR];
      break;
      
    default:
      return;
  }
  
  hdev_state << error << endl;
}


void serialDevice::translate_command(int err)
{
  if(err) {}
  
}



void serialDevice::translate_state(int com)
{
  if(com) {}
}


/****************************************     GUI     *****************************************************/



gboolean serialDevice::widget::time_handler(widget* pWid)
{
  
  if(NULL == pWid)
  {return false;}
  
  serialDevice& thyRef = *pWid->pObj;
  
  pthread_mutex_lock(thyRef.glock());
  if(!thyRef.get_update_flag())
  {
    pthread_mutex_unlock(thyRef.glock());
    return true;
  }
  
  
  /*   update state textbox   */
  gtk_entry_set_text (GTK_ENTRY(pWid->tboxState), pWid->get_state_string());
  
  
  stringstream ss("");
  ss.precision(4);
  ss << scientific;
  ss << thyRef.get_dd().data;
  
  gtk_entry_set_text (GTK_ENTRY(pWid->presTbox), ss.str().c_str());
  
  gtk_image_set_from_file((GtkImage*)pWid->serialDeviceImage, ntrans("rect_v", thyRef.get_state()).c_str());
  
  if(!thyRef.get_dd().error)
  { gtk_image_set_from_file((GtkImage*)pWid->errorImage, UIPATH"bs1_grey.png"); }
  else
  { gtk_image_set_from_file((GtkImage*)pWid->errorImage, UIPATH"bs1_red.png"); }
  
  if(thyRef.get_dd().link)
  { gtk_image_set_from_file((GtkImage*)pWid->linkImage, UIPATH"bs1_green.png"); }
  else
  { gtk_image_set_from_file((GtkImage*)pWid->linkImage, UIPATH"bs1_red.png"); }
  
  thyRef.reset_update_flag();
  pthread_mutex_unlock(thyRef.glock());
  
  return true;
}


void serialDevice::widget::init(GtkWidget* pWid, gpointer vc)
{
  if(NULL == pWid || NULL == vc)
  {return;}
  serialDevice& thyRef = *((widget*)vc)->pObj;
  
  pthread_mutex_lock(thyRef.glock());
  
  thyRef.get_dd().command = cmd::INIT;
  thyRef.set_emit();
  
  pthread_mutex_unlock(thyRef.glock());
}


gint serialDevice::widget::delete_event(GtkWidget *pWid, GdkEvent  *event, gpointer data)
{
  if(NULL == pWid || NULL == event || NULL == data)
  {}
  gtk_main_quit ();
  return(FALSE);
}



serialDevice::widget::widget(serialDevice* pDev)
{
  if(!pDev)
  { exit(1); }
  
  pObj = pDev;
  gint serialDeviceImWidth        = pWin->iconWidth;
  gint usize                   = pWin->usize;
  gint buttonWidth        = pWin->buttonWidth;
  gint buttonHeight       = pWin->buttonHeight;
  gint stateWidth           = pWin->controlsWidth - serialDeviceImWidth - 36;
  gint tboxHeight           = pWin->tboxHeight;
  gint shift                     = 7;
  gdouble alignVal         = 0.5;
  gint col1                      = 8;
  gint col2                      = col1 + tboxHeight;
  gint col3                      = col1 + stateWidth - buttonWidth;
  gint col4                      =  stateWidth + 10;
  gint row1                     = 8;
  gint row2                     = row1 + usize + 2;
  gint step1                    = tboxHeight;
  gint serialDevice_col = pWin->controlsWidth - serialDeviceImWidth - 26;
  
  
  frame = gtk_frame_new (NULL);
  gtk_box_pack_start (GTK_BOX (pWin->gasesVbox), frame, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_OUT);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 2);
  gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.36);
  
  serialDeviceLabel = gtk_label_new (pObj->get_label().c_str());
  gtk_frame_set_label_widget (GTK_FRAME (frame), serialDeviceLabel);
  gtk_misc_set_alignment (GTK_MISC (serialDeviceLabel), 0, 0);
  
  align = gtk_alignment_new (0, 0, 1, 1);
  gtk_container_add (GTK_CONTAINER (frame), align);
  
  fixed = gtk_fixed_new ();
  gtk_container_add (GTK_CONTAINER (align), fixed);
  
  tboxState = gtk_entry_new ();
  gtk_fixed_put (GTK_FIXED (fixed), tboxState, col1, row1);
  gtk_widget_set_size_request (tboxState, stateWidth, usize);
  gtk_editable_set_editable (GTK_EDITABLE (tboxState), FALSE);
  
  serialDeviceImage = gtk_image_new_from_file(UIPATH"rect_v_disabled.png");
  gtk_fixed_put (GTK_FIXED (fixed), serialDeviceImage, serialDevice_col, row1 + 1);
  gtk_misc_set_alignment (GTK_MISC (serialDeviceImage), 0, 0);
  
  /*   first column   */
  linkImage = gtk_image_new_from_file(UIPATH"bs1_grey.png");
  gtk_fixed_put (GTK_FIXED (fixed), linkImage, col1, row2);
  gtk_misc_set_alignment (GTK_MISC (linkImage), alignVal, alignVal);
  gtk_widget_set_size_request (linkImage, usize, usize);
  
  errorImage = gtk_image_new_from_file(UIPATH"bs1_grey.png");
  gtk_fixed_put (GTK_FIXED (fixed), errorImage, col1, row2 + step1);
  gtk_misc_set_alignment (GTK_MISC (errorImage), alignVal, alignVal);
  gtk_widget_set_size_request (errorImage, usize, usize + shift);
  
  /*   second column   */
  linkLabel = gtk_label_new (vcb::vocab::get()[vcb::lnk]);
  gtk_fixed_put (GTK_FIXED (fixed), linkLabel, col2, row2 + shift);
  
  errorLabel = gtk_label_new (vcb::vocab::get()[vcb::error]);
  gtk_fixed_put (GTK_FIXED (fixed), errorLabel, col2, row2 + step1 + shift + 2);
  
  /*   third column   */ 
  presTbox = gtk_entry_new ();
  gtk_fixed_put (GTK_FIXED (fixed), presTbox, col3, row2);
  gtk_widget_set_size_request (presTbox, buttonWidth, tboxHeight);
  gtk_editable_set_editable (GTK_EDITABLE (presTbox), FALSE);
  
  initButton = gtk_button_new_with_mnemonic (vcb::vocab::get()[vcb::init_cmd]);
  gtk_fixed_put (GTK_FIXED (fixed), initButton, col3, row2 + tboxHeight);
  gtk_widget_set_size_request (initButton, buttonWidth, buttonHeight);
  
  /*   fourth column    */
  presLabel = gtk_label_new ("Data");
  gtk_fixed_put (GTK_FIXED (fixed), presLabel, col4, row2 + shift);
  
  gtk_signal_connect (GTK_OBJECT (initButton), "clicked", GTK_SIGNAL_FUNC (init), (gpointer)this);
  g_timeout_add(200, (GSourceFunc) time_handler, (gpointer) this);
  
  time_handler(this);
}



const char* serialDevice::widget::get_state_string()
{
  switch(pObj->get_dd().error)
  {
    case err::NOERR:
      break;
      
    case err::NOLNK:
      return vcb::vocab::get()[vcb::NOLNK];
      
    case err::WRERR:
      return vcb::vocab::get()[vcb::WRERR];
      
    case err::TMERR:
      return vcb::vocab::get()[vcb::TMERR];
      
    default:
      break;
  }
  return "";
} 





