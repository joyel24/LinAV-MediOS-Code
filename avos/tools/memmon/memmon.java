import java.io.*;
import java.util.*;

/**
 *  avOS - http://avos.sourceforge.net
 *  Copyright (c) 2003 by Jimmy Moore
 *
 *  All files in this archive are subject to the GNU General Public License.
 *  See the file COPYING in the source tree root for full license agreement.
 *  This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 *  KIND, either express of implied.
 *
 *@author     DoggerMoore@yahoo.com
 *@created    20/03/2004
 */

public class memmon {
    static Process proc = null;
    static BufferedReader br;
    static InputStream i;
    static OutputStream o;

    static byte[] oldbuff = null;
    static byte[] membuff = null;
    
    static long startAddr = 0x30000;
    static int blocks = 1;
    
    public static void main(String[] args) {
        System.out.println("memmon v0.01 (c) Dogger");
        args = parseArgs(args);
        
        System.out.println("Monitoring " + hex8(startAddr) + " - " + hex8(startAddr + blocks*512));
        
        try {
            proc = Runtime.getRuntime().exec("./ser");
            i = proc.getInputStream();
            br = new BufferedReader(new InputStreamReader(i));
            o = proc.getOutputStream();
            
            while(true) {
                oldbuff = membuff;
                membuff = new byte[512*blocks];
                for (int u=0;u<blocks;u++) {
                    readMem(startAddr + (u*512), membuff, 512*u);
                }
                
                // now compare with last one...
                if (oldbuff!=null) {
                    for (int j=0;j<512*blocks;j+=2) {
                        int old = ((int)oldbuff[j] & 0xff) | (((int)oldbuff[j+1] & 0xff)<<8);
                        int mem = ((int)membuff[j] & 0xff) | (((int)membuff[j+1] & 0xff)<<8);
                        
                        if (old!=mem) {
                            System.out.println(hex8(startAddr + j)
                                              +": OLD "+hex4(old)
                                              +" NEW "+hex4(mem));
                        }
                    }
                }
                
            }
//            System.out.println(Bytes.getHexDump(membuff));
            
        } catch(Exception e) {
            System.out.println(e);
            e.printStackTrace();
        }
    }
    
    public static void readMem(long addr, byte[] dest, int off) throws IOException {
        String cmd = "\rmm\r" + addr + "\r";
        o.write(cmd.getBytes());
        o.flush();
        
        while(true) {
            String h = br.readLine().trim();
            if (!h.equals("")) {
                StringTokenizer st = new StringTokenizer(h);
                try {
                    int addre = Integer.parseInt(st.nextToken(), 16);
                    String c = st.nextToken();
                    for (int j=0;j<16;j++) {
                        int b = Integer.parseInt(st.nextToken(), 16);
                        dest[off++] = (byte) b;
                    }
                    if (addre==0x1f0) break;
                } catch(Exception e) {
                    // Don't care really...    
                }
            }
        }
        
    }
    
    public static String hex8(long v) {
        String sv = "00000000" + Long.toString((v & 0xffffffffL), 16);
        return sv.substring(sv.length() - 8, sv.length());
    }


    public static String hex4(long v) {
        String sv = "0000" + Long.toString((v & 0xffff), 16);
        return sv.substring(sv.length() - 4, sv.length());
    }

    
    public static String hex2(long v) {
        String sv = "00" + Long.toString((v & 0xff), 16);
        return sv.substring(sv.length() - 2, sv.length());
    }

    /**
     *  Parses the arguments
     *
     *@param  args  Command line arguments
     *@return       Description of the Return Value
     */
    public static String[] parseArgs(String[] args) {
        String[] newArgs = new String[128];
        int p = 0;

        try {
            for (int i = 0; i < args.length; i++) {
                String key = args[i];
                if (key.equals("-h")) {
                    showHelp();
                } else if(key.equals("-addr")) {
                    i++;
                    startAddr = Integer.parseInt(args[i], 16);
                } else if(key.equals("-n")) {
                    i++;
                    blocks = Integer.parseInt(args[i]);
                } else {
                    newArgs[p++] = key;
                }
            }
        } catch (Exception e) {
            System.out.println("Error parsing command line arguments!");
            showHelp();
        }

        String[] newp = new String[p];
        for (int i = 0; i < p; i++) {
            newp[i] = newArgs[i];
        }
        return newp;
    }


    /**
     *  Show a simple help screen
     */
    public static void showHelp() {
        System.out.println("memmon v1.00. By DoggerMoore\n"
             + " -h                 Display this screen\n\n"
             + " -addr <hex>        Specify start address in hex\n\n"
             + " -n <blocks>        Specify number of 512byte blocks in decimal\n\n"
             );
        System.exit(-1);
    }
    
}

