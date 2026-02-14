#pragma once

class Player;

class IGui {
public:
	/**
	 * @brief Render the menu GUI.
	 */
	virtual void render(Player* player) = 0;
};