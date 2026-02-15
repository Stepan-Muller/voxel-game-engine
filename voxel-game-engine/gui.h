#pragma once

class Player;

class IGui {
public:
	/**
	 * @brief Render the menu GUI.
	 * 
	 * @param player Pointer to the player, used for accessing the player's engine GUI and passing it to the game GUI when rendering the menu GUI.
	 */
	virtual void render(Player* player) = 0;
};