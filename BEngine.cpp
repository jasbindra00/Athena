#include "Game.h"

int main() {
	Game mygame;
	
	

	while (mygame.GetRenderWindow()->isOpen()) {
		
		mygame.Update();
		mygame.Draw();
		
		
	}
}