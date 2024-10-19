﻿#include <spritesetmap_doom.h>
#include <output.h>
#include "bitmap.h"
#include <filefinder.h>
#include <player.h>
#include <cmath>
#include <main_data.h>
#include <game_variables.h>
#include <map>
#include <iostream>
#include <input.h>
#include "game_map.h"
#include "game_player.h"
#include <cache.h>
#include "scene_map.h"

int Spriteset_MapDoom::mapWidth() {
	return mapW;
}
int Spriteset_MapDoom::mapHeight() {
	return mapH;
}

float Spriteset_MapDoom::castRay(float rayAngle, int& ray, std::vector<DrawingDoom>& d, int x, int &mx, int &my) {

	float rayPosX = player.x;
	float rayPosY = player.y;

	// Direction du rayon
	float rayDirX = cos(rayAngle);
	float rayDirY = sin(rayAngle);

	int mapX = (int)(rayPosX / TILE_SIZE);
	int mapY = (int)(rayPosY / TILE_SIZE);

	float rayDistance = 0.0;
	bool hit = false;

	int last_y = -1;

	while (!hit) {
		// Vérification si le rayon sort des limites de la carte
		if (mapX < 0 || mapX >= mapWidth() || mapY < 0 || mapY >= mapHeight()) {
			rayDistance = -1;
			break;  // Sortir de la boucle si on dépasse la carte
		}

		if ((int)rayPosX % TILE_SIZE == 8 && (int)rayPosY % TILE_SIZE == 8)
		{
			auto event = Game_Map::GetEventAt(mapX, mapY, true);
			if (event) {
				DrawingDoom de = { 1, x, rayDistance - 0.1f, (int)rayPosX % TILE_SIZE, event->GetId() };

				auto b = std::find_if(d.begin(),
					d.end(),
					[&d1 = de]
					(const DrawingDoom& d2) -> bool { return d2.type == 1 && d1.evID == d2.evID; });


				if (b == d.end()) {
					d.push_back(de);
				}
			}
		}

		if (last_y != mapY) {

			DrawingDoom de = { 2, x, rayDistance - 0.1f, (int)rayPosX % TILE_SIZE, (int)rayPosY % TILE_SIZE, { mapX, mapY }};
			d.push_back(de);

			last_y = mapY;
		}

		// Vérifier si le rayon touche un mur
		if (map[mapY][mapX] == 1) {
			hit = true;
		}
		else {
			// Incrémenter la position du rayon
			rayPosX += rayDirX;
			rayPosY += rayDirY;

			mapX = (int)(rayPosX / TILE_SIZE);
			mapY = (int)(rayPosY / TILE_SIZE);

			// Calcul de la distance actuelle du rayon
			rayDistance += sqrt(rayDirX * rayDirX + rayDirY * rayDirY);
		}
	}

	mx = mapX;
	my = mapY;

	ray = (int) rayPosX % TILE_SIZE;
	if (ray == 0 || ray == TILE_SIZE - 1)
		ray = (int)rayPosY % TILE_SIZE;

	return rayDistance;  // Retourner la distance si un mur est touché
}


