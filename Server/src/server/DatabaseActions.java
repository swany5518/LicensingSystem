package server;

import java.io.File;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.Iterator;

public class DatabaseActions 
{
	public static class LoginReturn
	{
		LoginResult result;
		String info;
		
		LoginReturn(LoginResult result, String info)
		{
			this.result = result;
			this.info = info;
		}
		
		LoginReturn(LoginResult result)
		{
			this.result = result;
			this.info = " ";
		}
	}
	
	public static class ProductRequestReturn
	{
		ProductRequestResult result;
		String info;
		int secondsLeft;
		
		public ProductRequestReturn(ProductRequestResult result)
		{
			this.result = result;
			this.info = " ";
			this.secondsLeft = 0;
		}
		
		public ProductRequestReturn(ProductRequestResult result, String info)
		{
			this.result = result;
			this.info = info;
			this.secondsLeft = 0;
		}
		
		public ProductRequestReturn(ProductRequestResult result, String info, int secondsLeft)
		{
			this.result = result;
			this.info = info;
			this.secondsLeft = secondsLeft;
		}
	}
	
	public static class LicensesReturn
	{
		String productID;
		String productName;
		int secondsLeft;
		
		public LicensesReturn(String productID, String productName, int secondsLeft)
		{
			this.productID = productID;
			this.productName = productName;
			this.secondsLeft = secondsLeft;
		}
	}
	
	public static enum RegisterResult
	{
		success,
		invalidUsername,
		invalidPassword,
		invalidProductKey,
		hwidHasAccount,
		usernameTaken,
		unknownError,
	}
	
	public static enum RedeemResult
	{
		success,
		invalidKey,
		unknownError
	}

	public static enum LoginResult
	{
		success,
		invalidUsername,
		invalidPassword,
		usernameNotFound,
		incorrectPassword,
		hwidMismatch,
		banned,
		unknownError
	}
	
	public static enum ProductRequestResult
	{
		success,
		noLicenseFound,
		expired,
		banned,
		productDown,
		productNotFound,
		unknownError
	}
	
	public static RegisterResult attemptRegister(String username, String password, String hwid, String key, String ip)
	{
		username = username.toLowerCase();
		// validate all field formats
		if (!filterUsername(username))
			return RegisterResult.invalidUsername;
		if (!filterPassword(password))
			return RegisterResult.invalidPassword;
		if (!filterHwid(hwid))
			return RegisterResult.unknownError;
		if (!filterKey(key))
			return RegisterResult.invalidProductKey;
		
		// check for username taken
		if (DatabaseAPI.getClient(username) != null)
			return RegisterResult.usernameTaken;
		
		// check for account on hwid
		if (DatabaseAPI.getClientByHwid(hwid) != null)
			return RegisterResult.hwidHasAccount;
		
		// check if license key exists
		DatabaseAPI.LicenseKeyRow lkResult = DatabaseAPI.getLicenseKey(key);
		if (lkResult == null)
			return RegisterResult.invalidProductKey;
		
		// at this point everything checks out. Insert new client and redeem key
		DatabaseAPI.ClientRow newClient = new DatabaseAPI.ClientRow(Random.getUUID(), username, Util.hashString(password), hwid, ip);
		if (!DatabaseAPI.insertClient(newClient))
			return RegisterResult.unknownError;
		
		RedeemResult rslt = attemptRedeem(newClient.ClientID, key);
		
		if (rslt != RedeemResult.success)
			return RegisterResult.unknownError;
		
		return RegisterResult.success;
	}
	
