<?php
  //Send 404 headers...
  header('HTTP/1.1 404 Not Found');
  header('Status: 404 Not Found');

  include_once('engine/engine.inc');

  $page = new Page;
  $page->printHead(__('Error 404 (Page Not Found)'), TAB_HOME);

  $page->printHeading(__('Page Not Found...'));
  $page->printPara(__('For some reason (mis-typed URL, faulty referral from another site, out-of-date search engine listing or we simply deleted a file) the page you were looking for could not be found.'),
                   __('This site has recently undergone a major re-working, so that might explain why you got this page instead.'));

  $page->printSubHeading(__('Were you looking for...'));
?>
<ul>
  <li><?php __e('<a href="%s">Download</a>?', $translations->prepareLink('/{lang}/#download'));?></li>
  <li><?php __e('<a href="%s">Screenshots</a>?', $translations->prepareLink('/{lang}/screenshots/'));?></li>
  <li><?php __e('<a href="%s">Support</a>?', $translations->prepareLink('/{lang}/#support'));?></li>
  <li><?php __e('<a href="%s">Translations</a>?', $translations->prepareLink('/{lang}/translations/'));?></li>
</ul>
<?php
  $page->printFoot();
?>