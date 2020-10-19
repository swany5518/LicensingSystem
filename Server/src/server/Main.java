package server;

import java.io.UnsupportedEncodingException;

public class Main 
{
	public static void main(String[] args) throws UnsupportedEncodingException 
	{
		Server server = new Server(5444);
		server.start();
	}
}
