
/**
 * Write a description of class LaserPower here.
 * 
 * @author (Prasanth Bhagavatula) 
 * @version (12/02/2015 - 0.1)
 */
public class LaserPower implements PowerupPanel
{
    private Hero laserPoweredHero;

    /**
     * Constructor for objects of class LaserPower
     */
    public LaserPower(Hero hero)
    {
        laserPoweredHero = hero;
    }

    /**
     * Overriding the execute method
     * @param  void
     * @return void
     */
    public void execute()
    {
        System.out.println("Using the Laser Power Up");
    }
}
