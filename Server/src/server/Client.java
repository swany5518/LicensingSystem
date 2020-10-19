package server;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.Socket;
import java.util.Arrays;

//
// Client class that represents a client connection to the server
//
public class Client implements Runnable
{
	private Socket socket;
	private DataInputStream in;
	private DataOutputStream out;
	
	String sessionBase64AesKey;
	String sessionBase64AesIv;
	String sessionToken;
	
	public Client(Socket socket)
	{
		try
		{
			this.socket = socket;
			this.in = new DataInputStream(socket.getInputStream());
			this.out = new DataOutputStream(socket.getOutputStream());
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	
	//
	// each client runs on separate thread
	//
	@Override
	public void run() 
	{
		this.keyExchange();
	}
	
	//
	// function that handles key exchange
	//
	public boolean keyExchange()
	{
		try
		{
			String clientHelloMsg= "hello server";
			int clientHelloWaitSeconds = 3;
			
			// give clients x seconds to send client hello, otherwise terminate
			int start_second = Util.getServerSecond();
			while (this.in.available() == 0)
			{
				if (Util.getServerSecond() - start_second >= clientHelloWaitSeconds)
				{
					System.out.println("client didn't send hello in time");
					this.out.write("error]".getBytes());
					return false;
				}
				Thread.sleep(20);
			}
			
			System.out.println(this.in.available());
			// receive client hello message
			byte[] clientHelloBuffer = new byte[clientHelloMsg.length()];
			this.in.read(clientHelloBuffer);
			
			
			// if the message is not "hello server", error
			if (!new String(clientHelloBuffer).equals(clientHelloMsg))
			{
				System.out.println("invalid client hello");
				return false;
			}
			
			// send client rsa public key with delimiter at the end
			this.out.write((Crypto.RsaBase64PublicKey + "]").getBytes());
			
			// receive the clients encrypted + encoded session aes key+iv
			int clientKeysBufferSize = 1024;
			byte[] clientKeysBuffer = new byte[clientKeysBufferSize];
			this.in.read(clientKeysBuffer);
			
			// loop through buffer to find delimiter
			int clientKeysSize = 0;
			while (true)
			{
				if (clientKeysBuffer[clientKeysSize] == (byte)']')
					break;
				
				if (clientKeysSize++ == clientKeysBufferSize)
				{
					System.out.println("no delimiter found");
					return false;
				}	
			}
			
			// copy the key data up to the delimiter, decode it from base64 and decrypt it
			byte[] clientKeys = Arrays.copyOf(clientKeysBuffer, clientKeysSize);
			clientKeys = Base64.Decode(clientKeys);
			clientKeys = Crypto.RsaDecrypt(clientKeys);
			
			// the key and iv are separated by a ','
			String[] keyAndIv = new String(clientKeys).split(",");
			if (keyAndIv.length < 2)
			{
				System.out.println("error parsing key and iv");
				return false;
			}
			this.sessionBase64AesKey = keyAndIv[0];
			this.sessionBase64AesIv = keyAndIv[1];
			
			// generate and set session token
			this.sessionToken = Random.getString(16, 32);
			
			System.out.println(this.sessionBase64AesIv);
			return true;
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return false;
		}
	}
	
	//
	// packet send and receive functions
	//
	public boolean sendPacket(byte[] packet)
	{
		try
		{
			String size = packet.length + "";
			
			while (size.length() < 16)
				size += " ";
			
			out.write(size.getBytes());
			
			out.write(packet);
			return true;
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return false;
		}
	}
	
	public byte[] receivePacket()
	{
		try
		{
			byte[] incoming_size = new byte[16];
			this.in.read(incoming_size);
			int size = Integer.parseInt(new String(incoming_size).replaceAll(" ", ""));
			System.out.println("Incoming packet size: " + size);
			
			byte[] packet = new byte[size];
			this.in.read(packet);
			return packet;
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return null;
		}
	}
}
