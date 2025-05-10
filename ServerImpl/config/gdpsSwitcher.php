<?php
# A message of the day for the server - Based off of Minecraft's system.
# Color tags are supported, although fade, delay, and shake tags are not.
# You can find all color tags at https://wyliemaster.github.io/gddocs/#/topics/tags?id=colour-tags
# Alternatively, you can use hex colors, for example <c-AEAEAE>Text</c> would be gray.
$motd = "A Geometry Dash Private Server.";

# The link to the icon for the server.
# This should be a link to a png.
$icon = "https://raw.githubusercontent.com/Kingminer7/pixel-art/refs/heads/main/pfp/pfp.png";



$mods = [
    "dependencies" => [
        # List of mods that are required to use the server.
        # The user will be prompted to install these mods if they are not already installed, unless they're not on index.
        # Mods not on index will have to be obtained by the user. I am not allowed to install them for the user, as that would be a security risk and against index guidelines.
        # Version numbers are not required, and is the minimum version required.
        "geode.node-ids" => "1.20.1",
        "thisdoes.notexist" => "1.0.0",
        "geode.devtools" => "fish",
    ],

    # Options: "whitelist", "blacklist"
    # If using "whitelist", only these mods will be allowed.
    # If using "blacklist", these mods will be blocked.
    "policy" => "blacklist",

    # List of mods based on the selected policy.
    # Dependencies will be ignored in these policies.
    "modList" => [
        "thesillydoggo.qolmod",
        "eclipse.eclipse-menu",
        "tobyadd.gdh",
        "absolllute.megahack",
        "firee.prism",
    ],
];
?>
