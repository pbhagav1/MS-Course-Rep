
/**
 * Write a description of class CloakDisguisedHero here.
 * 
 * @author (Prasanth Bhagavatula) 
 * @version (12/02/2015 - 0.1)
 */

import java.awt.*;

public class CloakDisguisedHero extends DisguisedHero
{
    /**
     * Constructor for objects of class CloakDisguisedHero
     */
    public CloakDisguisedHero(Hero hero)
    {
        super(hero);
    }

    /**
     * Overriding the draw method
     * @param  Graphics g
     * @return void
     */
    public void draw(Graphics g)
    {
        super.draw(g);
        System.out.println(super.getName() + " is decorated with Cape");
    }
}
