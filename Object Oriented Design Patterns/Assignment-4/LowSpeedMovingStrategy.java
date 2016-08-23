/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Prasanth_B
 */

import java.util.Random;

public class LowSpeedMovingStrategy implements MovingStrategy{
    @Override
    public int movingWithStrategy(int speed){
        if (speed > 10){
            Random rnd = new Random();
            int reduceSpeed = Math.abs(rnd.nextInt());
            reduceSpeed %= 10;
            if (reduceSpeed == 0){
                reduceSpeed = 1;
            }
            //int reduceSpeed = (int) (speed * 0.25);
            speed = speed - reduceSpeed;
            /*if (speed <= 0){
                speed = 1;
            }*/
        }
        return speed;
    }
}
