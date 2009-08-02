<?php
  include('../engine/engine.inc');
  include('translations.inc');

  $page = new Page;
  $page->addRssFeed('status_branch_rss.php', 'Frhed Translations Status (Stable Branch)');
  $page->addRssFeed('status_trunk_rss.php', 'Frhed Translations Status (Unstable Trunk)');
  $page->printHead('Translations - Frhed');
  
?>
<h2>Translations</h2>
<p>We currently have Frhed translated into the languages listed below:</p>
<ul>
<?php
  try {
    $status = New TranslationsStatus('status_trunk.xml');
    
    $languages = $status->getLanguagesArray();
    foreach ($languages as $language) { //for all languages...
      print("  <li>" . $language . "</li>\n");
    }
  }
  catch (Exception $ex) { //If problems with translations status...
    print("  <li>English</li>\n");
  }
?>
</ul>
<h3><a name="status">Status</a></h3>
<p>The following two pages inform you about the translations status of the stable and developer version of Frhed:</p>
<ul>
  <li><a href="status_branch.php">Stable Version (Branch)</a> <?php $page->printRssFeedLink('status_branch_rss.php'); ?></li>
  <li><a href="status_trunk.php">Unstable Version (Trunk)</a> <?php $page->printRssFeedLink('status_trunk_rss.php'); ?></li>
</ul>
<?php
  $page->printFoot();
?>