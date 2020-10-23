package server;

import java.util.ArrayList;
import java.io.File;

public class Main 
{
	public static void main(String[] args)
	{
		DatabaseActions.ProductRequestReturn rst = DatabaseActions.requestProduct("aE7j6NWg-D6nO-iI7y-gcSK-zlQzhomrQlzx", "17");
	
		if (rst != null)
		{
			System.out.println(rst.result.ordinal());
			System.out.println(rst.info);
			System.out.println(rst.secondsLeft);
		}
	}
}
