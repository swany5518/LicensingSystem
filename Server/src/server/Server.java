package server;

import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class Server 
{
	private ServerSocket serverSocket;
	private List<Client> clients;
	
	public Server(int port)
	{
		try
		{
			this.serverSocket = new ServerSocket(port);
			this.serverSocket.setReuseAddress(true);
			
			clients = Collections.synchronizedList(new ArrayList<Client>());
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	
	public void start()
	{
		try 
		{
			System.out.println("Server Started...Accepting Clients");
			
			while (true)
			{
				Socket newConnection = serverSocket.accept();
				Client newClient = new Client(newConnection);
				
				System.out.println("New Client From: " + newConnection.getInetAddress().getHostAddress());
				
				new Thread(newClient).start();
			}
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
}
