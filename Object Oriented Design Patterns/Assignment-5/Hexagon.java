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

public class Hexagon extends Figure{
    private static int sideLength;
    private static Figure h;
    
    /*public Hexagon(){
        super();
        sideLength = 50;
    }*/
    
    private Hexagon(int inpX, int inpY, int inpSideLength, int inpColor,String inpName){
        super(inpX, inpY, inpColor, inpName);
        sideLength = inpSideLength;
    }
    
    /*private Hexagon(){
        super(100, 625, Color.GREEN, "Hexagon", new HighSpeedMovingStrategy());
        sideLength = 50;
    }*/
    
    public static synchronized Figure getHexagon (int inpX, int inpY, int inpSideLength, int inpColor,String inpName){
        if (h == null){
            h = new Hexagon(inpX, inpY, inpSideLength, inpColor, inpName);
        }else{
            h.setX(inpX);
            h.setY(inpY);;
            h.setColor(inpColor);
            h.setName(inpName);
            sideLength = inpSideLength;
        }
        return h;
    }
    
    @Override
    public void draw (Graphics g){
        int halfSideLen, qtrSideLen;
        int[] xPoints, yPoints;
        xPoints = new int[6];
        yPoints = new int[6];
        halfSideLen = sideLength/2;
        qtrSideLen = sideLength/4;
        xPoints[0] = getX() - halfSideLen;
        yPoints[0] = getY() + qtrSideLen;
        xPoints[1] = getX() + halfSideLen;
        yPoints[1] = getY() + qtrSideLen;
        xPoints[2] = getX() + halfSideLen + qtrSideLen;
        yPoints[2] = getY();
        xPoints[3] = getX() + halfSideLen;
        yPoints[3] = getY() - qtrSideLen;
        xPoints[4] = getX() - halfSideLen;
        yPoints[4] = getY() - qtrSideLen;
        xPoints[5] = getX() - halfSideLen - qtrSideLen;
        yPoints[5] = getY();
        g.setColor(getColor());
        g.fillPolygon(xPoints, yPoints, 6);
    }
    
    @Override
    public Object clone() throws CloneNotSupportedException{
        throw new CloneNotSupportedException();
    }
}