	public static RedeemResult attemptRedeem(String clientID, String key)
	{
		if (!filterKey(key))
			return RedeemResult.invalidKey;
		
		// get keyRow
		DatabaseAPI.LicenseKeyRow keyRow = DatabaseAPI.getLicenseKey(key);
		
		if (keyRow == null)
			return RedeemResult.invalidKey;
		
		// check if client has sub to add on to
		DatabaseAPI.LicenseRow licenseRow = DatabaseAPI.getLicenseByClientAndProductID(clientID, keyRow.ProductID);
		
		// if no existing license for product, add a new one
		if (licenseRow == null)
		{
			int licenseStart = Util.getServerSecond();
			int licenseEnd = keyRow.LicenseDays == -1 ? -1 : keyRow.LicenseDays * 86400 + licenseStart;
			licenseRow = new DatabaseAPI.LicenseRow(Random.getUUID(), clientID, keyRow.ProductID, licenseStart, licenseEnd);
			
			// if new license is added we need to delete key and add it to redeemed keys
			if (DatabaseAPI.insertLicense(licenseRow))
			{
				DatabaseAPI.RedeemedKeyRow redeemed = new DatabaseAPI.RedeemedKeyRow(key, clientID, keyRow.ProductID, keyRow.LicenseDays, keyRow.Vendor, licenseStart);
				
				if (DatabaseAPI.removeLicenseKey(key) && DatabaseAPI.insertRedeemedKey(redeemed)) 
					return RedeemResult.success;
				
				return RedeemResult.unknownError;
			}
		}
		// if existing license for product
		else
		{
			int newEndTime = keyRow.LicenseDays == -1 ? -1 : keyRow.LicenseDays * 86400 + licenseRow.LicenseEnd;
			licenseRow.LicenseEnd = newEndTime;
			
			if (DatabaseAPI.updateLicense(licenseRow))
			{
				DatabaseAPI.RedeemedKeyRow redeemed = new DatabaseAPI.RedeemedKeyRow(key, clientID, keyRow.ProductID, keyRow.LicenseDays, keyRow.Vendor, Util.getServerSecond());
				if (DatabaseAPI.removeLicenseKey(key) && DatabaseAPI.insertRedeemedKey(redeemed)) 
					return RedeemResult.success;
				
				return RedeemResult.unknownError;
			}
		}
		
		return RedeemResult.unknownError;
	}

	public static LoginReturn attemptLogin(String username, String password, String hwid)
	{
		username = username.toLowerCase();
		// validate all field formats
		if (!filterUsername(username))
			return new LoginReturn(LoginResult.invalidUsername);
		if (!filterPassword(password))
			return new LoginReturn(LoginResult.invalidPassword);
		if (!filterHwid(hwid))
			return new LoginReturn(LoginResult.unknownError);
		
		password = Util.hashString(password);
		
		DatabaseAPI.ClientRow client = DatabaseAPI.getClient(username);
		
		if (client == null)
			return new LoginReturn(LoginResult.usernameNotFound);
		if (!client.HardwareID.equals(hwid))
		{
			// if the client needs to update hwid, there hwid will be set to 'reset'
			if (client.HardwareID.equals("reset"))
			{
				if (!DatabaseAPI.updateClientHwid(client.ClientID, hwid))
					return new LoginReturn(LoginResult.hwidMismatch, "reset failed");
			}
			else
				return new LoginReturn(LoginResult.hwidMismatch);
		}
		if (!client.PasswordHash.equals(password))
			return new LoginReturn(LoginResult.incorrectPassword);
		
		DatabaseAPI.RestrictionsRow restrictions = DatabaseAPI.getRestrictionByClientID(client.ClientID);
		
		// if banned from all products
		if (restrictions != null && restrictions.ProductID.equals("all products"))
			return new LoginReturn(LoginResult.banned, restrictions.Reason);
		
		return new LoginReturn(LoginResult.success, client.ClientID);
	}
	
	public static ArrayList<LicensesReturn> getActiveLicenses(String ClientID)
	{
		ArrayList<DatabaseAPI.LicenseRow> licenses = DatabaseAPI.getLicenses(ClientID);
		if (licenses == null)
			return null;
		
		ArrayList<DatabaseAPI.RestrictionsRow> restrictions = DatabaseAPI.getRestrictions(ClientID);
		if (restrictions != null)
		{
			for (DatabaseAPI.RestrictionsRow restriction : restrictions)
				if (restriction.ProductID.equals("all products"))
					return null;
			
			// remove any restricted licenses
			Iterator<DatabaseAPI.LicenseRow> it = licenses.iterator();
			while (it.hasNext())
			{
				DatabaseAPI.LicenseRow license = it.next();
				
				for (DatabaseAPI.RestrictionsRow restriction : restrictions)
					if (restriction.ProductID.equals(license.ProductID))
						it.remove();	
			}
		}
		
		if (licenses.size() == 0)
			return null;
		
		ArrayList<LicensesReturn> ret = new ArrayList<LicensesReturn>();
		
		for (DatabaseAPI.LicenseRow row : licenses)
		{
			DatabaseAPI.ProductRow pr = DatabaseAPI.getProduct(row.ProductID);
			ret.add(new LicensesReturn(row.ProductID, pr.ProductName, row.LicenseEnd - Util.getServerSecond()));
		}
		
		return ret;
	}
	
