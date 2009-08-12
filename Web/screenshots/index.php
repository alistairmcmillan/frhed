<?php
  include('../engine/engine.inc');
  
  $page = new Page;
  $page->printHead(__('Screenshots'));
  $page->printHeading(__('Screenshots'));
  
  $page->printSubHeading(__('Main Window'));
?>
<a href="frhed_main_window.png" target="_blank"><img src="frhed_main_window_small.png" alt="<?php __e('Main Window');?>" style="border:none;" /></a>
<?php
  $page->printPara(__('The main window from Frhed.'));
  
  $page->printFoot();
?>
