/*
 * BtManager.java
 *
 * © <your company here>, 2003-2005
 * Confidential and proprietary.
 */
import java.io.IOException;

import javax.bluetooth.BluetoothStateException;
import javax.bluetooth.DiscoveryAgent;
import javax.bluetooth.LocalDevice;
import javax.bluetooth.L2CAPConnection;
import javax.bluetooth.RemoteDevice;
import javax.microedition.io.ConnectionNotFoundException;
import javax.microedition.io.Connector;

import java.util.Calendar;

class BtManager 
{
    private L2CAPConnection connection;
    private String watchBtAddress;
    private String deviceName;
    private byte [] msg;
    
    public BtManager() 
    {    
        connection = null;
        watchBtAddress = null;
        deviceName = null;
        msg = null;
    }    
    public boolean connect()
    {
        if (!LocalDevice.isPowerOn()) 
            return false;
        try
        {
            // get local Bt device object
            LocalDevice localDevice = LocalDevice.getLocalDevice();
            // retrieve local discovery agent 
            DiscoveryAgent discAgent = localDevice.getDiscoveryAgent();
            // return array of pre-known devices
            RemoteDevice[] pairedDevices = discAgent.retrieveDevices(DiscoveryAgent.PREKNOWN);
            if(pairedDevices == null)  
                return false;       
            for(int i = 0; i < pairedDevices.length; i++)
            {
                // connect to device with matching name
                String devName = pairedDevices[i].getFriendlyName(false);
                // truncate pair code identifier
                if(devName.length() > 8)
                    devName= devName.substring(0, 7);
                if(devName.equalsIgnoreCase("inPulse"))
                {
                    deviceName = pairedDevices[i].getFriendlyName(false);
                    watchBtAddress = pairedDevices[i].getBluetoothAddress();
                    String connectionStr = "btl2cap://" + watchBtAddress + ":1001";
                    connection = (L2CAPConnection)Connector.open(connectionStr);                          
                    return true;
                }
            }
            return false;          
        } 
        catch (BluetoothStateException bse) 
        {
            return false;
        }
        catch (ConnectionNotFoundException e) 
        {
            return false;
        } 
        catch (IOException e) 
        {
            return false;
        }     
    } 
    private void writeInt32(int i, byte[] dest, int destOffset) 
    {
        dest[destOffset++] = (byte) i;
        dest[destOffset++] = (byte) (i >> 8);
        dest[destOffset++] = (byte) (i >> 16);
        dest[destOffset] = (byte) (i >> 24);
    }
    private void writeInt16(int i, byte[] dest, int destOffset) 
    {
        dest[destOffset++] = (byte) i;
        dest[destOffset] = (byte) (i >> 8);        
    }    
    private void writeString(String text, byte[] dest, int destOffset)
    {
        byte[] tempText = text.getBytes();
        
        for(int i = 0; i < text.length(); i++)
            dest[destOffset + i] = tempText[i];
        dest[destOffset + text.length()] = '\0';
    } 
    private void encodeNotification(int type, String headerText, String bodyText)
    {
        short msgLen = (short)(24 + headerText.length() + bodyText.length() + 2);     
        msg = new byte [msgLen];
       
        // size of msg header
        msg[0] = 8;
        // notification endpoint
        msg[1] = 1;
        // length of entire msg
        writeInt16(msgLen, msg, 2);
        // time msg was sent, unused
        writeInt32(0, msg, 4);
        // set notification type
        writeInt32(type, msg, 8);
        // configure alert
        msg[12] = 1;
        msg[13] = 0;
        msg[14] = 0;
        msg[15] = 0;
        msg[16] = 0;
        msg[17] = 20;
        msg[18] = 10;
        msg[19] = 20;
        msg[20] = 10;
        msg[21] = 20;
        msg[22] = 0;
        msg[23] = 0;
        // store body and header text
        writeString(headerText, msg, 24);
        writeString(bodyText, msg, 25 + headerText.length());        
    } 
    public void sendMsg()
    {
        try
        {   
            connection.send(msg);
        }
        catch(IOException e)
        {
            
        }
    }
    public void setTime()
    {
        msg = new byte [54];
        // retrieve instance of calendar
        Calendar cal = Calendar.getInstance();
        // size of msg header
        msg[0] = 8;
        // command endpoint
        msg[1] = 2;
        // length of entire msg
        writeInt16(54, msg, 2);
        // time msg was sent, unused
        writeInt32(0, msg, 4);
        // set_time command type 
        writeInt16(50, msg, 8);
        // unused paramters 1 and 2 
        writeInt32(0, msg, 10);
        writeInt32(0, msg, 14);
        // seconds
        writeInt32(cal.get(Calendar.SECOND), msg, 18);
        // minutes
        writeInt32(cal.get(Calendar.MINUTE), msg, 22);
        // hour
        writeInt32(cal.get(Calendar.HOUR_OF_DAY), msg, 26);
        // day of month
        writeInt32(cal.get(Calendar.DAY_OF_MONTH), msg, 30);
        // month
        writeInt32(cal.get(Calendar.MONTH), msg, 34);
        // year
        writeInt32((cal.get(Calendar.YEAR) % 100) + 2000, msg, 38);
        // day of week
        writeInt32(cal.get(Calendar.DAY_OF_WEEK) - 1, msg, 42);
        // day of year, unused
        writeInt32(0, msg, 46);
        // DST, unused
        writeInt32(0, msg, 50);
        
        sendMsg();     
    }   
    public void sendSMS()
    {
        encodeNotification(1, "SMS header", "SMS body");
        sendMsg();
    }    
    public void sendEmail()
    {
        encodeNotification(0, "Email header", "Email body");
        sendMsg();
    }
    public void sendPhone()
    {
        encodeNotification(3, "Phone header", "Phone body");
        sendMsg();        
    }
    public void sendBBM()
    {
        encodeNotification(6, "BBM header", "BBM body");
        sendMsg();        
    }
    public String getDeviceName()
    {
        return deviceName;
    }
} 
