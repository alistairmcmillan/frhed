<?php
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

  include('page.inc');

  function __($text) {
    $numargs = func_num_args();
    if ($numargs > 1 ) { //if more the one argument...
      $args = func_get_args();
      array_shift($args);
      return vsprintf($text, $args);
    }
    else { //if one argument...
      return $text;
    }
  }

  function __e($text) {
    $numargs = func_num_args();
    if ($numargs > 1 ) { //if more the one argument...
      $args = func_get_args();
      array_shift($args);
      vprintf($text, $args);
    }
    else { //if one argument...
      echo $text;
    }
  }
?>
