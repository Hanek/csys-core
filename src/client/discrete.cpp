// Copyright (C)  2014, 2015, 2016  Alexander Golant
// 
// This file is part of csys project. This project is free 
// software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>

#include <cstdlib>
#include <sstream>

#include "window.h"
#include "discrete.h"
#include "io.h"
#include "commac.h"

using namespace std;
using namespace csys;

window* dI::widget::pWin_ = &window::instance();


dI::dI(const char *lbl): 
dev(lbl, time::Sec1)
{
  pair<devMapIterator, bool> ret;
  ret = deviceMap_.insert(pair<string, dev*>(label_, this));
  if (false == ret.second) 
  {
    exit(1);
  }
}

dI::~dI()
{
  devMapIterator it = deviceMap_.find(label_);
  
  if(it != deviceMap_.end())
  { 
    delete(it->second);
    deviceMap_.erase(it); 
  }
  
}



void dI::process()
{
 std::cout << label_
            << "\tstate: " << cs_.state_
            << std::endl;
}



void dI::serialize()
{
     
}


void dI::unserialize()
{
  is_.deserialize<char>(&cs_.error_);
  is_.deserialize<bool>(&cs_.state_);
  
  set_update_flag();
  pthread_mutex_unlock(get_lock());
}




void dI::build_widget()
{
  widget_ = new widget(this);
}






/********************* gui **********************/


gboolean dI::widget::time_handler(widget* pWid)
{
  if(!pWid)
  {return false;}
                                                                                                                          
  dI& diRef  = *pWid->obj_;
  
  pthread_mutex_lock(diRef.get_lock());
  if(!diRef.get_update_flag())
  {
    pthread_mutex_unlock(diRef.get_lock());
    return true;
  }
  
  if(!diRef.get_data().state_)
  { gtk_image_set_from_file((GtkImage*)pWid->bitState_, UIPATH"bs1_grey.png"); }
  else
  { gtk_image_set_from_file((GtkImage*)pWid->bitState_, UIPATH"bs1_green.png"); }
    
  diRef.reset_update_flag();
  pthread_mutex_unlock(diRef.get_lock());
  
  return true;
}


void dI::widget::init(GtkWidget* pWid, gpointer pVal)
{
  if(!pWid || !pVal)
  {return;}
  
  dI& diRef = *((widget*)pVal)->obj_;
  
  pthread_mutex_lock(diRef.get_lock());
//  diRef.get_data().command = cmd::INIT;
  diRef.set_emit();
  pthread_mutex_unlock(diRef.get_lock());
}

/*        TODO  
 *   REVIEW TO ENSURE PROPER CLEANUP
 */
gint dI::widget::delete_event(GtkWidget *pWid, GdkEvent  *event, gpointer pVal)
{
  if(!pWid || !event || !pVal)
  {}
  //   discreteDevice& valRef = *((widget*)pVal)->pObj;
  //   valRef.~discreteDevice();
  cout << "delete_event" << endl;
  
  return(FALSE);
}

int i = 0;
int j = 0;

dI::widget::widget(dI* pDev)
{  
  if(!pDev)
  { exit(1); }
  
  obj_             = pDev;
  gint usize       = pWin_->usize;
  gdouble alignVal = 0.5;
   
  frame_ = gtk_frame_new(NULL);
  
  gtk_box_pack_start(GTK_BOX(pWin_->discreteVbox), frame_, FALSE, FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(frame_), 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame_), GTK_SHADOW_ETCHED_OUT);
  gtk_container_set_border_width(GTK_CONTAINER(frame_), 2);
  gtk_frame_set_label_align(GTK_FRAME(frame_), 0.5, 0.36);
    
  label_ = gtk_label_new (obj_->get_label().c_str());
  gtk_frame_set_label_widget (GTK_FRAME (frame_), label_);
  gtk_misc_set_alignment (GTK_MISC (label_), 0, 0);
  
  align_ = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_container_add (GTK_CONTAINER (frame_), align_);
  gtk_widget_set_size_request(align_, 44, 56);
  
  fixed_ = gtk_fixed_new ();
  gtk_container_add (GTK_CONTAINER (align_), fixed_);
  gtk_widget_set_size_request(fixed_, 44, 56);
  
  /* bit state image */
  bitState_ = gtk_image_new_from_file("../resources/bs1_grey.png");
  gtk_fixed_put (GTK_FIXED(fixed_), bitState_, 0, 0);
  gtk_misc_set_alignment(GTK_MISC(bitState_), alignVal, alignVal);
  gtk_widget_set_size_request(bitState_, usize, usize);
  
  g_timeout_add(200, (GSourceFunc) time_handler, (gpointer) this);
  gtk_signal_connect (GTK_OBJECT (pWin_->mainWindow), "delete_event", GTK_SIGNAL_FUNC (delete_event), (gpointer)this);
  
  time_handler(this);
}



const char* dI::widget::get_state_string()
{
  switch(obj_->get_data().error_)
  {
    case err::NOERR:
      break;
      
    case err::HWERR:
      return "hw error";
      
    default:
      break;
  }
  return "";
}





