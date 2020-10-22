package server;

import java.security.SecureRandom;

public class Random 
{
	static String characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvxyz";
	
	static String getString(int size)
	{
		SecureRandom rnd = new SecureRandom();
		StringBuilder str = new StringBuilder(size);
		
		for (int i = 0; i < size; ++i)
			str.append(characters.charAt(rnd.nextInt(characters.length())));
		
		return str.toString();
	}
	
	static String getString(int min, int max)
	{
		SecureRandom rnd = new SecureRandom();
		int size = rnd.nextInt(max - min + 1) + min;
		
		StringBuilder str = new StringBuilder(size);
		
		for (int i = 0; i < size; ++i)
			str.append(characters.charAt(rnd.nextInt(characters.length())));
		
		return str.toString();
	}
	
	static String getUUID()
	{
		return getString(8) + "-" + getString(4) + "-" + getString(4) + "-" + getString(4) + "-" + getString(12);
	}
}
