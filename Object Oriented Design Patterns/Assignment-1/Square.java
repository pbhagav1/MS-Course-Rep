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
    int sideLength;
    
    public Square(){
        super();
        sideLength = 50;
    }
    
    public Square(int inpX, int inpY, int inpSideLength, Color inpColor,String inpName){
        super(inpX, inpY, inpColor, inpName);
        sideLength = inpSideLength;
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
        int newX;
        newX = getX() + getSpeed();
        setX(newX);
    }
}
