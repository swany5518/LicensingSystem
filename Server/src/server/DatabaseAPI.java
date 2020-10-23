package server;

import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import java.sql.Connection;


public class DatabaseAPI 
{
	private static final String JDBC_DRIVER = "com.mysql.cj.jdbc.Driver";
	private static final String DB_URL = "jdbc:mysql://165.227.219.1/LicensingSystem";
	private static final String USER = "swany55";
	private static final String PASS = "BA7YlD6AUWbociPs";
	
	//
	// simple connection pool implementation
	//
	private static final int MAX_CONNECTIONS = 20;
	private static int totalConnections = 0;
	private static List<Connection> connectionPool = Collections.synchronizedList(new ArrayList<Connection>());
	
	static
	{
		try
		{
			Class.forName(JDBC_DRIVER);
		}
		catch (Exception e) {};
	}
	
	private static Connection getConnection()
	{
		while (true)
		{
			try
			{
				synchronized (connectionPool)
				{
					//System.out.print(Thread.currentThread() + " in getConnection SyncBlock. Pool size: " + connectionPool.size() + " ");
					if (connectionPool.size() == 0 && totalConnections < MAX_CONNECTIONS)
					{
						//System.out.print("No available connections, creating.\n");
						++totalConnections;
						return DriverManager.getConnection(DB_URL, USER, PASS);
					}
					else if (connectionPool.size() > 0)	
					{
						//System.out.print("connection available, using.\n");
						Connection temp = connectionPool.remove(connectionPool.size() - 1);
						if (!temp.isValid(3))
						{
							temp.close();
							return DriverManager.getConnection(DB_URL, USER, PASS);
						}
						
						return temp;
					}
					else
					{
						//System.out.print("no connections available, trying again in 1 second\n");
					}
				}
				
				Thread.sleep(100);
			}
			catch (Exception e) 
			{
				e.printStackTrace();
				//System.out.println("error getting connection");
			}
		}
	}
	
	private static void releaseConnection(Connection con)
	{
		while (true)
		{
			try
			{
				synchronized (connectionPool)
				{
					//System.out.print(Thread.currentThread() + " in relConnection() SyncBlock. Returning Connection\n");
					connectionPool.add(con);
					return;
				}
			}
			catch (Exception e) 
			{
				e.printStackTrace();
				//System.out.println("error releasing connection");
			}
		}
	}
	
	// 
	// database retrieval functions
	//
	static ClientRow getClient(String username)
	{
		Connection con = getConnection();
		
		try 
		{
			PreparedStatement stmt = con.prepareStatement("SELECT ClientID, PasswordHash, HardwareID, IpAddress FROM client WHERE Username=?");
			stmt.setString(1, username);
			
			ResultSet rs = stmt.executeQuery();
			
			// if rs.next() returns false, no results
			if (!rs.next())
				return null;
			
			releaseConnection(con);
			
			return new ClientRow(rs.getString("ClientID"), username, rs.getString("PasswordHash"), rs.getString("HardwareID"), rs.getString("IpAddress"));
		} 
		catch (Exception e) 
		{
			totalConnections--;
			e.printStackTrace();
			return null;
		}
	}
	
	static ClientRow getClientByHwid(String hwid)
	{
		try 
		{
			Connection con = getConnection();
			
			PreparedStatement stmt = con.prepareStatement("SELECT ClientID, PasswordHash, Username, IpAddress FROM client WHERE HardwareID=?");
			stmt.setString(1, hwid);
			
			ResultSet rs = stmt.executeQuery();
		
			// if rs.next() returns false, no results
			if (!rs.next())
				return null;
			
			releaseConnection(con);
			
			return new ClientRow(rs.getString("ClientID"), rs.getString("Username"), rs.getString("PasswordHash"), hwid, rs.getString("IpAddress"));
		} 
		catch (Exception e) 
		{
			totalConnections--;
			e.printStackTrace();
			return null;
		}
	}
	
