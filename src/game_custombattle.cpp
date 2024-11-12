
#include "game_custombattle.h"
#include <main_data.h>
#include "game_variables.h"
#include "game_enemyparty.h"
#include "game_enemy.h"
#include "game_actors.h"
#include "game_party.h"
#include <output.h>
#include <output.h>
#include <regex>

#include <iostream>
#include <sstream>
#include <cctype>
#include <cmath>
#include <stack>
#include <string>
#include <lcf/reader_util.h>
#include <rand.h>
#include <game_interpreter_map.h>
#include <scene_battle.h>
#include <game_map.h>
#include <game_interpreter_battle.h>


using namespace std;



using std::string;
using std::stringstream;

namespace Game_CustomBattle {

	std::vector<Skill> skills = {};
	std::vector<Actor> actors = {};
	std::vector<Actor> enemies = {};

	std::vector<Item> items = {};

	Skill basic_skill;


	// BattleHUD
	int commonEventID = 0;
	bool skipStartMessage = false;
	bool showAllBattleMSG = false;

	int targetShakeStrength = 0;
	int targetShakeSpeed = 0;
	int targetShakeDuration = 0;

	map<string, string> dictionary = {};

	bool playerWalkInPlace = false;
	int walkAnimationSpeed = -1;

}

void Game_CustomBattle::loadCustomBattle() {

	loadCustomisation();

}

void Game_CustomBattle::loadCustomisation(){
	std::string ini_file = FileFinder::Game().FindFile(EASYRPG_INI_NAME);

	auto ini_stream = FileFinder::Game().OpenInputStream(ini_file, std::ios_base::in);
	if (ini_stream) {
		lcf::INIReader ini(ini_stream);
		if (ini.ParseError() != -1) {

			skills.clear();
			items.clear();
			actors.clear();

			std::string attack_algo = ini.GetString("Algorithm", "alg_basic", "");
			Skill s;
			s.dmg_algo = attack_algo;
			skills.push_back(s);

			for (auto skill : lcf::Data::skills) {

				Skill s;

				std::string algo_dmg_n = "alg_skill_" + std::to_string(skill.ID);
				std::string str = ini.GetString("Skill", algo_dmg_n, "");
				s.dmg_algo = str;

				std::string name_n = "name_" + std::to_string(skill.ID);
				str = ini.GetString("Skill", name_n, "");
				s.name = str;

				skills.push_back(s);
			}

			Actor a_empty;
			actors.push_back(a_empty);
			for (auto actor : lcf::Data::actors) {
				Actor a;

				attack_algo = ini.GetString("Algorithm", "alg_hero" + std::to_string(actor.ID), "");
				a.basic_skill.dmg_algo = attack_algo;

				Skill s_empty;
				a.skills.push_back(s_empty);
				for (auto skill : lcf::Data::skills) {

					Skill s;

					std::string algo_dmg_n = "alg_hero_" + std::to_string(actor.ID) + "_skill_" + std::to_string(skill.ID);
					std::string str = ini.GetString("Skill", algo_dmg_n, "");
					s.dmg_algo = str;

					a.skills.push_back(s);

				}
				actors.push_back(a);
			}

			Actor e_empty;
			enemies.push_back(e_empty);
			for (auto actor : lcf::Data::enemies) {
				Actor a;

				attack_algo = ini.GetString("Algorithm", "alg_enemy" + std::to_string(actor.ID), "");
				a.basic_skill.dmg_algo = attack_algo;

				Skill s_empty;
				a.skills.push_back(s_empty);
				for (auto skill : lcf::Data::skills) {

					Skill s;

					std::string algo_dmg_n = "alg_enemy_" + std::to_string(actor.ID) + "_skill_" + std::to_string(skill.ID);
					std::string str = ini.GetString("Skill", algo_dmg_n, "");
					s.dmg_algo = str;

					a.skills.push_back(s);

				}
				enemies.push_back(a);
			}

			Item i;

			// No weapon
			std::string algo_dmg_n = "alg_item_0";
			std::string str = ini.GetString("Item", algo_dmg_n, "");
			i.dmg_algo = str;
			items.push_back(i);

			for (auto item : lcf::Data::items) {

				std::string algo_dmg_n = "alg_item_" + std::to_string(item.ID);
				std::string str = ini.GetString("Item", algo_dmg_n, "");
				i.dmg_algo = str;

				std::string item_animation_n = "animation_id_" + std::to_string(item.ID);
				i.animation_id = ini.GetInteger("Item", item_animation_n, 0);

				items.push_back(i);
			}


			attack_algo = ini.GetString("Algorithm", "alg_skill_basic", "");
			basic_skill.dmg_algo = attack_algo;
		}
	}
}

