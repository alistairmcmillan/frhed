<?php
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

  include('sidenav.inc');

  /**
   * ...
   */
  class Page {
    var $_title, $_charset;
    var $_description, $_keywords, $_rssfeeds;

    /**
     * Constructor
     */
    function Page() {
      $this->_title = '';
      $this->_charset = 'UTF-8';
      $this->_description = '';
      $this->_keywords = '';
      $this->_rssfeeds = array();
    }

    /**
     * set the title from the page
     */
    function setTitle($title) {
      $this->_title = $title;
    }

    /**
     * set the charset from the page
     */
    function setCharset($charset) {
      $this->_charset = $charset;
    }

    /**
     * set the description from the page
     */
    function setDescription($description) {
      $this->_description = $description;
    }

    /**
     * set the keywords from the page
     */
    function setKeywords($keywords) {
      $this->_keywords = $keywords;
    }

    /**
     * add RSS feed to the page head
     */
    function addRssFeed($url, $title = 'RSS') {
      $this->_rssfeeds[$url] = $title;
    }

    /**
     * print the head from the page
     */
    function printHead($title = '') {
      global $translations;
      
      $this->_title = $title;
      $temp = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n";
      $temp .= "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"" . $translations->getActiveLanguageId() . "\">\n";
      $temp .= "<head>\n";
      if ($this->_title != '') {
        $temp .= "  <title>" . $this->_title . " - " . __('Frhed') . "</title>\n";
      }
      else {
        $temp .= "  <title>" . __('Frhed - Free hex editor') . "</title>\n";
      }
      $temp .= "  <meta http-equiv=\"content-type\" content=\"text/html; charset=" . $this->_charset . "\"/>\n";
      if ($this->_description != '') {
        $temp .= "  <meta name=\"description\" content=\"" . $this->_description . "\"/>\n";
      }
      if ($this->_keywords != '') {
        $temp .= "  <meta name=\"keywords\" content=\"" . $this->_keywords . "\"/>\n";
      }
      $temp .= "  <link rel=\"stylesheet\" type=\"text/css\" media=\"screen\" href=\"/engine/css/default.css\"/>\n";
      $temp .= "  <link rel=\"stylesheet\" type=\"text/css\" media=\"print\" href=\"/engine/css/print.css\"/>\n";
      $temp .= "  <link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\"/>\n";
      foreach ($this->_rssfeeds as $rssurl => $rsstitle) { //for all RSS feeds...
        $temp .= "  <link rel=\"alternate\" type=\"application/rss+xml\" title=\"" . $rsstitle . "\" href=\"" . $rssurl . "\" />\n";
      }
      $temp .= "</head>\n";
      $temp .= "<body>\n";
      $temp .= "<div id=\"wrapper\">\n";
      $temp .= "  <div id=\"container\">\n";
      $temp .= "    <div id=\"main\">\n";
      $temp .= "      <div id=\"content\">\n";
      $temp .= "<h1 class=\"title\">" . __('Frhed') . "</h1>\n";
      $temp .= "<p class=\"olive\">" . __('<b>Fr</b>ee <b>h</b>ex <b>ed</b>itor') . "</p>\n";
      print($temp);
    }

    /**
     * print the foot from the page
     */
    function printFoot() {
      $temp = "      </div> <!-- #content -->\n";
      $temp .= $this->getSideNavigation();
      $temp .= "      <div class=\"clearer\"></div>\n";
      $temp .= "    </div> <!-- #main -->\n";
      $temp .= "    <div id=\"footer\">\n";
      $temp .= "      <div class=\"left\">Valid <a href=\"http://jigsaw.w3.org/css-validator/check/referer\">CSS</a> &amp; <a href=\"http://validator.w3.org/check?uri=referer\">XHTML</a></div>\n";
      $temp .= "      <div class=\"right\">Design by <a href=\"http://arcsin.se/\">Arcsin</a> <a href=\"http://templates.arcsin.se/\">Web Templates</a></div>\n";
      $temp .= "      <div class=\"clearer\"><span></span></div>\n";
      $temp .= "    </div> <!-- #footer -->\n";
      $temp .= "  </div> <!-- #container -->\n";
      $temp .= "</div> <!-- #wrapper -->\n";
      $temp .= "</body>\n";
      $temp .= "</html>\n";
      print($temp);
    }

    /**
     * print a <h2> heading
     */
    function printHeading($title, $id = '') {
      $htmlId = empty($id) ? '' : ' id="' . $id. '"';
      print("<h2" . $htmlId . ">" . $title . "</h2>\n");
    }

    /**
     * print a <h2> heading with a RSS feed link
     */
    function printRssHeading($title, $rssfeedurl, $id = '') {
      $htmlId = empty($id) ? '' : ' id="' . $id. '"';
      print("<h2" . $htmlId . ">" . $title . " " . $this->getRssFeedLink($rssfeedurl) . "</h2>\n");
    }

    /**
     * print a <h3> heading
     */
    function printSubHeading($title) {
      print("<h3>" . $title . "</h3>\n");
    }

    /**
     * print a <h3> heading with a RSS feed link
     */
    function printRssSubHeading($title, $rssfeedurl) {
      print("<h3>" . $title . " " . $this->getRssFeedLink($rssfeedurl) . "</h3>\n");
    }

    /**
     * print a paragraph
     */
    function printPara() {
      print("<p>");
      $numargs = func_num_args();
      $args = func_get_args();
      for ($i = 0; $i < $numargs; $i++) { //for all arguments...
        if ($i > 0) //if NOT first argument...
          print(' ' . $args[$i]);
        else //if first argument...
          print($args[$i]);
      }
      print("</p>\n");
    }

    /**
     * get a RSS feed link
     */
    function getRssFeedLink($url, $text = '') {
      $temp = "<a href=\"" . $url . "\"><img src=\"/engine/images/rss.png\" alt=\"RSS\" style=\"border:none;\" /></a>";
      if ($text != '') { //if WITH text...
        $temp .= " <a href=\"" . $url . "\"> " . $text . "</a>";
      }
      return $temp;
    }

    /**
     * print a RSS feed link
     */
    function printRssFeedLink($url, $text = '') {
      print($this->getRssFeedLink($url, $text));
    }
    
    /**
     * get the side navigation from the page
     */
    function getSideNavigation() {
      global $translations;
      
      $sidenaventry = new SideNavEntry('', __('Frhed'));
      $sidenaventry->addSubEntry($translations->prepareLink('/{lang}/'), __('Home'));
      $sidenaventry->addSubEntry($translations->prepareLink('/{lang}/screenshots/'), __('Screenshots'));
      $sidenaventry->addSubEntry('http://sourceforge.net/projects/frhed', __('Project Page'));
      $sidenaventry->addSubEntry('/frhed_pad.xml', __('PAD File'));
      $sidenaventries[] = $sidenaventry;
      $sidenaventry = new SideNavEntry('', __('Support'));
      $sidenaventry->addSubEntry('http://sourceforge.net/tracker/?atid=1100163&amp;group_id=236485', __('Bugs'));
      $sidenaventry->addSubEntry('http://sourceforge.net/tracker/?atid=1100166&amp;group_id=236485', __('Feature Requests'));
      $sidenaventry->addSubEntry('http://apps.sourceforge.net/phpbb/frhed/', __('Forums'));
      $sidenaventry->addSubEntry('http://sourceforge.net/mail/?group_id=236485', __('Mailing Lists'));
      $sidenaventries[] = $sidenaventry;
      $sidenaventry = new SideNavEntry('', __('Translations'));
      $sidenaventry->addSubEntry($translations->prepareLink('/{lang}/translations/'), __('Index'));
      $sidenaventry->addSubEntry($translations->prepareLink('/{lang}/translations/status_branch.php'), __('Status (Stable Branch)'));
      $sidenaventry->addSubEntry($translations->prepareLink('/{lang}/translations/status_trunk.php'), __('Status (Unstable Trunk)'));
      $sidenaventries[] = $sidenaventry;
      $sidenaventry = new SideNavEntry('', __('Links'));
      $sidenaventry->addSubEntry('http://www.kibria.de/frhed.html', __('Original Version'), __('Original version from Raihan Kibria'));
      $sidenaventry->addSubEntry('http://winmerge.org/', __('WinMerge'));
      $sidenaventries[] = $sidenaventry;
      
      $temp = "<div id=\"sidenav\">\n";
      foreach ($sidenaventries as $sidenaventry) { //for all sidenav entries...
        $temp .= "  <h2>" . $sidenaventry->getTitle() . "</h2>\n";
        $subsidenaventries = $sidenaventry->getSubEntries();
        if (!empty($subsidenaventries)) { //if sub sidenav entries exists...
          $temp .= "  <ul>\n";
          foreach ($subsidenaventries as $subsidenaventry) { //for all sub sidenav entries...
            $tooltiptext = '';
            if ($subsidenaventry->getToolTipText() != '') { //if tool tip text exists...
              $tooltiptext = " title=\"" . $subsidenaventry->getToolTipText() . "\"";
            }
            if ($subsidenaventry->isCurrent() == true) { //if current sub sidenav entry...
              $temp .= "    <li><strong><a href=\"" . $subsidenaventry->getLink() . "\"" . $tooltiptext . ">" . $subsidenaventry->getTitle() . "</a></strong></li>\n";
            }
            else {
              $temp .= "    <li><a href=\"" . $subsidenaventry->getLink() . "\"" . $tooltiptext . ">" . $subsidenaventry->getTitle() . "</a></li>\n";
            }
          }
          $temp .= "  </ul>\n";
        }
      }
      $temp .= "  <h2>" . __('Donate') . "</h2>\n";
      $temp .= "  <p>" . __('Support this project by donating money.') . "</p>\n";
      $temp .= "  <a href=\"http://sourceforge.net/donate/index.php?group_id=236485\"><img src=\"http://images.sourceforge.net/images/project-support.jpg\" width=\"88\" height=\"32\" style=\"border:none;\" alt=\"" . __('Support this project') . "\" /></a>\n";
      
      $temp .= "<h2>" . __('Website Language') . "</h2>\n";
      $temp .= "<ul>\n";
      $languages = $translations->getLanguages();
      foreach ($languages as $language) { //for all languages...
        if ($translations->isActiveLanguage($language)) { //if current language...
          $temp .= "  <li><strong><a href=\"" . $translations->getLink($language, true) . "\" hreflang=\"" . $language->getId() . "\">" . __($language->getName()) . "</a></strong></li>\n";
        }
        else { //if NOT current language...
          $temp .= "  <li><a href=\"" . $translations->getLink($language, true) . "\" hreflang=\"" . $language->getId() . "\">" . __($language->getName()) . "</a></li>\n";
        }
      }
      $temp .= "</ul>\n";
      
      $temp .= "</div> <!-- #sidenav -->\n";
      return $temp;
    }
  }
?>
