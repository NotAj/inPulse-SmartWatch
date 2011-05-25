/*
 * ExampleApp.java
 *
 * © <your company here>, 2003-2005
 * Confidential and proprietary.
 */

import net.rim.device.api.ui.UiApplication;

class ExampleApp extends UiApplication
{          
    public static void main(String[] args) 
    {                        
        ExampleApp mainApp = new ExampleApp();
        mainApp.enterEventDispatcher();
    }
    public ExampleApp() 
    {   
        pushScreen(new AppScreen());
    }
} 
