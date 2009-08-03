<?php
  include('../engine/engine.inc');
  
  $page = new Page;
  $page->printHead(__('Screenshots - Frhed'));
  
  $page->printHeading(__('Screenshots'));
  $page->printSubHeading(__('Main Window'));
?>
<a href="frhed_main_window.png" target="_blank"><img src="frhed_main_window_small.png" alt="Main Window" style="border:none;" /></a>
<p>The main window from Frhed.</p>
<?php
  $page->printFoot();
?>
