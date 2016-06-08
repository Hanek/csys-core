#include "window.h"

#include <stdlib.h>


using namespace csys;

window* window::pInstance = 0;


window& window::instance()
{
  static Cleanup cleanup;
  
  if (pInstance == 0)
    pInstance = new window();
  return *pInstance;
}


window::Cleanup::~Cleanup()
{
  delete window::pInstance;
  window::pInstance = 0;
}

window::~window() {}


void window::show()
{
  gtk_widget_show_all(instance().mainWindow);
  gtk_main ();
}



window::window()
{
  gint border            = 2;
  gint shift                = 4;
  width                     = 1024;
  height                    = 720;
  mnemoWidth         = 580;
  mnemoHeight        = 456;
  controlsWidth         = width - mnemoWidth - shift;
  controlsHeight        = 650;
  dataWidth               = mnemoWidth;
  dataHeight              = controlsHeight - mnemoHeight;
  lockersWidth           = width - shift;
  lockersHeight          = height - controlsHeight - shift;
  
  iconWidth                = 48;
  iconHeight               = 26;
  buttonWidth             = 144;
  buttonHeight            = 26;
  labelWidth                = 144; 
  labelHeight               = 26;
  usize                         = 24;
  tboxWidth                 = 56;
  tboxHeight                = 26;
  
  gtk_disable_setlocale();
  gtk_init(0, 0);
  
  /*   Window   */
  mainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request (mainWindow, width, height);
  gtk_container_set_border_width (GTK_CONTAINER (mainWindow), 2);
  gtk_window_set_position(GTK_WINDOW(mainWindow), GTK_WIN_POS_CENTER);
  gtk_window_set_title (GTK_WINDOW (mainWindow), "Control System");
  gtk_window_set_resizable (GTK_WINDOW (mainWindow), FALSE);



  layoutMain = gtk_fixed_new ();
  gtk_container_add (GTK_CONTAINER (mainWindow), layoutMain);
  
  settings = gtk_settings_get_default();
  g_object_set(G_OBJECT(settings), "gtk-font-name", "Sans 8", NULL);
  
  /*   mnemoSchema, left panel */  
  mnemoFrame = gtk_frame_new (NULL);
  gtk_fixed_put (GTK_FIXED (layoutMain), mnemoFrame, 0, 0);
  gtk_widget_set_size_request (mnemoFrame, mnemoWidth, mnemoHeight);
  gtk_container_set_border_width (GTK_CONTAINER (mnemoFrame), border);
  
  mnemoAlignment = gtk_alignment_new (0, 0, 0, 0);
  gtk_container_add (GTK_CONTAINER (mnemoFrame), mnemoAlignment);
  gtk_container_set_border_width (GTK_CONTAINER (mnemoAlignment), border);
  
  
  mnemoLabel = gtk_label_new ("Mnemo Schema");
  gtk_frame_set_label_widget (GTK_FRAME (mnemoFrame), mnemoLabel);
  gtk_label_set_use_markup (GTK_LABEL (mnemoLabel), TRUE);
  
  
  /*   RT, below mnemoSchema   */
  rtNbook = gtk_notebook_new ();
  gtk_fixed_put (GTK_FIXED (layoutMain), rtNbook, 0, mnemoHeight);
  gtk_widget_set_size_request (rtNbook, dataWidth, dataHeight);
  gtk_container_set_border_width (GTK_CONTAINER (rtNbook), border);
  
  /*   Log Tab   */
  logVbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER(rtNbook), logVbox);
  logLabel = gtk_label_new ("Journal");
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (rtNbook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (rtNbook), 0), logLabel);
  
  /* Plot Tab   */
  plotVbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER(rtNbook), plotVbox);
  plotLabel = gtk_label_new ("Plot");
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (rtNbook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (rtNbook), 1), plotLabel);
  
  
  /*    Lockers and other stuff, footer   */
  lockersFrame = gtk_frame_new (NULL);
  gtk_fixed_put (GTK_FIXED (layoutMain), lockersFrame, 0, mnemoHeight + dataHeight);
  gtk_widget_set_size_request (lockersFrame, lockersWidth, lockersHeight);
  gtk_container_set_border_width (GTK_CONTAINER (lockersFrame), border);
  
  lockersAlignment = gtk_alignment_new (0, 0, 0, 0);
  gtk_container_add (GTK_CONTAINER (lockersFrame), lockersAlignment);
  gtk_container_set_border_width (GTK_CONTAINER (lockersAlignment), border);
  
  lockersLabel = gtk_label_new ("Lockers");
  gtk_frame_set_label_widget (GTK_FRAME (lockersFrame), lockersLabel);
  gtk_label_set_use_markup (GTK_LABEL (lockersLabel), TRUE);
  
  
  /*   Controls, right panel   */
  controlsNbook = gtk_notebook_new ();
  gtk_fixed_put (GTK_FIXED (layoutMain), controlsNbook, mnemoWidth, 6);
  gtk_widget_set_size_request (controlsNbook, controlsWidth, controlsHeight - 6);
  gtk_container_set_border_width (GTK_CONTAINER (controlsNbook), border);
  
  /*   System Tab   */
  systemScrolled = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (controlsNbook), systemScrolled);
  
  systemViewport = gtk_viewport_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (systemScrolled), systemViewport);
  gtk_viewport_set_shadow_type (GTK_VIEWPORT (systemViewport), GTK_SHADOW_NONE);
  
  systemVbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (systemViewport), systemVbox);
  
  systemLabel = gtk_label_new ("System");
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (controlsNbook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (controlsNbook), 0), systemLabel);

  /*   Gases Tab   */
  gasesScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (controlsNbook), gasesScrolledWindow);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (gasesScrolledWindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  
  gasesViewport = gtk_viewport_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (gasesScrolledWindow), gasesViewport);
  gtk_viewport_set_shadow_type (GTK_VIEWPORT (gasesViewport), GTK_SHADOW_NONE);
  
  gasesVbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (gasesViewport), gasesVbox);
  
  gasesLabel = gtk_label_new ("Serial");
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (controlsNbook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (controlsNbook), 1), gasesLabel);
  
  /*   Valves Tab   */
  valvesScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (controlsNbook), valvesScrolledWindow);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (valvesScrolledWindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  
  valvesViewport = gtk_viewport_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (valvesScrolledWindow), valvesViewport);
  gtk_viewport_set_shadow_type (GTK_VIEWPORT (valvesViewport), GTK_SHADOW_NONE);
  
  valvesVbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (valvesViewport), valvesVbox);

  valvesLabel = gtk_label_new ("Discrete");
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (controlsNbook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (controlsNbook), 2), valvesLabel);
  
  /*   Pumps Tab   */
  pumpsScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (controlsNbook), pumpsScrolledWindow);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (pumpsScrolledWindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  
  pumpsViewport = gtk_viewport_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (pumpsScrolledWindow), pumpsViewport);
  gtk_viewport_set_shadow_type (GTK_VIEWPORT (pumpsViewport), GTK_SHADOW_NONE);
  
  pumpsVbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (pumpsViewport), pumpsVbox);
  
  pumpsLabel = gtk_label_new ("Analog");
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (controlsNbook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (controlsNbook), 3), pumpsLabel);
}





