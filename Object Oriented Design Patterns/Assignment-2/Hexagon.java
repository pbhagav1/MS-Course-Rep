
import java.awt.Color;
import java.awt.Graphics;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Prasanth_B
 */
public class Hexagon extends Figure{
    int sideLength;
    
    public Hexagon(){
        super();
        sideLength = 50;
    }
    
    public Hexagon(int inpX, int inpY, int inpSideLength, Color inpColor,String inpName){
        super(inpX, inpY, inpColor, inpName, new HighSpeedMovingStrategy());
        sideLength = inpSideLength;
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
    public void move(){
        int newX, newSpeed;
        MovingStrategy strategy = getMovingStrategy();
        newSpeed = strategy.movingWithStrategy(getSpeed());
        setSpeed(newSpeed);
        newX = getX() + getSpeed();
        setX(newX);
    }
}

