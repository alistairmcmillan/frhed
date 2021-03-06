<?php
  $page = new Page;
  $page->addRssFeed('status_branch_rss.php');
  $page->printHead(__('Translations Status (Stable Branch)'));
  
  $page->printRssHeading(__('Translations Status (Stable Branch)'), 'status_branch_rss.php');
  try {
    $status = New TranslationsStatus('status_branch.xml');
    $status->svnUrl = 'http://frhed.svn.sourceforge.net/viewvc/frhed/branches/R1_6/Translations/';
    $status->printTOC();
    $status->printStatus();
    $status->printTranslators();
  }
  catch (Exception $ex) { //If problems with translations status...
    print("<p>" . __('The translations status is currently not available...') . "</p>\n");
  }
  $page->printFoot();
?>