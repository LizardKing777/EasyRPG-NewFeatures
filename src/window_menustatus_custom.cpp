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
#include "window_menustatus_custom.h"
#include "cache.h"
#include "game_party.h"
#include "player.h"
#include "bitmap.h"
#include "feature.h"
#include "scene_menu_custom.h"
#include "game_strings.h"

Window_MenuStatus_Custom::Window_MenuStatus_Custom(int ix, int iy, int iwidth, int iheight) :
	Window_MenuStatus(ix, iy, iwidth, iheight) {

	if (Player::IsRPG2k3()) {
		SetContents(Bitmap::Create(width - 12, height - 16));
		SetBorderX(4);
		text_offset = 4;
	} else {
		SetContents(Bitmap::Create(width - 16, height - 16));
	}

	Refresh();
}

void Window_MenuStatus_Custom::Refresh() {
	contents->Clear();

	item_max = Main_Data::game_party->GetActors().size();

	auto it3 = CustomMenu::customWindows.find("Status");
	customised = it3 != CustomMenu::customWindows.end();
	if (!customised) {
		for (int i = 0; i < item_max; ++i) {
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

		}
	} else {
		MenuCustomWindow win = CustomMenu::customWindows["Status"];

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
				for (auto stat : CustomMenu::customWindows["Status"].stats) {
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

						contents->TextDraw(tx, ty, Font::ColorDefault, ParserText(this, stat.text, i, tx, ty, align), align);
					}
				}
			}
		}
		for (auto stat : CustomMenu::customWindows["Status"].stats) {
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
				text = ParserText(this, text, 0, tx, ty, align);
				contents->TextDraw(tx, ty, Font::ColorDefault, text, align);
			}
		}
	}

}

std::string Window_MenuStatus_Custom::replaceFunction(Window_Base* w, const std::smatch& match, int index, int tx, int ty, Text::Alignment align) {
	
	std::string variable = match[1].str();
	int i = 0;
	if (variable == "@id") {
		i = index;
	}
	else {
		i = atoi(variable.c_str());
	}
	const Game_Actor& actor = *(Main_Data::game_party->GetActors()[i]);

	std::string type = match[2].str();

	// Output::Debug("Regex : {} {}", variable, type);

	std::string t = "";

	std::string mod[] = { "name", "hp", "mhp", "mp", "mmp", "exp", "mexp", "level", "class","title", "state", "face", "ghp", "gmp", "gatb", "weapon", "shield","armor","helmet","accessory",
	"atk", "def", "spi", "agi"};

	for (auto m : mod) {
		if (type.rfind(m, 0) == 0) {

			if (m == "name") {
				auto s = actor.GetName();
				t = { s.begin(), s.end() };
			}
			else 	if (m == "hp") {
				t = std::to_string(actor.GetHp());
			}
			else if (m == "mhp") {
				t = std::to_string(actor.GetMaxHp());
			}
			else if (m == "mp") {
				t = std::to_string(actor.GetSp());

			}
			else if (m == "mmp") {
				t = std::to_string(actor.GetMaxSp());

			}
			else if (m == "exp") {
				t = std::to_string(actor.GetExp());

			}
			else if (m == "mexp") {
				t = std::to_string(actor.GetNextExp());

			}
			else if (m == "level") {
				t = std::to_string(actor.GetLevel());

			}
			else if (m == "class") {
				auto s = actor.GetClassName();
				t = { s.begin(), s.end() };

			}
			else if (m == "title") {
				auto s = actor.GetTitle();
				t = { s.begin(), s.end() };

			}
			else if (m == "state") {
				const lcf::rpg::State* state = actor.GetSignificantState();
				if (!state) {
					auto s = lcf::Data::terms.normal_status;
					t = { s.begin(), s.end() };
				}
				else {
					auto s = state->name;
					t = { s.begin(), s.end() };
				}


			}
			else if (m == "face") {
				if (align == Text::AlignCenter)
					tx -= 24;
				else if (align == Text::AlignRight)
					tx -= 48;

				w->DrawActorFace(actor, tx, ty);
				t = "";
			}
			else if (m == "ghp") {
				if (align == Text::AlignCenter)
					tx -= 12 + 16;
				else if (align == Text::AlignRight)
					tx -= 25 + 16;

				w->DrawCustomGauge(actor, 0, actor.GetHp(), actor.GetMaxHp(), tx, ty);
				t = "";
			}
			else if (m == "gmp") {
				if (align == Text::AlignCenter)
					tx -= 12 + 16;
				else if (align == Text::AlignRight)
					tx -= 25 + 16;

				w->DrawCustomGauge(actor, 1, actor.GetSp(), actor.GetMaxSp(), tx, ty);
				t = "";
			}
			else if (m == "gatb") {
				if (align == Text::AlignCenter)
					tx -= 12 + 16;
				else if (align == Text::AlignRight)
					tx -= 25 + 16;

				w->DrawCustomGauge(actor, 2, actor.GetAtbGauge(), actor.GetMaxAtbGauge(), tx, ty);
				t = "";
			}
			else if (m == "weapon") {
				if (actor.GetWeapon())
				{
					auto s = actor.GetWeapon()->name;
					t = { s.begin(), s.end() };
				}
			}
			else if (m == "shield") {
				if (actor.GetShield())
				{
					auto s = actor.GetShield()->name;
					t = { s.begin(), s.end() };
				}
				else if (actor.Get2ndWeapon()) {
					auto s = actor.Get2ndWeapon()->name;
					t = { s.begin(), s.end() };
				}
			}
			else if (m == "armor") {
				if (actor.GetArmor())
				{
					auto s = actor.GetArmor()->name;
					t = { s.begin(), s.end() };
				}
			}
			else if (m == "helmet") {
				if (actor.GetHelmet())
				{
					auto s = actor.GetHelmet()->name;
					t = { s.begin(), s.end() };
				}
			}
			else if (m == "accessory") {
				if (actor.GetAccessory())
				{
					auto s = actor.GetAccessory()->name;
					t = { s.begin(), s.end() };
				}
			}
			else if (m == "atk") {
				t = std::to_string(actor.GetAtk());

			}
			else if (m == "def") {
				t = std::to_string(actor.GetDef());

			}
			else if (m == "spi") {
				t = std::to_string(actor.GetSpi());

			}
			else if (m == "agi") {
				t = std::to_string(actor.GetAgi());

			}


			if (type.size() > m.size())
				t += type.substr(m.size(), type.size() - m.size());

			break;
		}
	}

	return t;
}


