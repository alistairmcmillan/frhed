<?php
  $page = new Page;
  $page->addRssFeed('status_branch_rss.php', __('Frhed Translations Status (Stable Branch)'));
  $page->addRssFeed('status_trunk_rss.php', __('Frhed Translations Status (Unstable Trunk)'));
  $page->printHead(__('Translations'));

  $page->printHeading(__('Translations'));
  $page->printPara(__('We currently have Frhed translated into the languages listed below:'));
  print("<ul>\n");
  try {
    $status = New TranslationsStatus('status_trunk.xml');
    
    $languages = $status->getLanguagesArray();
    foreach ($languages as $language) { //for all languages...
      print("  <li>" . __($language) . "</li>\n");
    }
  }
  catch (Exception $ex) { //If problems with translations status...
    print("  <li>" . __('English') . "</li>\n");
  }
  print("</ul>\n");

  $page->printSubHeading(__('Status'));
  $page->printPara(__('The following two pages inform you about the translations status of the stable and developer version of Frhed:'));
?>
<ul>
  <li><a href="status_branch.php"><?php __e('Stable Version (Branch)');?></a> <?php $page->printRssFeedLink('status_branch_rss.php'); ?></li>
  <li><a href="status_trunk.php"><?php __e('Unstable Version (Trunk)');?></a> <?php $page->printRssFeedLink('status_trunk_rss.php'); ?></li>
</ul>
<?php
  $page->printFoot();
?>