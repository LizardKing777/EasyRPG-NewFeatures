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

#ifndef EP_GAME_CUSTOM_BATTLE_H
#define EP_GAME_CUSTOM_BATTLE_H

#include <point.h>
#include <vector>
#include <lcf/inireader.h>
#include "game_battler.h"

namespace Game_CustomBattle {

	struct Vec2 {
		int x = 0;
		std::string s = "";
	};

	struct Skill {
		std::string dmg_algo = "";

		std::string name = "";
	};

	struct Item {
		std::string dmg_algo = "";

		int animation_id = 0;
	};

	struct Actor {
		std::vector<Skill> skills = { };
		Skill basic_skill;
	};

	void loadCustomBattle();
	void loadCustomisation();

	/**
	 *	ID == -1 => BasicSkill
	 *	ID == 0 => Basic Skill
	 *	ID > 0 => Skill XX
	 *
	 * Bool exactly => Get the "exactly" alg. For exemple, If I try to get the skill alg 18, but nothing is configure,
	 * if exactly == true => return ""
	 * Otherwise, it will return the Basic Skill alg
	 */
	std::string getSkillAlgo(int ID, const Game_Battler& src, bool exactly = false);

	std::string getItemAlgo(int ID, const Game_Battler& src, bool exactly = false);
	int getItemAnimationID(int id);

	bool isOperator(std::string c);
	int precedence(std::string op);
	double applyOp(double a, double b, std::string op);
	double evaluateExpression(const std::string& expression);
	bool is_number(const std::string& s);
	std::string ReplaceString(std::string subject, const std::string& search,
		const std::string& replace);

	std::string parseExpression(const std::string& expression, const Game_Battler* src_btl, const Game_Battler* trg_btl, int skill_ID, bool critical, bool charged);

}

#endif