std::string Window_MenuStatus_Custom::ParserText(Window_Base* w, std::string text, int index, int tx, int ty, Text::Alignment align) {
	std::regex pattern("\\\\a\\[(.*?)\\]\\.([^\\s]*)");

	// Utilisation de std::regex_iterator pour parcourir les correspondances
	std::sregex_iterator it(text.begin(), text.end(), pattern);
	std::sregex_iterator end;

	// Résultat final
	std::string resultat;

	// Parcours des correspondances
	size_t lastPos = 0;
	while (it != end) {
		// Ajout du texte entre les correspondances précédentes et actuelles
		resultat += it->prefix();

		// Appel de la fonction de remplacement et ajout du résultat au texte final
		resultat += replaceFunction(w, *it, index, tx, ty, align);

		// Déplacement au prochain match
		lastPos = it->position() + it->length();
		++it;
	}

	// Ajout du texte après la dernière correspondance
	resultat += text.substr(lastPos);

	text = resultat;
	resultat = "";

	std::regex pattern2("@pic\\[(.*)\\]");
	std::sregex_iterator it2(text.begin(), text.end(), pattern2);

	lastPos = 0;
	while (it2 != end) {
		resultat += it2->prefix();

		resultat += "";
		std::string pic_name = it2->str(1);
		w->DrawPicture(pic_name, tx, ty);

		lastPos = it2->position() + it2->length();
		++it2;
	}

	resultat += text.substr(lastPos);

	return resultat;
}


void Window_MenuStatus_Custom::UpdateCursorRect()
{
	if (index < 0) {
		cursor_rect = { 0, 0, 0, 0 };
	} else if (customised){
		if (CustomMenu::customWindows["Status"].stats.empty())
			cursor_rect = { 48 + 4 + text_offset, (index - GetTopRow()) * (menu_item_height), 168, menu_item_height };
		else
			cursor_rect = { 4 + text_offset + (index % column_max) * (GetWidth() - 24) / column_max, (index / column_max - GetTopRow()) * (menu_item_height), (GetWidth() - 24)/column_max, menu_item_height };
		// cursor_rect = { 4 + text_offset, (index - GetTopRow()) * (menu_item_height + 10), (GetWidth() - 24)/column_max, menu_item_height };
	}
	else {
		cursor_rect = { 48 + 4 + text_offset, (index - GetTopRow()) * (menu_item_height), 168, menu_item_height };
	}
}

Game_Actor* Window_MenuStatus_Custom::GetActor() const {
	return &(*Main_Data::game_party)[GetIndex()];
}
