package mainPackage;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.SystemColor;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.PrintWriter;
import java.util.Scanner;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;

public class EndWindow extends JFrame implements Runnable{
	Image BackgroundImg = new ImageIcon(MainWindow.class.getResource("../image/Background.png")).getImage();
	private int showScore = 0;
	Image buffImage;
	Graphics buffg;
	int i = -1;
	
	Thread end;
	
	public EndWindow(int score) {
		showScore = score;
		
		setSize(600, 400);
		setTitle("G A M E O V E R");
		setIconImage(BackgroundImg);
		setResizable(false);
        setVisible(true);
        setLocationRelativeTo(null);
        getContentPane().setLayout(null);
 	   
        JButton endBT = new JButton("End");
        endBT.setBackground(Color.BLACK);
        endBT.setForeground(SystemColor.WHITE);
        endBT.setFont(new Font("DialogInput", Font.PLAIN, 12));
        endBT.setSize(100, 25);
        endBT.setLocation(100, 300);
        getContentPane().add(endBT);
        
        endBT.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				System.exit(0);
			}
		});
        
        JButton replyBT = new JButton("Replay");
        replyBT.setBackground(Color.BLACK);
        replyBT.setForeground(SystemColor.WHITE);
        replyBT.setFont(new Font("DialogInput", Font.PLAIN, 12));
        replyBT.setSize(100, 25);
        replyBT.setLocation(400, 300);
        getContentPane().add(replyBT);
        
        replyBT.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				TitleWindow MW = new TitleWindow();
				dispose();
			}
		});
        
	    setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	    
	    start();
	}
	
	public void start() {
		end = new Thread(this);
		end.start();
	}
	
	public void run() {
		while(true) {
			repaint();
			i++;
			if(showScore == i) {
				break;
			}
			try {
				Thread.sleep(3);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		saveHighScore();
	}
	
	
	public void paint(Graphics g) {
		buffImage = createImage(600,400);
		buffg = buffImage.getGraphics();
		buffg.drawImage(BackgroundImg, 0, 0, this);
		buffg.setFont(new Font("HanWangZonYi", Font.PLAIN, 50));
		buffg.setColor(Color.WHITE);
		buffg.drawString("G A M E O V E R", 60, 100);
		buffg.setFont(new Font("HanWangZonYi", Font.PLAIN, 30));
		buffg.drawString("SCORE", 60, 160);
		update(g);
	}
	
	public void update(Graphics g) {
		buffg.setColor(Color.CYAN);
		buffg.setFont(new Font("HanWangZonYi", Font.PLAIN, 100));
		buffg.drawString(Integer.toString(i), 60, 260);
		g.drawImage(buffImage, 0, 0, this);
	}
	
	private void saveHighScore() {
		try {
			FileInputStream fileObjectREAD = new FileInputStream("C:\\Users\\CafeAlle\\eclipse-workspace\\JSHOOTER2\\src\\save\\HighScore.txt"); // this might differ by PC Directory!
			Scanner R = new Scanner(fileObjectREAD);
			String S = new String();
			while(R.hasNext()) {
				S = S + R.nextLine();
			}
			int highScore = Integer.parseInt(S);
			if(showScore > highScore) {
				FileOutputStream fileObjectWRITE = new FileOutputStream("C:\\Users\\CafeAlle\\eclipse-workspace\\JSHOOTER2\\src\\save\\HighScore.txt", false); // 이것도요!
				PrintWriter W = new PrintWriter(fileObjectWRITE);
				W.println(showScore);
				W.close();
			}
		}catch (FileNotFoundException e) {	
			e.printStackTrace();		
		}
	}
	
	
}
