#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <cstdint>

namespace j_n
{
	// MATHS ==================================================================
	template<typename T>
	struct Vector2D
	{
		T x, y;
	};

	using Vector2i = Vector2D         <int>;
	using Vector2f = Vector2D       <float>;
	using Vector2u = Vector2D<unsigned int>;
	using Vector2d = Vector2D      <double>;

	template<typename T>
	T Clamp(T value, T min, T max)
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	// SOME OTHER STUFF =======================================================
	class Level
	{
	private:
		uint16_t* m_level;

		int m_width, m_height;

		void create()
		{
			m_level = new uint16_t[m_width * m_height];
		}

	public:
		Level() :
			m_width(32), m_height(16)
		{
			create();
		}

		Level(int width, int height) :
			m_width(width), m_height(height)
		{
			create();
		}

		~Level() // using heap alocation so need to clean up after program finishes
		{
			delete[] m_level;
		}

	public:
		void setTile(int x, int y, uint16_t value)
		{
			if (x >= 0 && x < m_width && y >= 0 && y < m_height) m_level[y * m_width + x] = value;
		}

		uint16_t getTile(int x, int y)
		{
			if (x >= 0 && x < m_width && y >= 0 && y < m_height) return m_level[y * m_width + x];
			return UINT16_MAX;
		}

		char getTileToChar(int x, int y)
		{
			switch (getTile(x, y))
			{
			case 0:
				return '#';
				break;

			case 1:
				return '.';
				break;

			default:
				return ' ';
				break;
			}
		}

		void fromString(std::vector<std::string> string)
		{
			m_width  = string[0].size();
			m_height = string.size();
			create();

			int currentX = 0;
			int currentY = 0;
			for (auto str : string)
			{
				for (auto ch : str)
				{
					uint16_t tile;
					switch (ch)
					{
					case '#':
						tile = 0;
						break;

					case '.':
						tile = 1;
						break;

					default:
						tile = 0;
						break;
					}
					setTile(currentX, currentY, tile);
					currentX++;
				}
				currentY++;
			}
		}

		int getWidth()
		{
			return m_width;
		}

		int getHeight()
		{
			return m_height;
		}
	};

	// ACTUAL GAME CODE =======================================================
	class PlatformerGame : public olc::PixelGameEngine
	{
	private:
		Level level;
		Vector2f cameraPos = { 0.0f, 0.0f };
		Vector2f playerPos = { 0.0f, 11.0f };

	public:
		bool OnUserCreate() override
		{
			sAppName = "Platformer Game";
			level.fromString(
				{
					"..................................................................................................",
					"..................................................................................................",
					"..................................................................................................",
					"..................................................................................................",
					"..................................................................................................",
					"..................................................................................................",
					"..................#..............................#.#.......###################....................",
					".................................................#.#......##..................#...................",
					".........................................................###...................#..................",
					"........................................................####....................#.................",
					"......##.##............................................#####.....................#................",
					".......#.#............................................######......................#...............",
					"########.##################################.##########################################..##########",
					".......#.#................................#.#..........................................#..........",
					".......#.#................................#.#...........................###############...........",
					".......#.#.................################.#..........................#..........................",
					".......#.#.................#................#.........................#...........................",
					".......#.#.................#.################........................#............................",
					"...........................#.#......................................#.............................",
					"...................................................................#..............................",
					"##################################################################################################",
					"##################################################################################################",
				}
			);
			return true;
		}

		bool OnUserUpdate(float deltaTime) override
		{
			float playerMoveSpeed = 5.0f;

			if (GetKey(olc::RIGHT).bHeld)
			{
				playerPos.x += playerMoveSpeed * deltaTime;
			}
			if (GetKey(olc::LEFT).bHeld)
			{
				playerPos.x -= playerMoveSpeed * deltaTime;
			}
			if (GetKey(olc::UP).bHeld)
			{
				playerPos.y -= playerMoveSpeed * deltaTime;
			}
			if (GetKey(olc::DOWN).bHeld)
			{
				playerPos.y += playerMoveSpeed * deltaTime;
			}

			cameraPos = playerPos;

			Clear(olc::BLACK);

			Vector2i tileSize = { 16, 16 };
			Vector2i visibleTiles = { ScreenWidth() / tileSize.x, ScreenHeight() / tileSize.y };
			Vector2f offset = { cameraPos.x - (float)visibleTiles.x / 2.0f, cameraPos.y - (float)visibleTiles.y / 2.0f };

			offset.x = Clamp<int>(offset.x, 0, level.getWidth() - visibleTiles.x);
			offset.y = Clamp<int>(offset.y, 0, level.getHeight() - visibleTiles.y);

			for (int x = 0; x < visibleTiles.x; x++)
			{
				for (int y = 0; y < visibleTiles.y; y++)
				{
					char tile = level.getTileToChar(x + offset.x, y + offset.y);
					switch (tile)
					{
					case '.': // blank/air
						FillRect({ x * tileSize.x, y * tileSize.y }, { tileSize.x, tileSize.y }, olc::CYAN);
						break;

					case '#': // ground
						FillRect({ x * tileSize.x, y * tileSize.y }, { tileSize.x, tileSize.y }, olc::DARK_RED);
						break;

					default:
						FillRect({ x * tileSize.x, y * tileSize.y }, { tileSize.x, tileSize.y }, olc::CYAN);
						break;
					}
				}
			}

			return true;
		}
	};
}

int main()
{
	j_n::PlatformerGame game;
	if (game.Construct(256, 240, 4, 4)) game.Start();
	return 0;
}