	static LicenseRow getLicense(String LicenseID)
	{
		try 
		{
			Connection con = getConnection();
			
			PreparedStatement stmt = con.prepareStatement("SELECT ClientID, ProductID, LicenseStart, LicenseEnd FROM license WHERE LicenseID=?");
			stmt.setString(1, LicenseID);
			
			ResultSet rs = stmt.executeQuery();
			
			// if rs.next() returns false, no results
			if (!rs.next())
				return null;
			
			releaseConnection(con);
			
			return new LicenseRow(LicenseID, rs.getString("ClientID"), rs.getString("ProductID"), rs.getInt("LicenseStart"), rs.getInt("LicenseEnd"));
		} 
		catch (Exception e) 
		{
			totalConnections--;
			e.printStackTrace();
			return null;
		}
	}
	
	static LicenseRow getLicenseByClientAndProductID(String ClientID, String ProductID)
	{
		try 
		{
			Connection con = getConnection(); 
			
			PreparedStatement stmt = con.prepareStatement("SELECT LicenseID, LicenseStart, LicenseEnd FROM license WHERE ClientID=? AND ProductID=?");
			stmt.setString(1, ClientID);
			stmt.setString(2, ProductID);
			
			ResultSet rs = stmt.executeQuery();
			
			// if rs.next() returns false, no results
			if (!rs.next())
				return null;
			
			releaseConnection(con);
			
			return new LicenseRow(rs.getString("LicenseID"), ClientID, ProductID, rs.getInt("LicenseStart"), rs.getInt("LicenseEnd"));
		} 
		catch (Exception e) 
		{
			totalConnections--;
			e.printStackTrace();
			return null;
		}
	}
	
	static ArrayList<LicenseRow> getLicenses(String ClientID)
	{
		try 
		{
			Connection con = getConnection();
			
			PreparedStatement stmt = con.prepareStatement("SELECT LicenseID, ProductID, LicenseStart, LicenseEnd FROM license WHERE ClientID=?");
			stmt.setString(1, ClientID);
			
			ResultSet rs = stmt.executeQuery();
			ArrayList<LicenseRow> list = new ArrayList<LicenseRow>();
			
			while (rs.next())
			{
				LicenseRow row = new LicenseRow(rs.getString("LicenseID"), ClientID, rs.getString("ProductID"), rs.getInt("LicenseStart"), rs.getInt("LicenseEnd"));
				
				if (row.LicenseEnd >= Util.getServerSecond())
					list.add(row);
				else
					removeLicense(row.LicenseID);
			}
			
			releaseConnection(con);
			
			if (list.size() == 0)
				return null;
			
			return list;
			
		} 
		catch (Exception e) 
		{
			totalConnections--;
			e.printStackTrace();
			return null;
		}
	}
	
	static LicenseKeyRow getLicenseKey(String key)
	{
		try 
		{
			Connection con = getConnection(); 
			
			PreparedStatement stmt = con.prepareStatement("SELECT ProductID, LicenseDays, Vendor FROM licenseKey WHERE LicenseKey=?");
			stmt.setString(1, key);
			
			ResultSet rs = stmt.executeQuery();
			
			// if rs.next() returns false, no results
			if (!rs.next())
				return null;
			
			releaseConnection(con);
			
			return new LicenseKeyRow(key, rs.getString("ProductID"), rs.getInt("LicenseDays"), rs.getString("Vendor"));
		} 
		catch (Exception e) 
		{
			totalConnections--;
			e.printStackTrace();
			return null;
		}
	}
	
	static ProductRow getProduct(String ProductID)
	{
		try 
		{
			Connection con = getConnection(); 
			
			PreparedStatement stmt = con.prepareStatement("SELECT ProductName, Status, ServerFilepath FROM product WHERE ProductID=?");
			stmt.setString(1, ProductID);
			
			ResultSet rs = stmt.executeQuery();
			
			// if rs.next() returns false, no results
			if (!rs.next())
				return null;
			
			releaseConnection(con);
			
			return new ProductRow(ProductID, rs.getString("ProductName"), rs.getString("Status"), rs.getString("ServerFilepath"));
		} 
		catch (Exception e) 
		{
			totalConnections--;
			e.printStackTrace();
			return null;
		}
	}
	
