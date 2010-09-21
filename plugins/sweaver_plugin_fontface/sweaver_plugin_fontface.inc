<?php
// $Id$

/**
 * @file
 * @font-face plugin.
 */
class sweaver_plugin_fontface extends sweaver_plugin {

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
    $used_fonts = fontyourface_get_fonts();

    foreach ($used_fonts as $font) {
      if (module_exists($font->provider)) {
        $css_function = $font->provider . '_fontyourface_css';
        $font_css = $css_function($font);
        $extra_fonts[$font_css['font-family']] = $font->group_name;
      }
    }

    $objects->property->all['font-family']->property_options += $extra_fonts;
    if (isset($objects->property->enabled['font-family'])) {
      $objects->property->enabled['font-family']->property_options += $extra_fonts;
    }
  }

}
