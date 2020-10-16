package server;

public class Util 
{
	public static int getServerSecond()
	{
		return (int)java.time.Instant.now().getEpochSecond();
	}
}
