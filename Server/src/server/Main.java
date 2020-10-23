package server;

import java.util.ArrayList;
import java.io.File;

public class Main 
{
	public static void main(String[] args)
	{
		new Server(5444).start();
	}
}