// Fonction pour dessiner la scène en utilisant les fonctions de Spriteset_MapDoom
void Spriteset_MapDoom::renderScene() {

	// Output::Debug("Render Scene");


	float FOV = player.fov;

	Scene_Map* scene = (Scene_Map*)Scene::Find(Scene::Map).get();

	renderTexturedFloor();

	std::vector<DrawingDoom> drawings = {};

	for (int x = 0; x < Player::screen_width; x++) {
		// Calculer l'angle du rayon
		//float rayAngle = player.angle + atan2(x - Player::screen_width / 2, Player::screen_width / 2);
		float rayAngle = player.angle + (x - Player::screen_width / 2) * (FOV / Player::screen_width);


		int ray = 0;

		int mx;
		int my;
		// Cast le rayon et obtenir la distance
		float distance = castRay(rayAngle, ray, drawings, x, mx, my);


		DrawingDoom d = { 0, x, distance, ray, 0, {mx, my} };

		drawings.push_back(d);

	}

	std::sort(drawings.begin(), drawings.end(), std::greater<DrawingDoom>());
	
	for (auto d : drawings) {

		if ( d.type == 2) {
			float distance = d.distance;

			//Output::Debug(" {}", distance);

			if (distance > 0) {

				// Test 1
				//
				// 
				// Calcul de la distance en fonction de la hauteur à laquelle le sol doit être projeté
				//float y = d.position.y * TILE_SIZE;
				//	float rowDistance = Player::screen_height / (2.0f * (Player::screen_height - y));  // y est la hauteur actuelle dans l'image

				//	float directionX = cos(player.angle);  // direction X basée sur l'angle
				//	float directionY = sin(player.angle);  // direction Y basée sur l'angle

				//	float posX = player.x;
				//	float posY = player.y;

				//	// Coordonnées du monde réel pour la position x, y
				//	float floorX = posX + rowDistance * (directionX - posX);
				//	float floorY = posY + rowDistance * (directionY - posY);

				//	// Charger la texture du sol à la position calculée
				//	int textureWidth = TILE_SIZE;
				//	int textureHeight = TILE_SIZE;
				//	int textureX = static_cast<int>(floorX * textureWidth) % textureWidth;
				//	int textureY = static_cast<int>(floorY * textureHeight) % textureHeight;

				//	Rect r = Rect(d.x, y, 1, 1); // 1 pixel à la fois
				//	BitmapRef texture = bitmap;// floorTexture(floorX, floorY);

				//	if (texture) {
				//		Rect srcRect = { textureX, textureY, 1, 1 };
				//		sprite->StretchBlit(r, *texture, srcRect, 255);  // Afficher un pixel du sol
				//	}

				// Test 2

				//int lineHeight = static_cast<int>(TILE_SIZE * Player::screen_height / distance);
				//int drawStart = (Player::screen_height - lineHeight) / 2;
				//int drawEnd = drawStart + lineHeight;

				//int textureX = d.textureX;

				//Rect r = Rect(d.x, drawStart, 1, drawEnd - drawStart);
				//BitmapRef texture = mapTexture(d.position.x, d.position.y);

				//if (texture) {
				//	Rect srcRect = { textureX, 0, 1, texture->height() };
				//	sprite->StretchBlit(r, *texture, srcRect, 255);
				//}

			}
		}
		else if (d.type == 0) {
			float distance = d.distance;

			//Output::Debug(" {}", distance);

			if (distance > 0) {
				int lineHeight = static_cast<int>(TILE_SIZE * Player::screen_height / distance);
				int drawStart = (Player::screen_height - lineHeight) / 2;
				int drawEnd = drawStart + lineHeight;

				int textureX = d.textureX;

				Rect r = Rect(d.x, drawStart, 1, drawEnd - drawStart);
				BitmapRef texture = mapTexture(d.position.x, d.position.y);

				if (texture) {
					Rect srcRect = { textureX, 0, 1, texture->height() };
					sprite->StretchBlit(r, *texture, srcRect, 255);
				}

			}
		}
		else if (d.type == 1) {
			float distance = d.distance;

			//Output::Debug(" {} {}", d.evID, d.distance);

			if (distance > 0) {

				BitmapRef texture = mapTexture(0, 0);
				texture = scene->GetEventSprite(d.evID);

				if (texture) {

					int lineHeight = static_cast<int>(TILE_SIZE * Player::screen_height / distance);
					int drawStart = (Player::screen_height - lineHeight) / 2;
					int textureX = d.textureX;

					float d5 = (float) lineHeight / Player::screen_height;
					int zx = d5 * 6 * texture->width();
					int zy = d5 * 6 * texture->height();


					//Output::Debug(" {} {} {} {} {}", z, textureX, d5, Player::screen_height/lineHeight, zf);

					Rect srcRect = { 0, 0, texture->width(), texture->height()};
					//Rect r = Rect(Game_Map::GetEvent(d.evID)->GetX() * TILE_SIZE - zx / 2, drawStart + zy / 4, zx, zy);
					int sprCorrection = 4;
					if (Game_Map::GetEvent(d.evID)->HasTileSprite())
						sprCorrection = 1;
					Rect r = Rect(d.x - zx / 2, drawStart + zy / sprCorrection, zx, zy);
					sprite->StretchBlit(r, *texture, srcRect, 255);
				}
			}
		}
	}

	/*
	//for (int x = 0; x < Player::screen_width; x++) {

	//	float rayAngle = player.angle + atan2(x - Player::screen_width / 2, Player::screen_width / 2);

	//	int ray = 0;
	//	// Lancer le rayon et calculer la distance entre le joueur et le mur touché
	//	float rayDistance = castRay(rayAngle, ray);

	//	// Correction du fish-eye effect
	//	rayDistance = rayDistance * cos(rayAngle - player.angle);

	//	// Calculer la hauteur de la ligne du mur (projetée correctement)
	//	int lineHeight = (int)(TILE_SIZE * Player::screen_height / rayDistance);


	//	int drawStart = -lineHeight / 2 + Player::screen_height / 2;
	//	if (drawStart < 0) drawStart = 0;
	//	int drawEnd = lineHeight / 2 + Player::screen_height / 2;
	//	if (drawEnd >= Player::screen_height) drawEnd = Player::screen_height - 1;

	//	float darknessFactor = 1.0f / (1.0f + rayDistance * 0.05f);

	//	Color baseColor = Color(255, 0, 0, 255);

	//	// Appliquer le facteur d'assombrissement sur les composantes RGB
	//	int red = (int)(baseColor.red * darknessFactor);
	//	int green = (int)(baseColor.green * darknessFactor);
	//	int blue = (int)(baseColor.blue * darknessFactor);

	//	// S'assurer que les valeurs de couleur restent dans les limites valides [0, 255]
	//	red = std::min(255, std::max(0, red));
	//	green = std::min(255, std::max(0, green));
	//	blue = std::min(255, std::max(0, blue));

	//	Color color = Color(red, green, blue, 255);

	//	int h = drawEnd - drawStart;
	//	if (h < 0)
	//		h *= -1;

	//	Rect r = Rect(x, drawStart, 1, h);

	//	sprite->FillRect(r, color);

	//	BitmapRef texture = mapTexture(1, 0);

	//	if (texture) {

	//		Rect srcRect = { 0, 0, 1, texture->height() };

	//		// Appliquer les textures
	//		r.height /= 2;
	//		sprite->StretchBlit(r, *texture, srcRect, 255);
	//		r.y += r.height;
	//		sprite->StretchBlit(r, *texture, srcRect, 255);

	//		//int textureWidth = texture->width();
	//		////float wallX = fmod(rayDistance, TILE_SIZE) / TILE_SIZE;  // Position horizontale dans la texture
	//		//float wallX = x % TILE_SIZE / (float)TILE_SIZE;
	//		//int textureX = (int)(wallX * textureWidth);
	//		//Rect srcRect = { textureX, 0, 1, texture->height() };

	//		////sprite->BlitScale(r, *texture, srcRect);
	//		//r.height /= 2;
	//		//sprite->StretchBlit(r, *texture, srcRect, 255);
	//		//r.y += r.height;
	//		//sprite->StretchBlit(r, *texture, srcRect, 255);
	//	}

	//}
	*/
}

void Spriteset_MapDoom::renderFloorAndCeiling(float playerX, float playerY, float playerAngle) {

}

BitmapRef Spriteset_MapDoom::mapTexture(int x, int y) {
	if (x >= 0 && y >= 0 && x < mapWidth() && y < mapHeight()) {
		Scene_Map* scene = (Scene_Map*)Scene::Find(Scene::Map).get();
		return scene->GetTile(x, y, 0);
	}
	else {
		BitmapRef b = Bitmap::Create(TILE_SIZE, TILE_SIZE, Color(0, 0, 0, 0));
		return b;
	}
}

