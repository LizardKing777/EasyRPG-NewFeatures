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
#include <algorithm>
#include "bitmap.h"
#include "cache.h"
#include "input.h"
#include "game_enemyparty.h"
#include "game_party.h"
#include "game_actor.h"
#include "game_system.h"
#include "game_battle.h"
#include "player.h"
#include "font.h"
#include "output.h"
#include "window_battlestatusCustom.h"
#include "window_battlestatus.h"
#include "feature.h"
#include "scene_battle.h"
#include "game_strings.h"
#include <lcf/reader_util.h>

Window_BattleStatusCustom::Window_BattleStatusCustom(int ix, int iy, int iwidth, int iheight, bool enemy) :
	Window_BattleStatus(ix, iy, iwidth, iheight, enemy) {

	SetBorderX(4);

	SetContents(Bitmap::Create(width - 8, height - 16));

	if (Player::IsRPG2k3() && lcf::Data::battlecommands.window_size == lcf::rpg::BattleCommands::WindowSize_small) {
		height = 68;
		menu_item_height = 14;
		actor_face_height = 17;
		SetBorderY(5);
		SetContents(Bitmap::Create(width - 8, height - 10));
	}

	index = -1;

	text_offset = 4;

	if (lcf::Data::battlecommands.battle_type == lcf::rpg::BattleCommands::BattleType_gauge) {
		// Simulate a borderless window
		// Doing it this way for gauge style makes the implementation on
		// scene-side easier
		border_x = 0;
		border_y = 0;
		SetContents(Bitmap::Create(width, height));
		SetOpacity(0);
	}

	Refresh();
}

void Window_BattleStatusCustom::RefreshGauge() {}

void Window_BattleStatusCustom::Refresh() {
	contents->Clear();

	item_max = Main_Data::game_party->GetActors().size();

	auto it3 = CustomBattle::customWindows.find("Status");

	MenuCustomWindow win = CustomBattle::customWindows["Status"];

	int y = 0;
	for (int i = 0; i < item_max; ++i) {
		if (win.stats.empty()) {
			// The party always contains valid battlers
			const Game_Actor& actor = *(Main_Data::game_party->GetActors()[i]);

			int face_x = 0;
			if (Player::IsRPG2k3()) {
				if (!Feature::HasRow()) {
					face_x = 4;
				}
				else {
					face_x = actor.GetBattleRow() == Game_Actor::RowType::RowType_back ? 8 : 0;
				}
			}
			DrawActorFace(actor, face_x, i * menu_item_height + y);

			DrawActorName(actor, 48 + 8 + text_offset, i * menu_item_height + 2 + y);
			DrawActorTitle(actor, 48 + 8 + 88 + text_offset, i * menu_item_height + 2 + y);
			DrawActorLevel(actor, 48 + 8 + text_offset, i * menu_item_height + 2 + 16 + y);
			DrawActorState(actor, 48 + 8 + 42 + text_offset, i * menu_item_height + 2 + 16 + y);
			DrawActorExp(actor, 48 + 8 + text_offset, i * menu_item_height + 2 + 16 + 16 + y);
			int digits = (actor.MaxHpValue() >= 1000 || actor.MaxSpValue() >= 1000) ? 4 : 3;
			DrawActorHp(actor, 48 + 8 + 106 + text_offset - (digits == 3 ? 0 : 12), i * menu_item_height + 2 + 16 + y, digits);
			DrawActorSp(actor, 48 + 8 + 106 + text_offset - (digits == 3 ? 0 : 12), i * menu_item_height + 2 + 16 + 16 + y, digits);

			//y += 10;

		}
		else {
			for (auto stat : CustomBattle::customWindows["Status"].stats) {
				if (!stat.unique) {

					auto align = Text::AlignLeft;
					if (stat.align == 1)
						align = Text::AlignCenter;
					else if (stat.align == 2)
						align = Text::AlignRight;
					int tx = stat.x + (i % column_max) * (GetWidth() - 24) / column_max;
					int ty = stat.y + (i / column_max - GetTopRow()) * (menu_item_height)+2 + y;

					// Output::Debug("{} {} {} {} {} {}", i, column_max, GetWidth(), (i % column_max) * (GetWidth() - 24) / column_max, (i % column_max), (GetWidth() - 24) / column_max);

					// cursor_rect = { 4 + text_offset + (index % column_max) * (GetWidth() - 24) / column_max, (index / column_max - GetTopRow()) * (menu_item_height + 10), (GetWidth() - 24) / column_max, menu_item_height };

					std::string text = Game_Strings::Extract(stat.text, false);
					contents->TextDraw(tx, ty, Font::ColorDefault, Window_MenuStatus_Custom::ParserText(this, text, i, tx, ty, align), align);
					//contents->TextDraw(tx, ty, Font::ColorDefault, stat.text, align);
				}
			}
		}
	}
	for (auto stat : CustomBattle::customWindows["Status"].stats) {
		if (stat.unique) {
			//DrawActorName(actor, 48 + 8 + text_offset, i * 48 + 2 + y);
			auto align = Text::AlignLeft;
			if (stat.align == 1)
				align = Text::AlignCenter;
			else if (stat.align == 2)
				align = Text::AlignRight;
			int tx = stat.x;
			int ty = stat.y;
			std::string text = Game_Strings::Extract(stat.text, false);
			//text = ParserText(text, 0, tx, ty, align);
			contents->TextDraw(tx, ty, Font::ColorDefault, text, align);
		}
	}
	RefreshGauge();
}
void Window_BattleStatusCustom::UpdateCursorRect()
{
	if (index < 0) {
		cursor_rect = { 0, 0, 0, 0 };
	}
	else {
		if (CustomBattle::customWindows["Status"].stats.empty())
			cursor_rect = { 48 + 4 + text_offset, (index - GetTopRow()) * (menu_item_height), 168, menu_item_height };
		else
			cursor_rect = { 4 + text_offset + (index % column_max) * (GetWidth() - 24) / column_max, (index / column_max - GetTopRow()) * (menu_item_height), (GetWidth() - 24) / column_max, menu_item_height };
		// cursor_rect = { 4 + text_offset, (index - GetTopRow()) * (menu_item_height + 10), (GetWidth() - 24)/column_max, menu_item_height };
	}
}