std::string Game_CustomBattle::getItemAlgo(int i, const Game_Battler& src, bool a) {
	if (i == -1)
		return basic_skill.dmg_algo;

	if (i < items.size())
		return items[i].dmg_algo;
	return "";
}

int Game_CustomBattle::getItemAnimationID(int i) {
	if (i == -1)
		return 0;

	if (i < items.size())
		return items[i].animation_id;
	return 0;
}

std::string Game_CustomBattle::getSkillAlgo(int i, const Game_Battler& src, bool exactly) {
	std::string str = "";


	if (src.GetType() == Game_Battler::Type_Ally) {
		if (i == -1)
			str = actors[src.GetId()].basic_skill.dmg_algo;
		else if (i < skills.size())
			str = actors[src.GetId()].skills[i].dmg_algo;
	}
	else {
		if (i == -1)
			str = enemies[src.GetId()].basic_skill.dmg_algo;
		else if (i < skills.size())
			str = enemies[src.GetId()].skills[i].dmg_algo;
	}

	if (str == "")
		if (i == -1)
			str = basic_skill.dmg_algo;
		else if (i < skills.size())
			str = skills[i].dmg_algo;

	if (i == -1)
		str = basic_skill.dmg_algo;
	else if (i < skills.size())
		str = skills[i].dmg_algo;


	if (!exactly) {
		if (str == "")
			str = actors[src.GetId()].basic_skill.dmg_algo;
		if (str == "")
			str = basic_skill.dmg_algo;
	}

	return str;
}

// Function to check if a character is an operator
bool Game_CustomBattle::isOperator(std::string c)
{
	// Returns true if the character is an operator
	return c == "+" || c == "-" || c == "*" || c == "/"
		|| c == "^" || c == "min" || c == "max" || c == "?"
		|| c == "<" || c == ">" || c == "=" || c == "!"
		|| c == "<=" || c == ">=";
}

// Function to get the precedence of an operator
int Game_CustomBattle::precedence(std::string op)
{
	// Returns the precedence of the operator
	if (op == "+" || op == "-")
		return 1;
	if (op == "*" || op == "/")
		return 2;
	if (op == "^" || op == "min" || op == "max" || op == "?" || op == "<" || op == ">" || op == "="
		|| op == "!" || op == "<=" || op == ">=")
		return 3;
	return 0;
}

// Function to apply an operator to two operands
double Game_CustomBattle::applyOp(double a, double b, std::string op)
{

	int value = 0;
	int rmax = std::max(a, b);
	int rmin = std::min(a, b);

	// Applies the operator to the operands and returns the
	// result
	if (op == "+")
		return a + b;
	if (op == "-")
		return a - b;
	if (op == "*")
		return a * b;
	if (op == "/")
		return a / b;
	if (op == "^")
		return pow(a, b);
	if (op == "min")
		return min(a, b);
	if (op == "max")
		return max(a, b);
	if (op == "?") {
		value = Rand::GetRandomNumber(rmin, rmax);
		return value;
	}
	if (op == "<")
		if (a < b)
			return 1;
		else
			return 0;
	if (op == ">")
		if (a > b)
			return 1;
		else
			return 0;
	if (op == "=")
		if (a == b)
			return 1;
		else
			return 0;
	if (op == "<=")
		if (a <= b)
			return 1;
		else
			return 0;
	if (op == ">=")
		if (a >= b)
			return 1;
		else
			return 0;
	if (op == "!")
		if (a != b)
			return 1;
		else
			return 0;

	return 0;
}

