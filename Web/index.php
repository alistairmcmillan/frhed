<?php
  include('engine/engine.inc');
  include('engine/simplepie/simplepie.inc');
  
  $page = new Page;
  $page->setDescription(__('Frhed is an binary file editor for Windows. It is small but has many advanced features like ability to load big files partially.'));
  $page->setKeywords(__('Frhed, free, hex editor, binary file, load partially, hexdump, compare'));
  $page->addRssFeed('http://sourceforge.net/export/rss2_projnews.php?group_id=236485', __('Frhed News'));
  $page->addRssFeed('http://sourceforge.net/export/rss2_projfiles.php?group_id=236485', __('Frhed File Releases'));
  $page->printHead();
  $page->printPara(__('Frhed is an binary file editor for Windows. It is small but has many advanced features like ability to load big files partially.'));
  
  $page->printHeading(__('Features'));
?>
<ul>
  <li><?php __e('Editable file size limited only by available system memory')?></li>
  <li><?php __e('Load files partially')?></li>
  <li><?php __e('Export as hexdump to file or clipboard')?></li>
  <li><?php __e('Search for any combination of text and binary values')?></li>
  <li><?php __e('Compare files')?></li>
</ul>

<?php
  $page->printRssHeading(__('News'), 'http://sourceforge.net/export/rss2_projnews.php?group_id=236485');
  $feed = new SimplePie();
  $feed->set_feed_url('http://sourceforge.net/export/rss2_projnews.php?group_id=236485');
  $feed->set_cache_location('./engine/simplepie/cache');
  $feed->init();
  print("<ul class=\"rssfeeditems\">\n");
  foreach ($feed->get_items(0, 3) as $item) { //for the last 3 news items...
    print("  <li><a href=\"".$item->get_link()."\">".$item->get_title()."</a> <em>".$item->get_date(__('Y-m-d'))."</em></li>\n");
  }
  print("  <li><a href=\"http://sourceforge.net/news/?group_id=236485\">" . __('View all news &hellip;') . "</a></li>\n");
  print("</ul>\n");
?>

<?php
  $page->printRssHeading(__('Download'), 'http://sourceforge.net/export/rss2_projfiles.php?group_id=236485');
  $page->printSubHeading(__('Stable releases'));
?>
				<ul>
					<li><a href="https://sourceforge.net/project/showfiles.php?group_id=236485&amp;package_id=308801&amp;release_id=688790">
						Frhed 1.6.0</a> (2009-06-23)</li>
					<li><a href="https://sourceforge.net/project/showfiles.php?group_id=236485&amp;package_id=308801&amp;release_id=674196">
						Frhed 1.4.2</a> (2009-04-07)</li>
					<li><a href="https://sourceforge.net/project/showfiles.php?group_id=236485&amp;package_id=308801&amp;release_id=658634">
						Frhed 1.4.0</a> (2009-02-04)</li>
				</ul>
<?php
  $page->printSubHeading(__('Beta releases'));
?>
				<ul>
					<li><a href="https://sourceforge.net/project/showfiles.php?group_id=236485&amp;package_id=307097&amp;release_id=688792">
						Frhed 1.5.10</a> (2009-06-10)</li>
				</ul>
<?php
  $page->printSubHeading(__('Alpha releases'));
?>
				<ul>
					<li><a href="https://sourceforge.net/projects/frhed/files/3.%20Alpha%20Releases/1.7.1/">
						Frhed 1.7.1</a> (2009-07-30)</li>
				</ul>

<?php
  $page->printHeading(__('Support'));
  $page->printPara(__('For support you can try the <a href="%1$s">forums</a> or the <a href="%2$s">mailing lists</a>.', 'http://apps.sourceforge.net/phpbb/frhed/', 'https://sourceforge.net/mail/?group_id=236485'), 
                   __('If you found bugs, please submit them to our <a href="%1$s">bug tracker</a> or leave wishes at the <a href="%2$s">feature requests</a> list.', 'https://sourceforge.net/tracker/?atid=1100163&amp;group_id=236485', 'https://sourceforge.net/tracker/?atid=1100166&amp;group_id=236485'));
  
  $page->printHeading(__('Open Source'));
  $page->printPara(__('Frhed is <a href="%1$s">open source</a> software and is licenced under the <a href="%2$s">GNU General Public License</a>. 
This means everybody can download the source code and improve and modify them.', 'http://www.opensource.org/', 'http://www.gnu.org/licenses/gpl-2.0.html'));
  
  $page->printHeading(__('History'));
  $page->printPara(__('Frhed is originally developed by <a href="%s">Raihan Kibria</a>.', 'http://www.kibria.de/frhed.html'));
  $page->printPara(__('Version 1.1 included many improvements and new features from versions that several other programmers had been working on. 
Unfortunately version 1.1 never matured to "stable" release. Latest 1.1 release is labeled as "beta" release.'));
  $page->printPara(__('At autumn 2008 the <a href="%1$s">WinMerge Team</a> decided to use Frhed as binary file editor component 
for WinMerge. For developing Frhed further new <a href="%2$s">SourceForge project</a> 
was created. This new project will develop Frhed as stand alone product.', 'http://winmerge.org/' , 'https://sourceforge.net/projects/frhed'));
  
  $page->printFoot();
?>
