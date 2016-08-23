
/**
 * Write a description of class DisguisedHero here.
 * 
 * @author (Prasanth Bhagavatula) 
 * @version (12/02/2015 - 0.1)
 */

import java.awt.*;
import java.lang.*;

public class DisguisedHero extends Hero
{
    protected final Hero disguisedHero;
    
    /**
     * Constructor for objects of class DisguisedHero
     */
    public DisguisedHero(Hero hero)
    {
        disguisedHero = hero;
    }

    /**
     * Overriding the getLevel method
     * @parm - void
     * @return Level 
     */
    public int getLevel()
    {
        return disguisedHero.getLevel();
    }
    
    /**
     * Overriding the setLevel method
     * @parm inpLevel - The Level to be set
     * @return void 
     */
    public void setLevel(int inpLevel)
    {
       disguisedHero.setLevel(inpLevel);
    }
    
    /**
     * Overriding the getPowerLevel method
     * @parm - void
     * @return Powerlevel 
     */
    public int getPowerLevel()
    {
        return disguisedHero.getPowerLevel();
    }
    
    /**
     * Overriding the setPowerLevel method
     * @parm inpPowerLevel - The Level to be set
     * @return void 
     */
    public void setPowerLevel(int inpPowerLevel)
    {
       disguisedHero.setPowerLevel(inpPowerLevel);
    }
    
    /**
     * Overriding the getName method
     * @parm - void
     * @return Name
     */
    public String getName()
    {
        return disguisedHero.getName();
    }
    
    
    /**
     * Overriding the draw method
     * @param  Graphics
     * @return void
     */
    public void draw(Graphics g)
    {
        disguisedHero.draw(g);
    }
}
