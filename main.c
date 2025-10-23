#include <raylib.h>
#include <raymath.h>
#include <stddef.h>
#include <time.h>
#include "cplus.h"

typedef struct {
	int kind;
	Vector2 pos;
} Point;

typedef DA(Point) Points;

Color get_color_by_kind(int kind) {
	switch (kind) {
		case 0: return RED;    break;
		case 1: return BLUE;   break;
		case 2: return GREEN;  break;
		case 3: return YELLOW; break;
		case 4: return PURPLE; break;
		case 5: return BROWN;  break;
	}

	return GRAY;
}

Point get_random_point(Points *ps) {
	return ps->items[rand()%ps->count];
}

void assign_means(Points *means, Points *ps) {
	da_foreach(Point, point, ps) {
		float min_d = 99999;
		int best_k;
		da_foreach(Point, mean, means) {
			float dist = Vector2Distance(point->pos, mean->pos);
			if (dist < min_d) {
				min_d = dist;
				best_k = mean->kind;
			}
		}

		point->kind = best_k;
	}
}

void update_means(Points *means, Points *ps) {
	da_foreach(Point, mean, means) {
		Vector2 new_mean = {0};
		size_t cnt = 0;
		da_foreach(Point, point, ps) {
			if (point->kind == mean->kind) {
				new_mean = Vector2Add(new_mean, point->pos);
				cnt++;
			}
		}

		mean->pos = Vector2Scale(new_mean, 1.0/cnt);
	}
}

int main() {
	const int k = 3;
	Points means = {0};
	Points ps = {0};
	da_resize(&means, k);

	InitWindow(800, 600, "k-means");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground((Color) {0x18, 0x18, 0x18, 0xFF});

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { // MOUSE_LEFT to place a point
			da_append(&ps, ((Point) {-1, GetMousePosition()}));
		} else if (IsKeyPressed(KEY_ENTER)) { // ENTER to init the means
			da_foreach(Point, mean, &means) {
				mean->kind = mean - means.items;
				mean->pos = get_random_point(&ps).pos;
			}
		} else if (IsKeyPressed(KEY_SPACE)) { // SPACE to perform an iteration
			assign_means(&means, &ps);
			update_means(&means, &ps);
		}

		da_foreach(Point, p, &ps) {
			DrawCircle(p->pos.x, p->pos.y, 5, get_color_by_kind(p->kind));
		}

		da_foreach(Point, m, &means) {
			Color clr = get_color_by_kind(m->kind);
			DrawRectangle(m->pos.x - 5, m->pos.y - 5, 10, 10, clr);
		}

		EndDrawing();
	}

	return 0;
}
