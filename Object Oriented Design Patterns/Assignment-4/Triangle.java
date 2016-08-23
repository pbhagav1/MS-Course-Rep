
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
public class Triangle extends Figure{
    int sideLength;
    private static Figure t;
    
    /*public Triangle(){
        super();
        sideLength = 50;
    }
    
    public Triangle(int inpX, int inpY, int inpSideLength, Color inpColor,String inpName){
        super(inpX, inpY, inpColor, inpName, new HighSpeedMovingStrategy());
        sideLength = inpSideLength;
    }*/
    
    private Triangle(){
        super(100, 450, Color.BLUE, "Triangle", new HighSpeedMovingStrategy());
        sideLength = 50;
    }
    
    public static synchronized Figure getTriangle(){
        if (t == null){
            t = new Triangle();
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