
/**
 * Write a description of class SpiderMan here.
 * 
 * @author (Prasanth Bhagavatula) 
 * @version (12/02/2015 - 0.1)
 */

import java.awt.*;
import java.lang.*;

public class BatMan extends Hero
{
    private int specialPower;
    private static BatMan s;

    /**
     * Constructor for objects of class BatMan
     */
    private BatMan()
    {
        super("BatMan");
        specialPower = 3;
    }

    /**
     * Constructor for objects of class BatMan
     */
    private BatMan(int inpLevel, int inpPowerLevel)
    {
        super(inpLevel, inpPowerLevel, "BatMan");
        specialPower = 3;
    }

    /**
     * Method to create an instance of BatMan
     * @param void
     * @return BatMan Object pointed by a Hero reference
     */
    public static synchronized Hero getBatMan()
    {
        if (s == null){
            s = new BatMan();
        }
        return s;
    }

    /**
     * Method to create an instance of BatMan
     * @param inpLevel - Level of BatMan, inpPowerLevel - Initial Power Level of BatMan
     * @return BatMan Object pointed by a Hero reference
     */
    public static synchronized Hero getBatMan(int inpLevel, int inpPowerLevel)
    {
        if (s == null){
            s = new BatMan(inpLevel, inpPowerLevel);
        }
        return s;
    }

    /**
     * Method to draw a Spider Man using Graphice
     * @param Graphics - a Graphics object to draw the BatMan
     * @return void
     */
    public void draw(Graphics g)
    {
        System.out.println("This Hero is BatMan");
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
