/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Prasanth_B
 */

public class TestingRemoteCar {
    public static void main(String args[]){
        CarToyota ct= new CarToyota("Toyota");
        CarFerrari cf = new CarFerrari("Ferrari");
        CarLamborghini cl = new CarLamborghini("Lamborghini");
        
        RemoteControlAlarm alarmToyota = new RemoteControlAlarm(ct);
        
        RemoteControlAlarm alarmFerrari = new RemoteControlAlarm(cf);
        RemoteControlLock lockFerrari = new RemoteControlLock(cf);
        
        RemoteControlAlarm alarmLamborghini = new RemoteControlAlarm(cl);
        RemoteControlLock lockLamborghini = new RemoteControlLock(cl);
        RemoteControlMusic musicLamborghini = new RemoteControlMusic(cl);
        
        alarmToyota.operate();
                
        alarmFerrari.operate();
        lockFerrari.operate();
        
        alarmLamborghini.operate();
        lockLamborghini.operate();
        musicLamborghini.operate();
    }
}
