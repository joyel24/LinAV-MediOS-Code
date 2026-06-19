import java.io.*;

/**
 *  Description of the Class
 *
 *@author     jimmy
 *@created    October 27, 2003
 */
public class Bytes {

    /**
     *  Description of the Method
     *
     *@param  source      Description of the Parameter
     *@param  startIndex  Description of the Parameter
     *@return             Description of the Return Value
     */
    public static long get32(byte[] source, int startIndex) {
        long rr = (((int) source[startIndex]) & 0xffL)
             + ((((int) source[startIndex + 1]) & 0xffL) << 8)
             + ((((int) source[startIndex + 2]) & 0xffL) << 16)
             + ((((int) source[startIndex + 3]) & 0xffL) << 24);
        return rr;
    }


    /**
     *  Description of the Method
     *
     *@param  source      Description of the Parameter
     *@param  startIndex  Description of the Parameter
     *@return             Description of the Return Value
     */
    public static long get32N(byte[] source, int startIndex) {
        long rr = (((int) source[startIndex + 3]) & 0xffL)
             + ((((int) source[startIndex + 2]) & 0xffL) << 8)
             + ((((int) source[startIndex + 1]) & 0xffL) << 16)
             + ((((int) source[startIndex]) & 0xffL) << 24);
        return rr;
    }


    /**
     *  Description of the Method
     *
     *@param  source      Description of the Parameter
     *@param  startIndex  Description of the Parameter
     *@param  v           Description of the Parameter
     */
    public static void put32(byte[] source, int startIndex, long v) {
        source[startIndex] = (byte) (v & 0xff);
        source[startIndex + 1] = (byte) ((v >> 8) & 0xff);
        source[startIndex + 2] = (byte) ((v >> 16) & 0xff);
        source[startIndex + 3] = (byte) ((v >> 24) & 0xff);
    }


    /**
     *  Description of the Method
     *
     *@param  source      Description of the Parameter
     *@param  startIndex  Description of the Parameter
     *@param  v           Description of the Parameter
     */
    public static void put32N(byte[] source, int startIndex, long v) {
        source[startIndex + 3] = (byte) (v & 0xff);
        source[startIndex + 2] = (byte) ((v >> 8) & 0xff);
        source[startIndex + 1] = (byte) ((v >> 16) & 0xff);
        source[startIndex] = (byte) ((v >> 24) & 0xff);
    }


    /**
     *  Retrieves an int from a byte array (Little endian).
     *
     *@param  source      The byte array holding data.
     *@param  startIndex  The starting index in source.
     *@param  byteCount   The number of bytes to convert.
     *@return             Returns the int value requested.
     */
    public static int getInt(byte[] source, int startIndex, int byteCount) {
        int r = 0;
        for (int i = 0; i < byteCount; i++) {
            r = r | ((((int) source[startIndex + i]) & 0xff) << (i * 8));
        }
        return r;
    }


    /**
     *  Retrieves an int from a byte array (Big endian).
     *
     *@param  source      The byte array holding data.
     *@param  startIndex  The starting index in source.
     *@param  byteCount   The number of bytes to convert.
     *@return             Returns the int value requested.
     */
    public static int getIntN(byte[] source, int startIndex, int byteCount) {
        int r = 0;
        for (int i = 0; i < byteCount; i++) {
            r = (r << 8) | (((int) source[startIndex + i]) & 0xff);
        }
        return r;
    }


    /**
     *  Stores an int in a byte array (Little endian).
     *
     *@param  dest        The byte array holding data.
     *@param  startIndex  The starting index in source.
     *@param  byteCount   The number of bytes to store.
     *@param  value       The int value to store.
     */
    public static void putInt(byte[] dest, int startIndex, int byteCount, int value) {
        for (int i = 0; i < byteCount; i++) {
            dest[startIndex + i] = (byte) ((value >> (i * 8)) & 0xff);
        }
    }


    /**
     *  Stores an int in a byte array (Big endian).
     *
     *@param  dest        The byte array holding data.
     *@param  startIndex  The starting index in source.
     *@param  byteCount   The number of bytes to store.
     *@param  value       The int value to store.
     */
    public static void putIntN(byte[] dest, int startIndex, int byteCount, int value) {
        for (int i = 0; i < byteCount; i++) {
            dest[startIndex + byteCount - 1 - i] = (byte) ((value >> (i * 8)) & 0xff);
        }
    }


    /**
     *  Stores one byte array inside another.
     *
     *@param  dest        The byte array holding data.
     *@param  startIndex  The starting index in dest.
     *@param  byteCount   The number of bytes to store.
     *@param  src         The byte array to copy.
     */
    public static void putByteArray(byte[] dest, int startIndex, int byteCount, byte[] src) {
        putByteArray(dest, startIndex, byteCount, src, 0);
    }


    /**
     *  Stores one byte array inside another.
     *
     *@param  dest        The byte array holding data.
     *@param  startIndex  The starting index in dest.
     *@param  byteCount   The number of bytes to store.
     *@param  src         The byte array to copy.
     *@param  startSrc    The starting index in src to copy.
     */
    public static void putByteArray(byte[] dest, int startIndex, int byteCount, byte[] src, int startSrc) {
        for (int i = 0; i < byteCount; i++) {
            dest[startIndex + i] = src[startSrc + i];
        }
    }