	public static ProductRequestReturn requestProduct(String ClientID, String ProductID)
	{
		DatabaseAPI.LicenseRow row = DatabaseAPI.getLicenseByClientAndProductID(ClientID, ProductID);
		
		if (row == null)
			return new ProductRequestReturn(ProductRequestResult.noLicenseFound);
		
		if (row.LicenseEnd <= Util.getServerSecond())
			return new ProductRequestReturn(ProductRequestResult.expired);
		
		// verify product exists on server
		DatabaseAPI.ProductRow product = DatabaseAPI.getProduct(ProductID);
		if (product == null || !new File(product.ServerFilePath).isFile())
			return new ProductRequestReturn(ProductRequestResult.productNotFound);
		
		// check if product is down
		if (!product.Status.equals("up"))
			return new ProductRequestReturn(ProductRequestResult.productDown, product.Status);
			
		// check for restrictions
		DatabaseAPI.RestrictionsRow restriction = DatabaseAPI.getRestrictionByClientAndProductID(ClientID, ProductID);
		if (restriction != null && restriction.RestrictionEnd > Util.getServerSecond())
			return new ProductRequestReturn(ProductRequestResult.banned, restriction.Reason);
		else if (restriction != null)
			DatabaseAPI.removeRestriction(restriction.RestrictionID);
		
		return new ProductRequestReturn(ProductRequestResult.success, product.ServerFilePath, row.LicenseEnd - Util.getServerSecond());
	}
	
	private static boolean filterPassword(String password)
	{
		String SpecialPasswordCharacters = "!#$%&()*+-.:;<=>?@[]_{|}~";
		// passwords 6-30 characters long
		if (password.length() < 6 || password.length() > 30)
			return false;
		
		// passwords contain only certain characters
		for (int i = 0; i < password.length(); i++) 
		{
			Character token = Character.valueOf(password.charAt(i));
			
			if (!(Character.isLetter(token) || Character.isDigit(token) || SpecialPasswordCharacters.contains(token.toString())))
				return false;
		}
		
		return true;
	}

	private static boolean filterUsername(String username)
	{
		// usernames 4-16 characters long
		if (username.length() < 4 || username.length() > 24) 
			return false;
		
		// usernames contain only certain characters (-, . or _)
		for (int i = 0; i < username.length(); i++) 
		{
			Character token = Character.valueOf(username.charAt(i));
			
			if (!(Character.isLetter(token) || Character.isDigit(token) || token.equals('-') || token.equals('.') || token.equals('_')))
				return false;
		}
		
		return true;
	}

	private static boolean filterHwid(String hwid)
	{
		if (hwid.length() != 36) // valid keys always 38 long
			return false;
		
		// AAAAAAAA-BBBB-CCCC-DDDD-EEEEEEEEEEEE
		if (hwid.charAt(8)  != '-' 
			|| hwid.charAt(13) != '-'
			|| hwid.charAt(18) != '-'
			|| hwid.charAt(23) != '-')
			return false;
		
		return true;
	}
	
	private static boolean filterKey(String key)
	{
		if (key.length() != 36) // valid keys always 36 long
			return false;
		
		// AAAAAAAA-BBBB-CCCC-DDDD-EEEEEEEEEEEE
		if (key.charAt(8)  != '-' 
			|| key.charAt(13) != '-'
			|| key.charAt(18) != '-'
			|| key.charAt(23) != '-')
			return false;
		
		return true;
	}
}