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
import java.util.Random;

public abstract class Figure implements Movable{
    private int centreX,centreY, speed;
    private Color figColor;
    private String name;
    private MovingStrategy movingStrategy;
    
    public Figure(){
        centreX = 250;
        centreY = 250;
        figColor = Color.black;
        name = "Figure";
        Random rnd = new Random(System.currentTimeMillis());
        speed = (int) (((Math.abs(rnd.nextDouble()))%10)*10);
        movingStrategy = new HighSpeedMovingStrategy();
    }
    
    public Figure (int inpX, int inpY, Color inpColor, String inpName, MovingStrategy strategy){
        double doubleSpeed;
        centreX = inpX;
        centreY = inpY;
        figColor = inpColor;
        name = inpName;
        Random rnd = new Random();
        doubleSpeed = (Math.abs(rnd.nextDouble()));
        speed = (int) (doubleSpeed * 50);
        movingStrategy = strategy;
        speed = movingStrategy.movingWithStrategy(speed);
        
    }
    
    public void setX(int inpX){
        centreX = inpX;
    }
    
    public int getX(){
        return centreX;
    }

    public void setY(int inpY){
        centreY = inpY;
    }
    
    public int getY(){
        return centreY;
    }
    
    public void setSpeed(int inpSpeed){
        speed = inpSpeed;
    }
    
    public int getSpeed(){
        return speed;
    }

    public void setColor(Color inpColor){
        figColor = inpColor;
    }
    
    public Color getColor(){
        return figColor;
    }
    
    public void setName(String inpName){
        name = inpName;
    }
    
    public String getName(){
        return name;
    }
    
    public void setMovingStrategy(MovingStrategy strategy){
        movingStrategy = strategy;
    }
    
    public MovingStrategy getMovingStrategy(){
        return movingStrategy;
    }
    
    public abstract void draw(Graphics g); 
    
    public abstract void move();
}
