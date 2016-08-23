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
    
    public Figure(){
        centreX = 250;
        centreY = 250;
        figColor = Color.black;
        name = "Figure";
        Random rnd = new Random(System.currentTimeMillis());
        speed = (int) (((Math.abs(rnd.nextDouble()))%10)*10);
    }
    
    public Figure (int inpX, int inpY, Color inpColor, String inpName){
        double doubleSpeed;
        centreX = inpX;
        centreY = inpY;
        figColor = inpColor;
        name = inpName;
        Random rnd = new Random();
        //speed = (Math.abs(rnd.nextInt()))%50;
        //speed = (int) (((Math.abs(rnd.nextDouble()))%10)*50);
        //rnd.setSeed(System.currentTimeMillis());
        doubleSpeed = (Math.abs(rnd.nextDouble()));
        speed = (int) (doubleSpeed * 50);
        //speed = speed * 10;
    }
    
    public final void setX(int inpX){
        centreX = inpX;
    }
    
    public final int getX(){
        return centreX;
    }

    public final void setY(int inpY){
        centreY = inpY;
    }
    
    public final int getY(){
        return centreY;
    }
    
    public final void setSpeed(int inpSpeed){
        speed = inpSpeed;
    }
    
    public final int getSpeed(){
        return speed;
    }

    public final void setColor(Color inpColor){
        figColor = inpColor;
    }
    
    public final Color getColor(){
        return figColor;
    }
    
    public final void setName(String inpName){
        name = inpName;
    }
    
    public final String getName(){
        return name;
    }
    
    public abstract void draw(Graphics g); 
    
    public abstract void move();
}
