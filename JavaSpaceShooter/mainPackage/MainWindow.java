package mainPackage;

import java.awt.BasicStroke;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;


import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.Point;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;

import java.util.*;
import mainPackage.Bullet;
import mainPackage.Invader;
import mainPackage.GameState;

public class MainWindow extends JFrame implements Runnable, KeyListener {
	
	//Image BackgroundImage = new ImageIcon();
	Image BackgroundImg = new ImageIcon(MainWindow.class.getResource("../image/Background.png")).getImage();
	Image ShipImg = new ImageIcon(MainWindow.class.getResource("../image/Ship.png")).getImage();
	Image BulletImg = new ImageIcon(MainWindow.class.getResource("../image/Bullet.png")).getImage();
	Image InvaderBigImg = new ImageIcon(MainWindow.class.getResource("../image/InvaderBig.png")).getImage();
	Image ShootedImg = new ImageIcon(MainWindow.class.getResource("../image/InvaderAnimation.png")).getImage();
	Image buffImage; Graphics buffOS;
	Font Statefont = new Font("HanWangZonYi", Font.PLAIN, 20);
	
	boolean keyL = false;
	boolean keyR = false;
	boolean keyS = false;
	public boolean loopState = true;
	
	boolean keyF = false;
	boolean collidCheck = false;
	boolean attackCheck = false;
	double rad = 0.0;
	double deg = (Math.PI / 180)* 4;
	long frameDelaySpeed = 30;
	public int time = 0; 
	int colorCnt = 1;
	
	Bullet B;
	Invader I;
	GameState G = new GameState();
	ArrayList Bullets = new ArrayList();
	ArrayList Invaders = new ArrayList();
	Thread first;
	
	MainWindow() {
		setSize(900, 900);
		setTitle("I N V A D E R S");
		setIconImage(BackgroundImg);
		setResizable(false);
		setLocationRelativeTo(null);
		setLayout(null);
		setVisible(true);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		start();
	}
	
	public void start() {
		addKeyListener(this);
		first = new Thread(this);
		first.start();
	}

	@Override
	public void run() {
		while(loopState){
			Rotate();
			BulletShoot();
			InvaderMov();
			repaint();
			G.GameOver(this);
			try {
				time++;
				Thread.sleep(frameDelaySpeed);
			}catch(Exception e){}
		}
		first.interrupt();
	}
	
	public void stop() {
		loopState = false;
	}
	
	public void paint(Graphics g) {
		buffImage = createImage(900, 900);
		buffOS = buffImage.getGraphics();
		update(g);		
	}
	
	public void update(Graphics g) {
		backgroundDraw();
		raderDraw();
		explodeDraw();
		shipDraw();
		bulletDraw();
		invaderDraw();
		stateDraw();
		g.drawImage(buffImage, 0, 0, this);
	}
	
	public void backgroundDraw() {
		buffOS.clearRect(0, 0, 900, 900);
		buffOS.drawImage(BackgroundImg, 0, 0, this);
	}
	
	public void raderDraw() {
		Graphics2D g2 = (Graphics2D)buffOS; 
		int R = time*5 + 30;
		int radius = R / 2;
		int radius2 = (R - 17) / 2;
		int radius3 = (R - 25)  / 2;
		
		g2.setColor(Color.WHITE);
		g2.setStroke(new BasicStroke(10));
		g2.drawOval(450 - radius, 450 - radius, radius*2, radius*2);
		g2.setStroke(new BasicStroke(3));
		g2.drawOval(450 - radius2, 450 - radius2, radius2*2, radius2*2);
		g2.setStroke(new BasicStroke());
		g2.drawOval(450 - radius3, 450 - radius3, radius3*2, radius3*2);
		
		g2.setColor(new Color(255, 255, 255, 50));
		g2.setStroke(new BasicStroke(30));
		g2.drawOval(450 - 60, 450 - 60, 120, 120);
		
		g2.setStroke(new BasicStroke(45));
		g2.drawOval(450 - radius3, 450 - radius3, radius3*2, radius3*2);
		
		if(G.flares >= 100 && keyS) {
			colorCnt += 3;
			if(colorCnt < 255) {
				Color flareColor = new Color(255, 255, 255, 255 - colorCnt);
				g2.setColor(flareColor);
				g2.fillRect(0, 0, 900, 900);
			}else {
				G.flares -= 100;
				colorCnt = 0;
				keyS = false;
			}
		}			
	}
	
	public void explodeDraw() {
		Graphics2D g2 = (Graphics2D)buffOS;
		g2.setColor(Color.WHITE);
		g2.setStroke(new BasicStroke(7));
		//g2.drawOval(100 , 100, 10, 10);
	}
	
	public void shipDraw() {
		buffOS.clearRect(440, 440, 20, 20);
		buffOS.drawImage(ShipImg, 440, 440, this);
	}
	
	public void bulletDraw() {
		for(int i = 0; i < Bullets.size(); i++) {
			
			B = (Bullet)(Bullets.get(i));
			
			buffOS.drawImage(BulletImg, B.bulletPos.x, B.bulletPos.y , this);
		}
	}
	
