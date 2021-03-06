package server;

import java.security.KeyFactory;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.NoSuchAlgorithmException;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.spec.PKCS8EncodedKeySpec;
import java.security.spec.X509EncodedKeySpec;

import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

public class Crypto 
{
	static String RsaBase64PrivateKey = "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" + 
			"jKdbM/DTtXadq2ZA==";
	
	static String RsaBase64PublicKey = "MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAoNxow2PoETtIqEJUWcs3NalTXwCkAXcc71n67Q2jt4G8KrFcrFCR4Sz3rKWCEkgJRHoewYDgcrpgeWF0PAReMmOhkVPcYQPUSGJD9/dPkd" + 
			"CfXz9GORKXeaAtwwM3BicDpvZ+9YP9/q3kLb6p/FbRtpyHbKLbrwwhLnJ4E7lusqUh/nlOSeyOrvtYk65yzFj2T9XOBDdtsjqikTMeC5BjazJTYlMtk2AgEGHZWNi27Je5p/027K9knsG1QjVU/qcl" + 
			"B6PdBSynqitiFmWadMclSGTQ5AkQly1A1xzEHeXloKtLmOZu+h64jHDcq1fJSkn3ik4SclFKyMKQO2JGdLsqut9yJPJ9GxJDMYhqrP5KVTjLIuABWurmRs+HHjadXcvkoW1F8YCAxSh5byERgwMsEU" + 
			"PWWlhDrjnXU5e0G1Mc1MWB78wF0lljhC4+XjUNGqhtkAocyQ6S+cQtMccJUYIkbZNLHXn03LvtlgWFg+wAxDRvDnuZTnlwRPUk8wbsm7hJk/8o4T8c7IjzUcYFzJnSFNVT5RDH26aUeplG0FLDQPk7" + 
			"pTyyWFtIMfk/pkJaZ3SIvEwMPx95BplRI1wPkArNDwwvNy2H+vkg9j569+r/wDYJ/m0BOebiAmd/tApgO25I5DyrIWQ0hJNinnnQZejrkIprq0bP7Vai85FxKD5a92MCAwEAAQ==";
	
	public static void GenerateRsaKeys()
	{
		try
		{
			KeyPairGenerator keyGen = KeyPairGenerator.getInstance("RSA");
			keyGen.initialize(4096);
			
			KeyPair key = keyGen.generateKeyPair();
			
			String priv = new String(Base64.Encode(key.getPrivate().getEncoded()));
			String pub = new String(Base64.Decode(key.getPublic().getEncoded()));
			
			System.out.println("private key:\n" + priv);
			System.out.println("\npublic key:\n" + pub);
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	
	// low level functions for byte arrays
	public static byte[] RsaEncrypt(byte[] data, byte[] publicKeyBytes)
	{
		try 
		{
			KeyFactory keyFactory = KeyFactory.getInstance("RSA");
			X509EncodedKeySpec keySpec = new X509EncodedKeySpec(publicKeyBytes);
			PublicKey publicKey = keyFactory.generatePublic(keySpec);
			
			Cipher cipher = Cipher.getInstance("RSA/ECB/OAEPWithSHA-1AndMGF1Padding");
			cipher.init(Cipher.ENCRYPT_MODE, publicKey);
			
			return cipher.doFinal(data);
		} 
		catch (Exception e)
		{
			e.printStackTrace();
			return null;
		}
	}
	
	public static byte[] RsaDecrypt(byte[] encrypted, byte[] privateKeyBytes)
	{
		try
		{
			KeyFactory kf = KeyFactory.getInstance("RSA");
			PKCS8EncodedKeySpec keySpecPKCS8 = new PKCS8EncodedKeySpec(privateKeyBytes);
			PrivateKey privKey = kf.generatePrivate(keySpecPKCS8);
			
			Cipher cipher = Cipher.getInstance("RSA/ECB/OAEPWithSHA-1AndMGF1Padding");
			cipher.init(Cipher.DECRYPT_MODE, privKey);
			
			return cipher.doFinal(encrypted);
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return null;
		}
		
	}

	public static byte[] AesEncrypt(byte[] data, byte[] key, byte[] iv)
	{
		try
		{
			IvParameterSpec initVector = new IvParameterSpec(iv);
	        SecretKeySpec aesKey = new SecretKeySpec(key, "AES");
			
	        Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
	        cipher.init(Cipher.ENCRYPT_MODE, aesKey, initVector);
	        
	        return cipher.doFinal(data);
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return null;
		}
	}
	
	public static byte[] AesDecrypt(byte[] encrypted, byte[] key, byte[] iv)
	{
		try
		{
			IvParameterSpec initVector= new IvParameterSpec(iv);
	        SecretKeySpec aesKey = new SecretKeySpec(key, "AES");
			
	        Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
	        cipher.init(Cipher.DECRYPT_MODE, aesKey, initVector);
	        
	        return cipher.doFinal(encrypted);
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return null;
		}
	}
	
	// easy use functions to call from Client
	public static byte[] RsaDecrypt(byte[] encrypted)
	{
		byte[] priv = Base64.Decode(Crypto.RsaBase64PrivateKey.getBytes());
		
		return RsaDecrypt(encrypted, priv);
	}
	
	public static byte[] RsaDecrypt(String encrypted)
	{
		byte[] priv = Base64.Decode(Crypto.RsaBase64PrivateKey.getBytes());
		
		return RsaDecrypt(encrypted.getBytes(), priv);
	}

	public static byte[] AesEncrypt(byte[] data, String base64Key, String base64Iv)
	{
		byte[] key = Base64.Decode(base64Key.getBytes());
		byte[] iv = Base64.Decode(base64Iv.getBytes());
		
		return AesEncrypt(data, key, iv);
	}
	
	public static byte[] AesEncrypt(String data, String base64Key, String base64Iv)
	{
		byte[] key = Base64.Decode(base64Key.getBytes());
		byte[] iv = Base64.Decode(base64Iv.getBytes());
		
		return AesEncrypt(data.getBytes(), key, iv);
	}
	
	public static byte[] AesDecrypt(byte[] encrypted, String base64Key, String base64Iv)
	{
		byte[] key = Base64.Decode(base64Key.getBytes());
		byte[] iv = Base64.Decode(base64Iv.getBytes());
		
		return AesDecrypt(encrypted, key, iv);
	}
}