bool Game_CustomBattle::is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

std::string Game_CustomBattle::ReplaceString(std::string subject, const std::string& search,
	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

std::string Game_CustomBattle::parseExpression(const string& expression, const Game_Battler* src_btl, const Game_Battler* trg_btl, int skill_ID, bool critical, bool charged) {

	string token;

	std::string chaine = expression;
	std::transform(chaine.begin(), chaine.end(), chaine.begin(),
		[](unsigned char c) { return std::tolower(c); });

	std::sregex_iterator end;

	std::string sub_string = chaine;

	std::regex regW("weaponalg");
	std::sregex_iterator it_weapon(chaine.begin(), chaine.end(), regW);
	while (it_weapon != end)
	{
		int itemID = 0;
		if (src_btl->GetType() == Game_Battler::Type_Ally) {
			Game_Actor* a = (Game_Actor*) src_btl;
			if (a)
				itemID = a->GetWeaponId();
		}
		std::string str = Game_CustomBattle::getItemAlgo(itemID, (const Game_Battler&) src_btl);
		std::transform(str.begin(), str.end(), str.begin(),
			[](unsigned char c) { return std::tolower(c); });

		sub_string = std::regex_replace(sub_string, std::regex(it_weapon->str()), str);

		it_weapon++;
	}

	chaine = sub_string;

	std::regex regE("callevent\\((\\d*),(\\d*),(\\d*)\\)");
	std::sregex_iterator it_event(chaine.begin(), chaine.end(), regE);
	while (it_event != end)
	{
		int eventID = std::stoi(it_event->str(1));
		int enemy_index = std::stoi(it_event->str(2));
		int enemyID = std::stoi(it_event->str(3));

		// Output::Debug("{} {} {} {}", eventID, enemy_index, enemyID, it_event->str());

		if (eventID > 0) {

			if (trg_btl->GetType() == Game_Battler::Type_Ally)
				Main_Data::game_variables->Set(enemy_index, Main_Data::game_party->GetActorPositionInParty(trg_btl->GetId()));
			else
				Main_Data::game_variables->Set(enemy_index, Main_Data::game_enemyparty->GetEnemyPositionInParty((const Game_Enemy*)trg_btl));

			Main_Data::game_variables->Set(enemyID, trg_btl->GetId());

			Game_CommonEvent* common_event = lcf::ReaderUtil::GetElement(Game_Map::GetCommonEvents(), eventID);

			Game_Battle::GetInterpreterBattle().Push(common_event);

		}

		sub_string = ReplaceString(chaine, it_event->str(), "");

		it_event++;
	}

	chaine = sub_string;


	regex pattern("iscritical");
	std::string val = "0";
	if (critical)
		val = "1";
	chaine = std::regex_replace(chaine, pattern, val);

	pattern = "ischarged";
	val = "0";
	if (charged)
		val = "1";
	chaine = std::regex_replace(chaine, pattern, val);

	pattern = "gold";
	val = std::to_string(Main_Data::game_party->GetGold());
	chaine = std::regex_replace(chaine, pattern, val);

	if (skill_ID > 0) {

		lcf::rpg::Skill* skill = lcf::ReaderUtil::GetElement(lcf::Data::skills, skill_ID);

		pattern = "variance";
		val = std::to_string(skill->variance);
		chaine = std::regex_replace(chaine, pattern, val);

		pattern = "basevalue";
		val = std::to_string(skill->power);
		chaine = std::regex_replace(chaine, pattern, val);

		pattern = "physicalrate";
		val = std::to_string(skill->physical_rate);
		chaine = std::regex_replace(chaine, pattern, val);

		pattern = "magicalrate";
		val = std::to_string(skill->magical_rate);
		chaine = std::regex_replace(chaine, pattern, val);

		pattern = "hitrate";
		val = std::to_string(skill->hit);
		chaine = std::regex_replace(chaine, pattern, val);
	}

	sub_string = chaine;

	std::regex reg1("([ab])\\.((atk)*(def)*(int)*(agi)*(hp)*(mhp)*(mp)*(mmp)*(lvl)*(exp)*(id)*(isactor)*(isenemy)*)");
	std::sregex_iterator it_reg(chaine.begin(), chaine.end(), reg1);
	while (it_reg != end)
	{

		std::string src = "";
		std::string param;

		src = it_reg->str(1);
		param = it_reg->str(2);

		//for (int i = 1; i < it_reg->size(); i++) {
		//	std::string token = it_reg->str(i);
		//	Output::Debug("{} : {}", i, token);
		//}
		//Output::Debug("{} {}", src, param);

		const Game_Battler* btl{};
		int value = 0;
		if (src == "a")
			btl = src_btl;
		if (src == "b")
			btl = trg_btl;

		if (btl) {
			if (param == "atk")
				value = btl->GetAtk();
			else if (param == "def")
				value = btl->GetDef();
			else if (param == "int")
				value = btl->GetSpi();
			else if (param == "agi")
				value = btl->GetAgi();

			else if (param == "hp")
				value = btl->GetHp();
			else if (param == "mhp")
				value = btl->GetMaxHp();

			else if (param == "mp")
				value = btl->GetSp();
			else if (param == "mmp")
				value = btl->GetMaxSp();

			else if (param == "lvl") {
				if (btl->GetType() == Game_Battler::Type_Ally) {
					auto actor = (Game_Actor*) btl;
					value = actor->GetLevel();
				}
			}
			else if (param == "exp") {
				if (btl->GetType() == Game_Battler::Type_Ally) {
					auto actor = (Game_Actor*)btl;
					value = actor->GetExp();
				}
			}

			else if (param == "id") {
				value = btl->GetId();
			}

			else if (param == "isactor") {
				value = 0;
				if (btl->GetType() == Game_Battler::Type_Ally) {
					value = 1;
				}
			}

			else if (param == "isenemy") {
				value = 0;
				if (btl->GetType() == Game_Battler::Type_Enemy) {
					value = 1;
				}
			}
		}

		sub_string = std::regex_replace(sub_string, std::regex(src + "." + param), to_string(value));

		it_reg++;
	}

	std::stack<int> bracketIndices;
	std::string::size_type pos = 0;

	while (pos < sub_string.size()) {
		if (sub_string[pos] == '[') {
			if (pos > 0 && (sub_string[pos - 1] == 'v' || sub_string[pos - 1] == 'i')) {
				bracketIndices.push(pos - 1); // Push the index of 'v'
			}
		}
		else if (sub_string[pos] == ']') {
			if (!bracketIndices.empty()) {
				int start = bracketIndices.top();
				bracketIndices.pop();
				// Extract the content between the brackets
				std::string valeurEntreCrochets = sub_string.substr(start + 2, pos - start - 2);
				// Generate the replacement based on the content
				std::string replacement = "";
				int value = 0;
				bool replacement_found = false;

				if (start >= 0 && sub_string[start] == 'v') {
					if (is_number(valeurEntreCrochets)) {
						std::string varID = valeurEntreCrochets;
						value = Main_Data::game_variables->Get(stoi(varID));
					}
					else {
						valeurEntreCrochets = parseExpression(valeurEntreCrochets, src_btl, trg_btl, skill_ID, critical, charged);
						value = evaluateExpression(valeurEntreCrochets);
						value = Main_Data::game_variables->Get(value);
					}
					replacement_found = true;
				}

				if (start >= 0 && sub_string[start] == 'i') {
					if (is_number(valeurEntreCrochets)) {
						std::string varID = valeurEntreCrochets;
						value = Main_Data::game_party->GetItemCount(stoi(varID));
					}
					else {
						valeurEntreCrochets = parseExpression(valeurEntreCrochets, src_btl, trg_btl, skill_ID, critical, charged);
						value = evaluateExpression(valeurEntreCrochets);
						value = Main_Data::game_party->GetItemCount(value);
					}
					replacement_found = true;
				}

				if (replacement_found) {
					replacement = to_string(value);
					// Replace from the character 'v' before the '[' to the closing ']'
					sub_string.replace(start, pos - start + 1, replacement);
					// Adjust pos to account for the replacement
					pos = start + replacement.size() - 1;
				}
			}
		}
		++pos;
	}

	// Output::Debug("AA {} ", sub_string);

	if (sub_string == "")
		sub_string = "0";

	return sub_string;
}

// Function to parse and evaluate a mathematical expression
// Found here : https://www.geeksforgeeks.org/how-to-parse-mathematical-expressions-in-cpp/
double Game_CustomBattle::evaluateExpression(const string& expression)
{
	stack<std::string> operators; // Stack to hold operators
	stack<double> operands; // Stack to hold operands

	string token;
	std::string chaine = expression;

	// Output::Debug("{}", chaine);

	std::regex pattern("(\\()?(<=|>=|min|max|[-+*\\/{}<>=!\?])?(-?[0-9]*)(\\))?"); // ("([-+*\\/])?(-?[0-9]+)"); // (\\()?([-+*\\/])?(-?[0-9]+)\\)?

	std::sregex_iterator it(chaine.begin(), chaine.end(), pattern);
	std::sregex_iterator end;

	std::string op = "";

	std::vector<Vec2> algo = {};
	int w = 0;
	char old_op = ' ';
	while (it != end)
	{
		for (int i = 1; i < it->size(); i++) {
			token = it->str(i);

			if (token.empty())
				continue; // Skip empty tokens
			if (isdigit(token[0])) { // If the token is a number
				double num;
				stringstream(token)
					>> num; // Convert the token to a number
				operands.push(num); // Push the number onto the
				// operand stack
			}
			else if (Game_CustomBattle::isOperator(token)) { // If the token is
				// an operator
				std::string op = token;
				// While the operator stack is not empty and the
				// top operator has equal or higher precedence
				while (!operators.empty()
					&& Game_CustomBattle::precedence(operators.top())
					>= Game_CustomBattle::precedence(op)) {
					// Pop two operands and one operator
					double b = operands.top();
					operands.pop();
					double a = operands.top();
					operands.pop();
					std::string op = operators.top();
					operators.pop();
					// Apply the operator to the operands and
					// push the result onto the operand stack
					operands.push(Game_CustomBattle::applyOp(a, b, op));
				}
				// Push the current operator onto the operator
				// stack
				operators.push(op);
			}
			else if (token[0] == '(') { // If the token is an
				// opening parenthesis
	// Push it onto the operator stack
				operators.push("(");
			}
			else if (token[0] == ')') { // If the token is a
				// closing parenthesis
	// While the operator stack is not empty and the
	// top operator is not an opening parenthesis
				while (!operators.empty()
					&& operators.top() != "(") {
					// Pop two operands and one operator
					double b = operands.top();
					operands.pop();
					double a = operands.top();
					operands.pop();
					std::string op = operators.top();
					operators.pop();
					// Apply the operator to the operands and
					// push the result onto the operand stack
					operands.push(Game_CustomBattle::applyOp(a, b, op));
				}
				// Pop the opening parenthesis
				operators.pop();
			}
		}

		it++;
	}

	// While the operator stack is not empty
	while (!operators.empty()) {
		// Pop two operands and one operator
		double b = operands.top();
		operands.pop();
		double a = operands.top();
		operands.pop();
		std::string op = operators.top();
		operators.pop();
		// Apply the operator to the operands and push the
		// result onto the operand stack
		operands.push(Game_CustomBattle::applyOp(a, b, op));
	}

	// The result is at the top of the operand stack
	return operands.top();
}
