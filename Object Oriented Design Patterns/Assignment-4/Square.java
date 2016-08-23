/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Prasanth_B
 */

import java.awt.Graphics;
import java.awt.Color;

public class Square extends Figure{
    private int sideLength;
    private static Figure s;
    /*public Square(){
        super();
        sideLength = 50;
    }
    
    public Square(int inpX, int inpY, int inpSideLength, Color inpColor,String inpName){
        super(inpX, inpY, inpColor, inpName, new HighSpeedMovingStrategy());
        sideLength = inpSideLength;
    }*/
    
    private Square(){
        super(100, 100, Color.GRAY, "Square", new HighSpeedMovingStrategy());
        sideLength = 50;
    }
    
    public static synchronized Figure getSquare(){
        if (s == null){
            s = new Square();
        }
        return s;
    }
    
    @Override
    public void draw (Graphics g){
        int startX, startY, diffSideLen;
        diffSideLen = sideLength/2;
        startX = getX() - diffSideLen;
        startY = getY() - diffSideLen;
        g.setColor(getColor());
        g.fillRect(startX, startY, sideLength, sideLength);
    }
    
    @Override
    public void move(){
        int newX, newSpeed;
        MovingStrategy strategy = getMovingStrategy();
        newSpeed = strategy.movingWithStrategy(getSpeed());
        setSpeed(newSpeed);
        newX = getX() + getSpeed();
        setX(newX);
    }
    
    @Override
    public Object clone() throws CloneNotSupportedException{
        throw new CloneNotSupportedException();
    }
}