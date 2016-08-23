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

public class HighSpeedMovingStrategy implements MovingStrategy{
    @Override
    public int movingWithStrategy(int speed){
        if (speed < 20){
            Random rnd = new Random();
            int increaseSpeed = Math.abs(rnd.nextInt());
            increaseSpeed %= 10;
            if (increaseSpeed == 0){
                increaseSpeed = 1;
            }
            //int increaseSpeed = (int) (speed * 0.25);
            speed = speed + increaseSpeed;
        }
        return speed;
    }
}
