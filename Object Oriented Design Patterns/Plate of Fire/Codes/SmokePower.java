
/**
 * Write a description of class SmokePower here.
 * 
 * @author (Prasanth Bhagavatula) 
 * @version (12/02/2015 - 0.1)
 */
public class SmokePower implements PowerupPanel
{
    private Hero smokePoweredHero;

    /**
     * Constructor for objects of class SmokePower
     */
    public SmokePower(Hero hero)
    {
        smokePoweredHero = hero;
    }

    /**
     * Overriding the execute method
     * @param  void
     * @return void
     */
    public void execute()
    {
        System.out.println("Using the Smoke Power Up");
    }
}
