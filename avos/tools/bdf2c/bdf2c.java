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
 *@created    12/02/2004
 */

public class bdf2c {
    static String name = "font";    
    static boolean all = false;
    
    public static void main(String[] args) {
        boolean inchar = false;
        boolean inbitmap = false;
        int encoding = 0;
        String data = "";
        int datalen = 0;

        String chardata = "";

        String[] defs = new String[65536];

        args = parseArgs(args);

        try {
            BufferedReader fi = new BufferedReader(new FileReader(args[0]));
            while (true) {
                String s = fi.readLine();
                if (s == null) {
                    break;
                }

                if (s.startsWith("STARTCHAR")) {
                    inchar = true;
                    inbitmap = false;
                    data = "";
                    datalen = 0;
                } else if (s.startsWith("ENDCHAR")) {
                    inchar = false;
                    if (all || (encoding<256)) {
                        chardata += "    char " + name + encoding + "["+datalen+"] = {" + data + "};\n";
                        defs[encoding] = name + encoding;
                    }
                }

                if (inchar) {
                    if (inbitmap) {
                        for (int x=0;x<s.length();x=x+2) {
                            if (!data.equals("")) data += ", ";
                            data += "0x" + s.substring(x, x+2);
                            datalen++;
                        }
                    }

                    if (s.startsWith("ENCODING")) {
                        encoding = Integer.parseInt(s.substring(9, s.length()).trim());    
                    } else if (s.startsWith("BITMAP")) {
                        inbitmap = true;    
                    }

                }
            }
            fi.close();

        } catch (Exception e) {
            System.out.println("Error " + e);
            e.printStackTrace();
        }

        int pp = 256;
        if (all) pp = 65536;
        
        System.out.println(chardata);
        System.out.println("char (*" + name + "[]) [] = {");
            
        for (int i=0;i<pp;i++) {
            String def = defs[i];
            if (def==null) def = "0";
            else def = "&" + def;
            if (i>0) {
                System.out.print(", ");
                if ((i%8)==0) System.out.println();
            }
            System.out.print(def); 
        }
        System.out.println("};");
        
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

        if (args.length == 0) {
            showHelp();
        }
        try {
            for (int i = 0; i < args.length; i++) {
                String key = args[i];
                if (key.equals("-h")) {
                    showHelp();
                } else if(key.equals("-name")) {
                    i++;
                    name = args[i];
                } else if(key.equals("--all")) {
                    all = true;
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
        System.out.println("bdf2c v1.00. By DoggerMoore\n"
             + " -h                 Display this screen\n\n"
             + " -all               Convert chars > 255\n\n"
             + " -name <name>       Name for the font\n\n"
             );
        System.exit(-1);
    }

}

