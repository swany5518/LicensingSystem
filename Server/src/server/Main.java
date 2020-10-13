package server;

import java.security.NoSuchAlgorithmException;

public class Main 
{
	public static void main(String[] args) throws NoSuchAlgorithmException 
	{
		String enc = "QzlVvI8gfBRxWwNkFoDBl11HsleT9fjmXimQY82gwDKkzU4X0TQuMtJDBDgHX/9Gb4ydWWWx" + 
				"sR3vby3EtwK1h9JTk6WE5RDtFMsJX/awKF1rEyyOIixJJIM/1hUbBPNDCIMppeRk78TJBJvq" + 
				"aB/7O1gzG+28dAFsTPfBBxxSkL5EfK30tGs3DagsBebc8RNGocS5NXFDpkY/TxTrw6o+U6/U" + 
				"uGa5+aO0eYoLehjeg7ZzxVKQ5spyHmQ9lcBlIVxtSIOgpaHWl9CTPOnZDQ8nBXg8rayJdZ4j" + 
				"wL0eglSaPX9HICQwvq4jKAjaKKCuXo4B8NQ0Xw4DbGsfrTaIrTJlIfI1JxX5jdo04ABvdH1c" + 
				"yKvyWGAjYo/mJ0pUrhsITPjE67M9dcj5TNMFJztk6g+JHBbg4tOaRrd7iWpjJ1Ws98d8nPxs" + 
				"IF4imrprQaPbYyeXH+yShvBOx4WS2P705mJ8fcVYOpaH6kzN2JSoa1NjawLCi1wTrPwjiUSK" + 
				"nXrousFNzHSC5RMu8A83+i1VUxxduQ/AHcL02voMdfVZQ3zGFA0UC9JKVlqprkRn4UgI36ne" + 
				"PapQialIKlLenuop7QhwNLWSW9ungUsMxvhd7EZxv6IM4uY0BgyuY96J8gc7Elym5C1htrGt" + 
				"qFuTrLXkUyp2bY4sbFvnbNOk3mLn1JPZJH4=";
		
		String key = "Lcelv9iYYgt0SORANC4f7g==";
		String iv = "42K7Hyw5T5jz251zLQcy+Q==";
		
		String msg = "super secret message";
		
		System.out.println(Crypto.AesEncrypt(msg, key, iv));

	}
}