void Window_BattleStatusCustom::RefreshResult(std::string win_name, int exp, int money, std::vector<int> drops) {
	contents->Clear();

	item_max = Main_Data::game_party->GetActors().size();

	auto it3 = CustomBattle::customWindows.find(win_name);

	MenuCustomWindow win = CustomBattle::customWindows[win_name];

	int y = 0;
	for (int i = 0; i < item_max; ++i) {

		for (auto stat : CustomBattle::customWindows[win_name].stats) {
			if (!stat.unique) {

				auto align = Text::AlignLeft;
				if (stat.align == 1)
					align = Text::AlignCenter;
				else if (stat.align == 2)
					align = Text::AlignRight;
				int tx = stat.x + (i % column_max) * (GetWidth() - 24) / column_max;
				int ty = stat.y + (i / column_max - GetTopRow()) * (menu_item_height)+2 + y;

				// Output::Debug("{} {} {} {} {} {}", i, column_max, GetWidth(), (i % column_max) * (GetWidth() - 24) / column_max, (i % column_max), (GetWidth() - 24) / column_max);

				// cursor_rect = { 4 + text_offset + (index % column_max) * (GetWidth() - 24) / column_max, (index / column_max - GetTopRow()) * (menu_item_height + 10), (GetWidth() - 24) / column_max, menu_item_height };

				std::string text = Game_Strings::Extract(stat.text, false);
				contents->TextDraw(tx, ty, Font::ColorDefault, Window_MenuStatus_Custom::ParserText(this, text, i, tx, ty, align), align);
				//contents->TextDraw(tx, ty, Font::ColorDefault, stat.text, align);
			}
		}

	}
	for (auto stat : CustomBattle::customWindows[win_name].stats) {
		if (stat.unique) {
			//DrawActorName(actor, 48 + 8 + text_offset, i * 48 + 2 + y);
			auto align = Text::AlignLeft;
			if (stat.align == 1)
				align = Text::AlignCenter;
			else if (stat.align == 2)
				align = Text::AlignRight;
			int tx = stat.x;
			int ty = stat.y;
			std::string text = Game_Strings::Extract(stat.text, false);

			std::regex patternE("@exp");
			std::string val = std::to_string(exp);
			text = std::regex_replace(text, patternE, val);

			std::regex patternG("@gold");
			val = std::to_string(money);
			text = std::regex_replace(text, patternG, val);

			std::string items = "";
			for (auto item_id : drops) {
				auto item = lcf::ReaderUtil::GetElement(lcf::Data::items, item_id);
				items = items + item->name.c_str() + "\n";
			}

			std::regex patternD("@drops");
			text = std::regex_replace(text, patternD, items);

			std::stringstream ss(text);
			std::string item;

			while (getline(ss, item, '\n')) {
				//contents->TextDraw(tx, ty, Font::ColorDefault, item, align);

				text = Window_MenuStatus_Custom::ParserText(this, item, 0, tx, ty, align);

				contents->TextDraw(tx, ty, Font::ColorDefault, text, align);
				ty += 14;
			}

			//text = ParserText(text, 0, tx, ty, align);
			//contents->TextDraw(tx, ty, Font::ColorDefault, text, align);
		}
	}
	RefreshGauge();
}
