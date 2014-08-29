-----------------------
--  script/init.lua  --
-----------------------

dofile "script/cforms.lua"
dofile "script/iforms.lua"
dofile "script/mats.lua"

player {
	name = "you";
	max_health = 10;
	attack = 3;
	sight = 30;
	on_spawn = function ()
		memo("Hello from Lua!")
	end
}
