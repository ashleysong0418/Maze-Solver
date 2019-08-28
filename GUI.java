import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.io.File;
import java.awt.image.BufferedImage;
import javax.imageio.*;
import java.io.*;
import java.util.Scanner;
import java.util.NoSuchElementException;
public class GUI
{
    private static class Stopwatch implements ActionListener
    {
      public void actionPerformed(ActionEvent e){
          maze.readFile();
          maze.repaint();
      }
    }
    private static class Display extends JPanel{

        private Block[][] blocks;
        private Point[] avatars;
        private String file;
        private int blockSize;

        public Display(int wide, int high, int blockSize, int num, String f){
            blocks = new Block[high][wide];
            this.blockSize = blockSize;
            makeMaze();
            avatars = new Point[num];
            for (int i = 0; i < num; i++)
                avatars[i] = new Point(0,0);
            file = f;
        }
        public void paintComponent(Graphics g){
            super.paintComponent(g);

            //draw background
            // g.drawImage(picture,0,0,null);

            g.setColor(Color.BLACK);
            for (int i = 0; i < blocks.length; i++) {
                for (int j = 0; j < blocks[i].length; j++) {
                    if (blocks[i][j].getWall("north"))
                        g.fillRect(blockSize*j,blockSize*i,
                                   blockSize,2);
                    if (blocks[i][j].getWall("south"))
                        g.fillRect(blockSize*j,blockSize*(i+1)-2,
                                   blockSize,2);
                    if (blocks[i][j].getWall("west"))
                        g.fillRect(blockSize*j,blockSize*i,
                                   2,blockSize);
                    if (blocks[i][j].getWall("east"))
                        g.fillRect(blockSize*(j+1)-2,blockSize*i,
                                   2, blockSize);
                }
            }

            //draw each avatar
            g.setColor(Color.RED);
            for (int i = 0; i < avatars.length; i++) {
                if (avatars[i] != null)
                    g.fillOval((int)(blockSize*(avatars[i].x+.25)),
                               (int)(blockSize*(avatars[i].y+.25)),
                               10,10);
            }
        }

        private void makeMaze(){
            for (int i = 0; i < blocks.length; i++) {
                for (int j = 0; j < blocks[i].length; j++) {
                    blocks[i][j] = new Block();
                    if (i == 0)
                        blocks[i][j].setWall("north",true);
                    else if (i == blocks.length-1)
                        blocks[i][j].setWall("south",true);
                    if (j == 0)
                        blocks[i][j].setWall("west",true);
                    else if (j == blocks[i].length-1)
                        blocks[i][j].setWall("east",true);
                }
            }
        }

        private void readFile(){
            String block = "chmod a-w " + file;
            String unblock =  "chmod a+w " + file;

            //TODO
            //read the file
            boolean flag = true;
            File fp = new File(file);
            try{
                while(flag){
                    if (!fp.canRead()){
                        Thread.sleep(100); //tenth of a second
                    }
                    else{
                        try{
                            Process p = Runtime.getRuntime().exec(block);
                        } catch(IOException e) {
                            System.err.println("uh oh couldn't run chmod a-w");
                            return;
                        }
                        flag = false;
                    }
                }
            } catch (InterruptedException e){
                System.err.println("Idk what's going on");
            }

            try{
                Scanner sc = new Scanner(fp);
                //update avatar locations
                for (int i = 0; i < avatars.length; i++){
                    int x = sc.nextInt();
                    int y = sc.nextInt();
                    avatars[i].x = x;
                    avatars[i].y = y;
                }
                //update maze
                for (int i = 0; i < blocks.length; i++) {
                    for (int j = 0; j < blocks[i].length; j++) {
                        int north = sc.nextInt();
                        int south = sc.nextInt();
                        int east = sc.nextInt();
                        int west = sc.nextInt();
                        if (north==1)
                            blocks[i][j].setWall("north",true);
                        else
                            blocks[i][j].setWall("north",false);
                        if (south==1)
                            blocks[i][j].setWall("south",true);
                        else
                            blocks[i][j].setWall("south",false);
                        if (east==1)
                            blocks[i][j].setWall("east",true);
                        else
                            blocks[i][j].setWall("east",false);
                        if (west==1)
                            blocks[i][j].setWall("west",true);
                        else
                            blocks[i][j].setWall("west",false);
                    }
                }
            } catch(FileNotFoundException e){
                // System.err.println("zoinks");
            } catch(NoSuchElementException e){

            }



            try {
                Process p2 = Runtime.getRuntime().exec(unblock);
            } catch (IOException e) {
                System.err.println("uh oh couldn't run chmod a+w");
                return;
            }
        }
    }

    private static class Block{

        private boolean north, south, east, west, visited;

        public Block(){
            north = false;
            south = false;
            east = false;
            west = false;
            visited = false;
        }

        public void setWall(String s, boolean b){
            switch(s){
                case "north": north = b; break;
                case "south": south = b; break;
                case "east": east = b; break;
                case "west": west = b; break;
                default: break;
            }
        }

        public boolean getWall(String s) {
            switch(s) {
                case "north": return north;
                case "south": return south;
                case "east": return east;
                case "west": return west;
                default: return false;
            }
        }
    }


    private static JFrame frame;
    private static Display maze;
    private static int blockSize = 21;
    private static Timer watch;

    public static void main(String[] args){
       int blocksWide = Integer.parseInt(args[0]);
       int blocksHigh = Integer.parseInt(args[1]);
       int numAvatars = Integer.parseInt(args[2]);
       String fileName = args[3];
       frame = new JFrame("Project Anon");
       maze = new Display(blocksWide,blocksHigh,blockSize,numAvatars,fileName);
       frame.add(maze);
       frame.setSize(blockSize*(blocksWide+1),blockSize*(blocksHigh+2));
       frame.setLocation(100,100);
       frame.setDefaultCloseOperation(3);
       try{
           emptyFile(blocksWide,blocksHigh,numAvatars,fileName);
       } catch(IOException e) {}
       Stopwatch a = new Stopwatch();
       watch = new Timer(50,a); // tenth of a second

       maze.setLayout(null);
       frame.setVisible(true);
       watch.start();
    }

    private static void emptyFile(int blocksWide, int blocksHigh,
                                  int numAvatars,String fileName)
                                  throws IOException
    {
        FileWriter fileWriter = new FileWriter(fileName);
        PrintWriter printWriter = new PrintWriter(fileWriter);
        for (int i = 0; i < numAvatars; i++)
            printWriter.println("0 0");
        for (int i = 0; i < blocksHigh; i++) {
            for (int j = 0; j < blocksWide; j++)
                printWriter.print("0 0 0 0 ");
            printWriter.print("\n");
        }
        printWriter.close();
        fileWriter.close();
    }
}
