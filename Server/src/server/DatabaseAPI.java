package server;

import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.time.Instant;
import java.util.ArrayList;
import java.sql.ResultSet;
import java.sql.Connection;

public class DatabaseAPI 
{
	private static final String JDBC_DRIVER = "com.mysql.cj.jdbc.Driver";
	private static final String DB_URL = "jdbc:mysql://165.227.219.1/LicensingSystem";
	private static final String USER = "swany55";
	private static final String PASS = "BA7YlD6AUWbociPs";
	
	private static Connection con = null;
	
	static void connect()
	{
		try 
		{
			if (con == null || con.isClosed())
			{
				Class.forName(JDBC_DRIVER);
				con = DriverManager.getConnection(DB_URL, USER, PASS);
			}
			
		} 
		catch (Exception e) 
		{
			e.printStackTrace();
		}
	}

	// 
	// database retrieval functions
	//
	static ClientRow getClient(String username)
	{
		try 
		{
			connect();
			
			PreparedStatement stmt = con.prepareStatement("SELECT ClientID, PasswordHash, HardwareID, IpAddress FROM client WHERE Username=?");
			stmt.setString(1, username);
			
			ResultSet rs = stmt.executeQuery();
			
			// if rs.next() returns false, no results
			if (!rs.next())
				return null;
			
			return new ClientRow(rs.getString("ClientID"), username, rs.getString("PasswordHash"), rs.getString("HardwareID"), rs.getString("IpAddress"));
		} 
		catch (Exception e) 
		{
			e.printStackTrace();
			return null;
		}
	}
	
	static LicenseRow getLicense(String LicenseID)
	{
		try 
		{
			connect();
			
			PreparedStatement stmt = con.prepareStatement("SELECT ClientID, ProductID, LicenseStart, LicenseEnd FROM license WHERE LicenseID=?");
			stmt.setString(1, LicenseID);
			
			ResultSet rs = stmt.executeQuery();
			
			// if rs.next() returns false, no results
			if (!rs.next())
				return null;
			
			return new LicenseRow(LicenseID, rs.getString("ClientID"), rs.getString("ProductID"), rs.getInt("LicenseStart"), rs.getInt("LicenseEnd"));
		} 
		catch (Exception e) 
		{
			e.printStackTrace();
			return null;
		}
	}
	
	static LicenseKeyRow getLicenseKey(String key)
	{
		try 
		{
			connect();
			
			PreparedStatement stmt = con.prepareStatement("SELECT LicenseKeyID, ProductID, LicenseDays, Vendor FROM licenseKey WHERE LicenseKey=?");
			stmt.setString(1, key);
			
			ResultSet rs = stmt.executeQuery();
			
			// if rs.next() returns false, no results
			if (!rs.next())
				return null;
			
			return new LicenseKeyRow(rs.getString("LicenseKeyID"), rs.getString("ProductID"), key, rs.getInt("LicenseDays"), rs.getString("Vendor"));
		} 
		catch (Exception e) 
		{
			e.printStackTrace();
			return null;
		}
	}
	
	static ProductRow getProduct(String ProductID)
	{
		try 
		{
			connect();
			
			PreparedStatement stmt = con.prepareStatement("SELECT ProductName, ServerFilepath FROM product WHERE ProductID=?");
			stmt.setString(1, ProductID);
			
			ResultSet rs = stmt.executeQuery();
			
			// if rs.next() returns false, no results
			if (!rs.next())
				return null;
			
			return new ProductRow(ProductID, rs.getString("ProductName"), rs.getString("ServerFilepath"));
		} 
		catch (Exception e) 
		{
			e.printStackTrace();
			return null;
		}
	}
	
	static RedeemedKeyRow getRedeemedKey(String)
	{
		
	}
	//
	// database insert functions
	//
	static boolean insertClient(ClientRow row)
	{
		try 
		{
			connect();
			
			PreparedStatement stmt = con.prepareStatement("INSERT INTO client VALUES(?, ?, ?, ?, ?)");
			stmt.setString(1, row.ClientID);
			stmt.setString(2, row.Username);
			stmt.setString(3, row.PasswordHash);
			stmt.setString(4, row.HardwareID);
			stmt.setString(5, row.IpAddress);
			
			stmt.executeUpdate();
			
			return true;
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return false;
		}
	}
	
	static boolean insertLicense(LicenseRow row)
	{
		try 
		{
			connect();
			
			PreparedStatement stmt = con.prepareStatement("INSERT INTO license VALUES(?, ?, ?, ?, ?)");
			stmt.setString(1, row.LicenseID);
			stmt.setString(2, row.ClientID);
			stmt.setString(3, row.ProductID);
			stmt.setInt(4, row.LicenseStart);
			stmt.setInt(5, row.LicenseEnd);
			
			stmt.executeUpdate();
			
			return true;
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return false;
		}
	}
	