void Spriteset_MapDoom::renderTexturedFloor() {


	return;

	// Test 3

	const float FOV = player.fov; // Field of view
	const float HALF_FOV = FOV / 2.0;

	float playerAngle = player.angle;
	float playerPosX = player.x;
	float playerPosY = player.y;

	for (int y = Player::screen_height / 2; y < Player::screen_height; y++) {
		float dist = (Player::screen_height / 2.0f) / (y - Player::screen_height / 2.0f);

		for (int x = 0; x < Player::screen_width; x++) {
			float cameraX = 2 * x / (float)Player::screen_width - 1;
			float rayDirX = playerPosX + dist * (cos(playerAngle) + cameraX * cos(playerAngle + HALF_FOV));
			float rayDirY = playerPosY + dist * (sin(playerAngle) + cameraX * sin(playerAngle + HALF_FOV));

			int tileX = (int)(rayDirX / TILE_SIZE);
			int tileY = (int)(rayDirY / TILE_SIZE);

			if (tileX >= 0 && tileY >= 0 && tileX < mapWidth() && tileY < mapHeight()) {
				BitmapRef floorTexture = bitmap;

				if (floorTexture) {
					int texX = (int)(rayDirX * 256 / TILE_SIZE) % 16;
					int texY = (int)(rayDirY * 256 / TILE_SIZE) % 16;

					Rect srcRect = { texX, texY, 1, 1 };
					Rect destRect = { x, y, 1, 1 };
					sprite->StretchBlit(destRect, *floorTexture, srcRect, 255);
				}
			}
		}
	}

	// Test 4
	
	//int middleY = Player::screen_height / 2;

	//// Précalculer les directions des rayons du champ de vision (FOV)
	//float rayDirX0 = cos(player.angle - player.fov / 2);  // Rayon gauche du FOV
	//float rayDirY0 = sin(player.angle - player.fov / 2);
	//float rayDirX1 = cos(player.angle + player.fov / 2);  // Rayon droit du FOV
	//float rayDirY1 = sin(player.angle + player.fov / 2);


	//float playerX = player.x / TILE_SIZE;
	//float playerY = player.y / TILE_SIZE;


	//// Itérer sur chaque ligne sous l'horizon
	//for (int y = middleY; y < Player::screen_height; ++y) {
	//	// Distance projetée pour cette ligne
	//	float rowDistance = Player::screen_height / (2.0f * (y - middleY));

	//	// Calculer les étapes dans les directions X et Y du sol
	//	float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / Player::screen_width;
	//	float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / Player::screen_width;

	//	// Position initiale sur le sol (gauche de la vue)
	//	float floorX = playerX + rowDistance * rayDirX0;
	//	float floorY = playerY + rowDistance * rayDirY0;

	//	// Précharger les dimensions de l'écran
	//	const int screenWidth = Player::screen_width;

	//	// Itérer sur chaque colonne de la ligne
	//	for (int x = 0; x < screenWidth; ++x) {
	//		// Obtenir la tuile correspondante
	//		int mapX = static_cast<int>(floorX) % mapWidth();
	//		int mapY = static_cast<int>(floorY) % mapHeight();

	//		// Récupérer la texture de la tuile correspondante
	//		BitmapRef floorTexture = mapTexture(mapX, mapY);

	//		// Si une texture est présente pour cette tuile
	//		if (floorTexture) {
	//			// Calculer la position dans la texture de la tuile
	//			int texX = static_cast<int>((floorX - static_cast<int>(floorX)) * floorTexture->width());
	//			int texY = static_cast<int>((floorY - static_cast<int>(floorY)) * floorTexture->height());

	//			// Accélérer en ne limitant texX et texY qu'une fois par pixel
	//			texX = texX >= floorTexture->width() ? floorTexture->width() - 1 : texX;
	//			texY = texY >= floorTexture->height() ? floorTexture->height() - 1 : texY;

	//			// Dessiner la portion correspondante de la texture sur l'écran
	//			Rect srcRect = { texX, texY, 1, 1 };
	//			Rect destRect = { x, y, 1, 1 };
	//			sprite->StretchBlit(destRect, *floorTexture, srcRect, 255);
	//		}

	//		// Avancer les coordonnées sur le sol
	//		floorX += floorStepX;
	//		floorY += floorStepY;
	//	}
	//}

	// Test 5

	//
	//// Calcul du milieu de l'écran
	//int middleY = Player::screen_height / 2;

	//// Position du joueur dans le monde (en tuiles)
	//float playerX = player.x / TILE_SIZE;
	//float playerY = player.y / TILE_SIZE;

	//// Parcourir chaque ligne sous le milieu de l'écran (sol)
	//for (int y = middleY; y < Player::screen_height; y++) {
	//	// Calcul de la distance projetée (distance au sol pour cette ligne)
	//	float rowDistance = (Player::screen_height / (2.0f * (y - middleY)));

	//	// Pré-calculer les directions pour les rayons extrêmes du champ de vision
	//	float rayDirX0 = cos(player.angle - M_PI / 4);
	//	float rayDirY0 = sin(player.angle - M_PI / 4);
	//	float rayDirX1 = cos(player.angle + M_PI / 4);
	//	float rayDirY1 = sin(player.angle + M_PI / 4);

	//	// Calcul des étapes pour avancer dans les coordonnées du sol
	//	float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / Player::screen_width;
	//	float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / Player::screen_width;

	//	// Position initiale du sol pour la ligne (rayon gauche)
	//	float floorX = playerX + rowDistance * rayDirX0;
	//	float floorY = playerY + rowDistance * rayDirY0;

	//	// Parcourir chaque pixel de la ligne
	//	for (int x = 0; x < Player::screen_width; x++) {
	//		// Convertir les coordonnées du sol en indices de tuiles (fixer la texture à la tuile)
	//		int mapX = static_cast<int>(floorX) % mapWidth();  // Modulo pour rester dans les limites de la carte
	//		int mapY = static_cast<int>(floorY) % mapHeight();

	//		// Récupérer la texture de la tuile correspondante
	//		BitmapRef floorTexture = bitmap;// mapTexture(mapX, mapY);

	//		// Si une texture est trouvée pour cette tuile
	//		if (floorTexture) {
	//			// Calculer la position dans la texture de la tuile
	//			int texX = static_cast<int>((floorX - floor(floorX)) * floorTexture->width());
	//			int texY = static_cast<int>((floorY - floor(floorY)) * floorTexture->height());

	//			// Limiter texX et texY pour rester dans les bornes de la texture
	//			texX = std::min(texX, floorTexture->width() - 1);
	//			texY = std::min(texY, floorTexture->height() - 1);

	//			// Dessiner la portion correspondante de la texture sur l'écran
	//			Rect srcRect = { texX, texY, 1, 1 };
	//			Rect destRect = { x, y, 1, 1 };
	//			sprite->StretchBlit(destRect, *floorTexture, srcRect, 255);
	//		}

	//		// Avancer dans les coordonnées du sol pour le prochain pixel
	//		floorX += floorStepX;
	//		floorY += floorStepY;
	//	}
	//}

	//return;

	// Test 6

	//int middleY = Player::screen_height / 2;

	//// Pré-calculer les directions du champ de vision
	//float rayDirX0 = cos(player.angle - M_PI / 4);  // Rayon gauche du champ de vision
	//float rayDirY0 = sin(player.angle - M_PI / 4);
	//float rayDirX1 = cos(player.angle + M_PI / 4);  // Rayon droit du champ de vision
	//float rayDirY1 = sin(player.angle + M_PI / 4);

	//// Pour chaque ligne horizontale sous le milieu de l'écran (sol)
	//for (int y = middleY; y < Player::screen_height; y++) {
	//	// Distance au plan de projection (sol) pour cette ligne
	//	float rowDistance = (0.5 * Player::screen_height) / (y - middleY);

	//	// Calcul des coordonnées du sol en fonction de la distance (interpolation des rayons)
	//	float floorX = player.x / TILE_SIZE + rowDistance * rayDirX0;
	//	float floorY = player.y / TILE_SIZE + rowDistance * rayDirY0;

	//	// Calcule la progression sur chaque pixel horizontal de la ligne
	//	float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / Player::screen_width;
	//	float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / Player::screen_width;

	//	// On parcourt chaque pixel de la ligne
	//	for (int x = 0; x < Player::screen_width; x++) {
	//		// Récupérer la position dans la carte
	//		int cellX = static_cast<int>(floorX) % TILE_SIZE;
	//		int cellY = static_cast<int>(floorY) % TILE_SIZE;

	//		// Récupérer la texture pour cette tuile de sol
	//		//BitmapRef floorTexture = mapTexture(static_cast<int>(floorX), static_cast<int>(floorY));
	//		BitmapRef floorTexture = mapTexture(0, static_cast<int>(floorY));

	//		// Si une texture existe pour cette tuile
	//		if (floorTexture) {
	//			// Calculer les coordonnées de la texture
	//			int texX = static_cast<int>((floorX - int(floorX)) * floorTexture->width());
	//			int texY = static_cast<int>((floorY - int(floorY)) * floorTexture->height());

	//			// Dessiner un pixel à la bonne position dans l'écran
	//			Rect srcRect = { texX, texY, 1, 1 };
	//			Rect destRect = { x, y, 1, 1 };
	//			sprite->StretchBlit(destRect, *floorTexture, srcRect, 255);
	//		}

	//		// Avancer dans les coordonnées du sol (en fonction de la distance)
	//		floorX += floorStepX;
	//		floorY += floorStepY;
	//	}
	//}

	//return;

	// Test 7

	//for (int x = 0; x < Player::screen_width; x++) {
	//	// Calculer l'angle du rayon
	//	float rayAngle = player.angle + atan2(x - Player::screen_width / 2, Player::screen_width / 2);

	//	int ray = 0;

	//	// Cast le rayon et obtenir la distance au mur
	//	float distance = castRay(rayAngle, ray);

	//	// Si la distance est valide
	//	if (distance > 0) {
	//		float floorDistance = (Player::screen_height / 2) / tan(player.fov / 2);

	//		// On utilise rayDistance pour déterminer la distance du sol
	//		float actualFloorDistance = distance * cos(rayAngle - player.angle); // Correction avec l'angle du rayon

	//		// Calculer la position de la texture sur le sol
	//		float floorX = player.x + actualFloorDistance * cos(rayAngle);
	//		float floorY = player.y + actualFloorDistance * sin(rayAngle);

	//		// Pour récupérer la position de la texture correcte
	//		// Utiliser la position du joueur pour calculer textureX
	//		int mapX = static_cast<int>(floorX / TILE_SIZE);
	//		int mapY = static_cast<int>(floorY / TILE_SIZE);
	//		float wallX = fmod(floorX, TILE_SIZE); // Position horizontale dans la tuile


	//		// Dessiner le sol en utilisant la texture
	//		for (int y = Player::screen_height / 2; y < Player::screen_height; y++) {

	//			//drawTexture(image, x, y, 1, 1, floorTexture, textureX, textureY);


	//			Rect r = Rect(x, y, 1, 1);
	//			BitmapRef texture = mapTexture(0, 0);

	//			if (texture) {

	//				int textureWidth = texture->width();

	//				// Calculer textureX
	//				int textureX = static_cast<int>((wallX / TILE_SIZE) * textureWidth);
	//				int textureY = static_cast<int>(((y - Player::screen_height / 2) / (Player::screen_height / 2)) * texture->height());

	//				// Assurez-vous que textureY est dans les limites de la texture
	//				textureY = std::min(textureY, texture->height() - 1);

	//				Rect srcRect = { textureX, textureY, 1, 1 };
	//				sprite->StretchBlit(r, *texture, srcRect, 255);
	//			}
	//		}
	//	}
	//}

}


