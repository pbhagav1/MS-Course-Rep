/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Prasanth_B
 */
public abstract class Car {
    private String carName;
    public Car(String name){
        carName = name;
    }

    public String getName(){
        return carName;
    }
    
    public abstract void operateAlarm();
    public abstract void operateLock();
    public abstract void operateMusic();
}
