<?php
  include('engine/engine.inc');
  include('engine/simplepie/simplepie.inc');
  include('engine/translations.inc');

  function Send301($url) {
    header('HTTP/1.1 301 Moved Permanently');
    header('Location: ' . $url);
    header('Connection: close');
  }

  $parse = parse_url($_SERVER['REQUEST_URI']);
  $path = $parse['path'];
  $basepath = $translations->replaceRootDirectory($parse['path'], '{lang}');

  switch($basepath) {
     case '/{lang}/':
          include('templates/index.php');
          break;
      case '/{lang}/screenshots/':
          include('templates/screenshots/index.php');
          break;
      case '/{lang}/translations/':
          include('templates/translations/index.php');
          break;
      case '/{lang}/translations/status_branch.php':
          include('templates/translations/status_branch.php');
          break;
      case '/{lang}/translations/status_branch_rss.php':
          include('templates/translations/status_branch_rss.php');
          break;
      case '/{lang}/translations/status_trunk.php':
          include('templates/translations/status_trunk.php');
          break;
      case '/{lang}/translations/status_trunk_rss.php':
          include('templates/translations/status_trunk_rss.php');
          break;
      //Directories without / at the end...
      case '/{lang}':
      case '/{lang}/screenshots':
      case '/{lang}/translations':
          Send301($path . '/');
          break;
      //Old "screenshots" and "translations" paths...
      case '/{lang}reenshots':
      case '/{lang}reenshots/':
      case '/{lang}anslations':
      case '/{lang}anslations/':
      case '/{lang}anslations/status_branch.php':
      case '/{lang}anslations/status_branch_rss.php':
      case '/{lang}anslations/status_trunk.php':
      case '/{lang}anslations/status_trunk_rss.php':
          Send301('/en' . $path);
          break;
      //Unknown paths...
      default:
          header('HTTP/1.1 404 Not Found');
          header('Status: 404 Not Found');
  }
?>