<?php
// $Id$

/**
 * @file
 * @font-face plugin.
 */

class sweaver_plugin_fontface extends sweaver_plugin {

  function sweaver_enabled($plugin_name) {
    return variable_get('sweaver_plugin_status_'. $plugin_name, TRUE) && module_exists('fontyourface');
  }

  /**
   * Sweaver dependencies.
   */
  function sweaver_dependencies() {
    return array('fontyourface');
  }

  /**
   * Sweaver objects alter.
   */
  function sweaver_objects_alter(&$objects) {
    $extra_fonts = array();
    $fonts = db_query("SELECT * FROM {fontyourface}");
    while ($font = db_fetch_object($fonts)) {
      $extra_fonts[$font->group_name] = $font->group_name;
    }

    $objects->property->all['font-family']->property_options += $extra_fonts;
    if (isset($objects->property->enabled['font-family'])) {
      $objects->property->enabled['font-family']->property_options += $extra_fonts;
    }
  }

}
