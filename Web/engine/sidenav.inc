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
  class SideNavEntry {
    var $_link, $_title, $_tooltiptext;
    var $_subentries;

    /**
     * Constructor
     */
    function SideNavEntry($link, $title, $tooltiptext = '') {
      $this->_link = $link;
      $this->_title = $title;
      $this->_tooltiptext = $tooltiptext;
      $this->_subentries = array();
    }

    /**
     * set the link from the sidenav entry
     */
    function setLink($link) {
      $this->_link = $link;
    }

    /**
     * get the link from the sidenav entry
     */
    function getLink() {
      return $this->_link;
    }

    /**
     * set the title from the sidenav entry
     */
    function setTitle($title) {
      $this->_title = $title;
    }

    /**
     * get the title from the sidenav entry
     */
    function getTitle() {
      return $this->_title;
    }

    /**
     * set the tool tip text from the sidenav entry
     */
    function setToolTipText($id) {
      $this->_tooltiptext = $id;
    }

    /**
     * get the tool tip text from the sidenav entry
     */
    function getToolTipText() {
      return $this->_tooltiptext;
    }

    /**
     * ...
     */
    function addSubEntry($link, $title, $tooltiptext = '') {
      $this->_subentries[] = New SubSideNavEntry($link, $title, $tooltiptext);
    }

    /**
     * ...
     */
    function getSubEntries() {
      return $this->_subentries;
    }

    /**
     * is the current sidenav entry?
     */
    function isCurrent() {
      if ($this->_link == $_SERVER['PHP_SELF']) {
        return true;
      }
      elseif ($this->_link == str_replace('index.php', '', $_SERVER['PHP_SELF'])) {
        return true;
      }
      elseif ($this->_link == $_SERVER['REQUEST_URI']) {
        return true;
      }
      return false;
    }
  }

  /**
   * ...
   */
  class SubSideNavEntry {
    var $_link, $_title, $_tooltiptext;

    /**
     * Constructor
     */
    function SubSideNavEntry($link, $title, $tooltiptext = '') {
      $this->_link = $link;
      $this->_title = $title;
      $this->_tooltiptext = $tooltiptext;
    }

    /**
     * set the link from the sub sidenav entry
     */
    function setLink($link) {
      $this->_link = $link;
    }

    /**
     * get the link from the sub sidenav entry
     */
    function getLink() {
      return $this->_link;
    }

    /**
     * set the title from the sub sidenav entry
     */
    function setTitle($title) {
      $this->_title = $title;
    }

    /**
     * get the title from the sub sidenav entry
     */
    function getTitle() {
      return $this->_title;
    }

    /**
     * set the tool tip text from the sub sidenav entry
     */
    function setToolTipText($id) {
      $this->_tooltiptext = $id;
    }

    /**
     * get the tool tip text from the sub sidenav entry
     */
    function getToolTipText() {
      return $this->_tooltiptext;
    }

    /**
     * is the current sub sidenav entry?
     */
    function isCurrent() {
      if ($this->_link == $_SERVER['PHP_SELF']) {
        return true;
      }
      elseif ($this->_link == str_replace('index.php', '', $_SERVER['PHP_SELF'])) {
        return true;
      }
      elseif ($this->_link == $_SERVER['REQUEST_URI']) {
        return true;
      }
      return false;
    }
  }
?>