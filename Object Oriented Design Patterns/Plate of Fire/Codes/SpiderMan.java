
/**
 * Write a description of class SpiderMan here.
 * 
 * @author (Prasanth Bhagavatula) 
 * @version (12/02/2015 - 0.1)
 */

import java.awt.*;
import java.lang.*;

public class SpiderMan extends Hero
{
    private int specialPower;
    private static SpiderMan s;

    private SpiderMan()
    {
        super("SpiderMan");
        specialPower = 1;
    }

    /**
     * Constructor for objects of class SpiderMan
     */
    private SpiderMan(int inpLevel, int inpPowerLevel)
    {
        super(inpLevel, inpPowerLevel, "SpiderMan");
        specialPower = 1;
    }

    /**
     * Method to create an instance of Spiderman
     * @param void
     * @return Spiderman Object pointed by a Hero reference
     */
    public static synchronized Hero getSpiderMan()
    {
        if (s == null){
            s = new SpiderMan();
        }
        return s;
    }

    /**
     * Method to create an instance of Spiderman
     * @param inpLevel - Level of Spiderman, inpPowerLevel - Initial Power Level of Spiderman
     * @return Spiderman Object pointed by a Hero reference
     */
    public static synchronized Hero getSpiderMan(int inpLevel, int inpPowerLevel)
    {
        if (s == null){
            s = new SpiderMan(inpLevel, inpPowerLevel);
        }
        return s;
    }

    /**
     * Method to draw a Spider Man using Graphice
     * @param Graphics - a Graphics object to draw the SpiderMan
     * @return void
     */
    public void draw(Graphics g)
    {
        System.out.println("This Hero is SpiderMan");
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
