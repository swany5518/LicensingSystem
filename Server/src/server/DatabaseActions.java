package server;

public class DatabaseActions 
{
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
		UsernameNotFound,
		incorrectPassword,
		hwidMismatch,
		banned
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