Spriteset_MapDoom::Spriteset_MapDoom() {


	doomMap = true;

	mapW = Game_Map::GetTilesX();
	mapH = Game_Map::GetTilesY();

	Output::Debug("Map size {} {}", mapWidth(), mapHeight());

	auto m = Game_Map::GetPassagesDown();

	for (int x = 0;x < mapWidth(); x++) {
		for (int y = 0; y < mapHeight(); y++) {
			map[y][x] = Game_Map::GetTerrainTag(x, y) - 1;
		}
	}

	player.x = Main_Data::game_player->GetX() * TILE_SIZE + TILE_SIZE / 2;
	player.y = Main_Data::game_player->GetY() * TILE_SIZE + TILE_SIZE / 2;
	player.angle = (Main_Data::game_player->GetDirection() * 90 - 90) * M_PI / 180;

	sprite = Bitmap::Create(Player::screen_width, Player::screen_height);
	spriteUpper = Bitmap::Create(Player::screen_width, Player::screen_height);

	tilemap = std::make_unique<Tilemap>();
	tilemap->SetWidth(Game_Map::GetTilesX());
	tilemap->SetHeight(Game_Map::GetTilesY());

	FileRequestAsync* request = AsyncHandler::RequestFile("Pictures", "floor");
	request->SetGraphicFile(true);
	request_id = request->Bind(&Spriteset_MapDoom::OnTitleSpriteReady, this, 0);
	request->Start();


	request = AsyncHandler::RequestFile("Pictures", "wall");
	request->SetGraphicFile(true);
	request_id = request->Bind(&Spriteset_MapDoom::OnTitleSpriteReady, this, 1);
	request->Start();


	Scene_Map* scene = (Scene_Map*)Scene::Find(Scene::Map).get();
	chipset = scene->GetChipset();

}

