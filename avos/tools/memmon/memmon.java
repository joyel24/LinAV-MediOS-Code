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
    
    public static void main(String[] args) {
        System.out.println("memmon v0.01 (c) Dogger");
        try {
            proc = Runtime.getRuntime().exec("./ser");
            i = proc.getInputStream();
            br = new BufferedReader(new InputStreamReader(i));
            o = proc.getOutputStream();

            int startAddr = 0x30000;
            
            while(true) {
                oldbuff = membuff;
                membuff = new byte[512];
                readMem(startAddr, membuff, 0);
                
                // now compare with last one...
                if (oldbuff!=null) {
                    for (int j=0;j<512;j++) {
                        if (oldbuff[j]!=membuff[j]) {
                            System.out.println(hex8(startAddr + j)
                                              +": OLD "+hex2(oldbuff[j])
                                              +" NEW "+hex2(membuff[j]));
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
    
    public static void readMem(int addr, byte[] dest, int off) throws IOException {
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

}

