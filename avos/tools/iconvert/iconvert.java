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

public class iconvert {
    static int height = 240;
    static int width = 320;
    static String name = "image";
    static boolean pack = false;

    public static void main(String[] args) {
        args = parseArgs(args);
        
        if (args.length>0) {
            try {
                FileInputStream f = new FileInputStream(args[0]);
                if (!pack) {
                    System.out.println("static unsigned int "+name+"["+height+"]["+width+"] = {");
                } else {
                    System.out.println("static unsigned char "+name+"["+height+"]["+(width*3)+"] = {");                    
                }
                    
                for (int y=0;y<height;y++) {
                    System.out.print(" {");
                    for (int x=0;x<width;x++) {
                        int r = f.read();
                        int g = f.read();
                        int b = f.read();
    
                        long c = convert(r, g, b);
                        if (x!=0) System.out.print(",");
                        if (!pack) {
                            System.out.print("0x" + Long.toString(c, 16));
                        } else {
                            System.out.print("" + (c&0xff) + "," + ((c>>8)&0xff) + "," + ((c>>16)&0xff));
                        }
                    }
                    System.out.print("}");
                    if (y<239) System.out.println(","); else System.out.println("");
                }
                
                System.out.println("}");
                f.close();
            } catch(Exception e) {
                System.out.println("Error!");    
            }
        }
    }

    public static long convert(int r, int g, int b) {
        int y = (int) (0.299*r + 0.587*g + 0.114*b);
        int cb = (int)(-0.1687*r - 0.3313*g + 0.5*b + 128);
        int cr = (int)(0.5*r - 0.4187*g - 0.0813*b + 128);
        if (y<0) y=0;
        if (cb<0) cb=0;
        if (cr<0) cr=0;
        if (y>255) y=255;
        if (cb>255) cb=255;
        if (cr>255) cr=255;
        return (cr<<16) | (y<<8) | cb;
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
                } else if(key.equals("--pack")) {
                    pack=true;
                } else if(key.equals("-c3")) {
                    i++;
                    long val = Long.parseLong(args[i], 16);
                    System.out.println("RGB 0x" + Long.toString(val, 16)
                                      +" = 0x" + Long.toString(convert((int)(val>>16) & 0xff, (int)(val>>8)&0xff, (int)(val&0xff)), 16));
                } else if(key.equals("-c")) {
                    i++;
                    int r = Integer.parseInt(args[i++]);
                    int g = Integer.parseInt(args[i++]);
                    int b = Integer.parseInt(args[i++]);
                    System.out.println("RGB (" + r + "," + g + "," + b + ")"
                                      +" = 0x" + Long.toString(convert(r,g,b), 16));
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
        System.out.println("iconvert v1.00. By DoggerMoore\n"
             + " -h                 Display this screen\n\n"
             + " -c <r> <g> <b>     Convert an rgb tripple\n\n"
             + " -c <rgb>           Convert an rgb hex value\n\n"
             + " -name <name>       Name for the image\n\n"
             + " --pack             Extra pack (3bytes per pix)\n\n"
             + " [source.rgb]       Optional image to convert\n\n"
             );
        System.exit(-1);
    }
    
}