	static RedeemedKeyRow getRedeemedKey(String key)
	{
		try
		{
			Connection con = getConnection(); 
			
			PreparedStatement stmt = con.prepareStatement("SELECT ClientID, ProductID, LicenseDays, Vendor, RedemptionTime FROM redeemedKeys WHERE LicenseKey=?");
			stmt.setString(1, key);
			
			ResultSet rs = stmt.executeQuery();
			
			// if rs.next() returns false, no results
			if (!rs.next())
				return null;
		
			releaseConnection(con);
			
			return new RedeemedKeyRow(key, rs.getString("ClientID"), rs.getString("ProductID"), rs.getInt("LicenseDays"), 
					rs.getString("Vendor"), rs.getInt("RedemptionTime"));
		} 
		catch (Exception e) 
		{
			totalConnections--;
			e.printStackTrace();
			return null;
		}
	}
	
	static RestrictionsRow getRestriction(String RestrictionID)
	{
		try
		{
			Connection con = getConnection(); 
			
			PreparedStatement stmt = con.prepareStatement("SELECT ClientID, ProductID, RestrictionStart, RestrictionEnd, Reason FROM restrictions WHERE RestrictionID=?");
			stmt.setString(1, RestrictionID);
			
			ResultSet rs = stmt.executeQuery();
			
			// if rs.next() returns false, no results
			if (!rs.next())
				return null;
		
			releaseConnection(con);
			
			return new RestrictionsRow(RestrictionID, rs.getString("ClientID"), rs.getString("ProductID"), 
					rs.getInt("RestrictionStart"), rs.getInt("RestrictionEnd"), rs.getString("Reason"));
		} 
		catch (Exception e) 
		{
			totalConnections--;
			e.printStackTrace();
			return null;
		}
	}
	
	static ArrayList<RestrictionsRow> getRestrictions(String ClientID)
	{
		try
		{
			Connection con = getConnection(); 
			
			PreparedStatement stmt = con.prepareStatement("SELECT RestrictionID, ProductID, RestrictionStart, RestrictionEnd, Reason FROM restrictions WHERE ClientID=?");
			stmt.setString(1, ClientID);
			ResultSet rs = stmt.executeQuery();
			
			ArrayList<RestrictionsRow> list = new ArrayList<RestrictionsRow>();
			
			while (rs.next())
			{
				RestrictionsRow row = new RestrictionsRow(rs.getString("RestrictionID"), ClientID, rs.getString("ProductID"), rs.getInt("RestrictionStart"), rs.getInt("RestrictionEnd"), rs.getString("Reason"));
				
				if (row.RestrictionEnd >= Util.getServerSecond())
					list.add(row);
				else
					removeRestriction(row.RestrictionID);	
			}
		
			releaseConnection(con);
			
			if (list.size() == 0)
				return null;
			
			return list;
		} 
		catch (Exception e) 
		{
			totalConnections--;
			e.printStackTrace();
			return null;
		}
	}
	
	static RestrictionsRow getRestrictionByClientID(String ClientID)
	{
		try
		{
			Connection con = getConnection(); 
			
			PreparedStatement stmt = con.prepareStatement("SELECT RestrictionID, ProductID, RestrictionStart, RestrictionEnd, Reason FROM restrictions WHERE ClientID=?");
			stmt.setString(1, ClientID);
			
			ResultSet rs = stmt.executeQuery();
			
			// if rs.next() returns false, no results
			if (!rs.next())
				return null;
		
			releaseConnection(con);
			
			return new RestrictionsRow(rs.getString("RestrictionID"), ClientID, rs.getString("ProductID"), 
					rs.getInt("RestrictionStart"), rs.getInt("RestrictionEnd"), rs.getString("Reason"));
		} 
		catch (Exception e) 
		{
			totalConnections--;
			e.printStackTrace();
			return null;
		}
	}
	
	static RestrictionsRow getRestrictionByClientAndProductID(String ClientID, String ProductID)
	{
		try
		{
			Connection con = getConnection(); 
			
			PreparedStatement stmt = con.prepareStatement("SELECT RestrictionID, RestrictionStart, RestrictionEnd, Reason FROM restrictions WHERE ClientID=? AND ProductID=?");
			stmt.setString(1, ClientID);
			stmt.setString(2, ProductID);
			
			ResultSet rs = stmt.executeQuery();
			
			// if rs.next() returns false, no results
			if (!rs.next())
				return null;
		
			releaseConnection(con);
			
			return new RestrictionsRow(rs.getString("RestrictionID"), ClientID, ProductID, 
					rs.getInt("RestrictionStart"), rs.getInt("RestrictionEnd"), rs.getString("Reason"));
		} 
		catch (Exception e) 
		{
			totalConnections--;
			e.printStackTrace();
			return null;
		}
	}
	
