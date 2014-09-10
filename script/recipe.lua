-----------------------
-- script/recipe.lua --
-----------------------

-- ingots --
recipe {
	class = 'ingot';
	made_by = 'smelter';
	char = '-';

}

comp {
	mat = '1';
	class = 'ore';
	weight = 't';
	modify_ac = ' f + t';
	modify_attack = 'f + t';
}

-- slaging an item --
recipe {
	class = 'ore';
	made_by = 'smelter';
	mat = 'slag';
	char = '-';
	weight = 3000;
	modify_ac = 1;
	modify_attack = 1;


}

comp {
	class = 'slag';
}

-- links --
recipe {
	class = 'links';
	made_by = 'forge';
	made_with = 2;
	char = '8';
	modify_ac = 1;
	modify_attack = 0;


}

comp {
	mat = '1';
	class = 'ingot';
	weight = 't * 0.5';
	modify_ac = 't';
}

-- shaft --
recipe {
	class = 'shaft';
	made_by = 'forge';
	made_with = 4;
	char = '|';
	modify_attack = 1;
	equipable = true;
	slot = 'right-hand';

}

comp {
	mat = '1';
	class = 'ingot';
	weight = 't';
	modify_attack = 'f + t';
	modify_ac = 'f + t';
}

-- plate --
recipe {
	class = 'plate';
	made_by = 'forge';
	made_with = 6;
	char = '#';
	modify_ac = 1;

}

comp {
	mat = '1';
	class = 'ingot';
	weight = 't';
	modify_ac = 'f + t';
}

-- blade --
recipe {
	class = 'blade';
	made_by = 'forge';
	made_with = 8;
	char = '|';
	modify_attack = 1;

}

comp {
	mat = '1';
	class = 'ingot';
	weight = 't';
	modify_attack = 'f + t';
}

-- serrated blade --
recipe {
	class = 'serrated blade';
	made_by = 'forge';
	char = '?';
	modify_attack = 1;

}

comp {
	mat = '1';
	class = 'blade';
	weight = 't';
	modify_attack = 'f + t';
}

-- hilt --
recipe {
	class = 'hilt';
	made_by = 'forge';
	made_with = 5;
	char = 'T';

}

comp {
	mat = '1';
	class = 'ingot';
	weight = 't * 0.75';
}


---------------------------------
--         Here be wepons      --
---------------------------------

-- cross guard --
recipe {
	class = 'cross guard';
	made_by = 'forge';
	char = 'T';
	modify_ac = 1;
}

comp {
	mat = '1';
	class = 'hilt';
	modify_attack = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'shaft';
	modify_ac = 'f + t';
	weight = 'f + t';
}

-- sinclair hilt --
recipe {
	class = 'sinclair hilt';
	made_by = 'forge';
	char = 'T';
	modify_ac = 2;
}

comp {
	mat = '1';
	class = 'hilt';
	modify_attack = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'plate';
	modify_ac = 'f + t';
	weight = 'f + t';
}

-- short sword --
recipe {
	class = 'xiphos';
	made_by = 'forge';
	char = '/';
	modify_attack = 2;
	equipable = true;
	slot = 'right-hand';
}

comp {
	mat = '1';
	class = 'hilt';
	modify_attack = 'f + t';
	modify_ac     = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'blade';
	modify_attack = 'f + t';
	weight = 'f + t';
}

-- long sword --
recipe {
	class = 'spatha';
	made_by = 'forge';
	char = '/';
	modify_attack = 2;
	equipable = true;
	slot = 'right-hand';
}

comp {
	mat = '1';
	class = 'hilt';
	modify_attack = 'f + t';
	modify_ac     = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'blade';
	modify_attack = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'blade';
	modify_attack = 'f + t';
	weight = 'f + t';
}

-- paramerion --
recipe {
	class = 'paramerion';
	made_by = 'forge';
	char = '/';
	modify_attack = 2;
	equipable = true;
	slot = 'right-hand';
}

comp {
	mat = '1';
	class = 'hilt';
	modify_attack = 'f + t';
	modify_ac     = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'blade';
	modify_attack = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'blade';
	modify_attack = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'blade';
	modify_attack = 'f + t';
	weight = 'f + t';
}

-- estoc --
recipe {
	class = 'estoc';
	made_by = 'forge';
	char = '/';
	modify_attack = 2;
	equipable = true;
	slot = 'right-hand';
}

comp {
	mat = '1';
	class = 'hilt';
	modify_attack = 'f + t';
	modify_ac     = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'blade';
	modify_attack = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'blade';
	modify_attack = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'blade';
	modify_attack = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'blade';
	modify_attack = 'f + t';
	weight = 'f + t';
}

-- flambard --
recipe {
	class = 'flambard';
	made_by = 'forge';
	char = '/';
	modify_attack = 2;
	equipable = true;
	slot = 'right-hand';
}

comp {
	mat = '1';
	class = 'hilt';
	modify_attack = 'f + t';
	modify_ac     = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'serrated blade';
	modify_attack = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'serrated blade';
	modify_attack = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'blade';
	modify_attack = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'blade';
	modify_attack = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'blade';
	modify_attack = 'f + t';
	weight = 'f + t';
}

-- schiavona --
recipe {
	class = 'schiavona';
	made_by = 'forge';
	char = '/';
	modify_attack = 2;
	equipable = true;
	slot = 'right-hand';
}

comp {
	mat = '1';
	class = 'hilt';
	modify_attack = 'f + t';
	modify_ac     = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'blade';
	modify_attack = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'blade';
	modify_attack = 'f + t';
	weight = 'f + t';
}

comp {
	class = 'ingot';
	modify_attack = 'f + t';
	weight = 'f + t';
}


-- mace --
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

---------------------------------
--         Here be armour      --
---------------------------------

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

