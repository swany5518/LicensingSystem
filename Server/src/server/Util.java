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
	
	public static String hashFileBytes(String path)
	{
		try 
		{
			byte[] file = Files.readAllBytes(Paths.get(path));
			MessageDigest sha256 = MessageDigest.getInstance("SHA-256");
			
			return new String(Base64.Encode(sha256.digest(file)));
		} 
		catch (Exception e)
		{
			e.printStackTrace();
			return null;
		}
	}
	
	public static String hashString(String password)
	{
		try 
		{
			MessageDigest sha256 = MessageDigest.getInstance("SHA-256");
			return new String(Base64.Encode(sha256.digest(password.getBytes())), 0, 20);
		} 
		catch (Exception e)
		{
			return null;
		}
	}
}
