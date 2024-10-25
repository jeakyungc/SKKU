package mainPackage;

import java.awt.Point;

public class Invader {
	
	private double movSpeed = 2;
	public int health  = 7;
	public boolean shoot = false;
	
	public Point invaderPos;
	public int invaderX;
	public int invaderY;
	private double deltaX;
	private double deltaY;
	public double TOtheta;
	
	Bullet b = new Bullet();
	
	Invader(int iX, int iY){
		
		invaderX = iX;
		invaderY = iY;
		TOtheta = (double)Math.atan2((invaderX) - 450, (invaderY) - 450);
		invaderPos = new Point(invaderX, invaderY);
		pathCalculator(invaderX, invaderY);
	}
	
	public void shooted() {
		health -= b.getDMG();
		shoot = true;
	}
	
	
	public void pathCalculator(int invaderX, int invaderY) {
		if( -180 <= (TOtheta * (180/Math.PI)) && (TOtheta * (180/Math.PI)) < -90 ) {
			deltaX = +Math.abs((double) Math.sin(TOtheta) * movSpeed);
	        deltaY = +Math.abs((double) Math.cos(TOtheta) * movSpeed);
		}
		else if ( -90 <= TOtheta * (180/Math.PI) && TOtheta * (180/Math.PI) < 0 ) {
			deltaX = +Math.abs((double)Math.sin(TOtheta) * movSpeed);
	        deltaY = -Math.abs((double)Math.cos(TOtheta) * movSpeed);
		}
		else if ( 0 <= TOtheta * (180/Math.PI) && TOtheta * (180/Math.PI) < 90 ) {
			deltaX = -Math.abs((double)Math.sin(TOtheta) * movSpeed);
	        deltaY = -Math.abs((double)Math.cos(TOtheta) * movSpeed);
		}
		else {
			deltaX = -Math.abs((double)Math.sin(TOtheta) * movSpeed);
	        deltaY = +Math.abs((double)Math.cos(TOtheta) * movSpeed);
		}
	}
	
	public void setPos() {
		invaderPos.x +=(int)deltaX;
		invaderPos.y +=(int)deltaY;
	}
}


