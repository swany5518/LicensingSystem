package server;

public class Main 
{
	public static void main(String[] args)
	{
		DatabaseActions.RegisterResult rslt = DatabaseActions.attemptRegister("prusm4", "p1useword", "aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaab", "1iHFBy8M-46jN-90oM-WeKM-r50NUor84XtT", "ipaddress");
		//DatabaseActions.RedeemResult rslt1 = DatabaseActions.attemptRedeem("JK22z3Wb-aQrn-P4dR-NhoG-X1ZVJzNIo23U", "0tTNIPxW-aV07-Bfzl-D8eI-QnGtMkI8CPAG");
		
		//for (int i = 0; i < 100; ++i)
			//DatabaseAPI.insertLicenseKey(new DatabaseAPI.LicenseKeyRow(Random.getUUID(), "12", 10, "test vendor"));
		
		System.out.println(rslt.ordinal());
	}
}
