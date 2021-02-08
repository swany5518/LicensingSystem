package server;

import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class Server 
{
	private ServerSocket serverSocket;
	
	public Server(int port)
	{
		try
		{
			this.serverSocket = new ServerSocket(port);
			this.serverSocket.setReuseAddress(true);
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
			new Thread(new ClientSupervisor()).start();
			
			while (true)
			{
				Socket newConnection = serverSocket.accept();
				Client newClient = new Client(newConnection);
				
				boolean ipAlreadyConnected = false;
				
				synchronized (ClientSupervisor.clientList)
				{
					for (Client x : ClientSupervisor.clientList)
					{
						if (x.getIpAddress().equals(newClient.getIpAddress()))
						{
							if (x.getIsTerminated())
								continue;
							else
							{
								System.out.println("duplicate ip");
								newClient.terminate();
								ipAlreadyConnected = true;
								break;
							}
						}
					}	
					
					if (!ipAlreadyConnected)
						ClientSupervisor.clientList.add(newClient);
				}
				if (!ipAlreadyConnected)
				{
				System.out.println("New Client From: " + newConnection.getInetAddress().getHostAddress());
				
				new Thread(newClient).start();
				}
			}
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
}
