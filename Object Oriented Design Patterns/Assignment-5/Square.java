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
    private static int sideLength;
    private static Figure s;
    /*public Square(){
        super();
        sideLength = 50;
    }*/
    
    public Square(int inpX, int inpY, int inpSideLength, int inpColor,String inpName){
        super(inpX, inpY, inpColor, inpName);
        sideLength = inpSideLength;
    }
    
    /*private Square(){
        super(100, 100, Color.GRAY, "Square", new HighSpeedMovingStrategy());
        sideLength = 50;
    }*/
    
    public static synchronized Figure getSquare(int inpX, int inpY, int inpSideLength, int inpColor,String inpName){
        if (s == null){
            s = new Square(inpX, inpY, inpSideLength, inpColor,inpName);
        }else{
            s.setX(inpX);
            s.setY(inpY);
            s.setColor(inpColor);
            s.setName(inpName);
            sideLength = inpSideLength;
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
    public Object clone() throws CloneNotSupportedException{
        throw new CloneNotSupportedException();
    }
}