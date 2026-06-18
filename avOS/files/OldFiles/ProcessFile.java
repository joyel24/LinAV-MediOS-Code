import java.io.*;

/**
 *  This processes an AJZ file for the Archos AV product firmware updates.
 *
 *@author     jimmy
 *@created    October 16, 2003
 */
public class ProcessFile {
    public static long[] byteCounts = new long[256];


    /**
     *  The main program for the ProcessFile class
     *
     *@param  args  The command line arguments
     */
    public static void main(String[] args) {
        try {
            byte[] image = loadFile(args[0]);

            int p = 0;
            while (true) {
                if (p >= image.length) {
                    break;
                }

                                                            // Read header...
                byte[] head4 = getByteArray(image, p, 4);
                int usize = getInt(image, p + 4, 4);
                int csize = getInt(image, p + 8, 4);
                byte[] check = getByteArray(image, p + 12, 4);
                long checks = get32(image, p + 12);

                System.out.println("-------------------");
                System.out.println("Header:            " + new String(head4));
                System.out.println("Uncompressed Size: " + usize);
                System.out.println("Compressed Size:   " + csize);
                System.out.println("Checksum:          " + Long.toString(checks, 16));
                System.out.println("-------------------");

                long s = 0;
                for (int i = 0; i < csize; i++) {
                    int v = ((int) image[p + 16 + i]) & 0xff;
                    s = (s + v) & 0xffffffffL;

                                                            //v = ((v << 1) & 0xfe) | ((v >> 7) & 1);

                    byteCounts[v]++;
                }
                System.out.println("Checksum of data:  " + Long.toString(s, 16));

                                                            // Print out histogram of byte values : normalised counts.
//                System.out.println("--- Singles ---");

//                for (int u = 0; u < 256; u++) {
//                    long norm = (long) (byteCounts[u] * 1000000 / csize);
//                    if (norm > 0) {
//                        System.out.println(norm);
//                    }
//                    byteCounts[u] = 0;
//                }

                p = p + 16 + csize;
            }

        } catch (Exception e) {
            System.out.println("Error " + e);
        }
    }


    /**
     *  Gets a 32 bit unsigned from a byte array
     *
     *@param  source      Source array
     *@param  startIndex  Start index
     *@return             A 32 bit int
     */
    public static long get32(byte[] source, int startIndex) {
        long rr = (((int) source[startIndex]) & 0xffL)
             + ((((int) source[startIndex + 1]) & 0xffL) << 8)
             + ((((int) source[startIndex + 2]) & 0xffL) << 16)
             + ((((int) source[startIndex + 3]) & 0xffL) << 24);
        return rr;
    }


    /**
     *@param  source      Source array
     *@param  startIndex  Start index
     *@param  byteCount   How many bytes to convert
     *@return             The int value
     */
    public static int getInt(byte[] source, int startIndex, int byteCount) {
        int r = 0;
        for (int i = 0; i < byteCount; i++) {
            r = r | ((((int) source[startIndex + i]) & 0xff) << (i * 8));
        }
        return r;
    }


    /**
     *  Gets the byteArray attribute of the ProcessFile class
     *
     *@param  dest        Where to get from
     *@param  startIndex  Start index
     *@param  byteCount   Number of bytes to get
     *@return             The byteArray value
     */
    public static byte[] getByteArray(byte[] dest, int startIndex, int byteCount) {
        byte[] newb = new byte[byteCount];
        for (int i = 0; i < byteCount; i++) {
            newb[i] = dest[startIndex + i];
        }
        return newb;
    }


    /**
     *  Description of the Method
     *
     *@param  filename  Filename to load
     *@return           A byte array of the file contents.
     */
    public static byte[] loadFile(String filename) {
        System.out.println("Loading file [" + filename + "]");
        byte[] rr = null;
        try {
            File fl = new File(filename);
            FileInputStream i = new FileInputStream(fl);
            rr = new byte[(int) fl.length()];

            int p = 0;
            while (p != rr.length) {
                int l = i.read(rr, p, rr.length - p);
                if (l == -1) {
                    throw (new IOException("End Of Stream"));
                }
                p += l;
            }

            i.close();
        } catch (Exception e) {
            System.out.println("Error loading file! " + e);
        }
        return rr;
    }

}

