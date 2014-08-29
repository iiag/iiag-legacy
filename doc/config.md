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
 Control   | A value representing a key to press on the keyboard in order to utilize a control in game. The format is usually `a` for the A-key, `A` for shift+A-key, and so on. Digits are used for literial keycode values. The special values `%left%`, `%right%`, `%up%`, and `%down%` can be used to represent the arrow-keys.

Fields
------

 Field Name       | Type    | Description
------------------|---------|-------------
 lua-init         | string  | Specifies the initial lua file to run, like `-i`.
 show-all         | boolean | Specifies whether everything is shown to the player or not, like `-s`.
 forget-walls     | boolean | Specifies whether wall-forgetting is enabled or not, like `-f`.
 all-alone        | boolean | Specifies whether all-alone mode is enabled, like `-l`.
 god-mode         | boolean | Specifies whether the player may avoid damage during the game, like `-g`.
 log-level        | integer | Specifies the verbosity of the log. Valid values are: 0=ALL (no filtering), 1=DEBUG, 2=INFO, 3=NOTICE, 4=WARNING, 5=ERROR. The setting is inclusive.
 throw-anim-delay | integer | The delay between 'frames' of the throwing animation. 0 indicates the lack of animation
 ctrl-up          | control | How the player moves up.
 ctrl-down        | control | How the player moves down.
 ctrl-left        | control | How the player moves left.
 ctrl-right       | control | How the player moves right.
 ctrl-uleft       | control | How the player moves up-left.
 ctrl-uright      | control | How the player moves up-right.
 ctrl-dleft       | control | How the player moves down-left.
 ctrl-dright      | control | How the player moves down-right.
 ctrl-scrl-center | control | What key centers the player's scrolling on the player.
 ctrl-scrl-up     | control | How the player scrolls up.
 ctrl-scrl-down   | control | How the player scrolls down.
 ctrl-scrl-left   | control | How the player scrolls left.
 ctrl-scrl-right  | control | How the player scrolls right.
