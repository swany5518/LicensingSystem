package server;

public class Base64 
{
	public static byte[] Encode(byte[] data)
	{
		return java.util.Base64.getEncoder().encode(data);
	}
	
	public static String Encode(String data)
	{
		return new String(java.util.Base64.getEncoder().encode(data.getBytes()));
	}
	
	public static byte[] Decode(byte[] encoded)
	{
		return java.util.Base64.getDecoder().decode(encoded);
	}
	
	public static byte[] Decode(String encoded)
	{
		return java.util.Base64.getDecoder().decode(encoded.getBytes());
	}
}
