package server;

import java.security.KeyFactory;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.NoSuchAlgorithmException;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.interfaces.RSAPrivateKey;
import java.security.spec.KeySpec;
import java.security.spec.PKCS8EncodedKeySpec;
import java.security.spec.X509EncodedKeySpec;
import java.util.Base64;

import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

public class Crypto 
{
	static String RsaBase64PrivateKey = "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" + 
			"hkC/djkXONsrYibw==";
	
	public static void GenerateRsaKeys() throws NoSuchAlgorithmException
	{
		KeyPairGenerator keyGen = KeyPairGenerator.getInstance("RSA");
		keyGen.initialize(4096);
		
		KeyPair key = keyGen.generateKeyPair();
		
		String priv = new String(Base64.getEncoder().encode(key.getPrivate().getEncoded()));
		String pub = new String(Base64.getEncoder().encode(key.getPublic().getEncoded()));
		
		System.out.println("private key:\n" + priv);
		System.out.println("\npublic key:\n" + pub);
	}
	
	// low level functions for byte arrays
	public static byte[] RsaEncrypt(byte[] data, byte[] publicKeyBytes)
	{
		try 
		{
			KeyFactory keyFactory = KeyFactory.getInstance("RSA");
			X509EncodedKeySpec keySpec = new X509EncodedKeySpec(publicKeyBytes);
			PublicKey publicKey = keyFactory.generatePublic(keySpec);
			
			Cipher cipher = Cipher.getInstance("RSA/ECB/PKCS1Padding");
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
	
}
