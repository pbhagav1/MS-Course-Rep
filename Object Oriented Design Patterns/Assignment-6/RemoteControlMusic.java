/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Prasanth_B
 */
public class RemoteControlMusic extends RemoteControl{
    
    public RemoteControlMusic(Car c){
        super(c);
    }
    
    @Override
    public void operate(){
        System.out.print("\nThis Remote Control is used for Music and is used in the car ");
        car.operateMusic();
    }
    
}
