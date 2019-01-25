//FULL duplex
//With CIP
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Arrays;
import java.io.*;
public class Server_Temp {

    public static void main(String[] args) throws IOException {
    	String msg="";
	int temp;
    	System.out.println("start");
        ServerSocket ss=new ServerSocket(7808);
        
        while(true){
            Socket s=ss.accept();
            DataInputStream din=new DataInputStream(s.getInputStream());
            //System.out.println(din.readUTF());
            BufferedReader reader=new BufferedReader(new InputStreamReader(din));
            
            msg=reader.readLine();
            
            System.out.println("Temperature is: "+msg);
            temp=Integer.parseInt(msg);
            msg="";
            //to reply back to client
            OutputStream os = s.getOutputStream();
            OutputStreamWriter osw = new OutputStreamWriter(os);
            BufferedWriter bw = new BufferedWriter(osw);
             if(temp> 30){
		msg="Critical";
		}
             else{
		msg="Normal";
		}
 
            String sendMessage = msg + "\n";
            bw.write(sendMessage);
            bw.flush();
           // System.out.println(Arrays.toString(msg.getBytes()));
        }
    }
}