    /**
     *  Extracts one byte array from inside another.
     *
     *@param  dest        The byte array holding data.
     *@param  startIndex  The starting index in dest.
     *@param  byteCount   The number of bytes to store.
     *@return             The byte array requested.
     */
    public static byte[] getByteArray(byte[] dest, int startIndex, int byteCount) {
        byte[] newb = new byte[byteCount];
        for (int i = 0; i < byteCount; i++) {
            newb[i] = dest[startIndex + i];
        }
        return newb;
    }


    /**
     *  Converts a byte array to a string showing hex values.
     *
     *@param  source  The byte array to convert to a hex string.
     *@return         The String representation.
     */
    public static String getHexString(String source) {
        return getHexString(source.getBytes());
    }


    /**
     *  Gets the hexString attribute of the ByteStore class
     *
     *@param  source  Description of the Parameter
     *@return         The hexString value
     */
    public static String getHexString(byte[] source) {
        return getHexString(source, 0, source.length, true);
    }


    /**
     *  Gets the hexString attribute of the ByteStore class
     *
     *@param  source  Description of the Parameter
     *@param  spaces  Description of the Parameter
     *@return         The hexString value
     */
    public static String getHexString(byte[] source, boolean spaces) {
        return getHexString(source, 0, source.length, spaces);
    }


    /**
     *  Converts a portion of a byte array to a hex string.
     *
     *@param  source      The byte array to convert to a hex string.
     *@param  startIndex  Starting offset in source.
     *@param  count       Number of bytes to convert.
     *@return             The String representation.
     */
    public static String getHexString(byte[] source, int startIndex, int count) {
        return getHexString(source, startIndex, count, true);
    }


    /**
     *  Gets the hexString attribute of the ByteStore class
     *
     *@param  source      Description of the Parameter
     *@param  startIndex  Description of the Parameter
     *@param  count       Description of the Parameter
     *@param  spaces      Description of the Parameter
     *@return             The hexString value
     */
    public static String getHexString(byte[] source, int startIndex, int count, boolean spaces) {
        StringBuffer sb = new StringBuffer(count * 3);
        String hex = "0123456789abcdef";

        for (int i = 0; i < count; i++) {
            int c1 = (int) source[startIndex + i] & 0xf;
            int c2 = (int) (source[startIndex + i] >> 4) & 0xf;
            sb.append(hex.charAt(c2));
            sb.append(hex.charAt(c1));
            if (spaces && i != (count - 1)) {
                sb.append(' ');
            }
        }
        return sb.toString();
    }


    /**
     *  Converts a portion of a byte array to a hex dump
     *
     *@param  source  The byte array to convert to a hex string.
     *@return         The String representation.
     */
    public static String getHexDump(byte[] source) {
        return getHexDump(source, 0, source.length);
    }


    /**
     *  Converts a portion of a byte array to a hex dump
     *
     *@param  source      The byte array to convert to a hex string.
     *@param  startIndex  Starting offset in source.
     *@param  count       Number of bytes to convert.
     *@return             The String representation.
     */
    public static String getHexDump(byte[] source, int startIndex, int count) {
        String rr = "";
        int l = 0;

        while (true) {
            int n = 16;
            String pad = "";
            if ((l + 16) > count) {
                n = (count - l);
                for (int j = n; j < 16; j++) {
                    pad = pad + "   ";
                }
            }
            if (n == 0) {
                break;
            }
            String pp = "0000" + Integer.toHexString(l);
            pp = pp.substring(pp.length() - 4, pp.length());
            rr = rr + pp + " | " + getHexString(source, startIndex + l, n) + pad + " | " + getReadableString(source, startIndex + l, n) + "\n";
            l += 16;
            if (l > count) {
                break;
            }
        }
        return rr;
    }


    /**
     *  Converts a portion of a byte array to a readable ascii string.
     *
     *@param  source      The byte array to convert to a hex string.
     *@param  startIndex  Starting offset in source.
     *@param  count       Number of bytes to convert.
     *@return             The String representation.
     */
    public static String getReadableString(byte[] source, int startIndex, int count) {
        StringBuffer sb = new StringBuffer(count);
        String allowed = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\"$%^&*()_+-={}[];'#:@~,./<>?\\|";

        for (int i = 0; i < count; i++) {
            char c = (char) (source[startIndex + i] & 0xff);
            if (allowed.indexOf(c) == -1) {
                c = ' ';
            }
            sb.append(c);
        }
        return sb.toString();
    }


    /**
     *  Gets the readableString attribute of the ByteStore class
     *
     *@param  source  Description of the Parameter
     *@return         The readableString value
     */
    public static String getReadableString(String source) {
        return getReadableString(source.getBytes(), 0, source.length());
    }


    /**
     *  Gets the readableString attribute of the ByteStore class
     *
     *@param  bb  Description of the Parameter
     *@return     The readableString value
     */
    public static String getReadableString(byte[] bb) {
        return getReadableString(bb, 0, bb.length);
    }


    /**
     *  Loads a file into a byte array...
     *
     *@param  filename  Description of the Parameter
     *@return           Description of the Return Value
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


    /**
     *  Description of the Method
     *
     *@param  filename  Description of the Parameter
     *@param  data      Description of the Parameter
     */
    public static void saveFile(String filename, byte[] data) {
        try {
            FileOutputStream fo = new FileOutputStream(filename);
            fo.write(data);
            fo.close();
        } catch (Exception e) {
            System.out.println("Error saveing file! " + e);
        }
    }

}

