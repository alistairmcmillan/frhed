<?php
  header('Content-type: application/rss+xml');
  
  include('translations.inc');
  
  try {
    $status = New TranslationsStatus('status_trunk.xml');
    $status->version = 'Unstable Trunk';
    $status->svnUrl = 'http://frhed.svn.sourceforge.net/viewvc/frhed/trunk/Translations/';
    $status->rssLink = 'http://frhed.sourceforge.net/translations/status_trunk.php';
    $status->printRSS();
  }
  catch (Exception $ex) { //If problems with translations status...
    print("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    print("<rss version=\"2.0\">\n");
    print("</rss>\n");
  }
?>