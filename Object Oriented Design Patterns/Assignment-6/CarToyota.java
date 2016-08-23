/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Prasanth_B
 */
        
public class CarToyota extends Car{
    public CarToyota(String name){
        super(name);
    }
    
    @Override
    public void operateAlarm(){
        System.out.print(getName());
    }
    
    @Override
    public void operateLock(){
        System.out.print(getName());
    }
    
    @Override
    public void operateMusic(){
        System.out.print(getName());
    }
}