	//
	// database insert functions
	//
	static boolean insertClient(ClientRow row)
	{
		try 
		{
			Connection con = getConnection();
			
			PreparedStatement stmt = con.prepareStatement("INSERT INTO client VALUES(?, ?, ?, ?, ?)");
			stmt.setString(1, row.ClientID);
			stmt.setString(2, row.Username);
			stmt.setString(3, row.PasswordHash);
			stmt.setString(4, row.HardwareID);
			stmt.setString(5, row.IpAddress);
			
			stmt.executeUpdate();
			
			 releaseConnection(con);
			
			return true;
		}
		catch (Exception e)
		{
			totalConnections--;
			e.printStackTrace();
			return false;
		}
	}
	
	static boolean insertLicense(LicenseRow row)
	{
		try 
		{
			Connection con = getConnection();
			
			PreparedStatement stmt = con.prepareStatement("INSERT INTO license VALUES(?, ?, ?, ?, ?)");
			stmt.setString(1, row.LicenseID);
			stmt.setString(2, row.ClientID);
			stmt.setString(3, row.ProductID);
			stmt.setInt(4, row.LicenseStart);
			stmt.setInt(5, row.LicenseEnd);
			
			stmt.executeUpdate();
			
			 releaseConnection(con);
			
			return true;
		}
		catch (Exception e)
		{
			totalConnections--;
			e.printStackTrace();
			return false;
		}
	}
	
	static boolean insertLicenseKey(LicenseKeyRow row)
	{
		try 
		{
			Connection con = getConnection(); 
			
			PreparedStatement stmt = con.prepareStatement("INSERT INTO licenseKey VALUES(?, ?, ?, ?)");
			stmt.setString(1, row.LicenseKey);
			stmt.setString(2, row.ProductID);
			stmt.setInt(3, row.LicenseDays);
			stmt.setString(4, row.Vendor);
			
			stmt.executeUpdate();
			
			releaseConnection(con);
			
			return true;
		}
		catch (Exception e)
		{
			totalConnections--;
			e.printStackTrace();
			return false;
		}
	}

	static boolean insertProduct(ProductRow row)
	{
		try 
		{
			Connection con = getConnection(); 
			
			PreparedStatement stmt = con.prepareStatement("INSERT INTO product VALUES(?, ?, ?, ?)");
			stmt.setString(1, row.ProductID);
			stmt.setString(2, row.ProductName);
			stmt.setString(3, row.Status);
			stmt.setString(4, row.ServerFilePath);
			
			stmt.executeUpdate();
			
			releaseConnection(con);
			
			return true;
		}
		catch (Exception e)
		{
			totalConnections--;
			e.printStackTrace();
			return false;
		}
	}
	
	static boolean insertRedeemedKey(RedeemedKeyRow row)
	{
		try 
		{
			Connection con = getConnection(); 
			
			PreparedStatement stmt = con.prepareStatement("INSERT INTO redeemedKeys VALUES(?, ?, ?, ?, ?, ?)");
			stmt.setString(1, row.LicenseKey);
			stmt.setString(2, row.ClientID);
			stmt.setString(3, row.ProductID);
			stmt.setInt(4, row.LicenseDays);
			stmt.setString(5, row.Vendor);
			stmt.setInt(6, row.RedemptionTime);
			
			stmt.executeUpdate();
			
			releaseConnection(con);
			
			return true;
		}
		catch (Exception e)
		{
			totalConnections--;
			e.printStackTrace();
			return false;
		}
	}

