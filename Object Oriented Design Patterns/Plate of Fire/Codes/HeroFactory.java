
/**
 * Write a description of interface HeroFactory here.
 * 
 * @author (Prasanth Bhagavatula)
 * @version (12/02/2015 - 0.1)
 */
public interface HeroFactory
{
    /**
     * Abstract method to be overridden
     * @param  void
     * @return Hero object
     */
    Hero getHero();
    
    /**
     * Abstract method to be overridden
     * @param  inpLevel - Level of the Hero; inpPowerLevel - Power Level of the Hero
     * @return Hero object
     */
    Hero getHero(int inpLevel, int inpPowerLevel);
}
