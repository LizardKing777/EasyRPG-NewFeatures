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

 // Headers
#include "window_command_custom.h"
#include "color.h"
#include "bitmap.h"
#include "util_macro.h"
#include <output.h>

static int CalculateWidth(const std::vector<std::string>& commands, int width) {
	if (width < 0) {
		width = 0;
		for (size_t i = 0; i < commands.size(); ++i) {
			width += std::max(width, Text::GetSize(*Font::Default(), commands[i]).width) + 16;
		}
	}
	return width;
}

Window_Command_Custom::Window_Command_Custom(std::vector<std::string> in_commands, int width, int c) :
	Window_Command(in_commands, -1)
{
	col_max = c;
	SetWidth(CalculateWidth(in_commands, width));
	SetHeight(32);
	ReplaceCommands(std::move(in_commands));
}

void Window_Command_Custom::DrawItem(int index, Font::SystemColor color) {
	Rect rect = GetItemRect(index);
	column_max = col_max;
	if (column_max > 1) {
		rect.x = (index % column_max) * (width / column_max);
		rect.y = (index / column_max) * menu_item_height + 2;
	}

	//Output::Debug("{} {} {} {}", column_max, index, rect.x, rect.y);

	contents->ClearRect(rect);
	contents->TextDraw(rect.x, rect.y, color, commands[index]);
}


void Window_Command_Custom::ReplaceCommands(std::vector<std::string> in_commands) {
	commands = std::move(in_commands);
	index = 0;
	SetItemMax(commands.size());
	const int num_contents = item_max > 0 ? item_max : 1;
	SetContents(Bitmap::Create(this->width - 16, num_contents * menu_item_height));
	SetTopRow(0);
	SetColumnMax(commands.size());

	Refresh();
}
