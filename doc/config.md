Configuration File Format
=========================

The configuration file can be any file so long as it is explicitly
specified on the command line with the `-c` option. Standard input
can be used with `-c -`.

General Syntax
--------------

The file format itself contains a list of space-seperated fields
and values for said fields. Spaces themselves are considered anything
that is termed a space character by C's `isspace` function, or a
comment. Comments begin with a octothorpe `#` and end with a new
line or at the end of the file.

Each field is formatted as follows:

    <field-name> = <field-value>

The brackets, `<` and `>`, are for demonstrative purposes. Spaces between
the equal sign `=` and the `<field-name>` or `<field-value>` are optional,
but the equal sign itself is required. Furthermore, spaces are required to
seperate these field name-value pairs. The value itself is interpreted in
a manner specific to the type of the field.


Field Types
-----------

 Type Name | Description
-----------|-------------
 String    | A string value that is terminated with an Space. Note an equal sign also terminates the string. Escape characters (the backslash `\`) can be used to allow a space or equal sign to exist in the string.
 Boolean   | Either `true` or `false`. If neither, it defaults to `false` and logs an error.
 Integer   | A decimal numerical value that represents an integer.
 Control   | A value representing a key to press on the keyboard in order to utilize a control in game. The format is dependent on the tileset type, which influences the input backend. For ncurses backends, the literal key name is used when possible (e.g. 'a' for the a-key, 'A' for shift-a), and name strings like 'left' for the left arrow key.

Fields
------

 Field Name       | Type    | Description
------------------|---------|-------------
 lua-init         | string  | Specifies the initial lua file to run, like `-i`.
 server-ip        | string  | The IP address of the server to connect to. Unlike `-n`, does nothing unless multiplayer is set.
 log-file         | string  | Specifies the path to the file to log to, like `-l`.
 tileset-file     | string  | The path to the file containing the tileset, like `-t`.
 show-all         | boolean | Specifies whether everything is shown to the player or not, like `-s`.
 forget-walls     | boolean | Specifies whether wall-forgetting is enabled or not, like `-f`.
 all-alone        | boolean | Specifies whether all-alone mode is enabled, like `-a`.
 god-mode         | boolean | Specifies whether the player may avoid damage during the game, like `-g`.
 real-time        | boolean | Specifies whether the game should be run in real-time mode, like `-r`.
 multiplayer      | boolean | Specifies whether the game should try to connect to a remote server, a side effect of `-n` normally.
 log-stderr       | boolean | Specifies whether logging should be done additionally to stderr or not, like `-e`.
 port             | integer | The port to connect to on the multiplayer server.
 log-level        | integer | Specifies the verbosity of the log. Valid values are: 0=ALL (no filtering), 1=DEBUG, 2=INFO, 3=NOTICE, 4=WARNING, 5=ERROR. The setting is inclusive.
 throw-anim-delay | integer | The delay between 'frames' of the throwing animation. 0 indicates the lack of animation
 move-up          | control | How the player moves up.
 move-down        | control | How the player moves down.
 move-left        | control | How the player moves left.
 move-right       | control | How the player moves right.
 move-uleft       | control | How the player moves up-left.
 move-uright      | control | How the player moves up-right.
 move-dleft       | control | How the player moves down-left.
 move-dright      | control | How the player moves down-right.
 scroll-center    | control | What key centers the player's scrolling on the player.
 scroll-up        | control | How the player scrolls up.
 scroll-down      | control | How the player scrolls down.
 scroll-left      | control | How the player scrolls left.
 scroll-right     | control | How the player scrolls right.
 inventory        | control | How the player looks at their inventory.
 equipment        | control | How the player looks at their equipped items.
 pickup           | control | How the player picks up an item.
 drop             | control | How the player drops an item.
 consume          | control | How the player consumes (eats or quaffs) an item.
 equip            | control | How the player equips (wears or weilds) an item.
 throw            | control | How the player throws an item.
 stance-neutral   | control | How the player switchs their stance to neutral.
 stance-defend    | control | How the player switchs their stance to defensive.
 stance-attack    | control | How the player switchs their stance to attacking.
 idle             | control | How to ignore a turn.
 disp-ctrl        | control | How to display the control configuration menu.
 command          | control | How to display the command prompt interface.


