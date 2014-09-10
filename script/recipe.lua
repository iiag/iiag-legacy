-----------------------
-- script/recipe.lua --
-----------------------

-- ingots --
recipe {
	class = 'ingot';
	made_by = 'smelter';
	char = '-';
	weight = 2500;
	modify_ac = 1;
	modify_attack = 1;


}

comp {
	mat = '1';
	class = 'ore';
	modify_ac = ' f * t';
	modify_attack = 'f * t';
}

-- links --
recipe {
	class = 'links';
	made_by = 'forge';
	made_with = 2;
	char = '8';
	weight = 2500;
	modify_ac = 1;
	modify_attack = 0;


}

comp {
	mat = '1';
	class = 'ingot';
	modify_ac = 't';
}

-- shaft --
recipe {
	class = 'shaft';
	made_by = 'forge';
	made_with = 4;
	char = '|';
	weight = 2500;
	modify_attack = 1;
	equipable = true;
	slot = 'right-hand';

}

comp {
	mat = '1';
	class = 'ingot';
	modify_attack = 'f + t';
}

-- plate --
recipe {
	class = 'plate';
	made_by = 'forge';
	made_with = 6;
	char = '#';
	weight = 2500;
	modify_ac = 1;

}

comp {
	mat = '1';
	class = 'ingot';
	modify_ac = 'f + t';
}

-- blade --
recipe {
	class = 'blade';
	made_by = 'forge';
	made_with = 8;
	char = '|';
	weight = 2500;
	modify_attack = 1;

}

comp {
	mat = '1';
	class = 'ingot';
	modify_attack = 'f + t';
}

-- coif --
recipe {
	class = 'coif';
	made_by = 'forge';
	char = '^';
	equipable = true;
	weight = 1450;
	modify_ac = 2;
	slot = 'head';
}

comp {
	mat = '1';
	class = 'links';
	modify_attack = 'f + t';
}

-- hauberk --
recipe {
	class = 'hauberk';
	made_by = 'forge';
	char = ']';
	weight = 3000;
	modify_ac = 4;
	slot = 'torso';
}

comp {
	mat = '1';
	class = 'links';
	modify_attack = 'f + t';
}

comp {
	class = 'links';
	modify_attack = 'f + t';
}

-- hauberk --
recipe {
	class = 'mace';
	made_by = 'forge';
	equipable = true;
	char = '/';
	weight = 3000;
	modify_attack = 7;
	slot = 'right-hand';
}

comp {
	mat = '1';
	class = 'ingot';
	modify_attack = 't * 2 + f';
}

comp {
	class = 'shaft';
	modify_attack = 'f + t';
}
