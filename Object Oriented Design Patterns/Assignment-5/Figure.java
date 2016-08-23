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

public abstract class Figure{
    private int centreX,centreY;
    private Color figColor;
    private String name;
    
    public Figure(){
        centreX = 250;
        centreY = 250;
        figColor = Color.black;
        name = "Figure";
    }
    
    public Figure (int inpX, int inpY, int inpColor, String inpName){
        centreX = inpX;
        centreY = inpY;
        //figColor = inpColor;
        switch(inpColor){
            case 1:
                figColor = Color.WHITE;
                break;
            case 2:
                figColor = Color.BLUE;
                break;
            case 3:
                figColor = Color.CYAN;
                break;
            case 4:
                figColor = Color.DARK_GRAY;
                break;
            case 5:
                figColor = Color.GRAY;
                break;
            case 6:
                figColor = Color.GREEN;
                break;
            case 7:
                figColor = Color.LIGHT_GRAY;
                break;
            case 8:
                figColor = Color.MAGENTA;
                break;
            case 9:
                figColor = Color.ORANGE;
                break;
            case 10:
                figColor = Color.PINK;
                break;
            case 11:
                figColor = Color.RED;
                break;
            default:
                figColor = Color.YELLOW;
        }
        name = inpName;        
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

    public void setColor(int inpColor){
        switch(inpColor){
            case 1:
                figColor = Color.WHITE;
                break;
            case 2:
                figColor = Color.BLUE;
                break;
            case 3:
                figColor = Color.CYAN;
                break;
            case 4:
                figColor = Color.DARK_GRAY;
                break;
            case 5:
                figColor = Color.GRAY;
                break;
            case 6:
                figColor = Color.GREEN;
                break;
            case 7:
                figColor = Color.LIGHT_GRAY;
                break;
            case 8:
                figColor = Color.MAGENTA;
                break;
            case 9:
                figColor = Color.ORANGE;
                break;
            case 10:
                figColor = Color.PINK;
                break;
            case 11:
                figColor = Color.RED;
                break;
            default:
                figColor = Color.YELLOW;
        }
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
    
    public abstract void draw(Graphics g); 
}