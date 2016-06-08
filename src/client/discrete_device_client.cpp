#include <time.h>
#include <sstream>
#include <cstdlib>

#include "discrete_device.h"
#include "logger.h"
#include "vocab.h"
#include "commac.h"
#include "window.h"  


using namespace std;
using namespace csys;
using namespace dev;


window* discreteDevice::widget::pWin = &window::instance();

discreteDevice::discreteDevice(const char *lbl): device(lbl, time::Sec1)
{
  state                   = 0;
  pair<deviceMapIterator, bool> ret;
  ret = deviceMap.insert(pair<string, device*>(label, this));
  if (ret.second==false) 
  {
    cout <<  __func__ << "falied: label " << label << " already exists" << endl;
    exit(1);
  }
}  

discreteDevice::~discreteDevice()
{
  delete pWidget;
}



void discreteDevice::process()
{
  if(emit)
  {
    dLog.getstream(logLevel::DDEBUG, logType::DEVICE) << label << delim 
    <<  __func__ << ":command issued: "<< io_cs.command << endl;
  }
}


void discreteDevice::serialize()
{
  os << label << delim << io_cs.command << delim;
  dLog.getstream(logLevel::DDEBUG, logType::DEVICE) << label << delim 
  <<  __func__ << delim << io_cs.command << endl;
  
}




void discreteDevice::unserialize()
{
  is >> state
  >> io_cs.error 
  >> io_cs.command 
  >> io_cs.di_open 
  >> io_cs.di_closed 
  >> io_cs.do_control ;
  
  dLog.getstream(logLevel::DDEBUG, logType::DEVICE) << label << delim 
  <<  __func__ << delim
  << io_cs.command << delim
  << state << delim 
  << io_cs.error << delim
  << io_cs.command << delim
  << io_cs.di_open << delim
  << io_cs.di_closed << delim
  << io_cs.do_control << endl;
  
  pthread_mutex_lock(glock());
  
  /*TODO===========>*/
  translator();
  io_ps = io_cs;
  
  set_update_flag();
  pthread_mutex_unlock(glock());
}

void discreteDevice::build_widget()
{
  pWidget = new widget(this);
}

/********       translate device  RT parameters into readable format for disaplying at Journal      *********/

