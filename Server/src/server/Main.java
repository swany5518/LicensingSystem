package server;

import java.security.NoSuchAlgorithmException;

public class Main 
{
	public static void main(String[] args) throws NoSuchAlgorithmException 
	{
		//DatabaseAPI.ClientRow result = DatabaseAPI.getClient("test1");
		//System.out.println(result.ClientID);
		
		DatabaseAPI.insertClient(new DatabaseAPI.ClientRow(Random.getUUID(), "test2", "hash", "hwid", "ip"));
	}
}