	public void invaderDraw() {
		for(int i = 0; i < Invaders.size(); i++) {
			I = (Invader)(Invaders.get(i));
			if(I.shoot) {
				buffOS.drawImage(ShootedImg, I.invaderPos.x, I.invaderPos.y, this);
				I.shoot = false;
			}else {
				buffOS.drawImage(InvaderBigImg, I.invaderPos.x, I.invaderPos.y, this);
			}
		}
	}
	
	public void stateDraw(){
		buffOS.setFont(Statefont);
		buffOS.setColor(Color.WHITE);
		
		buffOS.drawString("S C O R E", 30, 100);
		buffOS.drawString(Integer.toString(G.score), 30, 120);
		
		buffOS.drawString("H E A L T H", 30, 150);
		buffOS.drawRect(32, 160, G.health * 2, 7);
		
		int ammo = G.flares / 100;
		buffOS.drawString("F L A R E", 30, 195);
		for(int i = 0 ; i < ammo; i++) {
			buffOS.drawRect(32+(i*15), 210, 3, 7);
		}
	}

	
	public void Rotate() {
		if(rad < -6.28319 || rad > 6.28319) {
			rad = 0;
		}
		if(keyL == true) {
			rad -= deg;
		}
		if(keyR == true) {
			rad += deg;
		}
	}
	
	public void BulletShoot() {
		if(keyF) {
			if(time % 3 == 0) {
				B = new Bullet(450, 450, rad);
				Bullets.add(B);
			}
		}
		for ( int i = 0 ; i < Bullets.size() ; i++){
			B = (Bullet) Bullets.get(i);
			B.setPos();
			
			if(B.bulletPos.x < -100 || B.bulletPos.x > 1000 || B.bulletPos.y < -100 || B.bulletPos.y > 1000) {
				Bullets.remove(i);
			}		
			for (int j = 0 ; j < Invaders.size(); j++){
				I = (Invader) Invaders.get(j);
				
				if (isCollidedbullet(B, I)){
					Bullets.remove(i);
					I.shooted();
					collidCheck = false;
				}
				if(I.health <= 0) {
					Invaders.remove(j);
					G.score += 10;
					G.flares += 5;
				}
			}	
		}
	}

	public void InvaderMov() {
		for(int i = 0; i < Invaders.size(); i++) {
			I = (Invader)(Invaders.get(i));
			I.setPos();
			if(I.invaderPos.x < -100 || I.invaderPos.x > 1000 || I.invaderPos.y < -100 || I.invaderPos.y > 1000) {
			Invaders.remove(i);
			}
			if(isColliedguardian(I)) {
				Invaders.remove(i);
				G.health -= 10;
				attackCheck = false;
			}
		}
				
		if (time % 250 == 0 ){
			int PosAllarea1 = (int)(Math.random() * 900 + 0);
			int Pos11 = (int)(Math.random() * 50 + 0);
			int Pos21 = (int)(Math.random() * 50 + 850);
			
			I = new Invader(PosAllarea1, Pos11);
			Invaders.add(I);
			
			
			I = new Invader(Pos11, PosAllarea1);
			Invaders.add(I);
			
			
			I = new Invader(PosAllarea1, Pos21);
			Invaders.add(I);
			
			
			I = new Invader(Pos21, PosAllarea1);
			Invaders.add(I);
			
			time = 0;
		}
	}
	
	public boolean isCollidedbullet(Bullet B, Invader I) {
		boolean horizontalCollied = false;
		boolean verticalCollied = false;
		if(I.invaderPos.x < (B.bulletPos.x + 5) && (I.invaderPos.x + 45) > B.bulletPos.x ) {
			horizontalCollied = true;
		}
		if(I.invaderPos.y < (B.bulletPos.y + 5) && (I.invaderPos.y + 45) > B.bulletPos.y  ) {
			verticalCollied = true;
		}
		if(horizontalCollied && verticalCollied) {
			collidCheck = true;
		}
		return collidCheck;
	}
	
	public boolean isColliedguardian(Invader I) {
		boolean horizontalCollied = false;
		boolean verticalCollied = false;
		if(I.invaderPos.x < 500 && (I.invaderPos.x + 45) > 400 ) {
			horizontalCollied = true;
		}
		if(I.invaderPos.y < 500 && (I.invaderPos.y + 45) > 400  ) {
			verticalCollied = true;
		}
		if(horizontalCollied && verticalCollied) {
			attackCheck = true;
		}
		return attackCheck;
	}	
	
	@Override
	public void keyPressed(KeyEvent e) {
		switch(e.getKeyCode()) {
			case KeyEvent.VK_LEFT:
				keyL = true;
				break;
			case KeyEvent.VK_RIGHT:
				keyR = true;
				break;
			case KeyEvent.VK_F:
				keyF = true;
				break;
			case KeyEvent.VK_SPACE:
				keyS = true;
				break;
		}
	}

	@Override
	public void keyReleased(KeyEvent e) {
		switch(e.getKeyCode()) {
		case KeyEvent.VK_LEFT:
			keyL = false;
			break;
		case KeyEvent.VK_RIGHT:
			keyR = false;
			break;
		case KeyEvent.VK_F:
			keyF = false;
			break;
		}
	}
	@Override
	public void keyTyped(KeyEvent e) {}
	
}