	static boolean insertLicenseKey(LicenseKeyRow row)
	{
		try 
		{
			connect();
			
			PreparedStatement stmt = con.prepareStatement("INSERT INTO licenseKey VALUES(?, ?, ?, ?, ?)");
			stmt.setString(1, row.LicenseKeyID);
			stmt.setString(2, row.ProductID);
			stmt.setString(3, row.LicenseKey);
			stmt.setInt(4, row.LicenseDays);
			stmt.setString(5, row.Vendor);
			
			stmt.executeUpdate();
			
			return true;
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return false;
		}
	}

	static boolean insertProduct(ProductRow row)
	{
		try 
		{
			connect();
			
			PreparedStatement stmt = con.prepareStatement("INSERT INTO product VALUES(?, ?, ?)");
			stmt.setString(1, row.ProductID);
			stmt.setString(2, row.ProductName);
			stmt.setString(3, row.ServerFilePath);
			
			stmt.executeUpdate();
			
			return true;
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return false;
		}
	}
	
	static boolean insertRedeemedKey(RedeemedKeyRow row)
	{
		try 
		{
			connect();
			
			PreparedStatement stmt = con.prepareStatement("INSERT INTO redeemedKeys VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
			stmt.setString(1, row.RedeemedKeyID);
			stmt.setString(2, row.LicenseKeyID);
			stmt.setString(3, row.LicenseKey);
			stmt.setString(4, row.ClientID);
			stmt.setString(5, row.ProductID);
			stmt.setInt(6, row.LicenseDays);
			stmt.setString(7, row.Vendor);
			stmt.setInt(8, row.RedemptionTime);
			
			stmt.executeUpdate();
			
			return true;
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return false;
		}
	}

	static boolean insertRestriction(RestrictionsRow row)
	{
		try 
		{
			connect();
			
			PreparedStatement stmt = con.prepareStatement("INSERT INTO restrictions VALUES(?, ?, ?, ?, ?, ?)");
			stmt.setString(1, row.RestrictionID);
			stmt.setString(2, row.ClientID);
			stmt.setString(3, row.ProductID);
			stmt.setInt(4, row.RestrictionStart);
			stmt.setInt(5, row.RestrictionEnd);
			stmt.setString(6, row.Reason);
			
			stmt.executeUpdate();
			
			return true;
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return false;
		}
	}
	
	//
	// objects that represent a row in a table
	//
	public static class ClientRow
	{
		String ClientID;
		String Username;
		String PasswordHash;
		String HardwareID;
		String IpAddress;
		
		public ClientRow(String clientID, String username, String passwordHash, String hardwareID, String ipAddress) 
		{
			ClientID = clientID;
			Username = username;
			PasswordHash = passwordHash;
			HardwareID = hardwareID;
			IpAddress = ipAddress;
		}
	}
	
	public static class LicenseRow
	{
		String LicenseID;
		String ClientID;
		String ProductID;
		int LicenseStart;
		int LicenseEnd;
		
		public LicenseRow(String licenseID, String clientID, String productID, int licenseStart, int licenseEnd) 
		{
			LicenseID = licenseID;
			ClientID = clientID;
			ProductID = productID;
			LicenseStart = licenseStart;
			LicenseEnd = licenseEnd;
		}
	}
	
	public static class LicenseKeyRow
	{
		String LicenseKeyID;
		String ProductID;
		String LicenseKey;
		int LicenseDays;
		String Vendor;
		
		public LicenseKeyRow(String licenseKeyID, String productID, String licenseKey, int licenseDays, String vendor) 
		{
			LicenseKeyID = licenseKeyID;
			ProductID = productID;
			LicenseKey = licenseKey;
			LicenseDays = licenseDays;
			Vendor = vendor;
		}
	}
	
	public static class ProductRow
	{
		String ProductID;
		String ProductName;
		String ServerFilePath;
		
		public ProductRow(String productID, String productName, String serverFilePath) 
		{
			ProductID = productID;
			ProductName = productName;
			ServerFilePath = serverFilePath;
		}
	}
	
	public static class RedeemedKeyRow
	{
		String RedeemedKeyID;
		String LicenseKeyID;
		String LicenseKey;
		String ClientID;
		String ProductID;
		int LicenseDays;
		String Vendor;
		int RedemptionTime;
		
		public RedeemedKeyRow(String redeemedKeyID, String licenseKeyID, String licenseKey, String clientID, String productID, int licenseDays, String vendor, int redemptionTime) 
		{
			RedeemedKeyID = redeemedKeyID;
			LicenseKeyID = licenseKeyID;
			LicenseKey = licenseKey;
			ClientID = clientID;
			ProductID = productID;
			LicenseDays = licenseDays;
			Vendor = vendor;
			RedemptionTime = redemptionTime;
		}
	}
	
	public static class RestrictionsRow
	{
		String RestrictionID;
		String ClientID;
		String ProductID;
		int RestrictionStart;
		int RestrictionEnd;
		String Reason;
		
		public RestrictionsRow(String restrictionID, String clientID, String productID, int restrictionStart, int restrictionEnd, String reason) 
		{
			RestrictionID = restrictionID;
			ClientID = clientID;
			ProductID = productID;
			RestrictionStart = restrictionStart;
			RestrictionEnd = restrictionEnd;
			Reason = reason;
		}
	}
}
