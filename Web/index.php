<?php
  include('engine/page.inc');
  
  $page = new Page;
  $page->setDescription('Frhed is an binary file editor for Windows. It is small but has many advanced features like ability to load big files partially.');
  $page->setKeywords('Frhed, free, hex editor, binary file, load partially, hexdump, compare');
  $page->printHead('Frhed - Free hex editor');
?>
				<h1 class="title">Frhed</h1>
				<p class="olive"><b>Fr</b>ee <b>h</b>ex <b>ed</b>itor</p>

				<p>Frhed is an binary file editor for Windows. It is small but has many advanced features like ability to load big files partially.</p>

				<h2>Features</h2>
				<ul>
					<li>Editable file size limited only by available system memory</li>
					<li>Load files partially</li>
					<li>Export as hexdump to file or clipboard</li>
					<li>Search for any combination of text and binary values</li>
					<li>Compare files</li>
				</ul>

				<h2>Download</h2>
				<h3>Beta releases</h3>
				<ul>
					<li><a href="https://sourceforge.net/project/showfiles.php?group_id=236485&amp;package_id=307097&amp;release_id=655390">
						Frhed 1.3.10</a> (2009-01-22)</li>
				</ul>
				<h3>Alpha releases</h3>
				<ul>
					<li><a href="https://sourceforge.net/project/showfiles.php?group_id=236485&amp;package_id=295443&amp;release_id=651776">
						Frhed 1.3.4</a> (2009-01-06)</li>
					<li><a href="https://sourceforge.net/project/showfiles.php?group_id=236485&amp;package_id=295443&amp;release_id=644901">
						Frhed 1.3.3</a> (2008-12-04)</li>
					<li><a href="https://sourceforge.net/project/showfiles.php?group_id=236485&amp;package_id=295443&amp;release_id=638916">
						Frhed 1.3.2</a> (2008-11-07)</li>
					<li><a href="https://sourceforge.net/project/showfiles.php?group_id=236485&amp;package_id=295443&amp;release_id=633733">
						Frhed 1.3.1</a> (2008-10-16)</li>
				</ul>
				<h3>Original project download</h3>
				<ul>
					<li><a href="http://www.rs.e-technik.tu-darmstadt.de/applets/frhed-v1.1.zip">Frhed 1.1 BETA 1</a></li>
				</ul>

				<h2>Support</h2>
				<p>For support you can try the <a href="http://apps.sourceforge.net/phpbb/frhed/">forums</a> or the <a href="https://sourceforge.net/mail/?group_id=236485">mailing lists</a>.
				If you found bugs, please submit them to our <a href="https://sourceforge.net/tracker/?atid=1100163&amp;group_id=236485">bug tracker</a>
				or leave wishes at the <a href="https://sourceforge.net/tracker/?atid=1100166&amp;group_id=236485">feature requests</a> list.</p>

				<h2>Open Source</h2>
				<p>Frhed is <a href="http://www.opensource.org/">open source</a> software and is licenced under the <a href="http://www.gnu.org/licenses/gpl-2.0.html">GNU General Public License</a>.
				This means everybody can download the source code and improve and modify them.</p>

				<h2>History</h2>
				<p>Frhed is originally developed by <a href="http://www.kibria.de/frhed.html">Raihan Kibria</a>.
				Version 1.1 included many improvements and new features from versions that several other programmers had been working on.
				Unfortunately version 1.1 never matured to "stable" release. Latest 1.1 release is labeled as "beta" release.
				At autumn 2008 the <a href="http://winmerge.org/">WinMerge Team</a> decided to use Frhed as binary file editor component
				for WinMerge. For developing Frhed further new <a href="https://sourceforge.net/projects/frhed">SourceForge project</a>
				was created. This new project will develop Frhed as stand alone product.</p>
<?php
  $page->printFoot();
?>
