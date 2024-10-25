package mainPackage;

import javax.swing.JFrame; 

public class GameState {
	public int health = 100;
	public int score = 0;
	public int flares = 300;

	public void GameOver(MainWindow MW) {
		if(health <=  0) {
			MW.stop();
			MW.dispose();
			EndWindow EW = new EndWindow(score);
		}
	}	
}
