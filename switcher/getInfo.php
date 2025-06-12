<?php

#######################################
# Code for GDPS Switcher integration. #
#   Don't mess with anything unless   #
#     you know what you're doing!     #
#######################################

include '../config/gdpsSwitcher.php';

# This is not for you to change - it's for the mod itself.
$version = 1;

echo json_encode([
    "motd" => $motd,
    "version" => $version,
    "icon" => $icon
]);
?>