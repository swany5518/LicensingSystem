package server;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class ClientSupervisor implements Runnable
{
	public static List<Client> clientList; 
	private final static int timeoutSeconds = 1200;
	
	public ClientSupervisor()
	{
		clientList = Collections.synchronizedList(new ArrayList<Client>());
	}
	
	public void run()
	{
		while (true)
		{
			try
			{
				synchronized (clientList)
				{
					for (int i = 0; i < clientList.size(); ++i)
					{
						Client client = clientList.get(i);
						
						if (client.getIsTerminated())
						{
							System.out.println("Removing " + client.getUsername() + "@" + client.getIpAddress());
							clientList.remove(i--);
						}
						else if (client.getIsInitialized() && client.getSecondsSinceLastInteraction() > timeoutSeconds)
						{
							System.out.println("Timing out " + client.getUsername() + "@" + client.getIpAddress());
							client.terminate();
							clientList.remove(i--);
						}
					
					}
				}
				Thread.sleep(1000);
			}
			catch (Exception e)
			{
				e.printStackTrace();
			}
		}
	}
}
