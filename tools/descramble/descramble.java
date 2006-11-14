/*
*   Descrambler for Jbmm, Av1xx, Av3xx
*
*   MediOS project
*   Copyright (c) 2006 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.filechooser.*;

public class descramble extends JPanel implements ActionListener{

    JButton exitButton, goButton;
    JTextArea log;
    JFileChooser fc;
    File file = null;

descramble()
{
    exitButton = new JButton("Exit");
    exitButton.addActionListener(this);
    goButton = new JButton("Open & Go");
    goButton.addActionListener(this);
    
    log = new JTextArea(10,40);
    log.setMargin(new Insets(5,5,5,5));
    log.setEditable(false);
    JScrollPane logScrollPane = new JScrollPane(log);

    //Create a file chooser
    fc = new JFileChooser();

    JPanel buttonPanel = new JPanel(new GridLayout(2,1)); //use FlowLayout
    
    buttonPanel.add(goButton);
    buttonPanel.add(exitButton);
    
    //setLayout(new GridLayout(2,1));
    
    //Add the buttons and the log to this panel.
    
    add(logScrollPane);
    add(buttonPanel);
    

}

public void actionPerformed(ActionEvent e) {

        //Handle open button action.
        if (e.getSource() == goButton) {
            int returnVal = fc.showOpenDialog(descramble.this);

            if (returnVal == JFileChooser.APPROVE_OPTION) {
                File file = fc.getSelectedFile();
                //This is where a real application would open the file.
                log.append("Opening: " + file.getName() + "." + "\n"); 
                String fname=file.toString();
                String basename=String.valueOf(fname.toCharArray(),0,fname.lastIndexOf('.'));
                processFile(basename,file.toString());
                
                
            }
        }
        if (e.getSource() == exitButton) {
            System.exit(0);
        }
        
    }

public void processFile(String bName,String iName)
{
    log.append("Starting with file " + bName+ "\n");
    byte[] inBuff = loadFile(iName);
    
    int startCode = 16;
    int keylen = 32;
    
    byte[] xorstring=new byte[keylen];
    
    byte[] head4 = getByteArray(inBuff, 0, 4);
    int usize = getInt(inBuff, 4, 4);
    int csize = getInt(inBuff, 8, 4);
    
    log.append("-------------------"+ "\n");
    log.append("Header:            " + new String(head4)+ "\n");
    log.append("Uncompressed Size: " + usize+ "\n");
    log.append("Compressed Size:   " + csize+ "\n");
    log.append("Buffer Size:       " + inBuff.length+ "\n");
    log.append("-------------------"+ "\n");
    log.setCaretPosition(log.getDocument().getLength());
    
    byte[] outBuff = new byte[usize];
    
    /* finding xor key */
    for (int i=0; i<keylen; i++)
    {
        int top=0, c;
        int[] bytecount=new int[256];
        byte topchar=0;
        
        for(c=0;c<256;c++) bytecount[c]=0;
        
        /* gather byte frequency statistics */
        for (c=i; c<csize; c+=keylen)
        {
            bytecount[(int)inBuff[c+startCode]& 0xff]++;
        }

        /* find the most frequent byte */
        for (c=0; c<256; c++) {
            if (bytecount[c] > top) {
                top = bytecount[c];
                topchar = (byte)c;
            }
        }
        xorstring[i] = topchar;
    }
    log.append("XOR string:\n"+new String(xorstring)+ "\n");
    log.setCaretPosition(log.getDocument().getLength());
    /* XoR */    
    for (int i=0; i<csize; i++)
        inBuff[i] = (byte)(inBuff[i+startCode] ^ xorstring[i & (keylen-1)]);
    
    /* depack */ 
    int j=0;
    for (int i=0; i<csize;)
    {
        int head = inBuff[i++];

        for (int bit=0; bit<8 && i<csize; bit++)
        {
            if ((head & (1 << (bit)))!=0)
            {
                outBuff[j++] = inBuff[i++];
            }
            else
            {
                int x;
                int byte1 = (int)inBuff[i]&0xFF;
                int byte2 = (int)inBuff[i+1]&0xFF;
                int count = (byte2 & 0x0f) + 3;
                int src =
                    (j & 0xfffff000) + (byte1 | ((byte2 & 0xf0)<<4)) + 18;
                if (src > j)
                    src -= 0x1000;

                for (x=0; x<count; x++)
                    outBuff[j++] = outBuff[src+x];
                i += 2;
            }
        }
    }
    log.append("Saving file " + bName+ ".bin\n");       
    log.setCaretPosition(log.getDocument().getLength());
    saveFile(bName+".bin",outBuff);
    
}

public long get32(byte[] source, int startIndex) {
        long rr = (((int) source[startIndex]) & 0xffL);
	rr+= ((((int) source[startIndex + 1]) & 0xffL) << 8);
        rr+= ((((int) source[startIndex + 2]) & 0xffL) << 16);
        rr+= ((((int) source[startIndex + 3]) & 0xffL) << 24);
        return rr;
    }
    
public static int getInt(byte[] source, int startIndex, int byteCount) {
        int r = 0;
        for (int i = 0; i < byteCount; i++) {
            r = r | ((((int) source[startIndex + i]) & 0xff) << (i * 8));
        }
        return r;
    }

public static byte[] getByteArray(byte[] dest, int startIndex, int byteCount) {
        byte[] newb = new byte[byteCount];
        for (int i = 0; i < byteCount; i++) {
            newb[i] = dest[startIndex + i];
        }
        return newb;
    }   
 
public byte[] loadFile(String filename) {
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
            System.out.println("\nError loading file! " + e);
        }
        return rr;
    }

public void saveFile(String filename, byte[] data) {
        try {
            FileOutputStream fo = new FileOutputStream(filename);
            fo.write(data);
            fo.close();
        } catch (Exception e) {
            System.out.println("\nError saving file! " + e);
        }
    }



/**
     * Create the GUI and show it.  For thread safety,
     * this method should be invoked from the
     * event-dispatching thread.
     */
    private static void createAndShowGUI() {
        //Create and set up the window.
        JFrame frame = new JFrame("Descramble");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        //Create and set up the content pane.
        JComponent newContentPane = new descramble();
        newContentPane.setOpaque(true); //content panes must be opaque
        frame.setContentPane(newContentPane);

        //Display the window.
        frame.pack();
        frame.setVisible(true);
    }

    public static void main(String[] args) {
        //Schedule a job for the event-dispatching thread:
        //creating and showing this application's GUI.
        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                createAndShowGUI();
            }
        });
    }
}