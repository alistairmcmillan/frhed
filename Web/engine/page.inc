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
      $this->_title = 'Frhed';
      $this->_charset = 'ISO-8859-1';
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
      if ($title != '') {
        $this->_title = $title;
      }
      $temp = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n";
      $temp .= "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
      $temp .= "<head>\n";
      $temp .= "  <title>" . $this->_title . "</title>\n";
      $temp .= "  <meta http-equiv=\"content-type\" content=\"text/html; charset=" . $this->_charset . "\"/>\n";
      if ($this->_description != '') {
        $temp .= "  <meta name=\"description\" content=\"" . $this->_description . "\"/>\n";
      }
      if ($this->_keywords != '') {
        $temp .= "  <meta name=\"keywords\" content=\"" . $this->_keywords . "\"/>\n";
      }
      $temp .= "  <link rel=\"stylesheet\" type=\"text/css\" media=\"screen\" href=\"/css/default.css\"/>\n";
      $temp .= "  <link rel=\"stylesheet\" type=\"text/css\" media=\"print\" href=\"/css/print.css\"/>\n";
      $temp .= "  <link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\"/>\n";
      foreach ($this->_rssfeeds as $rssurl => $rsstitle) { //for all RSS feeds...
        $temp .= "  <link rel=\"alternate\" type=\"application/rss+xml\" title=\"" . $rsstitle . "\" href=\"" . $rssurl . "\">\n";
      }
      $temp .= "</head>\n";
      $temp .= "<body>\n";
      $temp .= "<div class=\"wrapper\">\n";
      $temp .= "  <div class=\"container\">\n";
      $temp .= "    <div class=\"main\">\n";
      print($temp);
    }

    /**
     * print the foot from the page
     */
    function printFoot() {
      $temp = "    </div> <!-- .main -->\n";
      $temp .= "    <div class=\"footer\">\n";
      $temp .= "      <div class=\"left\">Valid <a href=\"http://jigsaw.w3.org/css-validator/check/referer\">CSS</a> &amp; <a href=\"http://validator.w3.org/check?uri=referer\">XHTML</a></div>\n";
      $temp .= "      <div class=\"right\">Design by <a href=\"http://arcsin.se/\">Arcsin</a> <a href=\"http://templates.arcsin.se/\">Web Templates</a></div>\n";
      $temp .= "      <div class=\"clearer\"><span></span></div>\n";
      $temp .= "    </div> <!-- .footer -->\n";
      $temp .= "  </div> <!-- .container -->\n";
      $temp .= "</div> <!-- .wrapper -->\n";
      $temp .= "</body>\n";
      $temp .= "</html>\n";
      print($temp);
    }
  }
?>
