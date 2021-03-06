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
  class TranslationsStatus {
    public $version;
    public $svnUrl;
    public $rssLink;
    
    private $_status;
    
    /**
     * Constructor
     */
    public function __construct($filename) {
      $this->version = '';
      $this->svnUrl = '';
      $this->rssLink = '';
      
      $this->_status = null;
      
      if (file_exists($filename)) { //if XML file exists...
        $xml = @simplexml_load_file($filename);
        if ($xml) { //if XML file was valid...
          $this->_status = $xml;
        }
        else //if XML file was NOT valid...
          throw new Exception(__('File not valid.'));
      }
      else //if XML file NOT exists...
        throw new Exception(__('File not found.'));
    }
    
    /**
     * ...
     *
     * Based partially on the TortoiseSVN Drupal module:
     * @link http://tortoisesvn.tigris.org/svn/tortoisesvn/trunk/contrib/drupal-modules/module/tortoisesvn.module
     */
    private function getStatusGraph($count, $translated, $fuzzy, $untranslated) {
      // Calculate width of bars
      $wc = 150;
      $wt = round($wc * $translated / $count);
      $wf = round($wc * $fuzzy / $count);
      $wu = round($wc * $untranslated / $count);
      
      // Make sure that each bar is at least 2px wide if its value is > 0
      if (($wt<2) && ($translated>0)) $wt=2;
      if (($wf<2) && ($fuzzy>0)) $wf=2;
      if (($wu<2) && ($untranslated>0)) $wu=2;
      
      // Make sure total width stays constant by determining the widest bar
      // and subtracting the width of the shorter bars from it.
      if ($wt+$wf+$wu!=$wc) {
        if ($wt>=$wf && $wt>=$wu) {
          $wt = $wc-$wf-$wu;
        } else if ($wf>=$wt && $wf>=$wu) {
          $wf = $wc-$wt-$wu;
        } else {
          $wu = $wc-$wt-$wf;
        }
      }
      
      // Calculate percentages
      $pt = ($translated * 100) / $count;
      $pf = ($fuzzy * 100) / $count;
      $pu = ($untranslated * 100) / $count;
      
      $temp = "";
      if ($wt > 0)
        $temp .= "<img src=\"/engine/images/translated.png\" alt=\"" . __('Translated') . "\" title=\"" . __('Translated: %d%%', round($pt, 1)) . "\" width=\"$wt\" height=\"16\" />";
      if ($wf > 0)
        $temp .= "<img src=\"/engine/images/fuzzy.png\" alt=\"" . __('Fuzzy') . "\" title=\"" . __('Fuzzy: %d%%', round($pf, 1)) . "\" width=\"$wf\" height=\"16\" />";
      if ($wu > 0)
        $temp .= "<img src=\"/engine/images/untranslated.png\" alt=\"" . __('Untranslated') . "\" title=\"" . __('Untranslated: %d%%', round($pu, 1)) . "\" width=\"$wu\" height=\"16\" />";
      
      return $temp;
    }
    
    /**
     * ...
     */
    private function getStatusGraphLegend() {
      $temp = "<ul class=\"inline\">\n";
      $temp .= "  <li><img src=\"/engine/images/translated.png\" alt=\"*\" width=\"16\" height=\"16\" /> " . __('Translated') . "</li>\n";
      $temp .= "  <li><img src=\"/engine/images/fuzzy.png\" alt=\"*\" width=\"16\" height=\"16\" /> " . __('Fuzzy') . "</li>\n";
      $temp .= "  <li><img src=\"/engine/images/untranslated.png\" alt=\"*\" width=\"16\" height=\"16\" /> " . __('Untranslated') . "</li>\n";
      $temp .= "</ul>\n";
      return $temp;
    }
    
    /**
     * ...
     */
    public function printTOC() {
      $status = $this->_status;
      
      $temp = "<p>Status from <strong>" . $status->update . "</strong>:</p>\n";
      $temp .= "<ul>\n";
      foreach ($status->translations as $translations) { //for all projects...
        $project = (string)$translations->attributes()->project;
        $temp .= "  <li><a href=\"#" . $project ."\">" . $project . "</a></li>\n";
      }
      $temp .= "  <li><a href=\"#translators\">Translators</a></li>\n";
      $temp .= "</ul>\n";
      print($temp);
    }
    
    /**
     * ...
     */
    public function printStatus() {
      $status = $this->_status;
      
      $temp = '';
      foreach ($status->translations as $translations) { //for all projects...
        $project = (string)$translations->attributes()->project;
        $temp .= "<h3><a name=\"" . $project . "\">" . $project .  "</a></h3>\n";
        $temp .= "<table class=\"status\">\n";
        $temp .= "  <tr>\n";
        $temp .= "    <th class=\"left\">" . __('Language') . "</th>\n";
        $temp .= "    <th class=\"center\">" . __('Graph') . "</th>\n";
        $temp .= "    <th class=\"right translated\">" . __('Translated') . "</th>\n";
        $temp .= "    <th class=\"right fuzzy\">" . __('Fuzzy') . "</th>\n";
        $temp .= "    <th class=\"right untranslated\">" . __('Untranslated') . "</th>\n";
        $temp .= "    <th class=\"center\">" . __('Last Update') . "</th>\n";
        $temp .= "  </tr>\n";
        //TODO: Sort languages!?
        foreach ($translations as $translation) { //for all translations...
          $language_name = (string)$translation->language;
          
          $template = (bool)$translation->attributes()->template;
          if ($template == false) { //If NO template...
            $file_name = (string)$translation->file;
            $translation_update = (string)$translation->update;
            $strings = $translation->strings;
            $strings_count = (int)$strings->count;
            $strings_translated = (int)$strings->translated;
            $strings_fuzzy = (int)$strings->fuzzy;
            $strings_untranslated = (int)$strings->untranslated;
            
            $temp .= "  <tr>\n";
            $temp .= "    <td class=\"left\"><a href=\"" . $this->svnUrl . $project . '/' . $file_name . "\" rel=\"nofollow\">" . __($language_name) . "</a></td>\n";
            $temp .= "    <td class=\"center\">" . $this->getStatusGraph($strings_count, $strings_translated, $strings_fuzzy, $strings_untranslated) . "</td>\n";
            $temp .= "    <td class=\"right translated\">" . $strings_translated . "</td>\n";
            $temp .= "    <td class=\"right fuzzy\">" . $strings_fuzzy . "</td>\n";
            $temp .= "    <td class=\"right untranslated\">" . $strings_untranslated . "</td>\n";
            $temp .= "    <td class=\"center\">" . $translation_update . "</td>\n";
            $temp .= "  </tr>\n";
          }
          else { //If template...
            $template_language_name = (string)$translation->language;
            $template_file_name = (string)$translation->file;
            $template_translation_update = (string)$translation->update;
            $template_strings_count = (int)$translation->strings->translated;
          }
        }
        $temp .= "  <tr>\n";
        $temp .= "    <td class=\"left\"><a href=\"" . $this->svnUrl . $project . '/' . $template_file_name . "\" rel=\"nofollow\">" . __($template_language_name) . " (" . __('Template') . ")</a></td>\n";
        $temp .= "    <td class=\"center\"></td>\n";
        $temp .= "    <td colspan=\"3\" class=\"center\">" . __('Total: %d' , $template_strings_count) . "</td>\n";
        $temp .= "    <td class=\"center\">" . $template_translation_update . "</td>\n";
        $temp .= "  </tr>\n";
        $temp .= "</table>\n";
        $temp .= $this->getStatusGraphLegend();
      }
      print($temp);
    }
    
    /**
     * ...
     */
    public function printRSS() {
      $status = $this->_status;
      
      $temp = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
      $temp .= "<rss version=\"2.0\">\n";
      $temp .= "  <channel>\n";
      $temp .= "    <title>Frhed Translations Status (" . $this->version . ")</title>\n";
      $temp .= "    <link>http://frhed.sourceforge.net/</link>\n";
      $temp .= "    <description>...</description>\n";
      foreach ($status->translations as $translations) { //for all projects...
        $project = (string)$translations->attributes()->project;
        
        $temp .= "    <item>\n";
        $temp .= "      <title>" . $project ." Status [" . $status->update . "]</title>\n";
        $temp .= "      <description>\n";
        $temp .= "        <![CDATA[\n";
        $temp .= "<ul>\n";
        //TODO: Sort languages!?
        foreach ($translations as $translation) { //for all translations...
          $language_name = (string)$translation->language;
          
          $template = (bool)$translation->attributes()->template;
          if ($template == false) { //If NO template...
            $file_name = (string)$translation->file;
            $translation_update = (string)$translation->update;
            $strings_untranslated = (int)$translation->strings->untranslated;
            
            $temp .= "  <li>\n";
            $temp .= "    <a href=\"" . $this->svnUrl . $project . '/' . $file_name . "\">" . $language_name . "</a> (" . $strings_untranslated . " Untranslated)\n";
            $temp .= "  </li>\n";
          }
        }
        $temp .= "</ul>\n";
        $temp .= "        ]]>\n";
        $temp .= "      </description>\n";
        $temp .= "      <guid>" . $project . "_" . $status->update . "</guid>\n";
        $temp .= "      <link>" . $this->rssLink . "#" . $project . "</link>\n";
        $temp .= "    </item>\n";
      }
      $temp .= "  </channel>\n";
      $temp .= "</rss>\n";
      print($temp);
    }
    
    /**
     * ...
     */
    public function printTranslators() {
      $projects = array();
      $languages = array();
      
      $status = $this->_status;
      foreach ($status->translations as $translations) { //for all projects...
        $project = (string)$translations->attributes()->project;
        $projects[$project] = $project;
        
        //TODO: Sort languages!?
        foreach ($translations as $translation) { //for all translations...
          $language_name = (string)$translation->language;
          
          $template = (bool)$translation->attributes()->template;
          if ($template == false) { //If NO template...
            $temp = '';
            foreach ($translation->translators as $translators) { //for all translators...
              foreach ($translators as $translator) { //for each translator...
                $translator_name = (string)$translator->name;
                $translator_mail = (string)$translator->mail;
                $maintainer = (bool)$translator->attributes()->maintainer;
                
                if ($maintainer)
                  $temp .= "<strong title=\"Maintainer\">";
                if ($translator_mail != '')
                  $temp .= "<a href=\"mailto:" . $translator_mail . "\" title=\"" . htmlentities($translator_name, ENT_COMPAT, 'UTF-8') . " &lt;" . $translator_mail . "&gt;\">" . htmlentities($translator_name, ENT_COMPAT, 'UTF-8') . "</a>";
                else
                  $temp .= $translator_name;
                if ($maintainer)
                  $temp .= "</strong>";
                $temp .= "<br />";
              }
            }
            
            $languages[$language_name][$project] = $temp;
          }
        }
      }
      
      $temp = "<h3><a name=\"translators\">" . __('Translators') . "</a></h3>\n";
      $temp .= "<table class=\"status\">\n";
      $temp .= "  <tr>\n";
      $temp .= "    <th class=\"left\">" . __('Language') . "</th>\n";
      foreach ($projects as $project) { //For all projects...
        $temp .= "    <th class=\"left\">" . $project . "</th>\n";
      }
      $temp .= "  </tr>\n";
      foreach ($languages as $language_name => $language_translators) { //For all languages...
        $temp .= "  <tr>\n";
        $temp .= "    <td class=\"left\">" . __($language_name) . "</td>\n";
        foreach ($projects as $project_key => $project_name) { //For all projects...
          $temp .= "    <td class=\"left\">" . @$language_translators[$project_key] . "</td>\n";
        }
        $temp .= "  </tr>\n";
      }
      $temp .= "</table>\n";
      print($temp);
    }
    
    /**
     * ...
     */
    public function getLanguagesArray() {
      $languages = array();
      
      $status = $this->_status;
      foreach ($status->translations as $translations) { //for all projects...
        foreach ($translations as $translation) { //for all translations...
          $language_name = (string)$translation->language;
          
          if (!in_array($language_name, $languages)) { //if language NOT exists...
            $languages[] = $language_name;
          }
        }
      }
      
      sort($languages, SORT_STRING);
      return $languages;
    }
  }
?>