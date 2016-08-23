/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Prasanth_B
 */

import java.awt.Color;
import java.awt.Graphics;

public class Triangle extends Figure{
    private static int sideLength;
    private static Figure t;
    
    /*public Triangle(){
        super();
        sideLength = 50;
    }*/
    
    private Triangle(int inpX, int inpY, int inpSideLength, int inpColor,String inpName){
        super(inpX, inpY, inpColor, inpName);
        sideLength = inpSideLength;
    }
    
    /*private Triangle(){
        super(100, 450, Color.BLUE, "Triangle", new HighSpeedMovingStrategy());
        sideLength = 50;
    }*/
    
    public static synchronized Figure getTriangle(int inpX, int inpY, int inpSideLength, int inpColor,String inpName){
        if (t == null){
            t = new Triangle( inpX, inpY, inpSideLength, inpColor, inpName);
        }else{
            t.setX(inpX);
            t.setY(inpY);
            t.setColor(inpColor);
            t.setName(inpName);
            sideLength = inpSideLength;
        }
        return t;
    }
    
    @Override
    public void draw (Graphics g){
        int diffSideLen;
        int[] xPoints, yPoints;
        xPoints = new int[3];
        yPoints = new int[3];
        diffSideLen = sideLength/2;
        xPoints[0] = getX() - diffSideLen;
        yPoints[0] = getY() + diffSideLen;
        xPoints[1] = getX() + diffSideLen;
        yPoints[1] = getY() + diffSideLen;
        xPoints[2] = getX();
        yPoints[2] = getY() - diffSideLen;
        g.setColor(getColor());
        g.fillPolygon(xPoints, yPoints, 3);
    }
    
    @Override
    public Object clone() throws CloneNotSupportedException{
        throw new CloneNotSupportedException();
    }
}