void discreteDevice::translator()
{
  /*   clear stream  */
  hdev_state.clear();
  hdev_state.str(string());
  
  if(io_cs.error != err::NOERR && io_cs.command == cmd::NOCMD)
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



void discreteDevice::translate_error(int err)
{
  string error;
  
  switch(err)
  {
    case err::NOERR:
      return;
      
    case err::ETMOP:
      error = vcb::vocab::get()[vcb::ETMOP];
      break;
      
    case err::ETMCL:
      error = vcb::vocab::get()[vcb::ETMCL];
      break;
      
    case err::EBNOP:
      error = vcb::vocab::get()[vcb::EBNOP];
      break;
      
    case err::EBNCL:
      error =  vcb::vocab::get()[vcb::EBNCL];
      
    default:
      return;
  }
  
  hdev_state << error << endl;
}


void discreteDevice::translate_command(int com)
{
  string command;
  
  switch(com)
  {
    case cmd::NOCMD:
      return;
      
    case cmd::CLOSE:
      command =  vcb::vocab::get()[vcb::close_cmd];
      break;
      
    case cmd::OPEN:
      command =  vcb::vocab::get()[vcb::open_cmd];
      break;
      
    case cmd::INIT:
      command =  vcb::vocab::get()[vcb::init_cmd];
      break;
      
    default:
      return;
  }
  hdev_state << command << endl;
}


void discreteDevice::translate_state(int state)
{
  if(state){}
}


/****************************************     GUI     *****************************************************/



gboolean discreteDevice::widget::time_handler(widget* pWid)
{
  
  if(NULL == pWid)
  {return false;}
  
  discreteDevice& valRef  = *pWid->pObj;
  
  pthread_mutex_lock(valRef.glock());
  if(!valRef.get_update_flag())
  {
    pthread_mutex_unlock(valRef.glock());
    return true;
  }
  
  
  /*   update state textbox   */
  gtk_entry_set_text (GTK_ENTRY(pWid->tboxState), pWid->get_state_string());
  
  gtk_image_set_from_file((GtkImage*)pWid->discreteDeviceImage, ntrans("rect_v", valRef.get_state()).c_str());
  
  
  if(false == valRef.get_dd().error)
  { gtk_image_set_from_file((GtkImage*)pWid->errorImage, UIPATH"bs1_grey.png"); }
  else
  { gtk_image_set_from_file((GtkImage*)pWid->errorImage, UIPATH"bs1_red.png"); }
  
  if(false == valRef.get_dd().di_open)
  { gtk_image_set_from_file((GtkImage*)pWid->openImage, UIPATH"bs1_grey.png"); }
  else
  { gtk_image_set_from_file((GtkImage*)pWid->openImage, UIPATH"bs1_green.png"); }
  
  if(false == valRef.get_dd().di_closed)
  { gtk_image_set_from_file((GtkImage*)pWid->closedImage, UIPATH"bs1_grey.png"); }
  else
  { gtk_image_set_from_file((GtkImage*)pWid->closedImage, UIPATH"bs1_green.png"); }
  
  if(false == valRef.get_dd().do_control)
  { gtk_image_set_from_file((GtkImage*)pWid->controlImage, UIPATH"bs1_grey.png"); }
  else
  { gtk_image_set_from_file((GtkImage*)pWid->controlImage, UIPATH"bs1_green.png"); }
  
  valRef.reset_update_flag();
  pthread_mutex_unlock(valRef.glock());
  
  return true;
}


void discreteDevice::widget::close(GtkWidget* pWid, gpointer pVal)
{
  if(NULL == pWid || NULL == pVal)
  {return;}
  discreteDevice& valRef = *((widget*)pVal)->pObj;
  
  pthread_mutex_lock(valRef.glock());
  valRef.get_dd().command = cmd::CLOSE;
  valRef.set_emit();
  pthread_mutex_unlock(valRef.glock());
}

void discreteDevice::widget::open(GtkWidget* pWid, gpointer pVal)
{
  if(NULL == pWid || NULL == pVal)
  {return;}
  discreteDevice& valRef = *((widget*)pVal)->pObj;
  
  pthread_mutex_lock(valRef.glock());
  valRef.get_dd().command = cmd::OPEN;
  valRef.set_emit();
  pthread_mutex_unlock(valRef.glock());
}

void discreteDevice::widget::init(GtkWidget* pWid, gpointer pVal)
{
  if(NULL == pWid || NULL == pVal)
  {return;}
  discreteDevice& valRef = *((widget*)pVal)->pObj;
  
  pthread_mutex_lock(valRef.glock());
  valRef.get_dd().command = cmd::INIT;
  valRef.set_emit();
  pthread_mutex_unlock(valRef.glock());
}

/*        TODO  
 *   REVIEW TO ENSURE PROPER CLEANUP
 */
gint discreteDevice::widget::delete_event(GtkWidget *pWid, GdkEvent  *event, gpointer pVal)
{
  if(NULL == pWid || NULL == event || NULL == pVal)
  {}
  //   discreteDevice& valRef = *((widget*)pVal)->pObj;
  //   valRef.~discreteDevice();
  cout << "delete_event" << endl;
  
  return(FALSE);
}




discreteDevice::widget::widget(discreteDevice* pDev)
{
  if(!pDev)
  { exit(1); }
  
  pObj = pDev;
  gint discreteDeviceImWidth     = pWin->iconWidth;
  gint usize                   = pWin->usize;
  gint buttonWidth        = pWin->buttonWidth;
  gint buttonHeight       = pWin->buttonHeight;
  gint stateWidth           = pWin->controlsWidth - discreteDeviceImWidth - 36;  // controls whole widget width
  gdouble alignVal         = 0.5;
  gint col1                      = 8;
  gint col2                      = col1 + pWin->tboxHeight;
  gint col3                      = col1 + stateWidth - buttonWidth;;
  gint row0                     = 8;
  gint row                       =  row0 + usize + 2;
  gint step1                    = usize;
  gint step2                    = usize + 10;
  gint valv_col = pWin->controlsWidth - discreteDeviceImWidth - 26;
  
  
  
  frame = gtk_frame_new (NULL);
  gtk_box_pack_start (GTK_BOX (pWin->discreteVbox), frame, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_OUT);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 2);
  gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.36);
  
  discreteDeviceLabel = gtk_label_new (pObj->get_label().c_str());
  gtk_frame_set_label_widget (GTK_FRAME (frame), discreteDeviceLabel);
  gtk_misc_set_alignment (GTK_MISC (discreteDeviceLabel), 0, 0);
  
  align = gtk_alignment_new (0, 0, 1, 1);
  gtk_container_add (GTK_CONTAINER (frame), align);
  
  fixed = gtk_fixed_new ();
  gtk_container_add (GTK_CONTAINER (align), fixed);
  
  tboxState = gtk_entry_new ();
  gtk_fixed_put (GTK_FIXED (fixed), tboxState, col1, row0);
  gtk_widget_set_size_request (tboxState, stateWidth, usize);
  gtk_editable_set_editable (GTK_EDITABLE (tboxState), FALSE);
  
  discreteDeviceImage = gtk_image_new_from_file(UIPATH"rect_v_disabled.png");
  gtk_fixed_put (GTK_FIXED (fixed), discreteDeviceImage, valv_col, row0);
  gtk_misc_set_alignment (GTK_MISC (discreteDeviceImage), 0, 0);
  
  /*   first column   */
  gint im_shift = 4;
  openImage = gtk_image_new_from_file(UIPATH"bs1_grey.png");
  gtk_fixed_put (GTK_FIXED (fixed), openImage, col1, row);
  gtk_misc_set_alignment (GTK_MISC (openImage), alignVal, alignVal);
  gtk_widget_set_size_request (openImage, usize, usize + im_shift);
  
  closedImage = gtk_image_new_from_file(UIPATH"bs1_grey.png");
  gtk_fixed_put (GTK_FIXED (fixed), closedImage, col1, row + step1);
  gtk_misc_set_alignment (GTK_MISC (closedImage), alignVal, alignVal);
  gtk_widget_set_size_request (closedImage, usize, usize + im_shift);
  
  controlImage = gtk_image_new_from_file(UIPATH"bs1_grey.png");
  gtk_fixed_put (GTK_FIXED (fixed), controlImage, col1, row + step1*2);
  gtk_misc_set_alignment (GTK_MISC (controlImage), alignVal, alignVal);
  gtk_widget_set_size_request (controlImage, usize, usize + im_shift);
  
  errorImage = gtk_image_new_from_file(UIPATH"bs1_greyn.png");
  gtk_fixed_put (GTK_FIXED (fixed), errorImage, col1, row + step1*3);
  gtk_misc_set_alignment (GTK_MISC (errorImage), alignVal, alignVal);
  gtk_widget_set_size_request (errorImage, usize, usize + im_shift);
  
  /*   second column   */
  gint lb_shift = 8;
  openLabel = gtk_label_new (vcb::vocab::get()[vcb::opend]); 
  gtk_fixed_put (GTK_FIXED (fixed), openLabel, col2, row + lb_shift);
  
  closedLabel = gtk_label_new (vcb::vocab::get()[vcb::closed]);
  gtk_fixed_put (GTK_FIXED (fixed), closedLabel, col2, row + step1 + lb_shift);
  
  controlLabel = gtk_label_new (vcb::vocab::get()[vcb::control]);
  gtk_fixed_put (GTK_FIXED (fixed), controlLabel, col2, row + step1*2 + lb_shift);
  
  errorLabel = gtk_label_new (vcb::vocab::get()[vcb::error]);
  gtk_fixed_put (GTK_FIXED (fixed), errorLabel, col2, row + step1*3 + lb_shift);
  
  /*   third column   */
  openButton = gtk_button_new_with_mnemonic (vcb::vocab::get()[vcb::open_cmd]);
  gtk_fixed_put (GTK_FIXED (fixed), openButton, col3, row);
  gtk_widget_set_size_request (openButton, buttonWidth, buttonHeight);
  
  closeButton = gtk_button_new_with_mnemonic (vcb::vocab::get()[vcb::close_cmd]);
  gtk_fixed_put (GTK_FIXED (fixed), closeButton, col3, row + step2);
  gtk_widget_set_size_request (closeButton, buttonWidth, buttonHeight);
  
  initButton = gtk_button_new_with_mnemonic (vcb::vocab::get()[vcb::init_cmd]);
  gtk_fixed_put (GTK_FIXED (fixed), initButton, col3, row + step2*2);
  gtk_widget_set_size_request (initButton, buttonWidth, buttonHeight);
  
  gtk_signal_connect (GTK_OBJECT (closeButton), "clicked", GTK_SIGNAL_FUNC (close), (gpointer)this);
  gtk_signal_connect (GTK_OBJECT (openButton), "clicked", GTK_SIGNAL_FUNC (open), (gpointer)this);
  gtk_signal_connect (GTK_OBJECT (initButton), "clicked", GTK_SIGNAL_FUNC (init), (gpointer)this);
  g_timeout_add(200, (GSourceFunc) time_handler, (gpointer) this);
  gtk_signal_connect (GTK_OBJECT (pWin->mainWindow), "delete_event", GTK_SIGNAL_FUNC (delete_event), (gpointer)this);
  time_handler(this);
}



const char* discreteDevice::widget::get_state_string()
{
  switch(pObj->get_dd().error)
  {
    case err::NOERR:
      break;
      
    case err::ETMOP:
      return vcb::vocab::get()[vcb::ETMOP];
      
    case err::ETMCL:
      return vcb::vocab::get()[vcb::ETMCL];
      
    case err::EBNOP:
      return vcb::vocab::get()[vcb::EBNOP];
      
    case err::EBNCL:
      return vcb::vocab::get()[vcb::EBNCL];
      
    default:
      break;
  }
  return "";
}





