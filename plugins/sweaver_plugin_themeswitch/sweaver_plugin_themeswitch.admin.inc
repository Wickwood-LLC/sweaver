<?php
// $Id$

/**
 * Theme switcher.
 */
function sweaver_plugin_themeswitch_switch($theme) {
  global $current_theme;
  $theme_default = variable_get('theme_default','garland');

  $all_themes = system_theme_data();
  if (isset($all_themes[$theme]) && $all_themes[$theme]->status == 1) {

    if ($theme != $theme_default) {
      $_SESSION['sweaver_plugin_switch_theme'] = $theme;
      $_SESSION['sweaver_editor_messages'][] = t('You have switched to @new_theme', array('@new_theme' => $all_themes[$theme]->info['name']));
    }
    else {
      // Reset session variable because it's the default.
      unset($_SESSION['sweaver_plugin_switch_theme']);
    }
  }

  // Go back to previous page.
  drupal_goto();
}