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
	
	boolean isInitialized;
	boolean isGarbage;
	
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
		if (!this.keyExchange())
		{
			System.out.println("key exchange failed for client");
			return;
		}
		
		
	}
	
	public void terminate()
	{
		try
		{
			this.in.close();
			this.out.close();
			this.socket.close();
			this.isInitialized = false;
			this.isGarbage = true;
		}
		catch (Exception e)
		{
			System.out.println("client terminate failed");
			this.isInitialized = false;
			this.isGarbage = true;
			return;
		}
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
			
			// encrypt and send session token back, sending size first encrypted as well
			byte[] tokenPacket = this.AesEncrypt((Random.getString(16, 32) + "," + this.sessionToken + "," + Random.getString(16, 32)));
			System.out.println(tokenPacket.length);
			this.out.write(this.AesEncrypt(new String(tokenPacket.length + "")));
			this.out.write(tokenPacket);
			
			byte[] handshakeCompleteBufferSize = new byte[16];
			this.in.read(handshakeCompleteBufferSize);
			int handshakeCompleteSize = Integer.parseInt(new String(this.AesDecrypt(handshakeCompleteBufferSize)));
			
			byte[] handshakeCompleteBuffer = new byte[handshakeCompleteSize];
			this.in.read(handshakeCompleteBuffer);
			String[] handshakeCompleteSegments = new String(this.AesDecrypt(handshakeCompleteBuffer)).split(",");
			
			// validate handshake complete message
			if (handshakeCompleteSegments.length != 4 || !handshakeCompleteSegments[1].equals("handshake success") || !handshakeCompleteSegments[2].equals(this.sessionToken))
			{
				System.out.println("invalid handshake complete message");
				return false;
			}
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
			return null;
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return null;
		}
	}

	//
	// client specific encryption functions using session key+iv
	//
	private byte[] AesEncrypt(byte[] data)
	{
		return Crypto.AesEncrypt(data, this.sessionBase64AesKey, this.sessionBase64AesIv);
	}
	
	private byte[] AesEncrypt(String data)
	{
		try
		{
			return Crypto.AesEncrypt(data.getBytes(), this.sessionBase64AesKey, this.sessionBase64AesIv);
		}
		catch (Exception e)
		{
			return null;
		}
	}
	
	private byte[] AesDecrypt(byte[] data)
	{
		return Crypto.AesDecrypt(data, this.sessionBase64AesKey, this.sessionBase64AesIv);
	}
	
	private byte[] AesDecrypt(String data)
	{
		try
		{
			return Crypto.AesDecrypt(data.getBytes("UTF-8"), this.sessionBase64AesKey, this.sessionBase64AesIv);
		}
		catch (Exception e)
		{
			return null;
		}
	}
}
