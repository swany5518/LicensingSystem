package server;

public class Random 
{
	static String characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvxyz";
	
	static String getString(int size)
	{
		StringBuilder str = new StringBuilder(size);
		
		for (int i = 0; i < size; ++i)
			str.append(characters.charAt((int)(characters.length() * Math.random())));
		
		return str.toString();
	}
	
	static String getString(int min, int max)
	{
		int size = min + (int)(Math.random() * (max - min));
		StringBuilder str = new StringBuilder(size);
		
		for (int i = 0; i < size; ++i)
			str.append(characters.charAt((int)(characters.length() * Math.random())));
		
		return str.toString();
	}
	
	static String getUUID()
	{
		return getString(8) + "-" + getString(4) + "-" + getString(4) + "-" + getString(4) + "-" + getString(12);
	}
}
