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
	
	static Connection con = null;
	
	static void Connect()
	{
		try 
		{
			if (con.equals(null) || con.isClosed())
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
	
	static void 
}
