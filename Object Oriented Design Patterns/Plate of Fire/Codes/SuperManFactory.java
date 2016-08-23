
/**
 * Write a description of class SuperManFactory here.
 * 
 * @author (Prasanth Bhagavatula) 
 * @version (12/02/2015 - 0.1)
 */
public class SuperManFactory implements HeroFactory
{
    private static SuperManFactory smf;

    /**
     * Overriding the Default Constructor
     */
    private SuperManFactory(){
    }
    
    /**
     * Method to create an instance of SuperManFactory
     * @param  void
     * @return SuperManFactory object
     */
    public static synchronized SuperManFactory getSuperManFactory(){
        if (smf == null){
            smf = new SuperManFactory();
        }
        return smf;
    }
    
    /**
     * Overriding the getHero Method
     * @param  void
     * @return SuperMan object pointed by Hero reference variable
     */
    public Hero getHero()
    {
        return SuperMan.getSuperMan();
    }
    
    /**
     * Overriding the getHero Method
     * @param  inpLevel - Level of SuperMan; inpPowerLevel - Power Level of SuperMan
     * @return SuperMan object pointed by Hero reference variable
     */
    public Hero getHero(int inpLevel, int inpPowerLevel)
    {
        return SuperMan.getSuperMan(inpLevel, inpPowerLevel);
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
