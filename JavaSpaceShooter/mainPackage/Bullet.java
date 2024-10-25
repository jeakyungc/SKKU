package mainPackage;

import java.awt.Point;

public class Bullet {
	public Point bulletPos;
	private int speed = 20;
	
	private int DMG = 1;
	
	public int bulletX;
	public int bulletY;
	public double theta;
	
	public double deltaX;
	public double deltaY;
	
	
	Bullet(int bX, int bY, double rad) {
		bulletX = bX;
		bulletY = bY; 
        bulletPos = new Point(bulletX, bulletY);
        theta = rad * 57.2958;
        deltaX = (double) Math.sin(rad) * speed;
        deltaY = -(double) Math.cos(rad) * speed;
	}
	Bullet() {}
	
	public void setPos() {
		
		bulletPos.x += (int)deltaX;
		bulletPos.y += (int)deltaY;
	}
	
	
	public void setDMG(int DMG) {
		this.DMG = DMG;
	}
	public int getDMG() {
		return DMG;
	}
}
