package server;

import java.security.KeyFactory;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.NoSuchAlgorithmException;
import java.security.PrivateKey;
import java.security.interfaces.RSAPrivateKey;
import java.security.spec.KeySpec;
import java.security.spec.PKCS8EncodedKeySpec;
import java.util.Base64;

import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

public class Crypto 
{
	static String RsaBase64PrivateKey = "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" + 
			"P7DkVtnrDN9d/yWg==";
	
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
	
	public static String RsaDecrypt(String base64EncryptedData)
	{
		try
		{
			KeyFactory kf = KeyFactory.getInstance("RSA");
			
			PKCS8EncodedKeySpec keySpecPKCS8 = new PKCS8EncodedKeySpec(Base64.getDecoder().decode(RsaBase64PrivateKey));
			PrivateKey privKey = kf.generatePrivate(keySpecPKCS8);
			
			Cipher cipher = Cipher.getInstance("RSA/ECB/OAEPWithSHA-1AndMGF1Padding");
			cipher.init(Cipher.DECRYPT_MODE, privKey);
			
			return new String(cipher.doFinal(Base64.getDecoder().decode(base64EncryptedData)));
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return null;
		}
		
	}

	public static String AesEncrypt(String data, String base64Key, String base64Iv)
	{
		try
		{
			IvParameterSpec iv = new IvParameterSpec(Base64.getDecoder().decode(base64Iv));
	        SecretKeySpec key = new SecretKeySpec(Base64.getDecoder().decode(base64Key), "AES");
			
	        Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
	        cipher.init(Cipher.ENCRYPT_MODE, key, iv);
	        
	        return new String(Base64.getEncoder().encode(cipher.doFinal(data.getBytes())));
		}
		catch (Exception e)
		{
			return null;
		}
	}
	
	public static String AesDecrypt(String base64EncryptedData, String base64Key, String base64Iv)
	{
		try
		{
			IvParameterSpec iv = new IvParameterSpec(Base64.getDecoder().decode(base64Iv));
	        SecretKeySpec key = new SecretKeySpec(Base64.getDecoder().decode(base64Key), "AES");
			
	        Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
	        cipher.init(Cipher.DECRYPT_MODE, key, iv);
	        
	        return new String(cipher.doFinal(Base64.getDecoder().decode(base64EncryptedData)));
		}
		catch (Exception e)
		{
			return null;
		}
	}
}
