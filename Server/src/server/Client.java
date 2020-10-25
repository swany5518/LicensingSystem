package server;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.ArrayList;

//
// Client class that represents a client connection to the server
//
public class Client implements Runnable
{
	private static enum PacketType
	{
		login,
		register,
		redeemKey,
		getLicences,
		productRequst,
		clientUpdate,
		heartbeat,
		disconnect
	}
	private Socket socket;
	private DataInputStream in;
	private DataOutputStream out;
	
	String sessionBase64AesKey;
	String sessionBase64AesIv;
	String sessionToken;
	
	long lastInteraction;
	boolean isInitialized;
	boolean isGarbage;
	
	String ipAddress;
	String clientID;
	String username;
	
	public Client(Socket socket)
	{
		try
		{
			this.socket = socket;
			this.ipAddress = socket.getInetAddress().getHostAddress();
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
		
		while (isInitialized)
		{
			try
			{
				System.out.println("waiting for packet");
				String packet = this.receivePacket();
				if (packet == null)
					this.terminate();
				
				this.handlePacket(packet);
			}
			catch (Exception e)
			{
				e.printStackTrace();
				this.terminate();
			}
		}
	}
	
	public void terminate()
	{
		try
		{
			System.out.println("terminating client");
			this.in.close();
			this.out.close();
			this.socket.close();
			this.clientID = null;
			this.username = null;
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
			
			this.isInitialized = true;
			
			return true;
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return false;
		}
	}
	//
	// function to handle incoming packets, and respond
	//
	public void handlePacket(String packet)
	{
		try
		{
			System.out.println("Client sent: " + packet);
			// all packets have randominpadding,type,sessiontoken,otherdata...
			String[] segments = packet.split(",");
			if (segments.length < 3)
				this.terminate();
			
			PacketType type = PacketType.values()[Integer.parseInt(segments[2])];
			if (!segments[1].equals(this.sessionToken))
				this.terminate();
			
			// randompadding,sessiontoken,0,username,password,hwid,randomPadding
			if (type == PacketType.login)
			{
				DatabaseActions.LoginReturn rslt = DatabaseActions.attemptLogin(segments[3], segments[4], segments[5]);
				
				// only success needs special handing, the result info will contain ClientID
				if (rslt.result == DatabaseActions.LoginResult.success)
				{
					this.clientID = rslt.info;
					this.sendPacket(Random.getString(16, 32) + "," + this.sessionToken + "," + rslt.result.ordinal() + ",," + Random.getString(16,  32));
				}
				else
					this.sendPacket(Random.getString(16, 32) + "," + this.sessionToken + "," + rslt.result.ordinal() + "," + rslt.info + "," + Random.getString(16,  32));
			}
			// randompadding,sessiontoken,1,username,password,hwid,productKey,randomPadding
			else if (type == PacketType.register)
			{
				DatabaseActions.RegisterResult rslt = DatabaseActions.attemptRegister(segments[3], segments[4], segments[5], segments[6], this.ipAddress);
				
				this.sendPacket(Random.getString(16, 32) + "," + this.sessionToken + "," + rslt.ordinal() + "," + Random.getString(16,  32));
			}
			// randompadding,sessiontoken,2,key,randomPadding
			else if (type == PacketType.redeemKey)
			{
				DatabaseActions.RedeemResult rslt = DatabaseActions.attemptRedeem(this.clientID, segments[3]);
				this.sendPacket(Random.getString(16, 32) + "," + this.sessionToken + "," + rslt.ordinal() + "," + Random.getString(16,  32));
			}
			// randompadding,sessiontoken,3,hwid,randompadding
			else if (type == PacketType.getLicences)
			{
				ArrayList<DatabaseActions.LicensesReturn> licenses = DatabaseActions.getActiveLicenses(this.clientID);
				
				if (licenses == null)
					this.sendPacket(Random.getString(16, 32) + "," + this.sessionToken + ",0," + Random.getString(16, 32));
				
				String response = Random.getString(16, 32) + "," + this.sessionToken + "," + licenses.size();
				
				for (DatabaseActions.LicensesReturn row : licenses)
				{
					response += "," + row.productID + "," + row.productName + "," + row.secondsLeft;
				}
				
				response += "," + Random.getString(16, 32);
				
				this.sendPacket(response);
			}
			// randompadding,sessiontoken,4,productID,fileHash,randomPadding
			else if (type == PacketType.productRequst)
			{
				DatabaseActions.ProductRequestReturn rslt = DatabaseActions.requestProduct(this.clientID, segments[3]);
				
				if (rslt.result == DatabaseActions.ProductRequestResult.success)
				{
					String serverFileHash = Util.hashFileBytes(rslt.info);
					// if the hash is the same, no update needed
					if (serverFileHash.equals(segments[4]))
					{
						this.sendPacket(Random.getString(16, 32) + "," + this.sessionToken + "," + rslt.result.ordinal() + "," + rslt.secondsLeft + ",0," + Random.getString(16,  32));
					}
					// else we need to send the new/updated file
					else
					{
						this.sendPacket(Random.getString(16, 32) + "," + this.sessionToken + "," + rslt.result.ordinal() + "," + rslt.secondsLeft + ",1," + Random.getString(16,  32));
						byte[] fileBytes = Files.readAllBytes(Paths.get(rslt.info));
						this.sendPacket(fileBytes);
					}
				}
				else
				{
					this.sendPacket(Random.getString(16, 32) + "," + this.sessionToken + "," + rslt.result.ordinal() + "," + rslt.info + ",," + Random.getString(16,  32));
				}
			}
			// randompadding,sessiontoken,5,hwid,clientHash,randomPadding
			else if (type == PacketType.clientUpdate)
			{
				
			}
			// randompadding,sessiontoken,6,randomPadding
			else if (type == PacketType.heartbeat)
			{
				this.sendPacket(Random.getString(16, 32) + "," + this.sessionToken + ",0," + Random.getString(16, 32));
			}
			else if (type == PacketType.disconnect)
			{
				this.terminate();
			}
		}
		catch (Exception e)
		{
			
		}
		
	}
	//
	// packet send and receive functions
	//
	public boolean sendPacket(String packet)
	{
		System.out.println("Sever sending: " + packet);
		try
		{
			byte[] packetBytes = this.AesEncrypt(packet);
			byte[] packetBlocks = this.AesEncrypt(packetBytes.length / 16 + "");
			
			this.out.write(packetBlocks);
			this.out.write(packetBytes);
			
			return true;
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return false;
		}
	}
	
	public boolean sendPacket(byte[] packet)
	{
		try
		{
			byte[] packetBytes = this.AesEncrypt(packet);
			byte[] packetBlocks = this.AesEncrypt(packetBytes.length / 16 + "");
			
			this.out.write(packetBlocks);
			this.out.write(packetBytes);
			
			return true;
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return false;
		}
	}
	
	public String receivePacket()
	{
		try
		{
			byte[] size = new byte[16];
			this.in.read(size);
			size = this.AesDecrypt(size);
			int packetLength = Integer.parseInt(new String(size)) * 16;
			
			byte[] packetData = new byte[packetLength];
			this.in.read(packetData);
			
			this.lastInteraction = Util.getServerSecond();
			return new String(this.AesDecrypt(packetData));
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