void Spriteset_MapDoom::OnTitleSpriteReady(FileRequestResult* result, int i) {
	BitmapRef bitmapRef = Cache::Picture(result->file, false);

	if (i == 0)
		bitmap = bitmapRef;
	else
		bitmap2 = bitmapRef;

	Update(true);
}

Spriteset_MapDoom::Spriteset_MapDoom(std::string n, int zoom, int dx, int dy, int rx, int ry, int rz) {

	Load_OBJ(n);

	displayX = dx;
	displayY = dy;
	rotationX = rx;
	rotationY = ry;
	rotationZ = rz;

	angleX = 0;
	angleY = 0;
	angleZ = 0;

	for (auto& p : points3D) {
		p.x *= zoom;
		p.y *= zoom;
		p.z *= zoom;
	}

	centeroid = { 0,0,0 };
	for (auto& p : points3D) {
		centeroid.x += p.x;
		centeroid.y += p.y;
		centeroid.z += p.z;
	}
	centeroid.x /= points3D.size();
	centeroid.y /= points3D.size();
	centeroid.z /= points3D.size();

	sprite = Bitmap::Create(Player::screen_width, Player::screen_height);
	spriteUpper = Bitmap::Create(Player::screen_width, Player::screen_height);

	Update(true);

	rotationX = 0;
	rotationY = 0;
	rotationZ = 0;

}

void Spriteset_MapDoom::setRotation(int rx, int ry, int rz) {
	rotationX = rx;
	rotationY = ry;
	rotationZ = rz;
}

void Spriteset_MapDoom::getRotation(int varX, int varY, int varZ) {

	//int x = (int)(angleX * 180 / M_PI / 1000) % 360;
	//if (x < 0)
	//	x = 360 + x;

	//int y = (int)(angleY * 180 / M_PI / 1000) % 360;
	//if (y < 0)
	//	y = 360 + y;

	//int z = (int)(angleZ * 180 / M_PI / 1000) % 360;
	//if (z < 0)
	//	z = 360 + z;

	int x = (int)(angleX * 180 / M_PI) % 360;
	int y = (int)(angleY * 180 / M_PI) % 360;
	int z = (int)(angleZ * 180 / M_PI) % 360;

	// S'assurer que les angles sont positifs
	if (x < 0) x = 360 + x;
	if (y < 0) y = 360 + y;
	if (z < 0) z = 360 + z;


	auto p = points3D[points3D.size() - 1];
	x = p.x;
	y = p.y;
	z = p.z;

	Main_Data::game_variables->Set(varX, x);
	Main_Data::game_variables->Set(varY, y);
	Main_Data::game_variables->Set(varZ, z);

	//Output::Debug("A {} {} {}", x, y, z);
}


