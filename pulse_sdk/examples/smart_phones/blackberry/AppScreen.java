/*
 * AppScreen.java
 *
 * © <your company here>, 2003-2005
 * Confidential and proprietary.
 */

import net.rim.device.api.ui.*;
import net.rim.device.api.ui.component.*;
import net.rim.device.api.ui.container.*;

class AppScreen extends MainScreen implements FieldChangeListener
{
    private BtManager msgHandler;
    private ButtonField timeButton;
    private ButtonField smsButton;
    private ButtonField emailButton;   
    private ButtonField phoneButton;
    private ButtonField bbmButton;
    
    public AppScreen() 
    {                     
        super();
        msgHandler = new BtManager();
        
        if(msgHandler.connect())     
            setTitle("Connected to " + msgHandler.getDeviceName());   
        else    
            setTitle("Not connected");
    
        timeButton = new ButtonField("Set Time", ButtonField.CONSUME_CLICK | ButtonField.FIELD_RIGHT);
        timeButton.setChangeListener(this);
        this.add(timeButton);
        this.add(new SeparatorField());
        
        smsButton = new ButtonField("Send SMS", ButtonField.CONSUME_CLICK);
        smsButton.setChangeListener(this);
        this.add(smsButton);
        
        emailButton = new ButtonField("Send Email", ButtonField.CONSUME_CLICK);
        emailButton.setChangeListener(this);
        this.add(emailButton);
        
        phoneButton = new ButtonField("Send Phone Call", ButtonField.CONSUME_CLICK);
        phoneButton.setChangeListener(this);
        this.add(phoneButton);
        
        bbmButton = new ButtonField("Send BBM", ButtonField.CONSUME_CLICK);
        bbmButton.setChangeListener(this);
        this.add(bbmButton);
    }   
    public void fieldChanged(Field field, int context)
    {        
        if(field == timeButton)
        {
            msgHandler.setTime();
            Dialog.inform("Time set");
        }
        else if(field == smsButton)
        {
            msgHandler.sendSMS();           
            Dialog.inform("SMS sent");
        }
        else if(field == emailButton)
        {
            msgHandler.sendEmail();
            Dialog.inform("Email sent");
        }
        else if(field == phoneButton)
        {
            msgHandler.sendPhone();
            Dialog.inform("Phone Call sent");
        }
        else if(field == bbmButton)
        {
            msgHandler.sendBBM();
            Dialog.inform("BBM sent");
        }
    }
    public boolean onSavePrompt()
    {
        return true;
    }
} 
