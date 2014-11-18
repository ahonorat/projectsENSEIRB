package pg306.server1;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import pg306.utils.CreateSampleDocuments;

/**
 * Hello world!
 *
 */
public class App 
{
    public static void main( String[] args )
    {
        System.out.println( "Hello World!" );
        Monitor1 monit = new Monitor1();
        CreateSampleDocuments.flushDB();
        CreateSampleDocuments.storeFakeDocs(monit);
        
    	try{
    		BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
     
    		String input;
    		while((input=br.readLine())!=null){
    			System.out.println(input);
    		}
     
    	}catch(IOException io){
    		io.printStackTrace();
    	}	
    }
}
