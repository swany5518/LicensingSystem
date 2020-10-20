package server;

import java.nio.file.Files;
import java.nio.file.Paths;
import java.security.MessageDigest;

public class Util 
{
	public static int getServerSecond()
	{
		return (int)java.time.Instant.now().getEpochSecond();
	}
	
	public static byte[] hashFileBytes(String path)
	{
		try 
		{
			byte[] file = Files.readAllBytes(Paths.get(path));
			
			MessageDigest sha256 = MessageDigest.getInstance("SHA-256");
			return sha256.digest(file);
			
		} 
		catch (Exception e)
		{
			return null;
		}
	}
}
