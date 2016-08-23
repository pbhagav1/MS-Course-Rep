
/**
 * Write a description of class SpiderMan here.
 * 
 * @author (Prasanth Bhagavatula) 
 * @version (12/02/2015 - 0.1)
 */

import java.awt.*;
import java.lang.*;

public class SuperMan extends Hero
{
    private int specialPower;
    private static SuperMan s;

    /**
     * Constructor for objects of class SuperMan
     */
    private SuperMan()
    {
        super("SuperMan");
        specialPower = 2;
    }

    /**
     * Constructor for objects of class SuperMan
     */
    private SuperMan(int inpLevel, int inpPowerLevel)
    {
        super(inpLevel, inpPowerLevel, "SuperMan");
        specialPower = 2;
    }

    /**
     * Method to create an instance of SuperMan
     * @param void
     * @return SuperMan Object pointed by a Hero reference
     */
    public static synchronized Hero getSuperMan()
    {
        if (s == null){
            s = new SuperMan();
        }
        return s;
    }

    /**
     * Method to create an instance of SuperMan
     * @param inpLevel - Level of SuperMan, inpPowerLevel - Initial Power Level of SuperMan
     * @return SuperMan Object pointed by a Hero reference
     */
    public static synchronized Hero getSuperMan(int inpLevel, int inpPowerLevel)
    {
        if (s == null){
            s = new SuperMan(inpLevel, inpPowerLevel);
        }
        return s;
    }

    /**
     * Method to draw a Spider Man using Graphice
     * @param Graphics - a Graphics object to draw the SuperMan
     * @return void
     */
    public void draw(Graphics g)
    {
        System.out.println("This Hero is SuperMan");
    }

    /**
     * Method to override the clone() method
     * @param void
     * @return Object
     * @throws CloneNotSupportedException
     */
    public Object clone() throws CloneNotSupportedException{
        throw new CloneNotSupportedException();
    }
}
