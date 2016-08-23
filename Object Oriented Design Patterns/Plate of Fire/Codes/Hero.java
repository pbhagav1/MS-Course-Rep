
/**
 * Abstract class Hero - write a description of the class here
 * 
 * @author (Prasanth Bhagavatula)
 * @version (12/02/2015 - 0.1)
 */

import java.awt.*;
import java.lang.*;

public abstract class Hero implements Glyph
{
    // instance variables - replace the example below with your own
    private int level;
    private int powerLevel;
    private String name;
    private FightStrategy fightStrategy;

    /**
     * This is the constructor
     * @parm - void
     */
    public Hero(String s)
    {
        level = 1;
        powerLevel = 50;
        name = s;
    }
    
    /**
     * This is the constructor
     * @parm - void
     */
    public Hero(int inpLevel, int inpPowerLevel, String s)
    {
        level = inpLevel;
        powerLevel = inpPowerLevel;
        name = s;
    }
    
    /**
     * This method is used to return the Fighting Strategy
     * @parm - void
     * @return Fighting Strategy 
     */
    public FightStrategy getFightStrategy()
    {
        return fightStrategy;
    }
    
    /**
     * This method is used to set the Fighting Strategy
     * @parm inpLevel - The Fighting Strategy to be set
     * @return void 
     */
    public void setFightStrategy(FightStrategy inpFightStrategy)
    {
       fightStrategy = inpFightStrategy;
    }
    
    /**
     * This method is used to return the Level
     * @parm - void
     * @return Level 
     */
    public int getLevel()
    {
        return level;
    }
    
    /**
     * This method is used to set the Level
     * @parm inpLevel - The Level to be set
     * @return void 
     */
    public void setLevel(int inpLevel)
    {
       level = inpLevel;
    }
    
    /**
     * This method is used to return the Power Level
     * @parm - void
     * @return Powerlevel 
     */
    public int getPowerLevel()
    {
        return powerLevel;
    }
    
    /**
     * This method is used to set the Level
     * @parm inpPowerLevel - The Level to be set
     * @return void 
     */
    public void setPowerLevel(int inpPowerLevel)
    {
       powerLevel = inpPowerLevel;
    }
    
    /**
     * This method is used to return the name
     * @parm - void
     * @return Name
     */
    public String getName()
    {
        return name;
    }
    
    /**
     * This abstract method is used to draw the Figure
     * @parm Graphics - The Graphics object using which the Hero is drawn
     * @return void 
     */
    public abstract void draw(Graphics g);
}
