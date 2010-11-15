package edu.teco.dpws.generator.util;

import java.io.BufferedReader;
import java.io.File;

import java.io.FileReader;
import java.io.IOException;

public class QuoteFile {
	
	public static String quote(String file) throws IOException
	{
		String buf="";
		File f=new File(file);
		BufferedReader r=new BufferedReader(new FileReader(f));try{
		String s;
		while(null!=(s=r.readLine()))
		{
			buf+="\""+s.replaceAll("\\\\", "\\\\\\\\").replace("\"", "\\\"")+"\\n\"\n";
		}	
		}finally{r.close();}

		return buf.toString();
	}
}
