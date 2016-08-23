
/**
 * Write a description of class FirePower here.
 * 
 * @author (Prasanth Bhagavatula) 
 * @version (12/02/2015 - 0.1)
 */
public class FirePower implements PowerupPanel
{
    private Hero firePoweredHero;

    /**
     * Constructor for objects of class FirePower
     */
    public FirePower(Hero hero)
    {
        firePoweredHero = hero;
    }

    /**
     * Overriding the execute method
     * @param  void
     * @return void
     */
    public void execute()
    {
        System.out.println("Using the Fire Power Up");
    }
}
