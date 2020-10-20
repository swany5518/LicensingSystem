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
	
	public static int hashFileBytes(String path)
	{
		try 
		{
			byte[] file = Files.readAllBytes(Paths.get(path));
			MessageDigest d = MessageDigest.getInstance("SHA-256");
			byte[] hash = d.digest(file);
			
			System.out.println(new String(Base64.Encode(hash)));
			return 1;
		} 
		catch (Exception e)
		{
			return 0;
		}
	}
}