	static boolean insertRestriction(RestrictionsRow row)
	{
		try 
		{
			Connection con = getConnection(); 
			
			PreparedStatement stmt = con.prepareStatement("INSERT INTO restrictions VALUES(?, ?, ?, ?, ?, ?)");
			stmt.setString(1, row.RestrictionID);
			stmt.setString(2, row.ClientID);
			stmt.setString(3, row.ProductID);
			stmt.setInt(4, row.RestrictionStart);
			stmt.setInt(5, row.RestrictionEnd);
			stmt.setString(6, row.Reason);
			
			stmt.executeUpdate();
			
			releaseConnection(con);
			
			return true;
		}
		catch (Exception e)
		{
			totalConnections--;
			e.printStackTrace();
			return false;
		}
	}
	
	//
	// database delete functions
	//
	static boolean removeLicenseKey(String key)
	{
		try
		{
			Connection con = getConnection();
			PreparedStatement stmt = con.prepareStatement("DELETE FROM licenseKey WHERE LicenseKey=?");
			stmt.setString(1, key);
			
			stmt.executeUpdate();
			
			releaseConnection(con);
			
			return true;
		}
		catch (Exception e)
		{
			totalConnections--;
			e.printStackTrace();
			return false;
		}
	}
	
	static boolean removeLicense(String LicenseID)
	{
		try
		{
			Connection con = getConnection();
			PreparedStatement stmt = con.prepareStatement("DELETE FROM license WHERE LicenseID=?");
			stmt.setString(1, LicenseID);
			
			stmt.executeUpdate();
			
			releaseConnection(con);
			
			return true;
		}
		catch (Exception e)
		{
			totalConnections--;
			e.printStackTrace();
			return false;
		}
	}
	
	static boolean removeRestriction(String RestrictionID)
	{
		try
		{
			Connection con = getConnection();
			PreparedStatement stmt = con.prepareStatement("DELETE FROM restrictions WHERE RestrictionID=?");
			stmt.setString(1, RestrictionID);
			
			stmt.executeUpdate();
			
			releaseConnection(con);
			
			return true;
		}
		catch (Exception e)
		{
			totalConnections--;
			e.printStackTrace();
			return false;
		}
	}
	
	//
	// database update functions
	//
	static boolean updateLicense(LicenseRow update)
	{
		try
		{
			Connection con = getConnection();
			
			//preparing statement 
			PreparedStatement stmt = con.prepareStatement("UPDATE license SET ClientID=?, ProductID=?, LicenseEnd=? WHERE LicenseID=?");
			stmt.setString(1, update.ClientID);
			stmt.setString(2, update.ProductID);
			stmt.setInt(3, update.LicenseEnd);
			
			stmt.executeUpdate();
		
			releaseConnection(con);
			
			return true;
		}
		catch (Exception e)
		{
			totalConnections--;
			e.printStackTrace();
			return false;
		}
	}
	
	static boolean updateClientHwid(String ClientID, String newHwid)
	{
		try
		{
			Connection con = getConnection();
			
			//preparing statement 
			PreparedStatement stmt = con.prepareStatement("UPDATE client SET HardwareID=? WHERE ClientID=?");
			stmt.setString(1, newHwid);
			stmt.setString(2, ClientID);
			
			stmt.executeUpdate();
		
			releaseConnection(con);
			
			return true;
		}
		catch (Exception e)
		{
			totalConnections--;
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
		String LicenseKey;
		String ProductID;
		int LicenseDays;
		String Vendor;
		
		public LicenseKeyRow(String licenseKey, String productID, int licenseDays, String vendor) 
		{
			LicenseKey = licenseKey;
			ProductID = productID;
			LicenseDays = licenseDays;
			Vendor = vendor;
		}
	}
	
	public static class ProductRow
	{
		String ProductID;
		String ProductName;
		String Status;
		String ServerFilePath;
		
		public ProductRow(String productID, String productName, String status, String serverFilePath) 
		{
			ProductID = productID;
			ProductName = productName;
			Status = status;
			ServerFilePath = serverFilePath;
		}
	}
	
	public static class RedeemedKeyRow
	{
		String LicenseKey;
		String ClientID;
		String ProductID;
		int LicenseDays;
		String Vendor;
		int RedemptionTime;
		
		public RedeemedKeyRow(String licenseKey, String clientID, String productID, int licenseDays, String vendor, int redemptionTime) 
		{
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
