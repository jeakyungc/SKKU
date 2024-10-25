package mainPackage;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.Scanner;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import java.awt.CardLayout;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Font;
import java.awt.SystemColor;
import javax.swing.JLabel;
import javax.swing.SwingConstants;

public class TitleWindow extends JFrame {
	Image TitleImage = new ImageIcon(TitleWindow.class.getResource("../image/Title.png")).getImage();
	Image BackgroundImg = new ImageIcon(TitleWindow.class.getResource("../image/Background.png")).getImage();
	
	private int highScore = 0;
	
	public TitleWindow() {
		setSize(600, 400);
		setTitle("I N V A D E R S");
		setResizable(false);
        setVisible(true);
        setLocationRelativeTo(null);
        getContentPane().setLayout(null);
 	   
        JButton startBT = new JButton("S T A R T");
        setIconImage(BackgroundImg);
        startBT.setBackground(Color.BLACK);
        startBT.setForeground(Color.WHITE);
        startBT.setFont(new Font("HanWangZonYi", Font.PLAIN, 20));
        startBT.setSize(200, 55);
        startBT.setLocation(200, 270);
        getContentPane().add(startBT);
        
        startBT.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				MainWindow MW = new MainWindow();
				//EndWindow E = new EndWindow(1000);
				dispose();
			}
		});
        
        loadHighScore();
	    setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);    
	}
	
	private void loadHighScore() {
		try {
			FileInputStream fileObjectREAD = new FileInputStream("C:\\Users\\CafeAlle\\eclipse-workspace\\JSHOOTER2\\src\\save\\HighScore.txt"); // this might differ by PC Directory!
			Scanner R = new Scanner(fileObjectREAD);
			String S = new String();
			while(R.hasNext()) {
				S = S + R.nextLine();
			}
			highScore = Integer.parseInt(S);
		}catch(FileNotFoundException e) {}
	}
	
	public void paint(Graphics g) {
		//g.drawRect(0, 0, 500, 500);
		g.drawImage(BackgroundImg, 0, 0, this);
		g.drawImage(TitleImage, 50, 120, 500, 67, this);
		g.setColor(Color.DARK_GRAY);
		g.setFont(new Font("HanWangZonYi", Font.PLAIN, 30)); 
		g.drawString("HIGHSCORE     " + Integer.toString(highScore), 70, 230);
	}
	
	
	public static void main(String ar[]) {
		TitleWindow TW = new TitleWindow();
		//MainWindow MW = new MainWindow();
	}

}
