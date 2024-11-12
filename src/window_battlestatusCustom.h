/*
 * This file is part of EasyRPG Player.
 *
 * EasyRPG Player is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * EasyRPG Player is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with EasyRPG Player. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EP_WINDOW_BATTLESTATUSCUSTOM_H
#define EP_WINDOW_BATTLESTATUSCUSTOM_H

// Headers
#include "window_battlestatus.h"
#include "bitmap.h"

/**
 * Window BattleStatus Class.
 * Displays the party battle status.
 */
class Window_BattleStatusCustom : public Window_BattleStatus {
public:
	/**
	 * Constructor.
	 */
	Window_BattleStatusCustom(int ix, int iy, int iwidth, int iheight, bool enemy = false);

	/**
	 * Renders the current status on the window.
	 */
	void Refresh() override;
	void RefreshResult(std::string win_name, int exp, int money, std::vector<int> drops);
	void RefreshGauge() override;

	void UpdateCursorRect() override;

protected:
	int text_offset = 0;
};

#endif
