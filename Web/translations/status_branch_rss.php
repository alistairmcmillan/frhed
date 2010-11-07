<?php
  header('Content-type: application/rss+xml');
  
  include('../engine/translations.inc');
  
  try {
    $status = New TranslationsStatus('status_branch.xml');
    $status->version = 'Stable Branch';
    $status->svnUrl = 'http://frhed.svn.sourceforge.net/viewvc/frhed/branches/R1_6/Translations/';
    $status->rssLink = 'http://frhed.sourceforge.net/translations/status_branch.php';
    $status->printRSS();
  }
  catch (Exception $ex) { //If problems with translations status...
    print("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    print("<rss version=\"2.0\">\n");
    print("</rss>\n");
  }
?>