std::vector<std::string> Spriteset_MapDoom::split(const std::string& s, const std::string& delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

void Spriteset_MapDoom::Load_OBJ(std::string name) {

	Output::Debug("Load_OBJ");

	points3D = {};
	connections3D = {};

	surfaces = {};

	Output::Debug(".mtl");

	std::string s = "";
	std::string ini_file = FileFinder::Game().FindFile("Models/" + name + ".mtl");
	auto ini_stream = FileFinder::Game().OpenInputStream(ini_file, std::ios_base::in);

	std::map<std::string, Color> colors;
	std::string material_name = "";

	for (std::string line; getline(ini_stream, line); )
	{
		// Output::Debug("{}", line);
		if (line._Starts_with("newmtl ")) {
			material_name = line.substr(7,line.length() - 7);
		} else if (line._Starts_with("Kd ")) {
			std::vector<std::string> v = split(line, " ");
			int r = std::stof(v[1]) * 255;
			int g = std::stof(v[2]) * 255;
			int b = std::stof(v[3]) * 255;
			colors[material_name] = Color(r, g, b, 255);

			Output::Debug(" {} {} {} {}", material_name, r, g, b);
		}
	}

	Output::Debug(".obj");

	std::vector <Point> pointsNoMaterial;
	s = "";
	ini_file = FileFinder::Game().FindFile("Models/" + name + ".obj");
	ini_stream = FileFinder::Game().OpenInputStream(ini_file, std::ios_base::in);
	Color color;

	for (std::string line; getline(ini_stream, line); )
	{
		// Output::Debug("{}", line);
		if (line._Starts_with("v ")) {
			std::vector<std::string> v = split(line, " ");
			float x = std::stof(v[1]);
			float y = std::stof(v[2]);
			float z = std::stof(v[3]);

			Point v3 = { x,y,z, false };
			points3D.push_back(v3);
			/*pointsNoMaterial.push_back(v3);*/
		}
		// Material
		else if (line._Starts_with("usemtl ")) {
			material_name = line.substr(7, line.length() - 7);
			Output::Debug("{}", material_name);
			color = colors[material_name];

			Output::Debug(" {} {} {}", color.red, color.green, color.blue);

			//for (auto p : pointsNoMaterial) {
			//	p.color = color;
			//	points3D.push_back(p);
			//}
			//pointsNoMaterial.clear();

		}
		// Surface
		else if (line._Starts_with("f ")) {
			std::vector<std::string> v = split(line, " ");
			std::vector<Point> s;
			Point p;
			p.color = color;
			for (int i = 1; i < v.size(); i++) {
				auto vc = v[i];
				std::vector<std::string> f = split(vc, "/");

				if (i == 1) {
					p.y = std::stof(f[0]) - 1;
				}
				else if (i == v.size() - 1) {
					p.x = p.y;
					p.y = std::stof(f[0]) - 1;
					s.push_back(p);
					p.x = p.y;
					p.y = s[0].x;
					s.push_back(p);
				}
				else {
					p.x = p.y;
					p.y = std::stof(f[0]) - 1;
					s.push_back(p);
				}

			}

			connection c;
			int cc;
			for (int i = 1; i < v.size(); i++) {
				auto vc = v[i];
				std::vector<std::string> f = split(vc, "/");
				if (i == 1) {
					c.a = std::stoi(f[0]) - 1;
					cc = c.a;
				}
				else {
					c.b = std::stoi(f[0]) - 1;
					connections3D.push_back(c);
					c.a = c.b;
					if (i == v.size() - 1) {
						c.b = cc;
						connections3D.push_back(c);
					}
				}
			}

			surfaces.push_back(s);
			
		}
		 // => Wireframe
		/*else if (line._Starts_with("f ")) {
			std::vector<std::string> v = split(line, " ");
			connection c;
			int cc;
			for (int i = 1; i < v.size(); i++) {
				auto vc = v[i];
				std::vector<std::string> f = split(vc, "/");
				if (i == 1) {
					c.a = std::stoi(f[0]) - 1;
					cc = c.a;
				}
				else {
					c.b = std::stoi(f[0]) - 1;
					connections3D.push_back(c);
					c.a = c.b;
					if (i == v.size() - 1) {
						c.b = cc;
						connections3D.push_back(c);
					}
				}
			}
		}*/
	}

	points3D.push_back({ 0,0,0 });

	//for (auto p : pointsNoMaterial) {
	//	p.color = color;
	//	points3D.push_back(p);
	//}
	//pointsNoMaterial.clear();
}

void Spriteset_MapDoom::Update(bool first) {
	// Output::Debug("Update");
	if (Input::IsRawKeyTriggered(Input::Keys::F3)) {
		refresh_index = (refresh_index + 1) % 6;
		Output::Info("Refresh rate : {} fps", 60 / refresh[refresh_index]);
	}
	int refresh_rate = refresh[refresh_index];

	if (doomMap) {
		int s = 1;
		float angle = 0.02;

		if (Input::IsPressed(Input::DIVIDE)) {
			player.FOVangle -= 1;
			player.fov = player.FOVangle * (M_PI / 180.0f);
		}
		if (Input::IsPressed(Input::MULTIPLY)) {
			player.FOVangle += 1;
			player.fov = player.FOVangle * (M_PI / 180.0f);
		}


		if (Main_Data::game_player->canMove) {
			if (Main_Data::game_player->doomMoveType == 0) { // Tile Movement

				if (Main_Data::game_player->doomWait != 0) {
					static const int move_speed[] = { 64, 32, 24, 16, 12, 8 };

					float x = Main_Data::game_player->GetX() * TILE_SIZE + TILE_SIZE / 2;
					float y = Main_Data::game_player->GetY() * TILE_SIZE + TILE_SIZE / 2;

					if (x != player.x || y != player.y) {

						float dx = (x - player.x) / Main_Data::game_player->doomWait;
						float dy = (y - player.y) / Main_Data::game_player->doomWait;

						if (dx != 0)
							player.x += dx;
						if (dy != 0)
							player.y += dy;

					}

					float a = (Main_Data::game_player->GetDirection() * 90 - 90) * M_PI / 180;
					if (player.angle < 0 && a > 0)
						player.angle += 2 * M_PI;

					if (player.angle > 0 && a < 0) {
						player.angle -= 2 * M_PI;
					}

					angle = abs(a - player.angle) / Main_Data::game_player->doomWait;


					if (a != player.angle) {
						if (a < player.angle)
							player.angle -= angle;
						if (a > player.angle)
							player.angle += angle;
						if (Main_Data::game_player->doomWait == 0)
							player.angle = a;
					}
					// Output::Debug("l {} {} {}", player.angle, a, angle);
				}

			}
			else { // Pixel Movement
				if (Input::IsPressed(Input::UP)) {  // Avancer
					float px = player.x + cos(player.angle) * s * TILE_SIZE;
					float py = player.y + sin(player.angle) * s * TILE_SIZE;

					int ppx = px / TILE_SIZE;
					int ppy = py / TILE_SIZE;

					//Output::Debug(" {} {} {}", ppx, ppy, map[ppx][ppy]);

					if (ppx < mapWidth() && ppy < mapHeight() && ppx >= 0 && ppy >= 0) {
						int x = player.x / TILE_SIZE;
						int y = player.y / TILE_SIZE;

						int dx = ppx;
						int dy = ppy;

						//Output::Debug(" {} {} {} {} {} {}", x, y, ppx, ppy, Main_Data::game_player->MakeWay(x, y, dx, y) , Main_Data::game_player->MakeWay(x, y, x, dy));

						if (Main_Data::game_player->MakeWay(x, y, dx, dy) || (x == dx && y == dy)) {
							player.x = player.x + cos(player.angle) * s;
							player.y = player.y + sin(player.angle) * s;

							Main_Data::game_player->SetX(player.x / TILE_SIZE);
							Main_Data::game_player->SetY(player.y / TILE_SIZE);
						}
						else {
							if (Main_Data::game_player->MakeWay(x, y, dx, y)) {
								player.x = player.x + cos(player.angle) * s;

								Main_Data::game_player->SetX(player.x / TILE_SIZE);
								Main_Data::game_player->SetY(player.y / TILE_SIZE);
							}
							else if (Main_Data::game_player->MakeWay(x, y, x, dy)) {
								player.y = player.y + sin(player.angle) * s;

								Main_Data::game_player->SetX(player.x / TILE_SIZE);
								Main_Data::game_player->SetY(player.y / TILE_SIZE);
							}
						}


						int front_x = Game_Map::XwithDirection(Main_Data::game_player->GetX(), Main_Data::game_player->GetDirection());
						int front_y = Game_Map::YwithDirection(Main_Data::game_player->GetY(), Main_Data::game_player->GetDirection());

						Output::Debug("XY {} {}", front_x, front_y);

						bool action = Main_Data::game_player->CheckEventTriggerThere({ lcf::rpg::EventPage::Trigger_touched, lcf::rpg::EventPage::Trigger_collision }, front_x, front_y, false);

						Output::Debug("Action {}", action);

					}
				}
				if (Input::IsPressed(Input::DOWN)) {  // Reculer
					float px = player.x - cos(player.angle) * s;
					float py = player.y - sin(player.angle) * s;

					int ppx = px / TILE_SIZE;
					int ppy = py / TILE_SIZE;

					//Output::Debug(" {} {} {}", ppx, ppy, map[ppx][ppy]);

					if (ppx < mapWidth() && ppy < mapHeight() && ppx >= 0 && ppy >= 0)
						if (map[ppy][ppx] == 0) {
							player.x = px;
							player.y = py;
						}
				}
				if (Input::IsPressed(Input::LEFT)) {  // Tourner à gauche
					player.angle -= angle;
					if (player.angle < 0)
						player.angle += 2 * M_PI;
					if (player.angle > 2 * M_PI)
						player.angle -= 2 * M_PI;

					float a = (player.angle) * 180.0f / M_PI + 45;
					if (a < 0)
						a += 360;
					if (a > 360)
						a -= 360;
					int dir = ((int)(a / 90) + 1) % 4;

					Main_Data::game_player->SetFacing(dir);
					Main_Data::game_player->SetDirection(dir);
				}
				if (Input::IsPressed(Input::RIGHT)) {  // Tourner à droite
					player.angle += angle;
					if (player.angle < 0)
						player.angle += 2 * M_PI;
					if (player.angle > 2 * M_PI)
						player.angle -= 2 * M_PI;

					float a = (player.angle) * 180.0f / M_PI + 45;
					if (a < 0)
						a += 360;
					if (a > 360)
						a -= 360;
					int dir = ((int)(a / 90) + 1) % 4;

					Main_Data::game_player->SetFacing(dir);
					Main_Data::game_player->SetDirection(dir);
				}
			}
		}

		if (timer % refresh_rate == 1 || refresh_rate == 1 || first) {
			sprite->Clear();
			renderScene();
		}
		points.clear();


		timer++;

		return;
	}

	if (rotationX != 0 || rotationY != 0 || rotationZ != 0 || first) {


		angleX += rotationX / 1000.0;
		angleY += rotationY / 1000.0;
		angleZ += rotationZ / 1000.0;

		normalizeAngle(angleX);
		normalizeAngle(angleY);
		normalizeAngle(angleZ);

		for (auto& p : points3D) {

			p.x -= centeroid.x;
			p.y -= centeroid.y;
			p.z -= centeroid.z;
			// rotate(p, rotationX / 1000.0, rotationY / 1000.0, rotationZ / 1000.0);
			rotate(p, rotationX / 1000.0, 0, 0);
			rotate(p, 0, rotationY / 1000.0, 0);
			rotate(p, 0, 0, rotationZ / 1000.0);
			p.x += centeroid.x;
			p.y += centeroid.y;
			p.z += centeroid.z;

			//if (timer % refresh_rate == 1 || refresh_rate == 1)
				//pixel(p.x, p.y, p.z, p.color);
		}

		if (timer % refresh_rate == 1 || refresh_rate == 1 || first) {

			for (auto& c : connections3D) {
				// line(points3D[c.a], points3D[c.b]);
			}
			for (auto s : surfaces) {

				std::vector<Point> s2;
				for (auto p : s) {
					Point p2 = { points3D[p.x].x, points3D[p.x].y, points3D[p.x].z, points3D[p.x].upper, p.color };
					s2.push_back(p2);

					p2 = { points3D[p.y].x, points3D[p.y].y, points3D[p.y].z, points3D[p.y].upper, p.color };
					s2.push_back(p2);

					// Output::Debug(" {} {} {}", p2.color.red, p2.color.green , p2.color.blue);
				}
				sortPoints(s2);
				drawPolygon(s2);

			}

			Show();

		}
		timer++;
		points.clear();
	}
}

// Normalisation des angles pour rester entre 0 et 360
void Spriteset_MapDoom::normalizeAngle(double& angle) {
	angle = fmod(angle, 2 * M_PI);  // Modulo 2π pour rester dans l'intervalle [0, 2π]
	if (angle < 0) {
		angle += 2 * M_PI;  // Si négatif, on ajoute 2π pour ramener l'angle dans l'intervalle [0, 2π]
	}
}


void Spriteset_MapDoom::pixel(float x, float y, float z, Color c) {

	Point p = { x, y, z, false, c, true};
	if (z < 0) {
		p.upper = true;
	}

	points.emplace_back(p);
}

void Spriteset_MapDoom::line(Point p1, Point p2) {

	float x1, y1, x2, y2, z1, z2;
	x1 = p1.x;
	y1 = p1.y;
	x2 = p2.x;
	y2 = p2.y;
	z1 = p1.z;
	z2 = p2.z;

	float dx = x2 - x1;
	float dy = y2 - y1;
	float length = std::sqrt(dx * dx + dy * dy);
	float angle = std::atan2(dy, dx);

	//Output::Debug(". {}", length);

	for (float i = 0; i < length; i++) {
		float zz = z1 + std::tanf(angle) * i;

		float f = i / length;
		zz = lerp(z1, z2, f);

		pixel(x1 + std::cosf(angle) * i, y1 + std::sin(angle) * i, zz, p1.color);
		// Output::Debug("..");
	}
}

// Fonction pour calculer le centroïde du polygone
Spriteset_MapDoom::Point Spriteset_MapDoom::computeCentroid(const std::vector<Spriteset_MapDoom::Point>& points) {
	int x_sum = 0, y_sum = 0;
	for (const auto& p : points) {
		x_sum += p.x;
		y_sum += p.y;
	}
	Point p;
	p.x = x_sum / static_cast<int>(points.size());
	p.y = y_sum / static_cast<int>(points.size());
	return p;
}

// Fonction pour trier les points en fonction de leur angle par rapport au centroïde
void Spriteset_MapDoom::sortPoints(std::vector<Spriteset_MapDoom::Point>& points) {
	Point centroid = computeCentroid(points);
	std::sort(points.begin(), points.end(), [centroid](const Point& a, const Point& b) {
		return std::atan2(a.y - centroid.y, a.x - centroid.x) < std::atan2(b.y - centroid.y, b.x - centroid.x);
		});
}

void Spriteset_MapDoom::drawPolygon(std::vector<Spriteset_MapDoom::Point> vertices) {

	if (vertices.size() < 3) return;  // Pas un polygone valide

	Color color = Color(255, 255, 0, 255);

	color = vertices[0].color;

	int z = 0;
	float z_sum = 0.0;

	// Additionner toutes les valeurs de Z
	for (const auto& point : vertices) {
		z_sum += point.z;
	}

	// Calculer la moyenne
	z = z_sum / vertices.size();

	// Trouver les bornes verticales (y_min et y_max)
	int y_min = vertices[0].y;
	int y_max = vertices[0].y;
	for (const auto& p : vertices) {
		y_min = std::min(y_min, (int) p.y);
		y_max = std::max(y_max, (int) p.y);
	}

	// Pour chaque ligne horizontale (scanline), déterminer les vertices d'intersection
	for (int y = y_min; y <= y_max; ++y) {
		std::vector<int> intersections;

		// Trouver les vertices d'intersection entre la ligne de balayage et les arêtes du polygone
		for (size_t i = 0; i < vertices.size(); ++i) {
			Point p1 = vertices[i];
			Point p2 = vertices[(i + 1) % vertices.size()];  // Cycle pour connecter le dernier point au premier

			// Vérifier si l'arête coupe la ligne de balayage
			if ((p1.y <= y && p2.y > y) || (p2.y <= y && p1.y > y)) {
				// Calculer le point d'intersection en x par interpolation linéaire
				int x = p1.x + (y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
				intersections.push_back(x);
			}
		}

		// Trier les intersections pour dessiner les segments horizontaux
		std::sort(intersections.begin(), intersections.end());

		// Remplir les pixels entre chaque paire de vertices d'intersection
		for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
			int x_start = intersections[i];
			int x_end = intersections[i + 1];
			// TODO Remplacer pr la fonction line ?
			for (int x = x_start; x <= x_end; ++x) {
				pixel(x, y, z, color);  // Dessiner les pixels dans l'intervalle
			}
		}
	}
}


float Spriteset_MapDoom::lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

//void Spriteset_MapDoom::rotate(Point& point, float x, float y, float z) {
//
//	float rad = 0;
//
//	rad = x;
//	point.y = std::cos(rad) * point.y - std::sin(rad) * point.z;
//	point.z = std::sin(rad) * point.y + std::cos(rad) * point.z;
//
//	rad = y;
//	point.x = std::cos(rad) * point.x + std::sin(rad) * point.z;
//	point.z = -std::sin(rad) * point.x + std::cos(rad) * point.z;
//
//	rad = z;
//	point.x = std::cos(rad) * point.x - std::sin(rad) * point.y;
//	point.y = std::sin(rad) * point.x + std::cos(rad) * point.y;
//
//}

void Spriteset_MapDoom::rotate(Point& point, float x, float y, float z) {
	float rad;

	// Variables temporaires pour stocker les nouvelles valeurs
	float new_x, new_y, new_z;

	// Rotation autour de l'axe X
	rad = x;
	new_y = std::cos(rad) * point.y - std::sin(rad) * point.z;
	new_z = std::sin(rad) * point.y + std::cos(rad) * point.z;

	point.y = new_y;
	point.z = new_z;

	// Rotation autour de l'axe Y
	rad = y;
	new_x = std::cos(rad) * point.x + std::sin(rad) * point.z;
	new_z = -std::sin(rad) * point.x + std::cos(rad) * point.z;

	point.x = new_x;
	point.z = new_z;

	// Rotation autour de l'axe Z
	rad = z;
	new_x = std::cos(rad) * point.x - std::sin(rad) * point.y;
	new_y = std::sin(rad) * point.x + std::cos(rad) * point.y;

	point.x = new_x;
	point.y = new_y;
}


bool Spriteset_MapDoom::comparePoints(const Point& p1, const Point& p2) {
	return p1.z < p2.z;
}

void Spriteset_MapDoom::Show() {

	//Output::Debug("Start render");

	sprite->Clear();
	spriteUpper->Clear();

	float zmin = 0;
	float zmax = 0;
	bool f = true;
	for (const auto& p : points) {
		//Output::Debug(" {}", p.z);
		if (f) {
			zmax = p.z;
			f = false;
		}
		else {
			zmin = std::max(zmin, p.z);
			zmax = std::min(zmax, p.z);
		}
	}

	std::sort(points.begin(), points.end(), std::greater<Point>());

	bool pointsZB[999][999] = { false };

	Rect r;
	Color c = Color(255,0,0,255);
	for (auto p : points) {

		//Output::Debug("AZE");

		int i = p.x + 999 / 2;
		int j = p.y + 999 / 2;

		if (p.exist && !pointsZB[i][j])
		{

			pointsZB[i][j] = true;

			r = Rect(p.x + displayX + Player::screen_width / 2, Player::screen_height - (p.y + displayY + Player::screen_height / 2), 1, 1);

			int mult = ((p.z - zmin) / (zmax - zmin)) * 100;

			int red, green, blue;
			red = p.color.red;
			green = p.color.green;
			blue = p.color.blue;

			// Output::Debug(" {} {} {}", red, green, blue);

			c = Color(red * mult / 100, green * mult / 100, blue * mult / 100, 255);

			sprite->FillRect(r, c);

			if (p.upper) {
				c = Color(255, 0, 0, 255);
				spriteUpper->FillRect(r, c);
			}
		}
	}


	//Output::Debug("Rendered");
}