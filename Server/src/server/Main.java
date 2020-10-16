package server;

import java.security.NoSuchAlgorithmException;

public class Main 
{
	public static void main(String[] args) throws NoSuchAlgorithmException, InterruptedException 
	{
		//DatabaseAPI.ClientRow result = DatabaseAPI.getClient("test1");
		//System.out.println(result.ClientID);
		
		System.out.println(DatabaseAPI.getProduct("213").ProductName);
	}
}
