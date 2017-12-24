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
  gtk_main();
}



window::window()
{
  gint border            = 2;
  width_                 = 768;
  height_                = 512;
  controlsWidth_         = 760;
  controlsHeight_        = 422;
  
  iconWidth_             = 48;
  iconHeight_            = 26;
  buttonWidth            = 144;
  buttonHeight           = 26;
  labelWidth             = 144; 
  labelHeight            = 26;
  usize                  = 24;
  tboxWidth              = 56;
  tboxHeight             = 26;
  
  gtk_disable_setlocale();
  gtk_init(0, 0);
  
  /*   Window   */
  mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request(mainWindow, width_, height_);
  gtk_container_set_border_width(GTK_CONTAINER(mainWindow), 2);
  gtk_window_set_position(GTK_WINDOW(mainWindow), GTK_WIN_POS_CENTER);
  gtk_window_set_title(GTK_WINDOW(mainWindow), "Control System");
  gtk_window_set_resizable(GTK_WINDOW(mainWindow), FALSE);

  layoutMain = gtk_fixed_new ();
  gtk_container_add (GTK_CONTAINER (mainWindow), layoutMain);
  
  settings = gtk_settings_get_default();
  g_object_set(G_OBJECT(settings), "gtk-font-name", "Sans 8", NULL);
  
  /*   Controls, right panel   */
  controlsNbook = gtk_notebook_new();
  gtk_fixed_put (GTK_FIXED(layoutMain), controlsNbook, border, 36);
  gtk_widget_set_size_request(controlsNbook, controlsWidth_, controlsHeight_);
  gtk_container_set_border_width(GTK_CONTAINER(controlsNbook), border);
  
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

  /*   Analog Devices Tab   */
  analogScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (controlsNbook), analogScrolledWindow);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (analogScrolledWindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  
  analogViewport = gtk_viewport_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (analogScrolledWindow), analogViewport);
  gtk_viewport_set_shadow_type (GTK_VIEWPORT (analogViewport), GTK_SHADOW_NONE);
  
  analogVbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (analogViewport), analogVbox);
  
  analogLabel = gtk_label_new ("Analog");
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (controlsNbook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (controlsNbook), 1), analogLabel);
  
  /*   Discrete Devices Tab   */
  discreteScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (controlsNbook), discreteScrolledWindow);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (discreteScrolledWindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  
  discreteViewport = gtk_viewport_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (discreteScrolledWindow), discreteViewport);
  gtk_viewport_set_shadow_type (GTK_VIEWPORT (discreteViewport), GTK_SHADOW_NONE);
  
  discreteVbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (discreteViewport), discreteVbox);

  discreteLabel = gtk_label_new ("Discrete");
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (controlsNbook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (controlsNbook), 2), discreteLabel);
  
  /*   Serial Devices Tab   */
  serialScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (controlsNbook), serialScrolledWindow);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (serialScrolledWindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  
  serialViewport = gtk_viewport_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (serialScrolledWindow), serialViewport);
  gtk_viewport_set_shadow_type (GTK_VIEWPORT (serialViewport), GTK_SHADOW_NONE);
  
  serialVbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (serialViewport), serialVbox);
  
  serialLabel = gtk_label_new ("Serial");
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (controlsNbook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (controlsNbook), 3), serialLabel);
}





