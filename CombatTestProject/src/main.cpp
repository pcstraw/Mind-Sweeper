
#include "Engine.h"
#include "TestCombatGame.h"

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);
	Native::TestCombatGame game;
	Native::Engine engine = Native::Engine(&game);
	engine.Run();
    return 0;
}

